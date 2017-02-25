#include "MapPoly.h"

MapPoly::MapPoly(const QList<Vertex> &vertices, const QList<Vertex> &normals,
                 const QList<TexCoord> &texCoords,
                 quint8 texPage, quint8 clutId, quint8 groundType,
                 quint8 u1, quint8 u2) :
    TrianglePoly(vertices, normals, QList<QRgb>(), texCoords),
    _texPage(texPage), _clutId(clutId), _groundType(groundType),
    _u1(u1), _u2(u2)
{
}
