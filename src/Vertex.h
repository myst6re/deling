#pragma once

#include <QtCore>

struct Vertex {
	qint16 x, y, z;
};

inline bool operator==(const Vertex &v1, const Vertex &v2)
{
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

inline size_t qHash(const Vertex &key, uint seed)
{
	return qHash(quint64(key.x | (key.y << 16)) | (quint64(key.z) << 32), seed);
}

struct TexCoord {
	quint8 x, y;
};
