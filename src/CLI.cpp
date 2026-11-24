/****************************************************************************
 ** Makou Reactor Final Fantasy VII Field Script Editor
 ** Copyright (C) 2009-2021 Arzel Jérôme <myst6re@gmail.com>
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
#include "CLI.h"
#include "Arguments.h"
#include "ArgumentsExport.h"
#include "ArgumentsImport.h"
#include "ArgumentsUnpack.h"
#include "ArgumentsPack.h"
#include "ArgumentsExportScripts.h"
#include "ArgumentsImportScripts.h"
#include "ScriptFiles.h"
#include "Field.h"
#include "files/JsmFile.h"
#include "FsArchive.h"
#include "TextExporter.h"
#include "LZS.h"
#include "QLZ4.h"
#include "FieldArchivePC.h"
#include "Config.h"
#include <iostream>
#include <QCoreApplication>
#include "QRegularExpressionWildcardCompat.h"

constexpr int BUFFER_SIZE = 4000000;

void CLIObserver::setObserverValue(int value)
{
	quint8 percent = quint8(value * 100.0 / double(_maximum));

	if (percent != _lastPercent) {
		_lastPercent = percent;
		setPercent(percent);
	}
}

void CLIObserver::setPercent(quint8 percent)
{
	printf("[%d%%] %s\r", percent, qPrintable(_filename.leftJustified(40, ' ', true)));
	fflush(stdout);
}

void CLIObserver::setObserverCanCancel(bool canCancel) const
{
	Q_UNUSED(canCancel)
}

CLIObserver CLI::observer;

// Changes a config value until the end of the scope, without saving it in Deling.ini
class ScopedConfigValue
{
public:
	ScopedConfigValue(const QString &key, const QString &value) :
	      _key(key), _active(!value.isEmpty())
	{
		if (_active) {
			_oldValue = Config::value(key);
			Config::setValue(key, value);
		}
	}
	~ScopedConfigValue()
	{
		if (!_active) {
			return;
		}
		if (_oldValue.isValid()) {
			Config::setValue(_key, _oldValue);
		} else {
			Config::removeValue(_key);
		}
	}
private:
	QString _key;
	QVariant _oldValue;
	bool _active;
};

void CLI::commandExport()
{
	ArgumentsExport args;
	if (args.help() || args.destination().isEmpty()) {
		args.showHelp();
	}

	FsArchive *archive = openArchive(args.inputFormat(), args.path());
	if (archive == nullptr) {
		return;
	}
	
	ScopedConfigValue encoding("encoding", args.encoding());
	FieldArchivePC fieldArchive;
	if (fieldArchive.open(args.path(), &observer) != 0) {
		qWarning() << fieldArchive.errorMessage();
		return;
	}
	QStringList langs = fieldArchive.languages();
	
	TextExporter exporter(&fieldArchive);
	
	if (!exporter.toCsv(args.destination(), langs, args.separator(), args.quoteCharacter(), CsvFile::Utf8, &observer) && !observer.observerWasCanceled()) {
		return;
	}
}

void CLI::commandImport()
{
	ArgumentsImport args;
	if (args.help() || args.source().isEmpty()) {
		args.showHelp();
	}

	ScopedConfigValue encoding("encoding", args.encoding());
	FieldArchivePC fieldArchive;
	if (fieldArchive.open(args.path(), &observer) != 0) {
		qWarning() << "Cannot open field archive" << fieldArchive.errorMessage();
		return;
	}

	TextExporter exporter(&fieldArchive);
	if (!exporter.fromCsv(args.source(), args.column(), args.separator(), args.quoteCharacter(), CsvFile::Utf8, &observer) && !observer.observerWasCanceled()) {
		qWarning() << "Cannot import CSV file" << exporter.errorString();
		return;
	}

	if (!fieldArchive.save(&observer)) {
		qWarning() << "Cannot save field archive" << fieldArchive.errorMessage();
		return;
	}
}

void CLI::commandUnpack()
{
	ArgumentsUnpack args;
	if (args.help() || args.destination().isEmpty()) {
		args.showHelp();
	}

	FsArchive *archive = openArchive(args.inputFormat(), args.path());
	if (archive == nullptr) {
		return;
	}
	
	QString commonPath = "c:\\ff8\\data\\";
	QStringList fileList = archive->tocInDirectory(commonPath);
	QStringList selectedFiles = filteredFiles(fileList, args.includes(), args.excludes());
	if (args.recursive()) {
		QStringList archiveFiles = filteredFiles(fileList, QStringList() << "*.fs" << "*.fi" << "*.fl", QStringList());

		FsArchive::Error error = archive->extractFiles(archiveFiles, commonPath, args.destination());
		if (error != FsArchive::Ok) {
			qWarning() << qPrintable(QCoreApplication::translate("CLI", "An error occured when exporting inner FS/FL/FI")) << qPrintable(FsArchive::errorString(error, args.path()));
		}
		
		for (const QString &archiveFile: archiveFiles) {
			if (!archiveFile.endsWith(".fs")) {
				continue;
			}
			
			QString fileName = QDir::cleanPath(args.destination() + QDir::separator() + archiveFile.mid(commonPath.size()).replace('\\', '/'));
			fileName.chop(1);
			{
				FsArchive subArchive(fileName);
				if (subArchive.isOpen()) {
					QStringList fileList2 = subArchive.tocInDirectory(commonPath);
					FsArchive::Error error = subArchive.extractFiles(filteredFiles(fileList2, args.includes(), args.excludes()), commonPath, args.destination());
					if (error != FsArchive::Ok) {
						qWarning() << qPrintable(QCoreApplication::translate("CLI", "An error occured when exporting file inside inner FS/FL/FI")) << fileName << qPrintable(FsArchive::errorString(error, args.path()));
					}
				}
			}
			
			QFile::remove(fileName % "s");
			QFile::remove(fileName % "l");
			QFile::remove(fileName % "i");
		}
		selectedFiles = filteredFiles(selectedFiles, QStringList(), QStringList() << "*.fs" << "*.fi" << "*.fl");
	}
	FsArchive::Error error = archive->extractFiles(selectedFiles, commonPath, args.destination(), args.noProgress() ? nullptr : &observer);
	if (error != FsArchive::Ok) {
		qWarning() << qPrintable(QCoreApplication::translate("CLI", "An error occured when exporting")) << qPrintable(FsArchive::errorString(error, args.path()));
	}

	delete archive;
}

void CLI::commandPack()
{
	ArgumentsPack args;
	if (args.help() || args.source().isEmpty()) {
		args.showHelp();
	}
	FiCompression compressionFormat = args.compressionFormat();
	
	QString path = args.path().left(args.path().size() - 1),
	        fsPath = FsArchive::fsPath(path),
	        fiPath = FsArchive::fiPath(path),
	        flPath = FsArchive::flPath(path);
	if (!args.force() && (QFile::exists(fsPath) || QFile::exists(fiPath) || QFile::exists(flPath))) {
		qWarning() << qPrintable(QCoreApplication::translate("CLI", "Destination file already exist, use --force to override"));
		return;
	}
	
	QString commonPath = args.prefix().replace('/', '\\');
	QStringList fileList;
	QDir dir(args.source());
	QDirIterator it(args.source(), QDir::Files, QDirIterator::Subdirectories);
	while (it.hasNext()) {
		it.next();
		fileList.append(dir.relativeFilePath(it.fileInfo().canonicalFilePath()));
	}
	QStringList selectedFiles = filteredFiles(fileList, args.includes(), args.excludes());
	QSaveFile fsFile(fsPath), fiFile(fiPath), flFile(flPath);
	if (!fsFile.open(QIODevice::WriteOnly | QIODevice::Truncate) || !fiFile.open(QIODevice::WriteOnly | QIODevice::Truncate) || !flFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		qWarning() << qPrintable(QCoreApplication::translate("CLI", "An error occured when opening target files")) << qPrintable(fsFile.errorString()) << qPrintable(fiFile.errorString()) << qPrintable(flFile.errorString());
		return;
	}

	if (!args.noProgress()) {
		observer.setObserverMaximum(selectedFiles.size());
	}
	int i = 0;
	for (QString fileName: selectedFiles) {
		if (!args.noProgress()) {
			observer.setFilename(fileName);
			if (observer.observerWasCanceled()) {
				return;
			}
			observer.setObserverValue(i++);
		}
		QString fullName = commonPath + fileName.replace('/', '\\');
		flFile.write(fullName.toLatin1() + "\r\n");
		quint32 pos = quint32(fsFile.pos());
		QFile f(dir.filePath(fileName));
		if (!f.open(QIODevice::ReadOnly)) {
			qWarning() << qPrintable(QCoreApplication::translate("CLI", "An error occured when exporting")) << qPrintable(f.errorString());
			return;
		}
		QByteArray data = f.readAll(), compressedData;
		f.close();
		quint32 uncompressedSize = quint32(data.size()), compressedSize = 0;
		
		switch (compressionFormat) {
		case FiCompression::CompressionLzs:
			compressedData = LZS::compress(data);
			compressedSize = quint32(data.size());
			compressedData.prepend((const char *)&compressedSize, 4);
			break;
		case FiCompression::CompressionLz4:
			compressedData = QLZ4::compress(data);
			compressedSize = quint32(data.size());
			compressedData.prepend((const char *)&compressedSize, 4);
			break;
		case FiCompression::CompressionNone:
		case FiCompression::CompressionUnknown:
			compressedData = data;
			break;
		}
		
		quint32 compression = quint32(compressionFormat);
		
		if (compressedData.size() >= data.size()) {
			compression = quint32(FiCompression::CompressionNone);
			compressedData = data;
		}
		
		fsFile.write(compressedData);

		fiFile.write((const char *)&uncompressedSize, 4);
		fiFile.write((const char *)&pos, 4);
		fiFile.write((const char *)&compression, 4);
	}
	
	if (!args.noProgress()) {
		if (observer.observerWasCanceled()) {
			return;
		}
		observer.setFilename(QCoreApplication::translate("CLI", "Apply changes..."));
		observer.flush();
	}
	
	if (!fsFile.commit() || !fiFile.commit() || !flFile.commit()) {
		qWarning() << qPrintable(QCoreApplication::translate("CLI", "An error occured when exporting")) << qPrintable(fsFile.errorString()) << qPrintable(fiFile.errorString()) << qPrintable(flFile.errorString());
	}
	
	if (!args.noProgress()) {
		observer.setFilename(QCoreApplication::translate("CLI", "Done"));
		observer.setObserverValue(i);
	}
}

void CLI::printLine(const QString &line)
{
	printf("%s\n", qUtf8Printable(line));
	fflush(stdout);
}

bool CLI::isJsmPath(const QString &path)
{
	return path.endsWith(".jsm", Qt::CaseInsensitive);
}

QString CLI::jsmName(const QString &jsmPath)
{
	return QFileInfo(jsmPath).completeBaseName();
}

QString CLI::symPath(const QString &jsmPath)
{
	QFileInfo info(jsmPath);
	return info.dir().filePath(info.completeBaseName() + ".sym");
}

bool CLI::openJsm(const QString &path, JsmFile &jsm)
{
	QFile jsmFile(path);
	if (!jsmFile.open(QIODevice::ReadOnly)) {
		qWarning() << "Error: cannot open" << qPrintable(path) << qPrintable(jsmFile.errorString());
		return false;
	}

	QByteArray sym;
	QFile symFile(symPath(path));
	if (symFile.exists()) {
		if (!symFile.open(QIODevice::ReadOnly)) {
			qWarning() << "Error: cannot open" << qPrintable(symFile.fileName()) << qPrintable(symFile.errorString());
			return false;
		}
		sym = symFile.readAll();
	}

	if (!jsm.open(jsmFile.readAll(), sym)) {
		qWarning() << "Error: invalid JSM file" << qPrintable(path);
		return false;
	}
	jsm.setOpen(true);

	return true;
}

bool CLI::writeFile(const QString &path, const QByteArray &data)
{
	QSaveFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate) || file.write(data) != data.size() || !file.commit()) {
		qWarning() << "Error: cannot write" << qPrintable(path) << qPrintable(file.errorString());
		return false;
	}

	return true;
}

bool CLI::saveJsm(const QString &path, JsmFile &jsm)
{
	QByteArray jsmData, sym;
	if (!jsm.save(jsmData, sym)) {
		qWarning() << "Error: cannot build JSM file" << qPrintable(path);
		return false;
	}

	if (!writeFile(path, jsmData)) {
		return false;
	}

	// Do not create a SYM file when there was none
	if (jsm.hasSym() && !sym.isEmpty()) {
		return writeFile(symPath(path), sym);
	}

	return true;
}

QList<QRegularExpression> CLI::fieldFilters(const QStringList &patterns)
{
	QList<QRegularExpression> filters;

	for (const QString &pattern: patterns) {
		filters.append(QRegularExpression(wildcardToRegularExpression(pattern, NonPathWildcardConversion),
		                                  QRegularExpression::CaseInsensitiveOption));
	}

	return filters;
}

bool CLI::matchesField(const QString &name, const QList<QRegularExpression> &filters)
{
	if (filters.isEmpty()) {
		return true;
	}

	for (const QRegularExpression &filter: filters) {
		if (filter.match(name).hasMatch()) {
			return true;
		}
	}

	return false;
}

void CLI::commandExportScripts()
{
	ArgumentsExportScripts args;
	if (args.help() || args.directory().isEmpty()) {
		args.showHelp();
	}

	QDir dir(args.directory());
	if (!dir.mkpath(".")) {
		qWarning() << "Error: cannot create directory" << qPrintable(args.directory());
		exit(1);
	}

	ScriptFiles scriptFiles(args.opcodes() ? ScriptFiles::Opcodes : ScriptFiles::PseudoCode);

	if (isJsmPath(args.path())) {
		JsmFile jsm;
		if (!openJsm(args.path(), jsm)) {
			exit(1);
		}

		QString fieldDir = dir.filePath(jsmName(args.path()));
		if (!scriptFiles.toDir(&jsm, QDir(fieldDir), args.force())) {
			qWarning() << "Error:" << qPrintable(scriptFiles.errorString());
			exit(1);
		}

		printLine(QString("Scripts exported to %1").arg(QDir::toNativeSeparators(fieldDir)));
		return;
	}

	SilentObserver silentObserver;
	ArchiveObserver *progress = args.noProgress() ? static_cast<ArchiveObserver *>(&silentObserver) : &observer;
	FieldArchivePC fieldArchive;
	if (fieldArchive.open(args.path(), progress) != 0) {
		qWarning() << "Error: cannot open field archive" << qPrintable(fieldArchive.errorMessage());
		exit(1);
	}
	if (!args.noProgress()) {
		printf("\n");
	}

	QList<QRegularExpression> filters = fieldFilters(args.fields());
	int fieldCount = 0;
	FieldArchiveIterator it = fieldArchive.iterator();

	while (it.hasNext()) {
		Field *field = it.next();

		if (field == nullptr || !field->isOpen() || !matchesField(field->name(), filters) || !field->hasJsmFile()) {
			continue;
		}

		JsmFile *jsm = field->getJsmFile();
		if (!jsm->isOpen()) {
			qWarning() << "Warning: cannot open the scripts of" << qPrintable(field->name());
			continue;
		}

		if (!scriptFiles.toDir(jsm, QDir(dir.filePath(field->name())), args.force())) {
			qWarning() << "Error:" << qPrintable(scriptFiles.errorString());
			exit(1);
		}
		++fieldCount;
	}

	if (fieldCount == 0) {
		qWarning() << "Error: no field scripts found" << (filters.isEmpty() ? "" : "matching --field");
		exit(1);
	}

	printLine(QString("Scripts of %1 field(s) exported to %2").arg(fieldCount).arg(QDir::toNativeSeparators(dir.absolutePath())));
}

void CLI::commandImportScripts()
{
	ArgumentsImportScripts args;
	if (args.help() || args.directory().isEmpty()) {
		args.showHelp();
	}

	QDir dir(args.directory());
	ScriptFiles scriptFiles(args.opcodes() ? ScriptFiles::Opcodes : ScriptFiles::PseudoCode);
	QString dryRunText = args.dryRun() ? QString(" (dry run, nothing saved)") : QString();

	if (isJsmPath(args.path())) {
		JsmFile jsm;
		if (!openJsm(args.path(), jsm)) {
			exit(1);
		}

		QString name = jsmName(args.path());
		if (!dir.exists(name)) {
			qWarning() << "Error: there is no" << qPrintable(name) << "directory in" << qPrintable(args.directory());
			exit(1);
		}

		int compiledCount = scriptFiles.fromDir(&jsm, QDir(dir.filePath(name)));
		for (const QString &message: scriptFiles.messages()) {
			printLine(message);
		}
		if (compiledCount < 0) {
			qWarning() << "Error:" << qPrintable(scriptFiles.errorString());
			qWarning() << "Nothing was saved";
			exit(1);
		}
		if (compiledCount == 0) {
			printLine("No script changed, nothing to save");
			return;
		}
		if (!args.dryRun() && !saveJsm(args.path(), jsm)) {
			exit(1);
		}

		printLine(QString("%1 method(s) compiled%2").arg(compiledCount).arg(dryRunText));
		return;
	}

	SilentObserver silentObserver;
	ArchiveObserver *progress = args.noProgress() ? static_cast<ArchiveObserver *>(&silentObserver) : &observer;
	FieldArchivePC fieldArchive;
	if (fieldArchive.open(args.path(), progress) != 0) {
		qWarning() << "Error: cannot open field archive" << qPrintable(fieldArchive.errorMessage());
		exit(1);
	}
	if (!args.noProgress()) {
		printf("\n");
	}

	QList<QRegularExpression> filters = fieldFilters(args.fields());
	QStringList unusedDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	int compiledTotal = 0, fieldCount = 0;
	FieldArchiveIterator it = fieldArchive.iterator();

	while (it.hasNext()) {
		Field *field = it.next();

		if (field == nullptr || !field->isOpen() || !matchesField(field->name(), filters) || !dir.exists(field->name())) {
			continue;
		}
		unusedDirs.removeAll(field->name());

		if (!field->hasJsmFile() || !field->getJsmFile()->isOpen()) {
			qWarning() << "Warning: cannot open the scripts of" << qPrintable(field->name());
			continue;
		}

		int compiledCount = scriptFiles.fromDir(field->getJsmFile(), QDir(dir.filePath(field->name())));
		for (const QString &message: scriptFiles.messages()) {
			printLine(message);
		}
		if (compiledCount < 0) {
			qWarning() << "Error:" << qPrintable(scriptFiles.errorString());
			qWarning() << "Nothing was saved";
			exit(1);
		}
		if (compiledCount > 0) {
			compiledTotal += compiledCount;
			++fieldCount;
		}
	}

	if (filters.isEmpty()) {
		for (const QString &unusedDir: unusedDirs) {
			printLine(QString("Warning: no field named %1 in the archive, directory ignored").arg(unusedDir));
		}
	}

	if (compiledTotal == 0) {
		printLine("No script changed, nothing to save");
		return;
	}

	if (!args.dryRun()) {
		if (!fieldArchive.save(progress)) {
			qWarning() << "Error: cannot save field archive" << qPrintable(fieldArchive.errorMessage());
			exit(1);
		}
		if (!args.noProgress()) {
			printf("\n");
		}
	}

	printLine(QString("%1 method(s) compiled in %2 field(s)%3").arg(compiledTotal).arg(fieldCount).arg(dryRunText));
}

FsArchive *CLI::openArchive(const QString &ext, const QString &path)
{
	Q_UNUSED(ext)
	FsArchive *archive = new FsArchive(path.left(path.size() - 1));
	if (!archive->isOpen()) {
		qWarning() << qPrintable(QCoreApplication::translate("CLI", "Error")) << qPrintable(QCoreApplication::translate("CLI", "Cannot open archive"));
		delete archive;
		return nullptr;
	}

	return archive;
}

QStringList CLI::filteredFiles(const QStringList &fileList, const QStringList &includePatterns, const QStringList &excludePatterns)
{
	QStringList selectedFiles;
	QList<QRegularExpression> includes, excludes;

	for (const QString &pattern: includePatterns) {
		includes.append(QRegularExpression(wildcardToRegularExpression(pattern, NonPathWildcardConversion)));
	}
	for (const QString &pattern: excludePatterns) {
		excludes.append(QRegularExpression(wildcardToRegularExpression(pattern, NonPathWildcardConversion)));
	}

	for (const QString &entry: fileList) {
		bool found = includes.isEmpty();
		for (const QRegularExpression &regExp: includes) {
			if (regExp.match(entry).hasMatch()) {
				found = true;
				break;
			}
		}
		for (const QRegularExpression &regExp: excludes) {
			if (regExp.match(entry).hasMatch()) {
				found = false;
				break;
			}
		}

		if (found) {
			selectedFiles.append(entry);
		}
	}
	
	return selectedFiles;
}

void CLI::exec()
{
	Arguments args;
	if (args.help()) {
		args.showHelp();
	}

	switch (args.command()) {
	case Arguments::None:
		args.showHelp();
	case Arguments::Export:
		commandExport();
		break;
	case Arguments::Import:
		commandImport();
		break;
	case Arguments::Unpack:
		commandUnpack();
		break;
	case Arguments::Pack:
		commandPack();
		break;
	case Arguments::ExportScripts:
		commandExportScripts();
		break;
	case Arguments::ImportScripts:
		commandImportScripts();
		break;
	}
}
