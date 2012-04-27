#ifndef FF8DISCARCHIVE_H
#define FF8DISCARCHIVE_H

#include <QtCore>
#include "IsoArchive.h"
#include "LZS.h"

class FF8DiscFile
{
public:
	FF8DiscFile();
	FF8DiscFile(quint32 pos, quint32 size);
	bool isValid() const;
	quint32 getPos() const;
	quint32 getSize() const;
private:
	quint32 pos, size;
};

class FF8DiscArchive : public IsoArchive
{
public:
	FF8DiscArchive(const QString &name);
	bool findIMG();
	bool IMGFound() const;
	quint8 discNumber() const;
	bool isJp() const;
	bool isPAL() const;
	QByteArray file(const FF8DiscFile &file);
	QByteArray fileLZS(const FF8DiscFile &file, bool strict=true);
	QByteArray fileGZ(const FF8DiscFile &file);
	bool extract(const FF8DiscFile &file, const QString &destination);
	bool extractLZS(const FF8DiscFile &file, const QString &destination, bool strict=true);
	bool extractGZ(const FF8DiscFile &file, const QString &destination);
	const QList<FF8DiscFile> &rootDirectory();
	int rootCount();
	const FF8DiscFile &rootFile(int id);
//	QList<FF8DiscFile> worldmapDirectory();
//	FF8DiscFile worldmapFile(int id);
	const QList<FF8DiscFile> &fieldDirectory();
	int fieldCount();
	const FF8DiscFile &fieldFile(int id);
//	QList<FF8DiscFile> menuDirectory();
//	FF8DiscFile menuFile(int id);
//	QList<FF8DiscFile> battleDirectory();
//	FF8DiscFile battleFile(int id);
private:
	quint8 disc;
	qint64 posIMG;
	QList<FF8DiscFile> rootFiles, fieldFiles;
	bool PAL;
};

#endif // FF8DISCARCHIVE_H
