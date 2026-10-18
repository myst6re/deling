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
#include "files/IdFile.h"
#include <algorithm>
#include <functional>

IdFile::IdFile()
	: File(), _unknown(0), _hasUnknownData(false)
{
}

bool IdFile::open(const QByteArray &id)
{
	const char *id_data = id.constData();
	int id_data_size = id.size();
	quint32 i, nbSector, accessStart;

	if (id_data_size < 4) {
		qWarning() << "size id error" << id_data_size;
		return false;
	}

	memcpy(&nbSector, id_data, 4);

	accessStart = 4+nbSector*24;

	if ((quint32)id_data_size != accessStart+nbSector*6) {
		if ((quint32)id_data_size == accessStart+nbSector*6+2) {
			memcpy(&_unknown, &id_data[accessStart+nbSector*6], 2);
			_hasUnknownData = true;
		} else {
			qWarning() << "size id error" << id_data_size << (accessStart+nbSector*6);
			return false;
		}
	}

	if (sizeof(Triangle) != 24) {
		qWarning() << "invalid sizeof(Triangle)" << sizeof(Triangle) << 24;
	}
	if (sizeof(Access) != 6) {
		qWarning() << "invalid sizeof(Triangle)" << sizeof(Access) << 6;
	}

	Triangle triangle;
	Access acc;
	triangles.clear();
	_access.clear();
	for (i = 0; i < nbSector; ++i) {
		memcpy(&triangle, &id_data[4+i*24], 24);

		triangles.append(triangle);
//		qDebug() << triangle.vertices[0].x << triangle.vertices[0].y << triangle.vertices[0].z << triangle.vertices[0].res;
//		qDebug() << triangle.vertices[1].x << triangle.vertices[1].y << triangle.vertices[1].z << triangle.vertices[1].res;
//		qDebug() << triangle.vertices[2].x << triangle.vertices[2].y << triangle.vertices[2].z << triangle.vertices[2].res;
//		qDebug() << "=====";
		memcpy(&acc, &id_data[accessStart+i*6], 6);
		_access.append(acc);
//		qDebug() << acc.a1 << acc.a2 << acc.a3;
//		qDebug() << "=====";
	}

	modified = false;

	return true;
}

bool IdFile::save(QByteArray &id) const
{
	quint32 count=triangles.size();

	id.append((char *)&count, 4);

	for (Triangle triangle: triangles) {
		triangle.vertices[0].res = triangle.vertices[0].z;
		triangle.vertices[1].res = triangle.vertices[0].z;
		triangle.vertices[2].res = triangle.vertices[0].z;
		id.append((char *)&triangle.vertices, sizeof(Triangle));
	}

	for (const Access &access: _access) {
		id.append((char *)&access, sizeof(Access));
	}

	if (_hasUnknownData) {
		id.append((char *)&_unknown, 2);
	}

	return true;
}

bool IdFile::hasTriangle() const
{
	return !triangles.empty();
}

int IdFile::triangleCount() const
{
	return triangles.size();
}

const QList<Triangle> &IdFile::getTriangles() const
{
	return triangles;
}

const Triangle &IdFile::triangle(int triangleID) const
{
	return triangles.at(triangleID);
}

void IdFile::setTriangle(int triangleID, const Triangle &triangle)
{
	triangles[triangleID] = triangle;
	modified = true;
}

void IdFile::insertTriangle(int triangleID, const Triangle &triangle, const Access &access)
{
	triangles.insert(triangleID, triangle);
	_access.insert(triangleID, access);
	modified = true;
}

void IdFile::removeTriangle(int triangleID)
{
	triangles.removeAt(triangleID);
	_access.removeAt(triangleID);
	modified = true;
}

const Access &IdFile::access(int triangleID) const
{
	return _access.at(triangleID);
}

void IdFile::setAccess(int triangleID, const Access &access)
{
	_access[triangleID] = access;
	modified = true;
}

bool IdFile::hasUnknownData() const
{
	return _hasUnknownData;
}

qint16 IdFile::unknown() const
{
	return _unknown;
}

Vertex_sr IdFile::fromVertex_s(const Vertex &vertex_s)
{
	Vertex_sr vertex_sr;

	vertex_sr.x = vertex_s.x;
	vertex_sr.y = vertex_s.y;
	vertex_sr.z = vertex_s.z;
	vertex_sr.res = 0;

	return vertex_sr;
}

Vertex IdFile::toVertex_s(const Vertex_sr &vertex_sr)
{
	Vertex vertex_s;

	vertex_s.x = vertex_sr.x;
	vertex_s.y = vertex_sr.y;
	vertex_s.z = vertex_sr.z;

	return vertex_s;
}

bool IdFile::Snapshot::operator==(const Snapshot &other) const
{
	if (triangles.size() != other.triangles.size()) {
		return false;
	}

	for (qsizetype i = 0; i < triangles.size(); ++i) {
		if (memcmp(&triangles.at(i), &other.triangles.at(i), sizeof(Triangle)) != 0
		        || memcmp(&access.at(i), &other.access.at(i), sizeof(Access)) != 0) {
			return false;
		}
	}

	return true;
}

IdFile::Snapshot IdFile::snapshot() const
{
	return Snapshot{triangles, _access};
}

void IdFile::restore(const Snapshot &snapshot)
{
	triangles = snapshot.triangles;
	_access = snapshot.access;
	modified = true;
}

