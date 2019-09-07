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

	inline quint32 groupId() const {
		return _groupId;
	}
	inline void setGroupId(quint32 groupId) {
		_groupId = groupId;
	}

private:
	QList<MapBlock> _blocks;
	quint32 _groupId;
};

#endif // MAPSEGMENT_H
