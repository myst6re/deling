#include "BattleTriangle.h"

BattleTriangle::BattleTriangle(const QVector<BattleVertex> &vertices,
                               const QVector<TexCoord> &texCoords,
                               quint16 unknown1, quint16 unknown2) :
    BattlePoly(vertices, texCoords, unknown1, unknown2)
{
	Q_ASSERT(vertices.size() == 3);
}
