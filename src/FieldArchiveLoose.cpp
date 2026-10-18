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
#include "FieldArchiveLoose.h"
#include "FieldLoose.h"
#include "ArchiveObserver.h"
#include "files/MchFile.h"

static const QRegularExpression MAIN_MODEL_NAME("^d(\\d\\d\\d)\\.mch$", QRegularExpression::CaseInsensitiveOption);

FieldArchiveLoose::FieldArchiveLoose()
    : FieldArchive()
{
}

QString FieldArchiveLoose::archivePath() const
{
	return _path;
}

bool FieldArchiveLoose::isField(const QStringList &paths)
{
	for (const QString &path: paths) {
		const QString ext = QFileInfo(path).suffix().toLower();
		if (ext == "inf" || ext == "jsm" || ext == "id") {
			return true;
		}
	}

	return false;
}

int FieldArchiveLoose::open(const QString &path, ArchiveObserver *progress)
{
	clearFields();
	_mainModelPaths.clear();
	_path = path;

	if (!QDir(path).exists()) {
		errorMsg = QObject::tr("Unable to open the folder.");
		return 1;
	}

	// Files grouped by folder and name, sorted so the fields always come in the same order
	QMap<QString, QStringList> groups;
	QString mapListPath;
	int scanned = 0;
	QDirIterator it(path, QDir::Files, QDirIterator::Subdirectories);

	progress->setObserverMaximum(0); // the number of files is not known before the end

	while (it.hasNext()) {
		const QFileInfo info(it.next());
		QString ext;

		if (++scanned % 500 == 0) {
			QCoreApplication::processEvents();
			if (progress->observerWasCanceled()) {
				errorMsg = QObject::tr("Opening canceled.");
				return 2;
			}
		}

		if (info.fileName().compare("maplist", Qt::CaseInsensitive) == 0) {
			mapListPath = info.filePath();
		} else if (MAIN_MODEL_NAME.match(info.fileName()).hasMatch()
		           && info.dir().dirName().compare("main_chr", Qt::CaseInsensitive) == 0) {
			_mainModelPaths.append(info.filePath());
		} else if (FieldLoose::looseExtension(info.filePath(), ext)) {
			groups[info.absolutePath() % "/" % info.completeBaseName()].append(info.filePath());
		}
	}

	// maplist gives each field its id, like mapdata\maplist in field.fs
	QStringList mapList;
	if (!mapListPath.isEmpty()) {
		QFile f(mapListPath);
		if (f.open(QIODevice::ReadOnly)) {
			for (const QString &line: QString::fromLatin1(f.readAll()).split('\n')) {
				mapList.append(line.trimmed());
			}
		}
	}
	setMapList(mapList);

	progress->setObserverMaximum(groups.size());
	int fieldID = 0, current = 0;

	for (auto group = groups.constBegin(); group != groups.constEnd(); ++group) {
		if (++current % 20 == 0) {
			QCoreApplication::processEvents();
			progress->setObserverValue(current);
			if (progress->observerWasCanceled()) {
				clearFields();
				errorMsg = QObject::tr("Opening canceled.");
				return 2;
			}
		}

		if (!isField(group.value())) {
			continue;
		}

		const QString name = QFileInfo(group.key()).fileName();
		FieldLoose *field = new FieldLoose(name);

		// Nothing is read yet: a file is read when something asks for it
		for (const QString &filePath: group.value()) {
			field->addFile(filePath);
		}
		field->setOpen(true);

		if (!field->hasFiles()) {
			delete field;
			continue;
		}

		const int index = this->mapList().indexOf(name);
		fields.append(field);
		fieldsSortByName.insert(name, fieldID);
		fieldsSortByMapId.insert(index == -1 ? "9999" : QString("%1").arg(index, 4, 10, QChar('0')), fieldID);
		++fieldID;
	}

	if (fields.isEmpty()) {
		errorMsg = QObject::tr("No field found in this folder.");
		return 4;
	}

	return 0;
}

bool FieldArchiveLoose::openModels()
{
	for (const QString &modelPath: _mainModelPaths) {
		const QString fileName = QFileInfo(modelPath).fileName();
		QFile f(modelPath);
		MchFile mch;

		if (f.open(QIODevice::ReadOnly) && mch.open(f.readAll(), fileName.left(4)) && mch.hasModel()) {
			_mainModels.insert(MAIN_MODEL_NAME.match(fileName).captured(1).toInt(), mch.model());
		}
	}

	return !_mainModels.isEmpty();
}

bool FieldArchiveLoose::openFull(Field *field) const
{
	FieldLoose *loose = dynamic_cast<FieldLoose *>(field);

	if (loose == nullptr) {
		return false;
	}

	loose->buildFiles(true);

	return true;
}

bool FieldArchiveLoose::save(ArchiveObserver *progress)
{
	bool ok = true;
	int current = 0;

	progress->setObserverMaximum(fields.size());

	for (Field *field: fields) {
		progress->setObserverValue(current++);

		if (field->isModified() && !static_cast<FieldLoose *>(field)->saveFiles()) {
			errorMsg = static_cast<FieldLoose *>(field)->errorString();
			ok = false;
		}
	}

	return ok;
}
