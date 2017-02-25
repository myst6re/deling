#ifndef MAPBLOCK_H
#define MAPBLOCK_H

#include <QtCore>
#include "game/worldmap/MapPoly.h"

class MapBlock
{
public:
	MapBlock();
	inline const QList<MapPoly> &polygons() const {
		return _polygons;
	}
	inline void setPolygons(const QList<MapPoly> &polygons) {
		_polygons = polygons;
	}
private:
	QList<MapPoly> _polygons;
};

#endif // MAPBLOCK_H
