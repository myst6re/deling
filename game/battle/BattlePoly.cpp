#include "BattlePoly.h"

BattlePoly::BattlePoly() :
    _unknown1(0), _unknown2(0), _flags(0)
{
}

BattlePoly::BattlePoly(const QVector<BattleVertex> &vertices,
                       const QVector<TexCoord> &texCoords,
                       quint16 unknown1,
                       quint16 unknown2,
                       quint8 flags) :
    _vertices(vertices), _texCoords(texCoords),
    _unknown1(unknown1), _unknown2(unknown2),
    _flags(flags)
{
	Q_ASSERT(vertices.size() == texCoords.size());
}

void BattlePoly::setVertices(const QVector<BattleVertex> &vertices,
                             const QVector<TexCoord> &texCoords)
{
	_vertices = vertices;
	_texCoords = texCoords;
}
