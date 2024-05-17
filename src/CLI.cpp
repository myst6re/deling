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
#include "FsArchive.h"
#include "LZS.h"
#include "QLZ4.h"
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
	printf("[%d%%] %s\r", percent, qPrintable(_filename));
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
	
	QString commonPath = "c:\\ff8\\data\\";
	QStringList fileList = archive->tocInDirectory(commonPath);
	QStringList selectedFiles = filteredFiles(fileList, commonPath.size(), args.includes(), args.excludes());
	FsArchive::Error error = archive->extractFiles(selectedFiles, commonPath, args.destination(), &observer);
	if (error != FsArchive::Ok) {
		qWarning() << qPrintable(QCoreApplication::translate("CLI", "An error occured when exporting")) << qPrintable(FsArchive::errorString(error, args.path()));
	}

	delete archive;
}

void CLI::commandImport()
{
	ArgumentsImport args;
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
	
	QString commonPath = "c:\\ff8\\data\\";
	QStringList fileList;
	QDirIterator it(args.source(), QDir::Files, QDirIterator::Subdirectories);
	while (it.hasNext()) {
		it.next();
		fileList.append(it.filePath());
	}
	QStringList selectedFiles = filteredFiles(fileList, 0, args.includes(), args.excludes());
	QSaveFile fsFile(fsPath), fiFile(fiPath), flFile(flPath);
	if (!fsFile.open(QIODevice::WriteOnly | QIODevice::Truncate) || !fiFile.open(QIODevice::WriteOnly | QIODevice::Truncate) || !flFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		qWarning() << qPrintable(QCoreApplication::translate("CLI", "An error occured when opening target files")) << qPrintable(fsFile.errorString()) << qPrintable(fiFile.errorString()) << qPrintable(flFile.errorString());
		return;
	}
	qDebug() << selectedFiles << fileList << args.source() << QDir::tempPath();
	observer.setObserverMaximum(selectedFiles.size());
	int i = 0;
	for (QString fileName: selectedFiles) {
		observer.setFilename(fileName);
		if (observer.observerWasCanceled()) {
			return;
		}
		observer.setObserverValue(i++);
		QString fullName = commonPath + fileName.replace('/', '\\');
		flFile.write(fullName.toLatin1() + "\r\n");
		quint32 pos = quint32(fsFile.pos());
		QFile f(fileName);
		if (!f.open(QIODevice::ReadOnly)) {
			qWarning() << qPrintable(QCoreApplication::translate("CLI", "An error occured when exporting")) << qPrintable(f.errorString());
			return;
		}
		QByteArray data = f.readAll(), compressedData;
		f.close();
		quint32 uncompressedSize = quint32(data.size());
		
		switch (compressionFormat) {
		case FiCompression::CompressionLzs:
			compressedData = LZS::compress(data);
			break;
		case FiCompression::CompressionLz4:
			compressedData = QLZ4::compress(data);
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
	
	if (observer.observerWasCanceled()) {
		return;
	}
	
	if (!fsFile.commit() || !fiFile.commit() || !flFile.commit()) {
		qWarning() << qPrintable(QCoreApplication::translate("CLI", "An error occured when exporting")) << qPrintable(fsFile.errorString()) << qPrintable(fiFile.errorString()) << qPrintable(flFile.errorString());
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

QStringList CLI::filteredFiles(const QStringList &fileList, int offset, const QStringList &includePatterns, const QStringList &excludePatterns)
{
	QStringList selectedFiles;
	QList<QRegularExpression> includes, excludes;

	for (const QString &pattern: includePatterns) {
		includes.append(QRegularExpression(QRegularExpression::anchoredPattern(QRegularExpression::wildcardToRegularExpression(pattern))));
	}
	for (const QString &pattern: excludePatterns) {
		excludes.append(QRegularExpression(QRegularExpression::anchoredPattern(QRegularExpression::wildcardToRegularExpression(pattern))));
	}

	for (const QString &entry: fileList) {
		bool found = includes.isEmpty();
		for (const QRegularExpression &regExp: includes) {
			if (regExp.match(entry, offset).hasMatch()) {
				found = true;
				break;
			}
		}
		for (const QRegularExpression &regExp: excludes) {
			if (regExp.match(entry, offset).hasMatch()) {
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
	}
}
