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
#include "FF8DiscArchive.h"
#include "LZS.h"
#include "GZIP.h"

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
	IsoFile *file;
	if ((file = fileInfos("FF8DISC1.IMG"))) {
		disc = 1;
	} else if ((file = fileInfos("FF8DISC2.IMG"))) {
		disc = 2;
	} else if ((file = fileInfos("FF8DISC3.IMG"))) {
		disc = 3;
	} else if ((file = fileInfos("FF8DISC4.IMG"))) {
		disc = 4;
	} else if ((file = fileInfos("FF8TRY.IMG"))) { // JP Demo disc
		disc = 10;
	} else {
		return false;
	}

	posIMG = file->location() * SECTOR_SIZE_DATA;
	sizeIMG = file->size();

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
	for (IsoFile *file: IsoArchive::rootDirectory()->files()) {
		const QString &name = file->name();
		if (name.startsWith("SLPS") || name.startsWith("SCPS") || name.startsWith("SLPM"))
			return true;
	}

	return false;
}

bool FF8DiscArchive::isDemo() const
{
	return disc == 10;
}

bool FF8DiscArchive::isPAL() const
{
	return PAL;
}

QByteArray FF8DiscArchive::file(const FF8DiscFile &file)
{
	if (!file.isValid())
		return QByteArray();

	if (seekToSector(file.getPos()))
		return readIso(file.getSize());
	else
		return QByteArray();
}

QByteArray FF8DiscArchive::fileLZS(const FF8DiscFile &file, bool strict)
{
	if (!file.isValid())		return QByteArray();

	quint32 lzsSize;

	if (!seekToSector(file.getPos()))	return QByteArray();
	if (readIso((char *)&lzsSize, 4) != 4)	return QByteArray();

	// strict = same size as specified | !strict = same sector count as specified
	if ((strict && file.getSize() != lzsSize+4) || (!strict && (lzsSize + 4)/SECTOR_SIZE_DATA + (int)(lzsSize%SECTOR_SIZE_DATA != 0) != file.getSize()/SECTOR_SIZE_DATA + (int)(file.getSize()%SECTOR_SIZE_DATA != 0)))
		return QByteArray();

	return LZS::decompress(readIso(lzsSize).constData(), lzsSize);
}

QByteArray FF8DiscArchive::fileGZ(const FF8DiscFile &file)
{
	if (!file.isValid())		return QByteArray();

	if (!seekToSector(file.getPos()))	return QByteArray();
	seekIso(posIso() + 8);

	return GZIP::decompress(readIso(file.getSize()-8));
}

bool FF8DiscArchive::extract(const FF8DiscFile &file, const QString &destination)
{
	QByteArray data = this->file(file);
	if (data.isEmpty())		return false;

	QFile ret(destination);
	if (!ret.open(QIODevice::WriteOnly))		return false;

	return ret.write(data) != -1;
}

bool FF8DiscArchive::extractLZS(const FF8DiscFile &file, const QString &destination, bool strict)
{
	QByteArray data = fileLZS(file, strict);
	if (data.isEmpty())		return false;

	QFile ret(destination);
	if (!ret.open(QIODevice::WriteOnly))		return false;

	return ret.write(data) != -1;
}

bool FF8DiscArchive::extractGZ(const FF8DiscFile &file, const QString &destination)
{
	QByteArray data = fileGZ(file);
	if (data.isEmpty())		return false;

	QFile ret(destination);
	if (!ret.open(QIODevice::WriteOnly))		return false;

	return ret.write(data) != -1;
}

