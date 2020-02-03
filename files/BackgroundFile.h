/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2012 Arzel Jérôme <myst6re@gmail.com>
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
#ifndef BACKGROUNDFILE_H
#define BACKGROUNDFILE_H

#include "files/File.h"
#include <QPixmap>
#include "FF8Image.h"

// Please do not use bitfields here

struct Tile1 {
	qint16 X, Y;
	quint16 srcX, srcY;
	quint16 Z;
	quint8 texID; // 1 bit = depth | 2 bits = blend2 | 1 bit = blend1 | 4 bits = textID
	quint8 ZZ1; // 0
	quint16 palID; // 6 bits = Always 30 | 4 bits = PaletteID | 6 bits = Always 0
	quint8 parameter;
	quint8 state;
};

struct Tile2 {
	qint16 X, Y;
	quint16 Z;
	quint8 texID; // 1 bit = depth | 2 bits = blend2 | 1 bit = blend1 | 4 bits = textID
	quint8 ZZ1; // 0-1 (ending)
	quint16 palID; // 6 bits = Always 30 | 4 bits = PaletteID | 6 bits = Always 0
	quint8 srcX, srcY;
	quint8 layerID; // 0-7
	quint8 blendType; // 0-4
	quint8 parameter;
	quint8 state;
};

struct Tile {
	qint16 X, Y;
	quint16 Z;
	quint8 texID;
	quint8 blend1; // 0-1 (0: ending, 1: everything else)
	quint8 blend2;
	quint8 depth;
	quint8 ZZ1; // 0-1 (1: ending, 0: everything else)
	quint8 palID;
	quint8 srcX, srcY;
	quint8 layerID; // 0-7
	quint8 blendType; // 0-4
	quint8 parameter;
	quint8 state;
	static Tile fromTile1(const Tile1 &tile, int sizeOfTile);
	static Tile fromTile2(const Tile2 &tile);
};

struct BackgroundBounds {
	qint16 left, right, top, bottom;
};

class BackgroundFile : public File
{
public:
	BackgroundFile();
	bool open(const QByteArray &map, const QByteArray &mim, const QMultiMap<quint8, quint8> *defaultParams = nullptr);
	inline QString filterText() const {
		return QString();
	}

	QImage background(bool hideBG=false) const;
	QImage background(const QList<quint8> &activeParams, bool hideBG = false);

	QMultiMap<quint8, quint8> params;
	QMultiMap<quint8, quint8> allparams;
	QMap<quint8, bool> layers;
private:
	using File::open;
	bool type1Parameters(const QByteArray &map,
	                     const QMultiMap<quint8, quint8> *defaultParams);
	bool type2Parameters(const QByteArray &map,
	                     const QMultiMap<quint8, quint8> *defaultParams);
	QMultiMap<quint16, Tile> type1(BackgroundBounds &bounds, bool hideBG = false) const;
	QMultiMap<quint16, Tile> type2(BackgroundBounds &bounds, bool hideBG = false) const;
	QImage toImage(int palOffset, int srcYWidth,
	               const QMultiMap<quint16, Tile> &tiles,
	               const BackgroundBounds &bounds) const;
	static QRgb BGcolor(int value, quint8 blendType=4, QRgb color0=0);
	static QByteArray map, mim;
	bool opened;
};

#endif // BACKGROUNDFILE_H
