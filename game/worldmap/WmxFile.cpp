#include "WmxFile.h"

struct MapBlockHeader
{
	quint8 polyCount, vertexCount, normalCount, padding;
};

struct MapBlockPolygon
{
	quint8 vi[3];
	quint8 ni[3];
	TexCoord pos[3];
	quint8 texi, groundType, u1, u2;
	/* texi:
	 *  unknown (5-bits) | clut id (3-bits)
	 * Ground type:
	 *  - 0 : galbadia forests
	 *  - 1 : trabia forests
	 *  - 2 : esthar forests
	 *  - 3 : centra forests
	 *  - 4 : balamb forests
	 *  - 5 : trabia forests
	 *  - 6 : normal grass galbadia, balamb
	 *  - 7 : normal ground galbadia
	 *  - 8 : normal desert galbadia
	 *  - 9 : normal ice trabia
	 *  - 10 : water steppable (waves)
	 *  - 12 : esthar city roads
	 *  - 14 : plateau
	 *  - 15 : ground to mountain galbadia
	 *  - 16 : ground to mountain galbadia, esthar, centra
	 *  - 17 : ground to mountain trabia (1)
	 *  - 18 : ground to mountain trabia (2)
	 *  - 23 : ground to mountain trabia (3)
	 *  - 24 : grass to mountain balamb
	 *  - 25 : some borders in Esthar, ground to mountain (?)
	 *  - 27 : railroads
	 *  - 28 : roads + FH + some towns
	 *  - 29 : inaccessible locations
	 *  - 31 : lakes
	 *  - 32 : water coasts
	 *  - 33 : water less close to coasts
	 *  - 34 : water
	 * u1:
	 *  on walking area (1-bit) | water tex affected + roads (1-bit)
	 * | road/railroad tex affected (except esthar roads) + esthar fence (1-bit)
	 * | lakes + esthar fence + esthar city roads (1-bit) | enter city (1-bit)
	 * | unused (1-bit)
	 * | trabia frozen lakes (below the surface), tunnels (2-bits)
	 * u2:
	 *  movability (1-bit) | walking area, except forests (1-bit)
	 * | all, except mountains and rocks (?) (1-bit)
	 * | most of continents, except northest galbadia peninsula (1-bit)
	 * | unused (1-bit) | unknown (1-bit)
	 * | unknown (1-bit) | on walking area, except half-galbadia and balamb (1-bit)
	 */
};

struct MapBlockVertex
{
	qint16 x, y, z, padding;
};

WmxFile::WmxFile(QIODevice *io) :
    _io(io)
{
}

bool WmxFile::seekSegment(int segment)
{
	return _io->seek(segment * WMXFILE_SEGMENT_SIZE);
}

bool WmxFile::readSegments(Map &map, int segmentCount)
{
	//_collect.clear();

	QList<MapSegment> segments;
	bool toTheEnd = segmentCount < 0;
	qint64 end = toTheEnd ? 0 : segmentCount * WMXFILE_SEGMENT_SIZE;

	while (canReadSegment() && (toTheEnd || _io->pos() < end)) {
		MapSegment segment;
		if (!readSegment(segment)) {
			return false;
		}
		segments.append(segment);
	}

	map.setSegments(segments);

	/* QList<int> collect = _collect.values();
	qSort(collect);
	qDebug() << _collect << collect.last(); */

	return true;
}

bool WmxFile::writeSegments(const Map &map)
{
	foreach (const MapSegment &segment, map.segments()) {
		if (!writeSegment(segment)) {
			return false;
		}
	};

	return true;
}

bool WmxFile::canReadSegment() const
{
	return _io->size() - _io->pos() >= WMXFILE_SEGMENT_SIZE;
}

bool WmxFile::readSegment(MapSegment &segment)
{
	QList<quint16> toc;
	qint64 initialPos = _io->pos();
	quint32 groupId;

	if (_io->read((char *)&groupId, sizeof(quint32)) != sizeof(quint32)) {
		return false;
	}

	if (!readSegmentToc(toc)) {
		return false;
	}

	QList<MapBlock> blocks;

	foreach (quint16 pos, toc) {
		/* // Check data between blocks
			if (pos != _io->pos() - initialPos) {
			if (pos > _io->pos() - initialPos) {
				QByteArray padd = _io->read(pos - (_io->pos() - initialPos));
				if (padd != QByteArray(padd.size(), '\0')) {
					qWarning() << "WmxFile::readSegment padding with bytes"
							   << padd.toHex();
				}
			} else {
				qWarning() << "WmxFile::readSegment pos not in toc"
						   << pos << (_io->pos() - initialPos);
				_io->seek(initialPos + pos);
			}
		} */
		if (!_io->seek(initialPos + pos)) {
			qWarning() << "WmxFile::readSegment cannot seek";
			return false;
		}

		MapBlock block;

		if (!readBlock(block)) {
			return false;
		}

		blocks.append(block);
	}

	/* qDebug() << "WmxFile::readSegment bytes remaining"
			 << (WMXFILE_SEGMENT_SIZE - (_io->pos() % WMXFILE_SEGMENT_SIZE)); */

	if (!_io->seek(initialPos + WMXFILE_SEGMENT_SIZE)) {
		qWarning() << "WmxFile::readSegment cannot seek";
		return false;
	}

	segment.setBlocks(blocks);
	segment.setGroupId(groupId);

	return true;
}

