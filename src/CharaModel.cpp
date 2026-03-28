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
#include "CharaModel.h"

CharaModel::CharaModel() :
    _unknown2(0), _unknown3(0), _scale(16), _unknownFlags(0), _isExternal(false),
	_externalTextureModelLoaderId(0xEEEEEEEE), _noTextureFlag(0)
{
}

CharaModel::CharaModel(const QString &name, quint8 scale, const QList<Animation> &animations) :
    _name(name), _animations(animations), _unknown2(0), _unknown3(0), _scale(scale),
    _unknownFlags(0), _isExternal(true), _externalTextureModelLoaderId(0xEEEEEEEE),
    _noTextureFlag(0)
{
}

CharaModel::CharaModel(
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
) :
    _name(name), _bones(bones), _vertices(vertices), _textureAnimations(textureAnimations),
    _faces(faces), _unknown1s(unknown1s), _skinObjects(skinObjects), _animations(animations),
    _unknown2(unknown2), _unknown3(unknown3), _unknownData(unknownData), _scale(16), _unknownFlags(0x00808080),
    _isExternal(false), _externalTextureModelLoaderId(0xEEEEEEEE), _noTextureFlag(0)
{
}

bool CharaModel::isEmpty2() const
{
	return _textures.isEmpty();
}

QString CharaModel::name() const
{
	return _name.left(4);
}

int CharaModel::id() const
{
	bool ok;
	int id = _name.mid(1, 3).toInt(&ok);
	return ok ? id : -1;
}

const TimFile &CharaModel::texture(int id) const
{
	return _textures.at(id);
}

int CharaModel::textureCount() const
{
	return _textures.size();
}

void CharaModel::setTextures(const QList<TimFile> &textures)
{
	_textures = textures;
}
