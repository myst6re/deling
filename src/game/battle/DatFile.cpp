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
#include "DatFile.h"
#include "Vertex.h"

struct SkeletonHeader {
	quint16 count;
	quint16 field4;
	quint16 field6;
	quint16 field8;
	qint16 scaleX, scaleY;
	quint16 scaleZ;
	quint16 padding;
};

struct SkeletonBone {
	quint16 parent;
	qint16 size;
	qint16 field6;
	qint16 field8;
	qint16 fieldA;
	qint16 fieldC;
	qint16 fieldE;
	qint16 field10;
	quint16 unknown[14];
};

DatFile::DatFile(QIODevice *io) : _io(io)
{
	_toc[0] = 0;
}

bool DatFile::openToc()
{
	_io->seek(4);
	if (_io->read((char *)_toc, DAT_FILE_SECTION_COUNT * sizeof(quint32)) != DAT_FILE_SECTION_COUNT * sizeof(quint32)) {
		qWarning() << "DatFile::openToc File too short";
		return false;
	}

	_toc[DAT_FILE_SECTION_COUNT] = _io->size();

	for (int i = 0; i <= DAT_FILE_SECTION_COUNT; ++i) {
		qDebug() << "DatFile::openToc" << i << _toc[i];
	}

	return true;
}

QByteArray DatFile::sectionData(int sectionNumber)
{
	if (_toc[0] == 0 && !openToc()) {
		return QByteArray();
	}

	const quint32 sizeSection = _toc[sectionNumber + 1] - _toc[sectionNumber];
	
	if (!_io->seek(_toc[sectionNumber])) {
		return QByteArray();
	}
	
	return _io->read(sizeSection);
}

bool DatFile::readSkeleton(BattleModelSkeleton &skeleton)
{
	QByteArray data = sectionData(0);
	
	if (data.isEmpty()) {
		return false;
	}
	
	const char *constData = data.constData();
	const SkeletonHeader *header = (const SkeletonHeader *)constData;
	constData += sizeof(SkeletonHeader);
	
	QList<BattleModelBone> bones;
	
	for (int i = 0; i < header->count; ++i) {
		const SkeletonBone *bone = (const SkeletonBone *)constData;
		constData += sizeof(SkeletonBone);

		BattleModelBone b;
		b.parent = bone->parent;
		b.size = bone->size;
		
		bones.append(b);

		qDebug() << "DatFile::readSkeleton" << i << "parent" << b.parent << "size" << b.size;
	}
	
	skeleton.setScale(header->scaleX, header->scaleY, header->scaleZ);
	skeleton.setBones(bones);
	
	return true;
}

bool DatFile::readObjects(QList<BattleModelObject> &objects)
{
	QByteArray data = sectionData(1);
	
	if (data.isEmpty()) {
		return false;
	}
	
	const char *constData = data.constData();
	const quint32 *toc = (const quint32 *)constData;
	
	for (quint32 i = 1; i <= toc[0]; ++i) {
		const quint16 *verticeToc = (const quint16 *)(constData + toc[i]);
		quint16 verticeDataCount = verticeToc[0];
		
		verticeToc += 1;

		for (int j = 0; j < verticeDataCount; ++j) {
			quint16 boneId = verticeToc[0], verticeCount = verticeToc[1];
			//Vertex *vertices = (Vertex *)(verticeToc + 2);
			
			verticeToc += 2 + verticeCount * 3;
			const char *currentPos = (const char *)verticeToc;
			
			if (quint64(currentPos - constData) % 4 != 0) {
				qDebug() << "padding" << currentPos - constData << quint64(currentPos - constData) % 4;
				verticeToc += (4 - quint64(currentPos - constData) % 4) / 2;
			}
			
			quint16 triangleCount = verticeToc[0], quadCount = verticeToc[1];
			// + 8 empty bytes
			
			verticeToc += 6 + 8 * triangleCount + 10 * quadCount;

			qDebug() << "DatFile::readObjects" << i << j << "boneId" << boneId << "verticeCount" << verticeCount << "(verticeDataCount)" << verticeDataCount << "triangleCount" << triangleCount << "quadCount" << quadCount;
		}
	}
	
	return true;
}