bool WmxFile::writeSegment(const MapSegment &segment)
{
	qint64 initialPos = _io->pos();
	quint32 groupId = segment.groupId();

	if (_io->write((char *)&groupId, sizeof(quint32)) != sizeof(quint32)) {
		return false;
	}

	// Empty toc
	if (!writeSegmentToc(QByteArray(WMXFILE_BLOCK_COUNT * sizeof(quint32), '\0'))) {
		return false;
	}

	if (segment.blocks().size() != WMXFILE_BLOCK_COUNT) {
		qWarning() << "WmxFile::writeSegment wrong blocks size"
		           << segment.blocks().size();
		Q_ASSERT(false);
		return false;
	}

	QByteArray toc;

	foreach (const MapBlock &block, segment.blocks()) {
		quint32 pos = _io->pos() - initialPos;

		if (!writeBlock(block)) {
			return false;
		}

		toc.append((char *)&pos, sizeof(quint32));
	}

	qint64 oldPos = _io->pos();

	if (!_io->seek(initialPos + sizeof(quint32))) {
		qWarning() << "WmxFile::writeSegment cannot seek 1";
		return false;
	}

	if (!writeSegmentToc(toc)) {
		return false;
	}

	if (!_io->seek(oldPos)) {
		qWarning() << "WmxFile::writeSegment cannot seek 2";
		return false;
	}

	qint64 segmentSize = _io->pos() - initialPos;
	if (segmentSize < WMXFILE_SEGMENT_SIZE
	        && !_io->write(QByteArray(WMXFILE_SEGMENT_SIZE - segmentSize, '\0'))) {
		qWarning() << "WmxFile::writeSegment cannot write segment padding";
		return false;
	} else if (segmentSize > WMXFILE_SEGMENT_SIZE) {
		qWarning() << "WmxFile::writeSegment segment too big" << segmentSize;
		return false;
	}

	return true;
}

bool WmxFile::readSegmentToc(QList<quint16> &toc)
{
	for (quint8 i = 0; i < WMXFILE_BLOCK_COUNT; ++i) {
		quint32 pos;

		if (_io->read((char *)&pos, sizeof(quint32)) != sizeof(quint32)) {
			return false;
		}

		toc.append(pos);
	}

	return true;
}

bool WmxFile::writeSegmentToc(const QByteArray &toc)
{
	if (toc.size() != WMXFILE_BLOCK_COUNT * sizeof(quint32)) {
		qWarning() << "WmxFile::writeSegmentToc wrong toc size"
		           << toc.size();
		Q_ASSERT(false);
		return false;
	}
	if (_io->write(toc) != toc.size()) {
		return false;
	}

	return true;
}

static bool appendVertex(int index,
                         const QList<Vertex> &vertices,
                         QList<Vertex> &pVertices)
{
	if (index > vertices.size()) {
		qWarning() << "WmxFile::appendVertex bad vertice index" << index;
		return false;
	}
	pVertices.append(vertices.at(index));
	return true;
}

bool WmxFile::readBlock(MapBlock &block)
{
	MapBlockHeader header;

	if (!readBlockHeader(header)) {
		return false;
	}

	QList<MapBlockPolygon> polys;
	for (quint16 i = 0; i < header.polyCount; ++i) {
		MapBlockPolygon poly;
		if (!readPolygon(poly)) {
			return false;
		}
		polys.append(poly);
	}

	QList<Vertex> vertices;
	for (quint16 i = 0; i < header.vertexCount; ++i) {
		MapBlockVertex vertex;
		if (!readVertex(vertex)) {
			return false;
		}
		Vertex v;
		v.x = vertex.x;
		v.y = vertex.y;
		v.z = vertex.z;
		vertices.append(v);
	}

	QList<Vertex> normals;
	for (quint16 i = 0; i < header.normalCount; ++i) {
		MapBlockVertex normal;
		if (!readVertex(normal)) {
			return false;
		}
		Vertex n;
		n.x = normal.x;
		n.y = normal.y;
		n.z = normal.z;
		normals.append(n);
	}

	// QSet<quint32> coll;

	QList<MapPoly> polygons;
	foreach (const MapBlockPolygon &poly, polys) {
		QList<Vertex> pVertices, pNormals;
		if (!appendVertex(poly.vi[0], vertices, pVertices)) {
			return false;
		}
		if (!appendVertex(poly.vi[1], vertices, pVertices)) {
			return false;
		}
		if (!appendVertex(poly.vi[2], vertices, pVertices)) {
			return false;
		}
		if (!appendVertex(poly.ni[0], normals, pNormals)) {
			return false;
		}
		if (!appendVertex(poly.ni[1], normals, pNormals)) {
			return false;
		}
		if (!appendVertex(poly.ni[2], normals, pNormals)) {
			return false;
		}

		QList<TexCoord> texCoords;
		texCoords.append(poly.pos[0]);
		texCoords.append(poly.pos[1]);
		texCoords.append(poly.pos[2]);

		// _collect.insert(poly.groundType, 0);

		/* if(poly.groundType < 10) {
			_collect.insert(poly.texi, _collect.value(poly.texi) + 1);
			coll.insert(poly.texi);
		} */

		polygons.append(MapPoly(pVertices, pNormals, texCoords,
		                        poly.texi >> 4, poly.texi & 0xF,
		                        poly.groundType,
		                        poly.u1, poly.u2));
	}

	/* foreach(quint32 texi, coll) {
		qDebug() << QString("%1").arg(texi, 8, 2, QChar('0'));
	} */

	block.setPolygons(polygons);

	return true;
}

