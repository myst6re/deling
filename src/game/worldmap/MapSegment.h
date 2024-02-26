/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2024 Arzel Jérôme <myst6re@gmail.com>
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#pragma once

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
	inline void setBlock(int id, const MapBlock &block) {
		_blocks[id] = block;
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
