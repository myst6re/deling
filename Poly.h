#ifndef POLY_H
#define POLY_H

#include <QtCore>
#include <QRgb>

struct Vertex {
	qint16 x, y, z;
};

inline bool operator==(const Vertex &v1, const Vertex &v2)
{
  return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

inline uint qHash(const Vertex &key, uint seed)
{
  return qHash(quint64(key.x | (key.y << 16)) | (quint64(key.z) << 32), seed);
}

struct TexCoord {
	quint8 x, y;
};

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

#endif // POLY_H
