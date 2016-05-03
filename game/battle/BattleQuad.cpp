#include "BattleQuad.h"

BattleQuad::BattleQuad(const QVector<BattleVertex> &vertices,
                       const QVector<TexCoord> &texCoords,
                       quint16 unknown1, quint16 unknown2,
                       quint8 flags) :
    BattlePoly(vertices, texCoords, unknown1, unknown2, flags)
{
	Q_ASSERT(vertices.size() == 4);
}
