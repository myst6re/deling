#ifndef MAP_H
#define MAP_H

#include <QtCore>
#include "game/worldmap/MapSegment.h"

class Map
{
public:
	Map();

	inline const QList<MapSegment> &segments() const {
		return _segments;
	}
	inline void setSegments(const QList<MapSegment> &segments) {
		_segments = segments;
	}

private:
	QList<MapSegment> _segments;

};

#endif // MAP_H
