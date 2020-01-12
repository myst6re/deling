#ifndef MAP_H
#define MAP_H

#include <QtCore>
#include "game/worldmap/MapSegment.h"
#include "game/worldmap/WmEncounter.h"

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

	inline const QList<WmEncounter> &encounters() const {
		return _encounters;
	}

	QList<WmEncounter> encounters(quint8 region) const;

	inline void setEncounters(const QList<WmEncounter> &encounters) {
		_encounters = encounters;
	}

	inline const QList<quint8> &encounterRegions() const {
		return _encounterRegions;
	}

	inline void setEncounterRegions(const QList<quint8> &regions) {
		_encounterRegions = regions;
	}


private:
	QList<MapSegment> _segments;
	QList<WmEncounter> _encounters;
	QList<quint8> _encounterRegions;
};

#endif // MAP_H
