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
