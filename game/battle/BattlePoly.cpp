#include "BattlePoly.h"

BattlePoly::BattlePoly(const QVector<BattleVertex> &vertices,
                       const QVector<TexCoord> &texCoords,
                       quint16 unknown1,
                       quint16 unknown2) :
    _vertices(vertices), _texCoords(texCoords),
    _unknown1(unknown1), _unknown2(unknown2)
{
	Q_ASSERT(vertices.size() == texCoords.size());
}

void BattlePoly::setVertices(const QVector<BattleVertex> &vertices,
                             const QVector<TexCoord> &texCoords)
{
	_vertices = vertices;
	_texCoords = texCoords;
}
