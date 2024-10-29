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
#include "FsArchive.h"
#include "TextExporter.h"
#include "LZS.h"
#include "QLZ4.h"
#include "FieldArchivePC.h"
#include <iostream>
#include <QCoreApplication>

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
	
	FieldArchivePC fieldArchive;
	if (fieldArchive.open(args.path(), &observer) != 0) {
		qWarning() << fieldArchive.errorMessage();
		return;
	}
	QStringList langs = fieldArchive.languages();
	
	TextExporter exporter(&fieldArchive);
	
	if (!exporter.toCsv(args.destination(), langs, QChar(','), QChar('"'), CsvFile::Utf8, &observer) && !observer.observerWasCanceled()) {
		return;
	}
}

void CLI::commandImport()
{
	ArgumentsImport args;
	if (args.help() || args.source().isEmpty()) {
		args.showHelp();
	}
	FsArchive *archive = openArchive(args.inputFormat(), args.path());
	if (archive == nullptr) {
		return;
	}
	
	FieldArchivePC fieldArchive;
	if (fieldArchive.open(args.path(), &observer) != 0) {
		qWarning() << fieldArchive.errorMessage();
		return;
	}

	TextExporter exporter(&fieldArchive);
	if (!exporter.fromCsv(args.source(), args.column(), QChar(','), QChar('"'), CsvFile::Utf8, &observer) && !observer.observerWasCanceled()) {
		return;
	}
	
	fieldArchive.save(&observer);
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
		includes.append(QRegularExpression(QRegularExpression::wildcardToRegularExpression(pattern, QRegularExpression::NonPathWildcardConversion)));
	}
	for (const QString &pattern: excludePatterns) {
		excludes.append(QRegularExpression(QRegularExpression::wildcardToRegularExpression(pattern, QRegularExpression::NonPathWildcardConversion)));
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
	}
}
