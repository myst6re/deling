#include "DatFile.h"

DatFile::DatFile(QIODevice *device) :
    IO(device)
{
}

bool DatFile::readModel(BattleModel &model)
{
	if(!readPositionsIfNotFilled()) {
		return false;
	}

	BattleSkeleton skeleton;
	QList<BattlePart> parts;
	QList<BattleAnimation> animations;
	QList<TimFile> textures;

	if(!readSkeleton(skeleton)
	        || !readParts(parts)
	        || !readAnimations(animations)
	        || !readTextures(textures)) {
		return false;
	}

	model.setSkeleton(skeleton);
	model.setParts(parts);
	model.setAnimations(animations);
	model.setTextures(textures);

	return true;
}

bool DatFile::readSkeleton(BattleSkeleton &skeleton)
{
	if(!readPositionsIfNotFilled()) {
		return false;
	}

	if(!device()->seek(position(Skeleton))) {
		return false;
	}

	QByteArray header = device()->read(16);
	if(header.size() != 16) {
		return false;
	}

	quint16 count;
	memcpy(&count, header.constData(), 2);

	// Validation
	if(size(Skeleton) != 16 + count * 48) {
		qWarning() << "DatFile::readSkeleton size of section must be equal to"
		           << (16 + count * 48) << size(Skeleton);
		return false;
	}

	skeleton.setUnknownHeader(header.mid(2));

	skeleton.clear();
	for(int i = 0 ; i < count ; ++i) {
		QByteArray bone = device()->read(48);
		if(bone.size() != 48) {
			return false;
		}
		const char *boneData = bone.constData();

		qint16 parent, size;

		memcpy(&parent, boneData, 2);
		memcpy(&size, boneData + 2, 2);

		skeleton.append(BattleBone(parent, size, bone.mid(4)));
	}

	return true;
}

