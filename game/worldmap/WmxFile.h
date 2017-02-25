#ifndef WMXFILE_H
#define WMXFILE_H

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
	WmxFile();
	inline void setDevice(QIODevice *device) {
		_io = device;
	}
	bool seekSegment(int segment);
	bool readSegments(Map &map, int segmentCount = -1);
	bool writeSegments(const Map &map);
private:
	bool canReadSegment() const;
	bool readSegment(MapSegment &segment);
	bool writeSegment(const MapSegment &segment);
	bool readSegmentToc(QList<quint16> &toc);
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
	QMap<quint32, int> _collect;
};

#endif // WMXFILE_H
