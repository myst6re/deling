#ifndef BATTLEPART_H
#define BATTLEPART_H

#include <QMap>
#include "game/battle/BattleTriangle.h"
#include "game/battle/BattleQuad.h"

class BattlePart
{
public:
	BattlePart(const QMap<int, QVector<BattleVertex> > &vertices,
	           const QList<BattleTriangle> &triangles,
	           const QList<BattleQuad> &quads);
private:
	QMap<int, QVector<BattleVertex> > _vertices;
	QList<BattleTriangle> _triangles;
	QList<BattleQuad> _quads;
};

#endif // BATTLEPART_H
