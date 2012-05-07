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
	qint16 x, y, z, res;
} Vertex_sr;

typedef struct {
	Vertex_sr vertices[3];
} Triangle;

typedef struct {
	qint16 a1, a2, a3;
} Access;

class WalkmeshFile
{
public:
	WalkmeshFile();
	bool open(const QByteArray &id, const QByteArray &ca=QByteArray());
	bool save(QByteArray &ca);
	const QList<Triangle> &getTriangles() const;
	const Vertex_s &camAxis(quint8) const;
	qint32 camPos(quint8) const;
	void setCamAxis(quint8 id, const Vertex_s &cam_axis);
	void setCamPos(quint8 id, qint32 camPos);
	bool isModified() const;
private:
	quint16 camera_unknown1;
	quint32 camera_unknown2;
	quint16 camera_zoom;
	Vertex_s camera_axis[3];
	qint32 camera_position[3];
	QList<Triangle> triangles;
	QList<Access> access;
	bool modified;
};

#endif // WALKMESHFILE_H
