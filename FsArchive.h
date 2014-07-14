/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2012 Arzel Jérôme <myst6re@gmail.com>
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
#ifndef FSHEADER_H
#define FSHEADER_H

#include <QtGui>
#include "Data.h"
#include "LZS.h"
#include "ArchiveObserver.h"

typedef struct {
	quint32 size;
	quint32 pos;
	quint32 isCompressed;
} Fi_infos;

class FsHeader
{
public:
	FsHeader();
	FsHeader(const QString &path, quint32 uncompressed_size, quint32 position, bool isCompressed);
    const QString &path() const;
	void setPath(const QString &);
	QString dirName() const;
	QString fileName() const;
	quint32 uncompressed_size() const;
	void setUncompressed_size(quint32);
	quint32 position() const;
	void setPosition(quint32);
	bool isCompressed() const;
	QByteArray data(const QByteArray &, bool uncompress=true, int maxUncompress=0) const;
	QByteArray data(QFile *, bool uncompress=true, int maxUncompress=0) const;
	int setData(QByteArray &, const QByteArray &);
	int setData(QFile *, QByteArray &);
private:
	QString _path;
	quint32 _uncompressed_size;
	quint32 _position;
	bool _isCompressed;
};

class FsArchive
{
public:
	enum Error{Ok, NonWritable, SourceCantBeOpened, TempCantBeOpened, TargetCantBeOpened, Canceled, SaveHeaderError, ReplaceArchiveError, FileExists, FileDontExists, EmptyFile};

//	FsArchive();
	FsArchive(const QByteArray &fl_data, const QByteArray &fi_data);
	FsArchive(const QString &path);
	virtual ~FsArchive();

	void addFile(const QString &path, bool isCompressed);
	FsHeader *getFile(const QString &path) const;
	void fileToTheEnd(const QString &path, QByteArray &fs_data);
	void rebuildInfos();

	bool fileExists(const QString &path) const;
	bool dirExists(QString dir) const;

	void setFilePosition(const QString &path, quint32);
	bool fileIsCompressed(const QString &path) const;
	QByteArray fileData(const QString &, const QByteArray &fs_data, bool uncompress=true, int maxUncompress=0);
	QByteArray fileData(const QString &, bool uncompress=true, int maxUncompress=0);
	void setFileData(const QString &, QByteArray &, const QByteArray &);
	void setFileData(const QString &, QByteArray &);
	QStringList toc() const;
	QStringList dirs() const;
	bool extractFile(const QString &fileName, const QByteArray &fs_data, const QString &filePath, bool uncompress=true);
	bool extractFile(const QString &fileName, const QString &filePath, bool uncompress=true);
	FsArchive::Error extractFiles(const QStringList &fileNames, const QString &baseFileName, const QString &fileDir, ArchiveObserver *progress, bool uncompress=true);
	Error replaceFile(const QString &source, const QString &destination, ArchiveObserver *progress);
	Error replaceDir(const QString &source, const QString &destination, bool compress, ArchiveObserver *progress);
	QList<FsArchive::Error> append(const QStringList &sources, const QStringList &destinations, bool compress, ArchiveObserver *progress);
	Error remove(QStringList destinations, ArchiveObserver *progress);
	Error rename(const QStringList &destinations, const QStringList &newDestinations);
	QMap<QString, FsHeader *> fileList(QString dir) const;
	QStringList tocInDirectory(QString dir) const;

	QMap<QString, FsHeader> getHeader() const;
	void setHeader(const QMap<QString, FsHeader> &header);

	void save(QByteArray &fl_data, QByteArray &fi_data) const;
	bool saveAs(const QString &path) const;
	int replaceArchive(QFile *newFile);
    static QString fsPath(const QString &path);
    static QString flPath(const QString &path);
    static QString fiPath(const QString &path);
	QString path() const;
	bool setPath(const QString &path);
	int size() const;
	bool isOpen() const;
	bool isWritable() const;
//	QString toString();
	static QString cleanPath(QString path);

	static QString errorString(Error, const QString &fileName=QString());
private:
	void addFile(const QString &path, quint32 uncompressed_size, quint32 position, bool isCompressed);
	bool removeFile(QString);
	QString filePath(const QString &path) const;
    bool setFilePath(QString path, const QString &newPath);
	quint32 uncompressedFileSize(const QString &path) const;
	quint32 filePosition(const QString &path) const;
	void changePositions(FsHeader *start, int diff);
	static QStringList listDirsRec(QDir *sourceDir);

	bool load(const QString &fl_data, const QByteArray &fi_data);
	bool load(const QString &path);

    QMultiMap<quint32, FsHeader *> sortedByPosition;// <order, headerData>
    QMap<QString, FsHeader *> toc_access;// <path, headerData>
	QFile fs, fl, fi;
	bool fromFile;
	bool _isOpen;
};

#endif // FSHEADER_H