bool DatFile::readParts(QList<BattlePart> &parts)
{
	Q_ASSERT(sizeof(BattleVertex) == 6);
	Q_ASSERT(sizeof(DatFileTriangle) == 16);
	Q_ASSERT(sizeof(DatFileQuad) == 20);

	if(!readPositionsIfNotFilled()) {
		return false;
	}

	if(!device()->seek(position(Parts))) {
		return false;
	}

	quint32 count;
	if(!device()->read((char *)&count, 4)) {
		return false;
	}

	const quint32 headerSize = count * 4;
	QVector<quint32> positions;
	positions.resize(count + 1);

	if(device()->read((char *)positions.data(), headerSize) != headerSize) {
		return false;
	}
	positions[count] = size(Parts);

	parts.clear();
	for(quint64 i = 0 ; i < count ; ++i) {
		// Validation: Asc ordered
		if(positions[i] > positions[i + 1]) {
			qWarning() << "DatFile::readParts invalid position order"
			           << i << positions[i] << positions[i + 1];
			return false;
		}

		if(device()->pos() != position(Parts) + positions[i]) {
			qWarning() << "DatFile::readParts wrong position"
			           << i
			           << (device()->pos() - position(Parts)) << positions[i];
			if(!device()->seek(position(Parts) + positions[i])) {
				return false;
			}
		}

		quint16 countVerticeList;
		if(!device()->read((char *)&countVerticeList, 2)) {
			return false;
		}

		QMap<int, QVector<BattleVertex> > verticesByBone;
		QList<BattleVertex> vertices;
		for(quint32 j = 0 ; j < countVerticeList ; ++j) {

			QByteArray verticesHeader = device()->read(4);
			if(verticesHeader.size() != 4) {
				return false;
			}

			quint16 boneId, countVertices;
			memcpy(&boneId, verticesHeader.constData(), 2);
			memcpy(&countVertices, verticesHeader.constData() + 2, 2);

			QVector<BattleVertex> vertexList;
			vertexList.resize(countVertices);
			if(device()->read((char *)vertexList.data(),
			                  sizeof(BattleVertex) * countVertices)
			        != sizeof(BattleVertex) * countVertices) {
				return false;
			}

			verticesByBone.insert(boneId, vertexList);
			vertices.append(vertexList.toList());
		}

		// 4-bytes alignment
		if(device()->pos() % 4 != 0) {
			if(!device()->seek(device()->pos() + 4 - (device()->pos() % 4))) {
				return false;
			}
		}

		QByteArray polyHeader = device()->read(12);
		if(polyHeader.size() != 12) {
			return false;
		}

		if(polyHeader.right(8) != QByteArray(8, '\0')) {
			qWarning() << "DatFile::readParts polyHeader filled with data"
			           << polyHeader.toHex();
		}

		quint16 countTriangles, countQuads;
		memcpy(&countTriangles, polyHeader.constData(), 2);
		memcpy(&countQuads, polyHeader.constData() + 2, 2);

		QVector<DatFileTriangle> triangleList;
		triangleList.resize(countTriangles);
		if(device()->read((char *)triangleList.data(),
		                  sizeof(DatFileTriangle) * countTriangles)
		        != sizeof(DatFileTriangle) * countTriangles) {
			return false;
		}

		QList<BattleTriangle> triangleList2;
		foreach(const DatFileTriangle &tri, triangleList) {
			BattleTriangle bt = toBattleTriangle(tri, vertices);
			if(bt.isEmpty()) {
				qWarning() << "DatFile::readParts wrong battle triangle"
				           << tri.vertexIndexes[0]
				           << tri.vertexIndexes[1]
				           << tri.vertexIndexes[2]
				           << vertices.size();
			} else {
				triangleList2.append(bt);
			}
		}

		QVector<DatFileQuad> quadList;
		quadList.resize(countQuads);
		if(device()->read((char *)quadList.data(),
		                  sizeof(DatFileQuad) * countQuads)
		        != sizeof(DatFileQuad) * countQuads) {
			return false;
		}

		QList<BattleQuad> quadList2;
		foreach(const DatFileQuad &quad, quadList) {
			BattleQuad bq = toBattleQuad(quad, vertices);
			if(bq.isEmpty()) {
				qWarning() << "DatFile::readParts wrong battle quad"
				           << quad.vertexIndexes[0]
				           << quad.vertexIndexes[1]
				           << quad.vertexIndexes[2]
				           << quad.vertexIndexes[3]
				           << vertices.size();
			} else {
				quadList2.append(bq);
			}
		}

		parts.append(BattlePart(verticesByBone, triangleList2, quadList2));
	}

	quint32 vertexCountTotal;
	if(device()->read((char *)&vertexCountTotal, 4) != 4) {
		return false;
	}
	qDebug() << "vertexCount" << vertexCountTotal;

	if(device()->pos() != position(Parts) + positions[count]) {
		qWarning() << "DatFile::readParts wrong position (2)"
		           << (device()->pos() - position(Parts)) << positions[count];
	}

	return true;
}

bool DatFile::readAnimations(QList<BattleAnimation> &animations)
{
	if(!readPositionsIfNotFilled()) {
		return false;
	}
	
	if(!device()->seek(position(Animations))) {
		return false;
	}

	Q_UNUSED(animations);
	// TODO
	return true;
}

bool DatFile::readTextures(QList<TimFile> &textures)
{
	if(!readPositionsIfNotFilled()) {
		return false;
	}

	if(!device()->seek(position(Textures))) {
		return false;
	}

	quint32 count;
	if(!device()->read((char *)&count, 4)) {
		return false;
	}

	const quint32 headerSize = (count + 1) * 4;
	QVector<quint32> positions;
	positions.resize(count + 1);

	if(device()->read((char *)positions.data(), headerSize) != headerSize) {
		return false;
	}

	if(positions[count] != size(Textures)) {
		qWarning() << "DatFile::readTextures invalid end of file indicator"
		           << positions[count];
		positions[count] = size(Textures);
	}

	textures.clear();
	for(quint64 i = 0 ; i < count ; ++i) {
		if(positions[i] > positions[i + 1]) {
			qWarning() << "DatFile::readTextures invalid position order"
			           << i << positions[i] << positions[i + 1];
			return false;
		}

		if(device()->pos() != position(Textures) + positions[i]) {
			qWarning() << "DatFile::readTextures wrong position"
			           << i
			           << (device()->pos() - position(Textures)) << positions[i];
			if(!device()->seek(position(Textures) + positions[i])) {
				return false;
			}
		}

		TimFile tim;
		if(!tim.open(device()->read(positions[i + 1] - positions[i]))) {
			return false;
		}
		textures.append(tim);
	}

	return true;
}