const QList<FF8DiscFile> &FF8DiscArchive::rootDirectory()
{
	if (!rootFiles.isEmpty() || !IMGFound())		return rootFiles;
	//searchFiles();

	quint32 position, size, numSectors = sizeIMG / SECTOR_SIZE_DATA;
	qint64 maxPos;

	if (!isDemo()) {
		seekIso(posIMG);
		readIso((char *)&position, 4);
		readIso((char *)&size, 4);
	
		if (position >= numSectors || size == 0 || position + size/SECTOR_SIZE_DATA >= numSectors) {
			seekIso(posIMG + 5 * SECTOR_SIZE_DATA); // PAL hack
			readIso((char *)&position, 4);
			readIso((char *)&size, 4);
			PAL = true;
		}
	} else {
		seekToFile("FF8.EXE");
		if (isJp()) {
			seekIso(posIso() + 0x33510);
		} else {
			seekIso(posIso() + 0x33584);
		}
		readIso((char *)&position, 4);
		readIso((char *)&size, 4);
	}

	if (position >= numSectors || size == 0 || position + size/SECTOR_SIZE_DATA >= numSectors) {
		return rootFiles;
	}

	rootFiles.append(FF8DiscFile(position, size));

	maxPos = posIso() + SECTOR_SIZE_DATA;

	while (posIso() < maxPos) {
		readIso((char *)&position, 4);
		readIso((char *)&size, 4);

		if (position < numSectors && size != 0 && position + size/SECTOR_SIZE_DATA < numSectors)
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

const FF8DiscFile &FF8DiscArchive::sysFntTdwFile()
{
	if (isDemo()) {
		return rootFile(isJp() ? 9 : 8);
	}
	return rootFile(129);
}

const FF8DiscFile &FF8DiscArchive::fieldBinFile()
{
	if (isDemo()) {
		return rootFile(isJp() ? 1 : 0);
	}
	return rootFile(2);
}

const QList<FF8DiscFile> &FF8DiscArchive::fieldDirectory()
{
	if (!fieldFiles.isEmpty() || 2 >= rootCount())		return fieldFiles;

	quint32 position, size, numSectors = sizeIMG / SECTOR_SIZE_DATA;
	const FF8DiscFile &fieldbinFile = fieldBinFile();

//	// nbSectorsLzs != nbSectors
//	if ((lzsSize + 4)/SECTOR_SIZE_DATA + (int)(lzsSize%SECTOR_SIZE_DATA != 0) != fieldbinFile.getSize()/SECTOR_SIZE_DATA + (int)(fieldbinFile.getSize()%SECTOR_SIZE_DATA != 0))
//		return fieldFiles;

	const QByteArray fieldBin = !isDemo() ? fileLZS(fieldbinFile, false) : file(fieldbinFile);
	if (fieldBin.isEmpty())
		return fieldFiles;

	const char *fieldBinData = fieldBin.constData();
	int index, lastIndex, tocSize;
	if (isDemo()) {
		if ((index = fieldBin.indexOf(QByteArray("\x76\xDF\x32\x6F\x34\xA8\xD0\xB8\x63\xC8\xC0\xEC\x4B\xE8\x17\xF8", 16))) != -1) {
			index += 16;// with main field models
		} else {
			qWarning() << "bin invalide : index introuvable !";
			return fieldFiles;
//			index = jap ? 0x28668 : 0x286a0;// with main field models
		}

		if ((lastIndex = fieldBin.indexOf(QByteArray(16, '\0') + QByteArray("\x00\x10\x00\x10", 4), index)) != -1) {
			if ((lastIndex - index) % 8 != 0) {
				qWarning() << "bin invalide : lastIndex invalide !" << index << lastIndex;
				return fieldFiles;
			}
			tocSize = (lastIndex - index) / 8;
		} else {
			qWarning() << "bin invalide : lastIndex introuvable !" << index;
			return fieldFiles;
		}
	} else {
		if ((index = fieldBin.indexOf(QByteArray("\x04\x00\x05\x24\x18\x00\xbf\x8f\x14\x00\xb1\x8f\x10\x00\xb0\x8f\x20\x00\xbd\x27\x08\x00\xe0\x03\x00\x00\x00\x00", 28))) != -1) {
			index += 28;// with main field models
		} else {
			qWarning() << "bin invalide : index introuvable !";
			return fieldFiles;
//			index = jap ? 0x28668 : 0x286a0;// with main field models
		}

		if ((lastIndex = fieldBin.indexOf(QByteArray("\x00\x00\x01\x00\x02\x00\x03\x00", 8), index)) != -1) {
			if ((lastIndex - index) % 8 != 0) {
				qWarning() << "bin invalide : lastIndex invalide !" << index << lastIndex;
				return fieldFiles;
			}
			tocSize = (lastIndex - index) / 8;
		} else {
			qWarning() << "bin invalide : lastIndex introuvable !" << index;
			return fieldFiles;
		}
	}

	for (int i = 0; i < tocSize; ++i) {
		memcpy(&position, fieldBinData + index + i*8, 4);
		memcpy(&size, fieldBinData + index + i*8 + 4, 4);

		if (position < numSectors && size != 0 && position + size/SECTOR_SIZE_DATA < numSectors)
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

void FF8DiscArchive::searchFiles()
{
	if (!IMGFound())		return;

	QList<qint64> sectors;

	quint32 firstSector = posIMG / SECTOR_SIZE_DATA,
	        numSectors = sizeIMG / SECTOR_SIZE_DATA;
	for (qint64 sector = firstSector; sector < numSectors; ++sector) {
		bool emptyBefore = false, filledAfter = false;
		if (sector > 0) {
			if (sector == firstSector) {
				emptyBefore = true;
			} else {
				seekToSector(sector);
				seekIso(posIso() - 64);
				if (readIso(64) == QByteArray(64, '\0')) {
					emptyBefore = true;
				}
			}
		}

		seekToSector(sector);
		if (readIso(64) != QByteArray(64, '\0')) {
			filledAfter = true;
		}

		if (emptyBefore && filledAfter) {
			sectors << sector;
		}
	}

	sectors << numSectors;

	for (qint64 i = 0; i < sectors.size() - 1; ++i) {
		qint64 sector = sectors.at(i),
		       sectorCount = sectors.at(i + 1) - sector;

		seekToSector(sector);
		QByteArray data = readIso(64);
		QString type;
		quint32 fileSize = 0;
		if (data.startsWith("AKAO")) {
			type = "AKAO";
		} else if (data.startsWith("SMN")) {
			type = "SMN -> AKAO ?";
		} else if (data.startsWith("SMJ")) {
			type = "SMJ";
		} else if (data.startsWith("SMR")) {
			type = "SMR";
		} else {
			quint32 firstBytes;
			memcpy(&firstBytes, data.constData(), 4);
			if (firstBytes == 16) {
				type = "TIM";
			} else {
				quint32 estFileSize = firstBytes + 4,
				        fileSectorSize = sectorCount * SECTOR_SIZE_DATA;
				if (estFileSize < fileSectorSize && estFileSize > (sectorCount - 1) * SECTOR_SIZE_DATA) {
					seekToSector(sector + sectorCount - 1);
					QByteArray lastSectorData = readIso(SECTOR_SIZE_DATA);
					int firstData = -1;
					for (int i = lastSectorData.size() - 1; i >= 0; i--) {
						if (lastSectorData.at(i) != '\0') {
							firstData = i + 1;
							break;
						}
					}
					if (firstData >= 0 && quint32(firstData) == estFileSize % SECTOR_SIZE_DATA) {
						fileSize = estFileSize;
						type = "LZS";
					}
				}
			}
		}

		qDebug() << "FF8DiscArchive::searchFiles sector" << sector << "sector count" << sectorCount << type << "file size (0 if undetected)" << fileSize;
	}
}
