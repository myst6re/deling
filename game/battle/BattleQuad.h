#ifndef BATTLEQUAD_H
#define BATTLEQUAD_H

#include "game/battle/BattlePoly.h"

class BattleQuad : BattlePoly
{
public:
	BattleQuad(const QVector<BattleVertex> &vertices,
	           const QVector<TexCoord> &texCoords,
	           quint16 unknown1,
	           quint16 unknown2);
};

#endif // BATTLEQUAD_H