bool IdFile::samePoint(const Vertex_sr &a, const Vertex_sr &b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

QList<int> IdFile::trianglesUsingPoint(const Vertex_sr &point) const
{
	QList<int> triangleIDs;

	for (int i = 0; i < triangles.size(); ++i) {
		for (const Vertex_sr &vertex: triangles.at(i).vertices) {
			if (samePoint(vertex, point)) {
				triangleIDs.append(i);
				break;
			}
		}
	}

	return triangleIDs;
}

// `from` is taken by value: a reference into one of the triangles would change as soon as the
// first copy moved, and the remaining copies would no longer be recognised
void IdFile::movePoint(Vertex_sr from, const Vertex_sr &to)
{
	for (Triangle &triangle: triangles) {
		for (Vertex_sr &vertex: triangle.vertices) {
			if (samePoint(vertex, from)) {
				vertex.x = to.x;
				vertex.y = to.y;
				vertex.z = to.z;
			}
		}
	}

	modified = true;
}

/**
 * Add a triangle made of one side of an existing triangle and a new point, and return its id,
 * or -1 when the three points are aligned and there is no triangle to make.
 */
int IdFile::addTriangleOnSide(int triangleID, int side, const Vertex_sr &point)
{
	const Triangle &source = triangles.at(triangleID);
	Triangle triangle = {};

	// Two neighbouring triangles run along the side they share in opposite directions
	triangle.vertices[0] = source.vertices[(side + 1) % 3];
	triangle.vertices[1] = source.vertices[side];
	triangle.vertices[2] = point;

	if (isFlat(triangle)) {
		return -1;
	}

	// Still the wrong way round when the source triangle was flipped itself
	if (isFlipped(triangle)) {
		std::swap(triangle.vertices[0], triangle.vertices[1]);
	}

	Access access = {};
	access.a[0] = access.a[1] = access.a[2] = -1;

	triangles.append(triangle);
	_access.append(access);
	modified = true;

	return triangles.size() - 1;
}

/**
 * Remove triangles by filling each hole with the last triangle, highest id first. Only the
 * moved triangle's id changes instead of every id after the hole, which matters because
 * gateways in other fields and IDLOCK/IDUNLOCK in this field's script refer to triangles by id.
 */
void IdFile::removeTriangles(QList<int> triangleIDs)
{
	std::sort(triangleIDs.begin(), triangleIDs.end(), std::greater<int>());
	triangleIDs.erase(std::unique(triangleIDs.begin(), triangleIDs.end()), triangleIDs.end());

	for (int triangleID: triangleIDs) {
		triangles[triangleID] = triangles.last();
		_access[triangleID] = _access.last();
		triangles.removeLast();
		_access.removeLast();
	}

	modified = true;
}

static bool hasSide(const Triangle &triangle, const Vertex_sr &a, const Vertex_sr &b)
{
	for (int side = 0; side < 3; ++side) {
		const Vertex_sr &p = triangle.vertices[side], &q = triangle.vertices[(side + 1) % 3];

		if ((IdFile::samePoint(p, a) && IdFile::samePoint(q, b))
		        || (IdFile::samePoint(p, b) && IdFile::samePoint(q, a))) {
			return true;
		}
	}

	return false;
}

/**
 * Work out, for every side of every triangle, which triangle is on the other side: the one
 * sharing both points of that side, or -1 (a wall) when there is none. This is exactly how
 * all 895 retail walkmeshes are built, so it can replace typing the numbers by hand.
 */
void IdFile::rebuildAccess()
{
	for (int triangleID = 0; triangleID < triangles.size(); ++triangleID) {
		for (int side = 0; side < 3; ++side) {
			const Vertex_sr &a = triangles.at(triangleID).vertices[side],
			                &b = triangles.at(triangleID).vertices[(side + 1) % 3];
			QList<int> neighbours;

			for (int other = 0; other < triangles.size(); ++other) {
				if (other != triangleID && hasSide(triangles.at(other), a, b)) {
					neighbours.append(other);
				}
			}

			// Keep the current neighbour while it is still valid: a side shared by more than two
			// triangles has several candidates, and the file's own choice should survive
			qint16 &neighbour = _access[triangleID].a[side];
			if (!neighbours.contains(neighbour)) {
				neighbour = neighbours.isEmpty() ? -1 : qint16(neighbours.first());
			}
		}
	}
}

/**
 * Twice the triangle's area on the ground plane, signed with the game's convention:
 * Field_Walkmesh_FindTriangleAt and Field_Walkmesh_ResolveMovement only consider a point to
 * be inside a triangle when this is positive, so a negative triangle can never be walked on.
 */
static qint64 groundArea2(const Triangle &triangle)
{
	const Vertex_sr &v0 = triangle.vertices[0], &v1 = triangle.vertices[1], &v2 = triangle.vertices[2];

	return qint64(v1.y - v0.y) * qint64(v2.x - v0.x) - qint64(v1.x - v0.x) * qint64(v2.y - v0.y);
}

bool IdFile::isFlipped(const Triangle &triangle)
{
	return groundArea2(triangle) < 0;
}

// Three aligned points: the height computation divides by zero and drops the player to 0
bool IdFile::isFlat(const Triangle &triangle)
{
	return groundArea2(triangle) == 0;
}
