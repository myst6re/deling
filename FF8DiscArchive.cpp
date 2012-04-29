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
#include "FF8DiscArchive.h"

FF8DiscFile::FF8DiscFile() :
	size(0)
{
}

FF8DiscFile::FF8DiscFile(quint32 pos, quint32 size) :
	pos(pos), size(size)
{
}

bool FF8DiscFile::isValid() const
{
	return size != 0;
}

quint32 FF8DiscFile::getPos() const
{
	return pos;
}

quint32 FF8DiscFile::getSize() const
{
	return size;
}

FF8DiscArchive::FF8DiscArchive(const QString &name) :
	IsoArchive(name), disc(0), PAL(false)
{
}

bool FF8DiscArchive::findIMG()
{
	if(seekToFile("FF8DISC1.IMG")) {
		disc = 1;
	} else if(seekToFile("FF8DISC2.IMG")) {
		disc = 2;
	} else if(seekToFile("FF8DISC3.IMG")) {
		disc = 3;
	} else if(seekToFile("FF8DISC4.IMG")) {
		disc = 4;
	} else {
		return false;
	}

	posIMG = posIso();

	return true;
}

bool FF8DiscArchive::IMGFound() const
{
	return disc != 0;
}

quint8 FF8DiscArchive::discNumber() const
{
	return disc;
}

bool FF8DiscArchive::isJp() const
{
	foreach(IsoFile *file, IsoArchive::rootDirectory()->files()) {
		const QString &name = file->name();
		if(name.startsWith("SLPS") || name.startsWith("SCPS") || name.startsWith("SLPM"))
			return true;
	}

	return false;
}

bool FF8DiscArchive::isPAL() const
{
	return PAL;
}

QByteArray FF8DiscArchive::file(const FF8DiscFile &file)
{
	if(!file.isValid())
		return QByteArray();

	if(seekToSector(file.getPos()))
		return readIso(file.getSize());
	else
		return QByteArray();
}

QByteArray FF8DiscArchive::fileLZS(const FF8DiscFile &file, bool strict)
{
	if(!file.isValid())		return QByteArray();

	quint32 lzsSize;

	if(!seekToSector(file.getPos()))	return QByteArray();
	if(readIso((char *)&lzsSize, 4) != 4)	return QByteArray();

	// strict = same size as specified | !strict = same sector count as specified
	if((strict && file.getSize() != lzsSize+4) || (!strict && (lzsSize + 4)/2048 + (int)(lzsSize%2048 != 0) != file.getSize()/2048 + (int)(file.getSize()%2048 != 0)))
		return QByteArray();

	return LZS::decompress(readIso(lzsSize).constData(), lzsSize);
}

QByteArray FF8DiscArchive::fileGZ(const FF8DiscFile &file)
{
	if(!file.isValid())		return QByteArray();

	if(!seekToSector(file.getPos()))	return QByteArray();
	seekIso(posIso() + 8);

	return GZIP::decompress(readIso(file.getSize()-8));
}

bool FF8DiscArchive::extract(const FF8DiscFile &file, const QString &destination)
{
	QByteArray data = this->file(file);
	if(data.isEmpty())		return false;

	QFile ret(destination);
	if(!ret.open(QIODevice::WriteOnly))		return false;

	return ret.write(data) != -1;
}

bool FF8DiscArchive::extractLZS(const FF8DiscFile &file, const QString &destination, bool strict)
{
	QByteArray data = fileLZS(file, strict);
	if(data.isEmpty())		return false;

	QFile ret(destination);
	if(!ret.open(QIODevice::WriteOnly))		return false;

	return ret.write(data) != -1;
}

bool FF8DiscArchive::extractGZ(const FF8DiscFile &file, const QString &destination)
{
	QByteArray data = fileGZ(file);
	if(data.isEmpty())		return false;

	QFile ret(destination);
	if(!ret.open(QIODevice::WriteOnly))		return false;

	return ret.write(data) != -1;
}

const QList<FF8DiscFile> &FF8DiscArchive::rootDirectory()
{
	if(!rootFiles.isEmpty() || !IMGFound())		return rootFiles;

	quint32 position, size, numSectors = sectorCount();
	qint64 maxPos;

	seekIso(posIMG);
	readIso((char *)&position, 4);
	readIso((char *)&size, 4);

	if(position >= numSectors || size == 0 || position + size/2048 >= numSectors) {
		seekIso(posIMG + 5 * 2048); // PAL hack
		readIso((char *)&position, 4);
		readIso((char *)&size, 4);

		if(position >= numSectors || size == 0 || position + size/2048 >= numSectors) {
			return rootFiles;
		}

		PAL = true;
	}

	rootFiles.append(FF8DiscFile(position, size));

	maxPos = posIso() + 2048;

	while(posIso() < maxPos) {
		readIso((char *)&position, 4);
		readIso((char *)&size, 4);

		if(position < numSectors && size != 0 && position + size/2048 < numSectors)
			rootFiles.append(FF8DiscFile(position, size));
		else
			break;
	}

	return rootFiles;
}

int FF8DiscArchive::rootCount()
{
	return rootDirectory().size();
}

const FF8DiscFile &FF8DiscArchive::rootFile(int id)
{
	return rootDirectory().at(id);
}

const QList<FF8DiscFile> &FF8DiscArchive::fieldDirectory()
{
	if(!fieldFiles.isEmpty())		return fieldFiles;

	quint32 position, size, numSectors = sectorCount();
	if(2 >= rootCount())		return fieldFiles;
	const FF8DiscFile &fieldbinFile = rootFile(2);

//	// nbSectorsLzs != nbSectors
//	if((lzsSize + 4)/2048 + (int)(lzsSize%2048 != 0) != fieldbinFile.getSize()/2048 + (int)(fieldbinFile.getSize()%2048 != 0))
//		return fieldFiles;

	const QByteArray fieldBin = fileLZS(fieldbinFile, false);
	if(fieldBin.isEmpty())
		return fieldFiles;
	const char *fieldBinData = fieldBin.constData();
	int index, lastIndex, tocSize;
	if((index = fieldBin.indexOf(QByteArray("\x04\x00\x05\x24\x18\x00\xbf\x8f\x14\x00\xb1\x8f\x10\x00\xb0\x8f\x20\x00\xbd\x27\x08\x00\xe0\x03\x00\x00\x00\x00", 28))) != -1) {
		index += 28;// with main field models
	} else {
		qWarning() << "bin invalide : index introuvable !";
		return fieldFiles;
//		index = jap ? 0x28668 : 0x286a0;// with main field models
	}
	if((lastIndex = fieldBin.indexOf(QByteArray("\x00\x00\x01\x00\x02\x00\x03\x00", 8), index)) != -1) {
		if((lastIndex - index) % 8 != 0) {
			qWarning() << "bin invalide : lastIndex invalide !" << index << lastIndex;
			return fieldFiles;
		}
		tocSize = (lastIndex - index) / 8;
	} else {
		qWarning() << "bin invalide : lastIndex introuvable !" << index;
		return fieldFiles;
	}

	for(int i=0 ; i<tocSize ; ++i) {
		memcpy(&position, &fieldBinData[index + i*8], 4);
		memcpy(&size, &fieldBinData[index + i*8 + 4], 4);

		if(position < numSectors && size != 0 && position + size/2048 < numSectors)
			fieldFiles.append(FF8DiscFile(position, size));
		else
			break;
	}

	return fieldFiles;
}

int FF8DiscArchive::fieldCount()
{
	return fieldDirectory().size();
}

const FF8DiscFile &FF8DiscArchive::fieldFile(int id)
{
	return fieldDirectory().at(id);
}
