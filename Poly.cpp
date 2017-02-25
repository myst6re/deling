#include "Poly.h"

Poly::Poly(int count, const QList<Vertex> &vertices,
           const QList<Vertex> &normals, const QList<QRgb> &colors,
           const QList<TexCoord> &texCoords) :
    _count(count), _normals(normals)
{
	setVertices(vertices, colors, texCoords);
}

Poly::Poly(int count, const QList<Vertex> &vertices,
           const QList<Vertex> &normals, const QRgb &color,
           const QList<TexCoord> &texCoords) :
    _count(count), _normals(normals)
{
	setVertices(vertices, color, texCoords);
}

void Poly::setVertices(const QList<Vertex> &vertices, const QList<QRgb> &colors,
                       const QList<TexCoord> &texCoords)
{
	_vertices = vertices;
	_colors = colors;
	_texCoords = texCoords;
}

void Poly::setVertices(const QList<Vertex> &vertices, const QRgb &color,
                       const QList<TexCoord> &texCoords)
{
	_vertices = vertices;
	_colors.clear();
	_colors.append(color);
	_texCoords = texCoords;
}

const Vertex &Poly::vertex(quint8 id) const
{
	return _vertices.at(id);
}

const Vertex &Poly::normal(quint8 id) const
{
	return _normals.at(id);
}

const QRgb &Poly::color() const
{
	return _colors.first();
}

QRgb Poly::color(quint8 id) const
{
	return _colors.value(id, _colors.first());
}

const TexCoord &Poly::texCoord(quint8 id) const
{
	return _texCoords.at(id);
}

void Poly::setTexCoord(quint8 id, const TexCoord &texCoord)
{
	_texCoords.replace(id, texCoord);
}

bool Poly::isMonochrome() const
{
	return _colors.size() == 1;
}

bool Poly::hasTexture() const
{
	return !_texCoords.isEmpty();
}

QuadPoly::QuadPoly(const QList<Vertex> &vertices,
                   const QList<Vertex> &normals, const QList<QRgb> &colors,
                   const QList<TexCoord> &texCoords) :
    Poly(4, vertices, normals, colors, texCoords)
{
	// swapping the two last vertices for right OpenGL quad order

	_vertices.swap(2, 3);
	_normals.swap(2, 3);

	if(colors.size() == 4) {
		_colors.swap(2, 3);
	}

	if(!texCoords.isEmpty()) {
		_texCoords.swap(2, 3);
	}
}

QuadPoly::QuadPoly(const QList<Vertex> &vertices,
                   const QList<Vertex> &normals, const QRgb &color,
                   const QList<TexCoord> &texCoords) :
    Poly(4, vertices, normals, color, texCoords)
{
	// swapping the two last vertices for right OpenGL quad order

	_vertices.swap(2, 3);
	_normals.swap(2, 3);

	if(!texCoords.isEmpty()) {
		_texCoords.swap(2, 3);
	}
}

TrianglePoly::TrianglePoly(const QList<Vertex> &vertices,
                           const QList<Vertex> &normals,
                           const QList<QRgb> &colors,
                           const QList<TexCoord> &texCoords) :
    Poly(3, vertices, normals, colors, texCoords)
{
}

TrianglePoly::TrianglePoly(const QList<Vertex> &vertices,
                           const QList<Vertex> &normals, const QRgb &color,
                           const QList<TexCoord> &texCoords) :
    Poly(3, vertices, normals, color, texCoords)
{
}
