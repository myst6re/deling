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
#include <QRgb>
#include "Vertex.h"

class Poly
{
public:
	Poly(int count, const QList<Vertex> &vertices, const QList<Vertex> &normals,
	     const QList<QRgb> &colors,
	     const QList<TexCoord> &texCoords=QList<TexCoord>());
	Poly(int count, const QList<Vertex> &vertices, const QList<Vertex> &normals,
	     const QRgb &color,
	     const QList<TexCoord> &texCoords=QList<TexCoord>());
	virtual ~Poly() {}
	void setVertices(const QList<Vertex> &vertices, const QList<QRgb> &colors,
	                 const QList<TexCoord> &texCoords=QList<TexCoord>());
	void setVertices(const QList<Vertex> &vertices, const QRgb &color,
	                 const QList<TexCoord> &texCoords=QList<TexCoord>());
	inline int count() const {
		return _count;
	}
	const Vertex &vertex(quint8 id) const;
	const Vertex &normal(quint8 id) const;
	inline const QList<Vertex> &vertices() const {
		return _vertices;
	}
	inline const QList<Vertex> &normals() const {
		return _normals;
	}
	const QRgb &color() const;
	QRgb color(quint8 id) const;
	const TexCoord &texCoord(quint8 id) const;
	void setTexCoord(quint8 id, const TexCoord &texCoord);
	inline const QList<TexCoord> &texCoords() const {
		return _texCoords;
	}
	bool isMonochrome() const;
	bool hasTexture() const;
protected:
	int _count;
	QList<Vertex> _vertices, _normals;
	QList<QRgb> _colors;
	QList<TexCoord> _texCoords;
};

class QuadPoly : public Poly
{
public:
	QuadPoly(const QList<Vertex> &vertices, const QList<Vertex> &normals,
	         const QList<QRgb> &colors,
	         const QList<TexCoord> &texCoords=QList<TexCoord>());
	QuadPoly(const QList<Vertex> &vertices, const QList<Vertex> &normals,
	         const QRgb &color,
	         const QList<TexCoord> &texCoords=QList<TexCoord>());
};

class TrianglePoly : public Poly
{
public:
	TrianglePoly(const QList<Vertex> &vertices, const QList<Vertex> &normals,
	             const QList<QRgb> &colors,
	             const QList<TexCoord> &texCoords=QList<TexCoord>());
	TrianglePoly(const QList<Vertex> &vertices, const QList<Vertex> &normals,
	             const QRgb &color,
	             const QList<TexCoord> &texCoords=QList<TexCoord>());
};
