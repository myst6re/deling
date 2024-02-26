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

class WmEncounter
{
public:
	WmEncounter(const QList<quint16> &sceneIds,
	            quint8 flags, quint8 region, quint8 ground,
	            const QList<quint16> &lunarCrySceneIds = QList<quint16>(),
	            quint8 lunarCryFlags = 0);

	inline const QList<quint16> &sceneIds() const {
		return _sceneIds;
	}

	inline void setSceneIds(const QList<quint16> &sceneIds) {
		_sceneIds = sceneIds;
	}

	inline const QList<quint16> &lunarCrySceneIds() const {
		return _lunarCrySceneIds;
	}

	inline void setLunarCrySceneIds(const QList<quint16> &sceneIds) {
		_lunarCrySceneIds = sceneIds;
	}

	inline quint8 flags() const {
		return _flags;
	}

	inline void setFlags(quint8 flags) {
		_flags = flags;
	}

	inline quint8 lunarCryFlags() const {
		return _lunarCryFlags;
	}

	inline void setLunarCryFlags(quint8 flags) {
		_lunarCryFlags = flags;
	}

	inline quint8 region() const {
		return _region;
	}

	inline void setRegion(quint8 region) {
		_region = region;
	}

	inline quint8 ground() const {
		return _ground;
	}

	inline void setGround(quint8 ground) {
		_ground = ground;
	}

private:
	QList<quint16> _sceneIds, _lunarCrySceneIds;
	quint8 _flags, _lunarCryFlags, _region, _ground;
};
