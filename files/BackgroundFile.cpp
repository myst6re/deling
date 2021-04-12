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
#include "files/BackgroundFile.h"

QByteArray BackgroundFile::mim = QByteArray();

Tile Tile::fromTile1(const Tile1 &tileType1, int sizeOfTile)
{
	Tile tile;
	tile.X = tileType1.X;
	tile.Y = tileType1.Y;
	tile.Z = tileType1.Z;
	tile.texID = tileType1.texID & 0xF;
	tile.draw = (tileType1.texID >> 4) & 1;
	tile.blend = (tileType1.texID >> 5) & 3;
	tile.depth = (tileType1.texID >> 7) & 3;
	tile.palID = (tileType1.palID >> 6) & 0xF;
	tile.srcX = tileType1.srcX;
	tile.srcY = tileType1.srcY;
	tile.layerID = 0;
	tile.blendType = tile.blend & 1 ? 1 : 4;
	if (sizeOfTile < 16) {
		tile.parameter = 255;
		tile.state = 0;
	} else {
		tile.parameter = tileType1.parameter;
		tile.state = tileType1.state;
	}
	return tile;
}

Tile Tile::fromTile2(const Tile2 &tileType2)
{
	Tile tile;
	tile.X = tileType2.X;
	tile.Y = tileType2.Y;
	tile.Z = tileType2.Z;
	tile.texID = tileType2.texID & 0xF;
	tile.draw = (tileType2.texID >> 4) & 1;
	tile.blend = (tileType2.texID >> 5) & 3;
	tile.depth = (tileType2.texID >> 7) & 3;
	tile.palID = (tileType2.palID >> 6) & 0xF;

	if (tile.X != 0x7fff && (tileType2.palID & 0x3F || tileType2.palID >> 10 != 0xF)) {
		qDebug() << "Tile::fromTile2 strange palID" << tileType2.palID;
	}
	if (tile.texID >> 9) {
		qDebug() << "Tile::fromTile2 strange texID" << tileType2.texID;
	}

	tile.srcX = tileType2.srcX;
	tile.srcY = tileType2.srcY;
	tile.layerID = tileType2.layerID;
	tile.blendType = tileType2.blendType;
	tile.parameter = tileType2.parameter;
	tile.state = tileType2.state;
	return tile;
}

Tile1 Tile::toTile1(const Tile &tile)
{
	Tile1 tileType1;
	tileType1.X = tile.X;
	tileType1.Y = tile.Y;
	tileType1.Z = tile.Z;
	quint8 blend = tile.blend & 3;
	if (tile.blendType != 4) {
		blend |= 1;
	}
	tileType1.texID = quint16(tile.depth << 7)
	                  | quint16(tile.blend << 5)
	                  | quint16(tile.draw << 4)
	                  | quint16(tile.texID);
	tileType1.palID = 0x3C00 | quint16((tile.palID & 0xF) << 6);
	tileType1.srcX = tile.srcX;
	tileType1.srcY = tile.srcY;
	tileType1.parameter = tile.parameter;
	tileType1.state = tile.state;
	return tileType1;
}

Tile2 Tile::toTile2(const Tile &tile)
{
	Tile2 tileType2;
	tileType2.X = tile.X;
	tileType2.Y = tile.Y;
	tileType2.Z = tile.Z;
	tileType2.texID = quint16(tile.depth << 7)
	                  | quint16(tile.blend << 5)
	                  | quint16(tile.draw << 4)
	                  | quint16(tile.texID);
	tileType2.palID = 0x3C00 | quint16((tile.palID & 0xF) << 6);
	tileType2.srcX = quint8(tile.srcX);
	tileType2.srcY = quint8(tile.srcY);
	tileType2.layerID = tile.layerID;
	tileType2.blendType = tile.blendType;
	tileType2.parameter = tile.parameter;
	tileType2.state = tile.state;
	return tileType2;
}

BackgroundFile::BackgroundFile() :
    File(), opened(false), _mapType(TypeNew), _visibleTile(-1)
{
}

