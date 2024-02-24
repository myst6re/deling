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
#ifndef FF8DISCARCHIVE_H
#define FF8DISCARCHIVE_H

#include <QtCore>
#include "IsoArchive.h"
#include "LZS.h"
#include "GZIP.h"

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
	bool isDemo() const;
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
	const FF8DiscFile &sysFntTdwFile();
	const FF8DiscFile &fieldBinFile();
	const FF8DiscFile &worldBinFile();
//	QList<FF8DiscFile> worldmapDirectory();
//	FF8DiscFile worldmapFile(int id);
	const QList<FF8DiscFile> &fieldDirectory();
	int fieldCount();
	const FF8DiscFile &fieldFile(int id);
//	QList<FF8DiscFile> menuDirectory();
//	FF8DiscFile menuFile(int id);
//	QList<FF8DiscFile> battleDirectory();
//	FF8DiscFile battleFile(int id);
	void searchFiles();
private:
	quint8 disc;
	qint64 posIMG, sizeIMG;
	QList<FF8DiscFile> rootFiles, fieldFiles;
	bool PAL;
};

#endif // FF8DISCARCHIVE_H
