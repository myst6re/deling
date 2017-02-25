#ifndef MAPSEGMENT_H
#define MAPSEGMENT_H

#include "game/worldmap/MapBlock.h"

class MapSegment
{
public:
	MapSegment();

	inline const QList<MapBlock> &blocks() const {
		return _blocks;
	}
	inline void setBlocks(const QList<MapBlock> &blocks) {
		_blocks = blocks;
	}

	inline quint32 u1() const {
		return _u1;
	}
	inline void setU1(quint32 u1) {
		_u1 = u1;
	}

private:
	QList<MapBlock> _blocks;
	quint32 _u1;
};

#endif // MAPSEGMENT_H
