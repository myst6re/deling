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
#include "WmEncounter.h"

WmEncounter::WmEncounter(const QList<quint16> &sceneIds,
                         quint8 flags, quint8 region, quint8 ground,
                         const QList<quint16> &lunarCrySceneIds,
                         quint8 lunarCryFlags) :
    _sceneIds(sceneIds), _lunarCrySceneIds(lunarCrySceneIds),
    _flags(flags), _lunarCryFlags(lunarCryFlags),
    _region(region), _ground(ground)
{
}
