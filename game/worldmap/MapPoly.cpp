#include "MapPoly.h"

MapPoly::MapPoly(const QList<Vertex> &vertices, const QList<Vertex> &normals,
                 const QList<TexCoord> &texCoords,
                 quint8 texPage, quint8 clutId, quint8 groundType,
                 quint8 flags1, quint8 flags2) :
    TrianglePoly(vertices, normals, QList<QRgb>(), texCoords),
    _texPage(texPage), _clutId(clutId), _groundType(groundType),
    _flags1(flags1), _flags2(flags2)
{
}