bool DatFile::readPositions()
{
	if(!canRead()) {
		return false;
	}
	if(!device()->reset()) {
		return false;
	}
	QByteArray header = device()->read(48);
	if(header.size() < 48) {
		qWarning() << "DatFile::readPositions file too short";
		return false;
	}
	const char *data = header.constData();
	quint32 count;

	memcpy(&count, data, 4);

	// Validation
	if(count != DAT_FILE_SECTION_COUNT) {
		qWarning() << "DatFile::readPositions invalid section count" << count;
		return false;
	}

	_positions.resize(DAT_FILE_SECTION_COUNT + 1);
	memcpy(_positions.data(), data + 4, DAT_FILE_SECTION_COUNT * 4);
	_positions[DAT_FILE_SECTION_COUNT] = device()->size();

	// Validation
	for(int i = 0 ; i < DAT_FILE_SECTION_COUNT ; ++i) {
		// Asc ordered
		if(_positions[i] > _positions[i + 1]) {
			qWarning() << "DatFile::readPositions invalid position order"
			           << i << _positions[i] << _positions[i + 1];
			return false;
		}
	}

	return true;
}

BattleTriangle toBattleTriangle(const DatFileTriangle &triangle,
                                const QList<BattleVertex> &vertices)
{
	QVector<BattleVertex> retVertices;
	QVector<TexCoord> retTexCoords;
	retVertices.resize(3);
	retTexCoords.resize(3);

	if((triangle.vertexIndexes[0] & 0x0FFF) >= vertices.size()
	        || triangle.vertexIndexes[1] >= vertices.size()
	        || triangle.vertexIndexes[2] >= vertices.size()) {
		return BattleTriangle();
	}

	retVertices.append(vertices.at(triangle.vertexIndexes[0] & 0x0FFF));
	retVertices.append(vertices.at(triangle.vertexIndexes[1]));
	retVertices.append(vertices.at(triangle.vertexIndexes[2]));

	retTexCoords.append(triangle.texCoords1);
	retTexCoords.append(triangle.texCoords2);
	retTexCoords.append(triangle.texCoords3);

	return BattleTriangle(retVertices, retTexCoords,
	                      triangle.unknown1, triangle.unknown2,
	                      triangle.vertexIndexes[0] >> 12);
}

BattleQuad toBattleQuad(const DatFileQuad &quad,
                        const QList<BattleVertex> &vertices)
{
	QVector<BattleVertex> retVertices;
	QVector<TexCoord> retTexCoords;
	retVertices.resize(4);
	retTexCoords.resize(4);

	if((quad.vertexIndexes[0] & 0x0FFF) >= vertices.size()
	        || quad.vertexIndexes[1] >= vertices.size()
	        || quad.vertexIndexes[2] >= vertices.size()
	        || quad.vertexIndexes[3] >= vertices.size()) {
		return BattleQuad();
	}

	retVertices.append(vertices.at(quad.vertexIndexes[0] & 0x0FFF));
	retVertices.append(vertices.at(quad.vertexIndexes[1]));
	retVertices.append(vertices.at(quad.vertexIndexes[2]));
	retVertices.append(vertices.at(quad.vertexIndexes[3]));

	retTexCoords.append(quad.texCoords1);
	retTexCoords.append(quad.texCoords2);
	retTexCoords.append(quad.texCoords3);
	retTexCoords.append(quad.texCoords4);

	return BattleQuad(retVertices, retTexCoords,
	                  quad.unknown1, quad.unknown2,
	                  quad.vertexIndexes[0] >> 12);
}
