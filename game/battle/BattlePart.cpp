#include "BattlePart.h"

BattlePart::BattlePart(const QMap<int, QVector<BattleVertex> > &vertices,
                       const QList<BattleTriangle> &triangles,
                       const QList<BattleQuad> &quads) :
    _vertices(vertices), _triangles(triangles), _quads(quads)
{
}
