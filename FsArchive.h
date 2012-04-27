#ifndef FSHEADER_H
#define FSHEADER_H

#include <QtGui>
#include "Data.h"
#include "LZS.h"

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
	FsArchive(const QString &);
	~FsArchive();

	FsHeader *getFile(const QString &path) const;
	void fileToTheEnd(const QString &path, QByteArray &fs_data);
	void rebuildInfos();

	bool fileExists(const QString &path) const;
	bool dirExists(QString) const;

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
	FsArchive::Error extractFiles(const QStringList &fileNames, const QString &baseFileName, const QString &fileDir, QProgressDialog *progress, bool uncompress=true);
	Error replace(const QString &source, const QString &destination, QProgressDialog *progress);
	QList<FsArchive::Error> append(const QStringList &sources, const QStringList &destinations, bool compress, QProgressDialog *progress);
	Error remove(QStringList destinations, QProgressDialog *progress);
	Error rename(const QStringList &destinations, const QStringList &newDestinations);
	QMap<QString, FsHeader *> fileList(QString dir) const;
	QStringList tocInDirectory(QString dir) const;

	QMap<QString, FsHeader> getHeader() const;
	void setHeader(const QMap<QString, FsHeader> &header);

	void save(QByteArray &fl_data, QByteArray &fi_data) const;
	bool saveAs(const QString &path) const;
	int replaceArchive(QFile *newFile);
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
	void setFilePath(QString path, const QString &newPath);
	quint32 uncompressedFileSize(const QString &path) const;
	quint32 filePosition(const QString &path) const;
	void changePositions(FsHeader *start, int diff);

	bool load(const QString &fl_data, const QByteArray &fi_data);
	bool load(const QString &path);

	QMap<quint32, FsHeader *> infos;// <order, headerData>
	QMap<QString, quint32> toc_access;// <path, order>
	QFile fs, fl, fi;
	bool fromFile;
	bool _isOpen;
};

#endif // FSHEADER_H
