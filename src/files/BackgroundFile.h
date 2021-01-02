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
	quint16 texID; // 2 bits = depth | 2 bits = blend | 1 bit = draw | 4 bits = textID
	quint16 palID; // 6 bits = Always 30 | 4 bits = PaletteID | 6 bits = Always 0
	quint8 parameter;
	quint8 state;
};

struct Tile2 {
	qint16 X, Y;
	quint16 Z;
	quint16 texID; // 2 bits = depth | 2 bits = blend | 1 bit = draw | 4 bits = textID
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
	quint16 texID : 4;
	quint16 draw : 1; // 0-1 (0: ending, 1: everything else)
	quint16 blend : 2; // 0-3
	quint16 depth : 2; // 0-2
	quint8 palID;
	quint16 srcX, srcY;
	quint8 layerID; // 0-7
	quint8 blendType; // 0-4
	quint8 parameter;
	quint8 state;
	static Tile fromTile1(const Tile1 &tile, int sizeOfTile);
	static Tile fromTile2(const Tile2 &tile);
	static Tile1 toTile1(const Tile &tile);
	static Tile2 toTile2(const Tile &tile);
};

struct BackgroundBounds {
	qint16 left, right, top, bottom;
};

class BackgroundFile : public File
{
public:
	enum MapDepth {
		DepthPal8,
		DepthPal4,
		DepthColor
	};

	BackgroundFile();
	bool open(const QByteArray &map, const QByteArray &mim, const QMultiMap<quint8, quint8> *defaultParams = nullptr);
	inline QString filterText() const {
		return QObject::tr("Fichier map tiles écran PC (*.map)");
	}
	bool save(QByteArray &map);

	QImage background(bool hideBG=false) const;
	QImage background(const QList<quint8> &activeParams, bool hideBG = false);
	static QImage mimToImage(MapDepth depth);

	inline const QList<Tile> &tiles() const {
		return _tiles;
	}
	void setTiles(const QList<Tile> &tiles);
	const Tile &tile(quint16 index) const;
	void setTile(quint16 index, const Tile &tile);

	void setVisibleTile(int index);

	QMultiMap<quint8, quint8> params;
	QMultiMap<quint8, quint8> allparams;
	QMap<quint8, bool> layers;
private:
	enum MapType {
		TypeOld,
		TypeOldShort,
		TypeNew
	};

	using File::open;
	bool openParameters();
	QList<Tile> parseTiles(const QByteArray &map, MapType &type) const;
	QImage toImage(int palOffset, int srcYWidth,
	               const QMultiMap<quint16, int> &tiles,
	               const BackgroundBounds &bounds, bool hideBG) const;
	static void drawTile(const Tile &tile, int palOffset, int srcYWidth,
	                     int imageWidth, const BackgroundBounds &bounds,
	                     const char *constMimData, QRgb *pixels);
	static void BGcolor(quint16 value, quint8 blendType, QRgb *pixels,
	                    int index, bool forceBlack);
	static QByteArray mim;
	bool opened;
	MapType _mapType;
	QList<Tile> _tiles;
	QMultiMap<quint16, int> _tilesZOrder;
	BackgroundBounds _bounds;
	int _visibleTile;
};

#endif // BACKGROUNDFILE_H
