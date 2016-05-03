#ifndef BATTLETRIANGLE_H
#define BATTLETRIANGLE_H

#include "game/battle/BattlePoly.h"

class BattleTriangle : public BattlePoly
{
public:
	BattleTriangle() : BattlePoly() {}
	BattleTriangle(const QVector<BattleVertex> &vertices,
	               const QVector<TexCoord> &texCoords,
	               quint16 unknown1,
	               quint16 unknown2,
	               quint8 flags);
};

#endif // BATTLETRIANGLE_H