bool BackgroundFile::open(const QByteArray &map, const QByteArray &mim,
                          const QMultiMap<quint8, quint8> *defaultParams)
{
	this->mim = mim;

	if(!opened) {
		allparams.clear();
		params.clear();
		layers.clear();
		int mimSize = mim.size();
		MapType mapType;

		if(mimSize == 401408) {
			mapType = TypeOld;
		} else if(mimSize == 438272) {
			mapType = TypeNew;
		}

		_tiles = parseTiles(map, mapType);
		_mapType = mapType;

		openParameters();

		if(defaultParams) {
			params = *defaultParams;
		} else {
			// Enable parameter only when state = 0
			foreach(quint8 param, allparams.keys(0)) {
				params.insert(param, 0);
			}
		}
	}

	opened = true;

	return true;
}

bool BackgroundFile::save(QByteArray &map)
{
	Tile1 tile1;
	Tile2 tile2;
	const size_t size = _mapType == TypeOldShort ? 14 : 16;

	map = QByteArray(int(size) * (_tiles.size() + 1), '\0');
	char *data = map.data();

	foreach(const Tile &tile, _tiles) {
		if (_mapType == TypeNew) {
			tile2 = Tile::toTile2(tile);
			memcpy(data, &tile2, 16);
		} else {
			tile1 = Tile::toTile1(tile);
			memcpy(data, &tile1, size);
		}
		data += size;
	}

	tile2 = Tile2();
	tile2.X = 0x7fff;
	memcpy(data, &tile2, size);

	return true;
}

bool BackgroundFile::openParameters()
{
	int i = 0;

	allparams.clear();
	layers.clear();
	_tilesZOrder.clear();
	_bounds = BackgroundBounds();

	foreach(const Tile &tile, _tiles) {
		if(tile.parameter != 255 &&
		        !allparams.contains(tile.parameter, tile.state)) {
			allparams.insert(tile.parameter, tile.state);
		}

		if(qAbs(tile.X) < 1000 && qAbs(tile.Y) < 1000) {
			if(tile.X >= 0 && tile.X > _bounds.right)
				_bounds.right = tile.X;
			else if(tile.X < 0 && -tile.X > _bounds.left)
				_bounds.left = -tile.X;
			if(tile.Y >= 0 && tile.Y > _bounds.bottom)
				_bounds.bottom = tile.Y;
			else if(tile.Y < 0 && -tile.Y > _bounds.top)
				_bounds.top = -tile.Y;
		} else {
			qDebug() << "BackgroundFile::background out of bounds";
		}

		layers.insert(tile.layerID, true);
		_tilesZOrder.insert(4096 - tile.Z, i);

		i += 1;
	}

	return true;
}

QImage BackgroundFile::background(bool hideBG) const
{
	int mimSize = mim.size(), palOffset = 4096, srcYWidth = 1664;
	MapType mapType;

	if(mimSize == 401408) {
		mapType = TypeOld;
		palOffset = 0;
		srcYWidth = 1536;
	} else if(mimSize == 438272) {
		mapType = TypeNew;
	} else {
		if (mimSize > 0) {
			qDebug() << "Error BackgroundFile::background" << mimSize;
		}
		return FF8Image::errorImage();
	}

	return toImage(palOffset, srcYWidth, _tilesZOrder, _bounds, hideBG);
}

QImage BackgroundFile::background(const QList<quint8> &activeParams, bool hideBG)
{
	QMultiMap<quint8, quint8> savParams = params;
	QMap<quint8, bool> savLayers = layers;

	// Enable activeParams only
	params.clear();

	QMapIterator<quint8, quint8> itParams(allparams);
	while(itParams.hasNext()){
		itParams.next();
		if(activeParams.contains(itParams.key())) {
			params.insert(itParams.key(), itParams.value());
		}
	}

	// Enable all layers
	QMapIterator<quint8, bool> itLayers(layers);
	while(itLayers.hasNext()){
		itLayers.next();
		layers.insert(itLayers.key(), true);
	}

	QImage image = background(hideBG);

	params = savParams;
	layers = savLayers;

	return image;
}

