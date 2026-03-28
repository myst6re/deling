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
#include "MchFile.h"
#include "files/TimFile.h"

struct ModelHeader {
	quint32 boneCount;
	quint32 verticeCount;
	quint32 textureAnimationCount;
	quint32 faceCount;
	quint32 unknown1Count;
	quint32 skinObjectCount;
	quint32 padding;
	quint16 triangleCount;
	quint16 quadCount;
	quint32 bonesOffset;
	quint32 verticesOffset;
	quint32 textureAnimationsOffset;
	quint32 facesOffset;
	quint32 unknown1sOffset;
	quint32 skinObjectsOffset;
	quint32 animationsOffset;
	quint16 unknown3a; // Can be 0x1180104 or 0x1FF0104 or 0x12E0104
	quint16 unknown3b; // Can be 0x1180104 or 0x1FF0104 or 0x12E0104
};

MchFile::MchFile() :
	File()
{
}

bool MchFile::open(const QByteArray &mch, const QString &name)
{
	_model = CharaModel();

	if (mch.size() < 0x100) {
		if (mch.size() != 33) { // "This is dummy file. Kazuo Suzuki{0a}"
			qWarning() << "MchFile::open mch file too short" << name << mch.size();
		}
		return false;
	}

	quint32 timOffset, modelOffset;
	QList<quint32> toc;
	const char *constData = mch.constData();
	const char * const startData = constData;

	do {
		memcpy(&timOffset, constData, 4);
		constData += 4;

		if (timOffset != 0xFFFFFFFF) {
			toc.append(timOffset);
		}

		//qDebug() << "tim offset" << QString::number(timOffset & 0xFFFFFF, 16) << QString::number((timOffset >> 24) & 0xFF, 16);
	} while (timOffset != 0xFFFFFFFF && constData - startData < 0x100);

	if (timOffset == 0xFFFFFFFF) {
		memcpy(&modelOffset, constData, 4);

		//qDebug() << "model data offset" << QString::number(modelOffset, 16);

		if (!toc.isEmpty()) {
			toc.append(modelOffset);
			toc.append(mch.size());

			QList<TimFile> textures;

			// Toc = tim offsets + data offset + data size

			for (int i = 0; i < toc.size()-2; ++i) {
				quint32 pos = toc.at(i) & 0xFFFFFF;
				//qDebug() << "ouverture tim" << pos << ((toc.at(i+1) & 0xFFFFFF) - pos);
				textures.append(TimFile(mch.mid(pos, (toc.at(i+1) & 0xFFFFFF) - pos)));
				if (!textures.last().isValid()) {
					qWarning() << "CharaModel::open tim error: unknown format!" << name << i;
				}
			}

			if (!readFullModel(startData + modelOffset, mch.size() - modelOffset, name, _model)) {
				return false;
			}
			_model.setTextures(textures);
		}
//		else {
//			qDebug() << "No tim";
//		}
	} else {
		qWarning() << "MchFile::open Unknown format (5)!" << QString::number(timOffset, 16);
		return false;
	}

	//qDebug() << "MchFile ouvert" << name;

	return true;
}

