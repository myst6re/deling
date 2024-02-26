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
#include "game/worldmap/Map.h"
#include "files/TimFile.h"

#define TEXLFILE_TEXTURE_SIZE 0x12800
#define TEXLFILE_TEXTURE_COUNT 20

class TexlFile
{
public:
	explicit TexlFile(QIODevice *io = Q_NULLPTR);
	inline void setDevice(QIODevice *device) {
		_io = device;
	}

	bool readTextures(Map &map);

private:
	bool seekTexture(quint8 id);
	bool readTexture(TimFile &tim);

	QIODevice *_io;
};
