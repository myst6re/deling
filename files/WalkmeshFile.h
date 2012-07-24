/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2012 Arzel Jérôme <myst6re@gmail.com>
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
#ifndef WALKMESHFILE_H
#define WALKMESHFILE_H

#include <QtCore>

typedef struct {
	qint16 x, y, z;
} Vertex_s;

typedef struct {
	qint16 x, y, z, res;// res = Triangle[0].z (padding)
} Vertex_sr;

typedef struct {
	Vertex_sr vertices[3];
} Triangle;

typedef struct {
	qint16 a1, a2, a3;
} Access;

typedef struct {
	Vertex_s camera_axis[3];
	qint16 camera_axis2z;// copy (padding)
	qint32 camera_position[3];
	qint32 blank;
	quint16 camera_zoom;
	quint16 camera_zoom2;// copy (padding)
} CaStruct;

class WalkmeshFile
{
public:
	WalkmeshFile();
	bool open(const QByteArray &id, const QByteArray &ca=QByteArray());
	bool save(QByteArray &ca);
	int triangleCount() const;
	const QList<Triangle> &getTriangles() const;
	const Triangle &triangle(int triangleID) const;
	void setTriangle(int triangleID, const Triangle &triangle);
	const Access &access(int triangleID) const;
	void setAccess(int triangleID, const Access &access);
	int cameraCount() const;
	const CaStruct &camera(int camID) const;
	void setCamera(int camID, const CaStruct &cam);
	bool hasUnknownData() const;
	qint16 unknown() const;
	bool isModified() const;
private:
	QList<CaStruct> cameras;
	QList<Triangle> triangles;
	QList<Access> _access;
	qint16 _unknown;
	bool modified, _hasUnknownData;
};

#endif // WALKMESHFILE_H
