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

#include <QtCore>
#include "game/worldmap/Map.h"

#define WMXFILE_BLOCK_COUNT 16
#define WMXFILE_SEGMENT_SIZE 0x9000

struct MapBlockHeader;
struct MapBlockPolygon;
struct MapBlockVertex;

class WmxFile
{
public:
	explicit WmxFile(QIODevice *io = Q_NULLPTR);
	inline void setDevice(QIODevice *device) {
		_io = device;
	}
	bool readSegments(Map &map, int segmentCount = -1);
	bool writeSegments(const Map &map);
private:
	bool seekSegment(int segment);
	bool canReadSegment() const;
	bool readSegment(MapSegment &segment);
	bool writeSegment(const MapSegment &segment);
	bool readSegmentToc(QList<quint32> &toc);
	bool writeSegmentToc(const QByteArray &toc);
	bool readBlock(MapBlock &block);
	bool writeBlock(const MapBlock &block);
	bool readBlockHeader(MapBlockHeader &header);
	bool writeBlockHeader(const MapBlockHeader &header);
	bool readPolygon(MapBlockPolygon &polygon);
	bool writePolygon(const MapBlockPolygon &polygon);
	bool readVertex(MapBlockVertex &vertex);
	bool writeVertex(const MapBlockVertex &polygon);

	QIODevice *_io;
	//QMap<quint32, int> _collect;
};