QList<Tile> BackgroundFile::parseTiles(const QByteArray &map,
                                       MapType &type) const
{
	int mapSize = map.size(), tilePos = 0,
	        sizeOfTile = mapSize - map.lastIndexOf("\xff\x7f");
	Tile1 tileType1;
	Tile2 tileType2;
	Tile tile;
	QList<Tile> tiles;
	const char *constMapData = map.constData();

	type = sizeOfTile == 14 ? TypeOldShort : type;

	if(sizeOfTile != 14 && sizeOfTile != 16) {
		qDebug() << "BackgroundFile::parseTiles: invalid map size" << mapSize << sizeOfTile;
		return tiles;
	}

	while(tilePos + sizeOfTile <= mapSize) {
		if(type != TypeNew) {
			memcpy(&tileType1, constMapData + tilePos, size_t(sizeOfTile));
			tile = Tile::fromTile1(tileType1, sizeOfTile);
		} else {
			memcpy(&tileType2, constMapData + tilePos, size_t(sizeOfTile));
			// HACK
			if(tileType2.blendType >= 60) {
				type = TypeOld;
				return parseTiles(map, type);
			}
			tile = Tile::fromTile2(tileType2);
		}

		if(tile.X == 0x7fff) {
			break;
		}

		tiles.append(tile);
		tilePos += sizeOfTile;
	}

	return tiles;
}

void BackgroundFile::setTiles(const QList<Tile> &tiles)
{
	_tiles = tiles;
	setModified(true);
	openParameters();
}

const Tile &BackgroundFile::tile(quint16 index) const
{
	return _tiles.at(index);
}

void BackgroundFile::setTile(quint16 index, const Tile &tile)
{
	_tiles.replace(index, tile);
	setModified(true);
	openParameters();
}

void BackgroundFile::setVisibleTile(int index)
{
	_visibleTile = index;
}

QImage BackgroundFile::toImage(int palOffset, int srcYWidth,
                               const QMultiMap<quint16, int> &tiles,
                               const BackgroundBounds &bounds, bool hideBG) const
{
	int width = bounds.left + bounds.right + 16,
	        height = bounds.top + bounds.bottom + 16;
	const char *constMimData = mim.constData();
	QImage image(width, height, QImage::Format_RGB32);
	QRgb *pixels = (QRgb *)image.bits();
	image.fill(0xFF000000);

	foreach(int tileIndex, tiles) {
		const Tile &tile = _tiles.at(tileIndex);

		if(((hideBG || tile.parameter != 255) &&
		    !params.contains(tile.parameter, tile.state)) ||
		        (!layers.isEmpty() && !layers.value(tile.layerID))) {
			continue;
		}

		if (_visibleTile < 0 || _visibleTile != tileIndex) {
			drawTile(tile, palOffset, srcYWidth, width, bounds,
			         constMimData, pixels);
		}
	}

	if (_visibleTile >= 0) {
		const Tile &tile = _tiles.at(_visibleTile);
		drawTile(tile, palOffset, srcYWidth, width, bounds,
		         constMimData, pixels);

		for(int i = 0 ; i < 16 ; ++i) {
			pixels[bounds.left + tile.X + (bounds.top + tile.Y) * width + i] = qRgb(255, 0, 0);
			pixels[bounds.left + tile.X + (bounds.top + tile.Y + 15) * width + i] = qRgb(255, 0, 0);
			pixels[bounds.left + tile.X + (bounds.top + tile.Y + i) * width] = qRgb(255, 0, 0);
			pixels[bounds.left + tile.X + 15 + (bounds.top + tile.Y + i) * width] = qRgb(255, 0, 0);
		}
	}

	return image;
}