static int storeVertex(const Vertex &vertex, QHash<Vertex, int> &hashed,
                       QList<MapBlockVertex> &ordered, int &cur)
{
	if (hashed.contains(vertex)) { // Fast lookup
		return hashed.value(vertex);
	}
	hashed.insert(vertex, cur);
	MapBlockVertex v;
	v.x = vertex.x;
	v.y = vertex.y;
	v.z = vertex.z;
	v.padding = 0;
	ordered.append(v);
	cur += 1;
	return cur - 1;
}

bool WmxFile::writeBlock(const MapBlock &block)
{
	QHash<Vertex, int> hashedVertices, hashedNormals;
	QList<MapBlockVertex> vertices, normals;
	int curVertex = 0, curNormal = 0;
	QList<MapBlockPolygon> polys;

	foreach (const MapPoly &polygon, block.polygons()) {
		MapBlockPolygon poly;
		poly.vi[0] = storeVertex(polygon.vertex(0), hashedVertices, vertices,
		                         curVertex);
		poly.vi[1] = storeVertex(polygon.vertex(1), hashedVertices, vertices,
		                         curVertex);
		poly.vi[2] = storeVertex(polygon.vertex(2), hashedVertices, vertices,
		                         curVertex);
		poly.ni[0] = storeVertex(polygon.normal(0), hashedNormals, normals,
		                         curNormal);
		poly.ni[1] = storeVertex(polygon.normal(1), hashedNormals, normals,
		                         curNormal);
		poly.ni[2] = storeVertex(polygon.normal(2), hashedNormals, normals,
		                         curNormal);
		poly.pos[0] = polygon.texCoord(0);
		poly.pos[1] = polygon.texCoord(1);
		poly.pos[2] = polygon.texCoord(2);
		if (curVertex < 4) {
			poly.texi = 0;
			poly.groundType = polygon.groundType();
			poly.u1 = polygon.u1();
			poly.u2 = polygon.u2();
		} else {
			poly.texi = (polygon.texPage() << 4) | (polygon.clutId() & 0xF);
			poly.groundType = polygon.groundType();
			poly.u1 = polygon.u1();
			poly.u2 = polygon.u2();
		}

		polys.append(poly);
	}

	MapBlockHeader header;
	header.polyCount = polys.size();
	header.vertexCount = curVertex;
	header.normalCount = curNormal;
	header.padding = 0;

	if (!writeBlockHeader(header)) {
		return false;
	}

	foreach (const MapBlockPolygon &poly, polys) {
		if (!writePolygon(poly)) {
			return false;
		}
	}

	foreach (const MapBlockVertex &vertex, vertices) {
		if (!writeVertex(vertex)) {
			return false;
		}
	}

	foreach (const MapBlockVertex &normal, normals) {
		if (!writeVertex(normal)) {
			return false;
		}
	}

	// Padding
	if (_io->write(QByteArray(sizeof(quint32), '\0')) != sizeof(quint32)) {
		return false;
	}

	return true;
}

bool WmxFile::readBlockHeader(MapBlockHeader &header)
{
	return _io->read((char *)&header, sizeof(MapBlockHeader))
	        == sizeof(MapBlockHeader);
}

bool WmxFile::writeBlockHeader(const MapBlockHeader &header)
{
	return _io->write((char *)&header, sizeof(MapBlockHeader))
	        == sizeof(MapBlockHeader);
}

bool WmxFile::readPolygon(MapBlockPolygon &polygon)
{
	return _io->read((char *)&polygon, sizeof(MapBlockPolygon))
	        == sizeof(MapBlockPolygon);
}

bool WmxFile::writePolygon(const MapBlockPolygon &polygon)
{
	return _io->write((char *)&polygon, sizeof(MapBlockPolygon))
	        == sizeof(MapBlockPolygon);
}

bool WmxFile::readVertex(MapBlockVertex &vertex)
{
	return _io->read((char *)&vertex, sizeof(MapBlockVertex))
	        == sizeof(MapBlockVertex);
}

bool WmxFile::writeVertex(const MapBlockVertex &vertex)
{
	return _io->write((char *)&vertex, sizeof(MapBlockVertex))
	        == sizeof(MapBlockVertex);
}
