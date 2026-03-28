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
#pragma once

#include <QtCore>
#include <QPixmap>
#include "files/TimFile.h"
#include "files/IdFile.h"
#include "Vertex.h"

struct Bone // sizeof = 64
{
	quint16 parent;
	quint16 u2;
	quint32 u4;
	quint16 size;
	quint8 uA[54];
};

struct TextureAnimation
{
	quint8 unknown;
};

struct Face // sizeof = 64
{
	quint32 polyType; // Can be either 0x25010607 (triangle) or 0x2D010709 (rect)
	quint32 unknown4; // unused can be 0x44 or 0x54
	quint16 unknown8;
	quint16 paddingA;
	quint16 vertexIndexes[4];
	quint16 normalIndexes[4];
	quint32 color[4];
	TexCoord texCoord[4];
	quint16 padding;
	quint16 texIndex;
	quint64 padding2;
};

struct ModelUnknown // sizeof = 32
{
	quint32 u0[8];
};

struct SkinObject
{
	quint16 vertexIndex;
	quint16 vertexCount;
	quint16 boneId;
	quint16 u6;
};

struct AnimationKeyPoint
{
	Vertex keyPoint;
	QList<Vertex_sr> rotations;
};

struct Animation
{
	QList<AnimationKeyPoint> keyPoints;
	inline qsizetype bonesCount() const {
		return keyPoints.isEmpty() ? 0 : keyPoints.at(0).rotations.size();
	}
};

class CharaModel
{
public:
	CharaModel();
	CharaModel(const QString &name, quint8 scale, const QList<Animation> &animations = QList<Animation>());
	CharaModel(
		const QString &name,
		const QList<Bone> &bones,
		const QList<Vertex_sr> &vertices,
		const QList<TextureAnimation> &textureAnimations,
		const QList<Face> &faces,
		const QList<ModelUnknown> &unknown1s,
		const QList<SkinObject> &skinObjects,
		const QList<Animation> &animations,
		quint16 unknown2, quint16 unknown3,
		const QByteArray &unknownData
	);
	bool open(const QList<quint32> &toc, const QByteArray &data);
	bool isEmpty2() const;
	inline bool isExternal() const {
		return _isExternal;
	}
	QString name() const;
	int id() const;
	const TimFile &texture(int id) const;
	int textureCount() const;
	void setTextures(const QList<TimFile> &textures);
	inline const QList<TimFile> &textures() const {
		return _textures;
	}
	inline const QList<Bone> &bones() const {
		return _bones;
	}
	inline const QList<Vertex_sr> &vertices() const {
		return _vertices;
	}
	inline const QList<TextureAnimation> &textureAnimations() const {
		return _textureAnimations;
	}
	inline const QList<Face> &faces() const {
		return _faces;
	}
	inline const QList<ModelUnknown> &unknown1s() const {
		return _unknown1s;
	}
	inline const QList<SkinObject> &skinObjects() const {
		return _skinObjects;
	}
	inline const QList<Animation> &animations() const {
		return _animations;
	}
	inline void setAnimations(const QList<Animation> &animations) {
		_animations = animations;
	}
	inline quint16 unknown2() const {
		return _unknown2;
	}
	inline quint16 unknown3() const {
		return _unknown3;
	}
	inline const QByteArray &unknownData() const {
		return _unknownData;
	}
	inline quint8 scale() const {
		return _scale;
	}
	inline quint32 unknownFlags() const {
		return _unknownFlags;
	}
	inline void setUnknownFlags(quint32 unknownFlags) {
		_unknownFlags = unknownFlags;
	}
	inline quint32 externalTextureModelLoaderId() const {
		return _externalTextureModelLoaderId;
	}
	inline void setExternalTextureModelLoaderId(quint32 externalTextureModelLoaderId) {
		_externalTextureModelLoaderId = externalTextureModelLoaderId;
	}
	inline quint32 noTextureFlag() const {
		return _noTextureFlag;
	}
	inline void setNoTextureFlag(quint32 noTextureFlag) {
		_noTextureFlag = noTextureFlag;
	}
private:
	QString _name;
	QList<Bone> _bones;
	QList<Vertex_sr> _vertices;
	QList<TextureAnimation> _textureAnimations;
	QList<Face> _faces;
	QList<ModelUnknown> _unknown1s;
	QList<SkinObject> _skinObjects;
	QList<Animation> _animations;
	QList<TimFile> _textures;
	quint16 _unknown2, _unknown3;
	QByteArray _unknownData;
	quint8 _scale;
	quint32 _unknownFlags;
	bool _isExternal;
	quint32 _externalTextureModelLoaderId;
	quint32 _noTextureFlag;
};
