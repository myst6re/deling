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

#include "files/File.h"
#include "Vertex.h"

struct Vertex_sr {
	qint16 x, y, z, res;// res = Triangle[0].z (padding)
};

struct Triangle {
	Vertex_sr vertices[3];
};

struct Access {
	qint16 a[3];
};

class IdFile : public File
{
public:
	IdFile();
	bool open(const QByteArray &id);
	bool save(QByteArray &id) const;
	inline QString filterText() const {
		return QObject::tr("Field walkmesh PC File (*.id)");
	}
	bool hasTriangle() const;
	int triangleCount() const;
	const QList<Triangle> &getTriangles() const;
	const Triangle &triangle(int triangleID) const;
	void setTriangle(int triangleID, const Triangle &triangle);
	void insertTriangle(int triangleID, const Triangle &triangle, const Access &access);
	void removeTriangle(int triangleID);
	const Access &access(int triangleID) const;
	void setAccess(int triangleID, const Access &access);
	bool hasUnknownData() const;
	qint16 unknown() const;
	static Vertex_sr fromVertex_s(const Vertex &vertex_s);
	static Vertex toVertex_s(const Vertex_sr &vertex_sr);
private:
	QList<Triangle> triangles;
	QList<Access> _access;
	qint16 _unknown;
	bool _hasUnknownData;
};