void BackgroundFile::drawTile(const Tile &tile, int palOffset, int srcYWidth,
                              int imageWidth, const BackgroundBounds &bounds,
                              const char *constMimData, QRgb *pixels)
{
	quint16 color;
	const int baseX = bounds.left + tile.X,
	        palStart = palOffset + tile.palID * 512;
	int pos = palOffset + 8192 + tile.texID * 128 + tile.srcY * srcYWidth,
	        x = 0, y = (bounds.top + tile.Y) * imageWidth;

	if(tile.depth == 2) {
		pos += tile.srcX * 2;
		srcYWidth /= 2;

		for(int i=0 ; i<srcYWidth * 16 ; ++i) {
			memcpy(&color, constMimData + pos + i * 2, 2);
			BGcolor(color, tile.blendType, pixels, baseX + x + y, !tile.draw);

			if(x == 15) {
				x = 0;
				i += srcYWidth - 16;
				y += imageWidth;
			} else {
				++x;
			}
		}
	} else if(tile.depth == 1) {
		pos += tile.srcX;

		for(int i=0 ; i<srcYWidth * 16 ; ++i) {
			memcpy(&color, constMimData + palStart + quint8(mim.at(pos+i)) * 2, 2);
			BGcolor(color, tile.blendType, pixels, baseX + x + y, !tile.draw);

			if(x == 15) {
				x = 0;
				i += srcYWidth - 16;
				y += imageWidth;
			} else {
				++x;
			}
		}
	} else {
		pos += tile.srcX / 2;

		for(int i=0 ; i<srcYWidth * 16 ; ++i) {
			quint8 index = quint8(mim.at(pos+i));
			memcpy(&color, constMimData + palStart + (index & 0xF) * 2, 2);
			BGcolor(color, tile.blendType, pixels, baseX + x + y, !tile.draw);
			++x;

			memcpy(&color, constMimData + palStart + (index >> 4) * 2, 2);
			BGcolor(color, tile.blendType, pixels, baseX + x + y, !tile.draw);

			if(x == 15) {
				x = 0;
				i += srcYWidth - 8;
				y += imageWidth;
			} else {
				++x;
			}
		}
	}
}

void BackgroundFile::BGcolor(quint16 value, quint8 blendType, QRgb *pixels,
                             int index, bool forceBlack)
{
	if(!forceBlack && value == 0) {
		return;
	}

	QRgb color;

	if(forceBlack) {
		color = 0;
	} else {
		color = FF8Image::fromPsColor(value);
	}

	if(blendType == 4) {
		pixels[index] = color;
	} else {
		QRgb color0 = pixels[index];
		int r = qRed(color), g = 0, b = 0;

		switch(blendType) {
		case 0:
			r = (qRed(color0) + r) / 2;
			g = (qGreen(color0) + r) / 2;
			b = (qBlue(color0) + r) / 2;
			break;
		case 1:
			r = qRed(color0) + r;
			if(r > 255)	r = 255;
			g = qGreen(color0) + qGreen(color);
			if(g > 255)	g = 255;
			b = qBlue(color0) + qBlue(color);
			if(b > 255)	b = 255;
			break;
		case 2:
			r = qRed(color0) - r;
			if(r < 0)	r = 0;
			g = qGreen(color0) - qGreen(color);
			if(g < 0)	g = 0;
			b = qBlue(color0) - qBlue(color);
			if(b < 0)	b = 0;
			break;
		case 3:
			r = qRed(color0) + int(.25 * r);
			if(r > 255)	r = 255;
			g = qGreen(color0) + int(.25 * qGreen(color));
			if(g > 255)	g = 255;
			b = qBlue(color0) + int(.25 * qBlue(color));
			if(b > 255)	b = 255;
			break;
		}

		pixels[index] = qRgb(r, g, b);
	}
}

QImage BackgroundFile::mimToImage(MapDepth depth)
{
	if(depth == DepthColor) {
		int width = 832;
		int height = (mim.size() - 0x3000) / (2 * width);
		const char *data = mim.constData();

		if(mim.size() < 0x3000 + width * height * 2) {
			return QImage();
		}

		QImage img(width, height, QImage::Format_RGB32);
		QRgb *px = (QRgb *)img.bits();

		for(int j=0 ; j<width*height ; ++j) {
			quint16 color;
			memcpy(&color, data + 0x3000 + j*2, 2);

			px[j] = FF8Image::fromPsColor(color);
		}

		return img;
	}
}
