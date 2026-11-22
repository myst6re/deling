/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2024 Arzel Jérôme <myst6re@gmail.com>
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#include "ScriptFiles.h"
#include "files/JsmFile.h"
#include "JsmPseudoCompiler.h"

ScriptFiles::ScriptFiles(Format format) :
    _format(format)
{
}

QString ScriptFiles::entryName(int id, const QString &name)
{
	static const QRegularExpression forbidden("[^A-Za-z0-9_]");
	QString cleanName = QString(name).replace(forbidden, "_");

	return QString("%1-%2").arg(id, 3, 10, QChar('0')).arg(cleanName.isEmpty() ? QString("unnamed") : cleanName);
}

QString ScriptFiles::methodText(JsmFile *jsm, int groupID, int methodID) const
{
	return jsm->toString(groupID, methodID, _format == PseudoCode, 0, true);
}

QString ScriptFiles::normalized(const QString &text)
{
	static const QRegularExpression lineBreaks("\\r\\n?");
	QString ret = QString(text).replace(lineBreaks, "\n");

	qsizetype end = ret.size();
	while (end > 0 && ret.at(end - 1).isSpace()) {
		--end;
	}

	return ret.left(end);
}

bool ScriptFiles::toDir(JsmFile *jsm, const QDir &dir, bool overwrite)
{
	const JsmScripts &scripts = jsm->getScripts();

	for (int groupID = 0; groupID < scripts.groups().size(); ++groupID) {
		const JsmGroup &group = scripts.group(groupID);
		QString groupDirName = entryName(groupID, group.name());

		if (!dir.mkpath(groupDirName)) {
			_lastErrorString = QObject::tr("Cannot create directory %1")
			                       .arg(QDir::toNativeSeparators(dir.filePath(groupDirName)));
			return false;
		}

		QDir groupDir(dir.filePath(groupDirName));

		for (int methodID = 0; methodID < group.methodCount(); ++methodID) {
			QString path = groupDir.filePath(entryName(methodID, group.method(methodID).name()) + ".txt");

			if (!overwrite && QFile::exists(path)) {
				_lastErrorString = QObject::tr("%1 already exists, use --force to overwrite it")
				                       .arg(QDir::toNativeSeparators(path));
				return false;
			}

			QString text = methodText(jsm, groupID, methodID);
			if (!text.endsWith('\n')) {
				text.append('\n');
			}

			QFile file(path);
			if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
				_lastErrorString = QObject::tr("Cannot write %1: %2")
				                       .arg(QDir::toNativeSeparators(path), file.errorString());
				return false;
			}
			file.write(text.toUtf8());
		}
	}

	return true;
}

bool ScriptFiles::listEntries(const QDir &dir, QDir::Filters filters, const QStringList &nameFilters,
                              QMap<int, QString> &entries, QString &errorStr)
{
	static const QRegularExpression idPrefix("^(\\d+)-");

	for (const QString &entry: dir.entryList(nameFilters, filters | QDir::NoDotAndDotDot, QDir::Name)) {
		QRegularExpressionMatch match = idPrefix.match(entry);
		if (!match.hasMatch()) {
			continue;
		}

		int id = match.captured(1).toInt();
		if (entries.contains(id)) {
			errorStr = QObject::tr("%1 and %2 have the same ID, remove one of them")
			               .arg(QDir::toNativeSeparators(dir.filePath(entries.value(id))),
			                    QDir::toNativeSeparators(dir.filePath(entry)));
			return false;
		}
		entries.insert(id, entry);
	}

	return true;
}

int ScriptFiles::fromDir(JsmFile *jsm, const QDir &dir)
{
	JsmScripts &scripts = jsm->getScripts();
	QMap<int, QString> groupDirs;
	int compiledCount = 0;

	_messages.clear();

	if (!listEntries(dir, QDir::Dirs, QStringList(), groupDirs, _lastErrorString)) {
		return -1;
	}

	QMapIterator<int, QString> itGroup(groupDirs);
	while (itGroup.hasNext()) {
		itGroup.next();
		int groupID = itGroup.key();
		QDir groupDir(dir.filePath(itGroup.value()));

		if (groupID >= scripts.groups().size()) {
			_lastErrorString = QObject::tr("%1: there is no group %2 in this script (%3 groups)")
			                       .arg(QDir::toNativeSeparators(groupDir.path())).arg(groupID).arg(scripts.groups().size());
			return -1;
		}

		JsmGroup &group = scripts.group(groupID);

		if (itGroup.value() != entryName(groupID, group.name())) {
			_messages.append(QObject::tr("Warning: %1: group %2 is named \"%3\" in the script")
			                     .arg(QDir::toNativeSeparators(groupDir.path())).arg(groupID).arg(group.name()));
		}

		QMap<int, QString> methodFiles;
		if (!listEntries(groupDir, QDir::Files, QStringList("*.txt"), methodFiles, _lastErrorString)) {
			return -1;
		}

		QMapIterator<int, QString> itMethod(methodFiles);
		while (itMethod.hasNext()) {
			itMethod.next();
			int methodID = itMethod.key();
			QString path = QDir::toNativeSeparators(groupDir.filePath(itMethod.value()));

			if (methodID >= group.methodCount()) {
				_lastErrorString = QObject::tr("%1: there is no method %2 in group %3 (%4 methods)")
				                       .arg(path).arg(methodID).arg(groupID).arg(group.methodCount());
				return -1;
			}

			QFile file(groupDir.filePath(itMethod.value()));
			if (!file.open(QIODevice::ReadOnly)) {
				_lastErrorString = QObject::tr("Cannot read %1: %2").arg(path, file.errorString());
				return -1;
			}
			QString text = QString::fromUtf8(file.readAll());
			file.close();

			if (normalized(text) == normalized(methodText(jsm, groupID, methodID))) {
				continue; // Unchanged
			}

			if (_format == PseudoCode) {
				JsmPseudoCompiler compiler;
				JsmData compiled;
				QString errorStr;
				int errorLine = 0;

				if (!compiler.compile(text, scripts, compiled, errorStr, errorLine)) {
					_lastErrorString = errorLine > 0
					                       ? QString("%1:%2: %3").arg(path).arg(errorLine).arg(errorStr)
					                       : QString("%1: %2").arg(path, errorStr);
					return -1;
				}

				group.method(methodID).setScriptData(compiled);
				jsm->setModified(true);
			} else {
				QString errorStr;
				int errorLine = jsm->fromString(groupID, methodID, text, errorStr);

				if (errorLine != 0) {
					_lastErrorString = errorLine > 0
					                       ? QString("%1:%2: %3").arg(path).arg(errorLine).arg(errorStr)
					                       : QString("%1: %2").arg(path, errorStr);
					return -1;
				}
			}

			// Clear cached decompiled scripts
			jsm->setDecompiledScript(groupID, methodID, QString(), false);
			jsm->setDecompiledScript(groupID, methodID, QString(), true);

			_messages.append(QObject::tr("%1: compiled (%2 opcodes)")
			                     .arg(path).arg(group.method(methodID).scriptData().nbOpcode()));
			++compiledCount;
		}
	}

	return compiledCount;
}