#define FILL_LIST(type, entity) \
	QList<type> entity##s; \
	for (quint64 i = 0; i < modelHeader.entity##Count; ++i) { \
		type e = type(); \
		if (modelHeader.entity##sOffset + sizeof(type) * (i + 1) > size) { \
			qWarning() << "MchFile::readFullModel: File too short" << #entity << i; \
			return false; \
		} \
		memcpy(&e, constData + modelHeader.entity##sOffset + sizeof(type) * i, sizeof(type)); \
		entity##s.append(e); \
	}

bool MchFile::readFullModel(const char *constData, int size, const QString &name, CharaModel &model)
{
	ModelHeader modelHeader = ModelHeader();
	memcpy(&modelHeader, constData, sizeof(ModelHeader));

	qDebug() << name << "bones" << QString::number(modelHeader.bonesOffset, 16) << modelHeader.boneCount
		<< "vertices" << QString::number(modelHeader.verticesOffset, 16) << modelHeader.verticeCount
		<< "textureAnimations" << QString::number(modelHeader.textureAnimationsOffset, 16) << modelHeader.textureAnimationCount
		<< "faces" << QString::number(modelHeader.facesOffset, 16) << modelHeader.faceCount
		<< "unknown1" << QString::number(modelHeader.unknown1sOffset, 16) << modelHeader.unknown1Count
		<< "skinObjects" << QString::number(modelHeader.skinObjectsOffset, 16) << modelHeader.skinObjectCount
		<< "animations" << QString::number(modelHeader.animationsOffset, 16)
		<< "padding" << modelHeader.padding
		<< "triangleCount" << modelHeader.triangleCount
		<< "quadCount" << modelHeader.quadCount
		<< "unknown3" << QString::number(modelHeader.unknown3a, 16) << QString::number(modelHeader.unknown3b, 16);

	FILL_LIST(Bone, bone)
	FILL_LIST(Vertex_sr, vertice)
	FILL_LIST(TextureAnimation, textureAnimation)
	FILL_LIST(Face, face)
	FILL_LIST(ModelUnknown, unknown1)
	FILL_LIST(SkinObject, skinObject)

	/* for (const Face &face: faces) {
		qDebug() << "face" << "polyType" << QString::number(face.polyType, 16) << "unknown4" << QString::number(face.unknown4, 16)
			<< "unknown8" << QString::number(face.unknown8, 16)
			<< "vertexIndexes" << face.vertexIndexes[0] << face.vertexIndexes[1] << face.vertexIndexes[2] << face.vertexIndexes[3]
			<< "normalIndexes" << face.normalIndexes[0] << face.normalIndexes[1] << face.normalIndexes[2] << face.normalIndexes[3]
			<< "texIndex" << face.texIndex;
	} */

	QByteArray unknownData;
	QList<Animation> animations = readAnimations(constData + modelHeader.animationsOffset, size - modelHeader.animationsOffset, &unknownData);

	model = CharaModel(name, bones, vertices, textureAnimations, faces, unknown1s, skinObjects, animations, modelHeader.unknown3a, modelHeader.unknown3b, unknownData);

	return true;
}

QList<Animation> MchFile::readAnimations(const char *constData, int size, QByteArray *unknownData)
{
	QList<Animation> ret;
	const char *endData = constData + size;
	if (constData + 2 > endData) {
		qWarning() << "MchFile::readAnimations" << "size too short (1)" << size;
		return ret;
	}

	quint16 animCount = 0;
	memcpy(&animCount, constData, 2);
	constData += 2;

	//qDebug() << "MchFile::readAnimations" << "animCount" << animCount;

	for (int animId = 0; animId < animCount; ++animId) {
		if (constData + 4 > endData) {
			qWarning() << "MchFile::readAnimations" << "size too short (2)" << size;
			return ret;
		}

		quint16 framesCount = 0, bonesCount = 0;
		memcpy(&framesCount, constData, 2);
		memcpy(&bonesCount, constData + 2, 2);
		constData += 4;
		//qDebug() << "MchFile::readAnimations" << "framesCount" << framesCount << "bonesCount" << bonesCount;

		Animation animation = Animation();

		for (int frame = 0; frame < framesCount; ++frame) {
			if (constData + 6 > endData) {
				qWarning() << "MchFile::readAnimations" << "size too short (3)" << size;
				return ret;
			}

			AnimationKeyPoint animationKeyPoint = AnimationKeyPoint();
			animationKeyPoint.keyPoint = Vertex();
			memcpy(&animationKeyPoint.keyPoint, constData, 6);
			//qDebug() << "MchFile::readAnimations" << "keyPoint" << animationKeyPoint.keyPoint.x << animationKeyPoint.keyPoint.y << animationKeyPoint.keyPoint.z;
			constData += 6;
			for (int bone = 0; bone < bonesCount; ++bone) {
				if (constData + 4 > endData) {
					qWarning() << "MchFile::readAnimations" << "size too short (4)" << size;
					return ret;
				}
				Vertex_sr v = Vertex_sr();
				v.x = (constData[0] << 2) | ((((constData[3] >> 2) * 0) & 3) << 10);
				v.y = (constData[1] << 2) | ((((constData[3] >> 2) * 1) & 3) << 10);
				v.z = (constData[2] << 2) | ((((constData[3] >> 2) * 2) & 3) << 10);
				v.res = constData[3]; // Backup this info for saving
				constData += 4;
				animationKeyPoint.rotations.append(v);
				//qDebug() << "MchFile::readAnimations" << "bone" << bone << v.x << v.y << v.z << v.res;
			}
			animation.keyPoints.append(animationKeyPoint);
		}

		ret.append(animation);
	}

	if (unknownData != nullptr) {
		*unknownData = QByteArray(constData, qsizetype(endData - constData));
	}

	return ret;
}

bool MchFile::save(QByteArray &data) const
{
	QByteArray header;
	qsizetype textureCount = _model.textures().size();
	const qsizetype headerSize = (textureCount + 2) * 4;
	const qsizetype paddedHeaderSize = (headerSize / 256 + int(headerSize % 256 != 0)) * 256;
	data = QByteArray(paddedHeaderSize, '\0');

	for (const TimFile &texture: _model.textures()) {
		QByteArray textureData;
		if (!texture.save(textureData)) {
			return false;
		}
		textureCount -= 1;
		// `textureCount` is skipped by the game, we put it anyway to respect the file format
		// It is more important here to not transform `offset` to a negative value than having a correct `textureCount`
		// so we prevent textureCount to set the high bit of offset
		quint32 offset = quint32(data.size()) | ((textureCount & 0x7) << 28);
		header.append((const char *)&offset, sizeof(offset));
		data.append(textureData);
	}
	header.append("\xFF\xFF\xFF\xFF", 4);
	quint32 offset = data.size();
	header.append((const char *)&offset, sizeof(offset));

	data.replace(0, paddedHeaderSize, header.leftJustified(paddedHeaderSize, '\0', true));
	data.append(writeFullModel(_model));

	return true;
}

#define WRITE_LIST(type, entity) \
	QList<type> entity##s = model.entity##s(); \
	modelHeader.entity##sOffset = data.size(); \
	modelHeader.entity##Count = entity##s.size(); \
	for (quint64 i = 0; i < modelHeader.entity##Count; ++i) { \
		const type &e = entity##s.at(i); \
		data.append((const char *)&e, sizeof(type)); \
	}

QByteArray MchFile::writeFullModel(const CharaModel &model)
{
	ModelHeader modelHeader = ModelHeader();
	QByteArray data(sizeof(modelHeader), '\0'); // Write empty header
 
	WRITE_LIST(Bone, bone)
	WRITE_LIST(Vertex_sr, vertice)
	WRITE_LIST(TextureAnimation, textureAnimation)
	WRITE_LIST(Face, face)
	WRITE_LIST(ModelUnknown, unknown1)
	WRITE_LIST(SkinObject, skinObject)

	modelHeader.animationsOffset = data.size();
	data.append(writeAnimations(model));

	for (const Face &face: model.faces()) {
		if (face.polyType == 0x2D010709) {
			modelHeader.quadCount += 1;
		} else {
			modelHeader.triangleCount += 1;
		}
	}

	modelHeader.unknown3a = model.unknown2();
	modelHeader.unknown3b = model.unknown3();

	// Update header
	data.replace(qsizetype(0), qsizetype(sizeof(modelHeader)), (const char *)&modelHeader, qsizetype(sizeof(modelHeader)));

	return data;
}

QByteArray MchFile::writeAnimations(const CharaModel &model)
{
	QByteArray data;
	quint16 animCount = model.animations().size();

	data.append((const char *)&animCount, 2);

	for (const Animation &animation: model.animations()) {
		quint16 framesCount = quint16(animation.keyPoints.size()),
			bonesCount = quint16(animation.bonesCount());

		data.append((const char *)&framesCount, 2);
		data.append((const char *)&bonesCount, 2);

		for (const AnimationKeyPoint &keyPoint: animation.keyPoints) {
			data.append((const char *)&keyPoint.keyPoint, 6);

			for (const Vertex_sr &rotation: keyPoint.rotations) {
				quint8 first = (rotation.x >> 2) & 0xFF, second = (rotation.y >> 2) & 0xFF, third = (rotation.z >> 2) & 0xFF;

				data.append(char(first));
				data.append(char(second));
				data.append(char(third));
				data.append(char(rotation.res));
			}
		}
	}

	data.append(model.unknownData());

	return data;
}

bool MchFile::hasModel() const
{
	return !_model.name().isEmpty();
}

const CharaModel &MchFile::model() const
{
	return _model;
}
