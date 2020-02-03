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
QByteArray BackgroundFile::map = QByteArray();

Tile Tile::fromTile1(const Tile1 &tileType1, int sizeOfTile)
{
	Tile tile;
	tile.X = tileType1.X;
	tile.Y = tileType1.Y;
	tile.Z = tileType1.Z;
	tile.texID = tileType1.texID & 0xF;
	tile.blend1 = (tileType1.texID >> 4) & 1;
	tile.blend2 = (tileType1.texID >> 5) & 3;
	tile.depth = tileType1.texID >> 7;
	tile.ZZ1 = tileType1.ZZ1;
	tile.palID = (tileType1.palID >> 6) & 0xF;
	tile.srcX = tileType1.srcX;
	tile.srcY = tileType1.srcY;
	tile.layerID = 0;
	tile.blendType = tile.blend2 & 1 ? 1 : 4;
	if (sizeOfTile < 16) {
		tile.parameter = 0;
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
	tile.blend1 = (tileType2.texID >> 4) & 1;
	tile.blend2 = (tileType2.texID >> 5) & 3;
	tile.depth = tileType2.texID >> 7;
	tile.ZZ1 = tileType2.ZZ1;
	tile.palID = (tileType2.palID >> 6) & 0xF;
	tile.srcX = tileType2.srcX;
	tile.srcY = tileType2.srcY;
	tile.layerID = tileType2.layerID & 0x7F;
	tile.blendType = tileType2.blendType;
	tile.parameter = tileType2.parameter;
	tile.state = tileType2.state;
	return tile;
}

BackgroundFile::BackgroundFile() :
	File(), opened(false)
{
}

bool BackgroundFile::open(const QByteArray &map, const QByteArray &mim,
                          const QMultiMap<quint8, quint8> *defaultParams)
{
	if(!opened) {
		allparams.clear();
		params.clear();
		layers.clear();
		int mimSize = mim.size();

		if(mimSize == 401408) {
			type1Parameters(map, defaultParams);
		} else if(mimSize == 438272) {
			type2Parameters(map, defaultParams);
		}

		if(defaultParams) {
			params = *defaultParams;
			qDebug() << "default" << params;
		}
	}

	this->map = map;
	this->mim = mim;

	opened = true;

	return true;
}

bool BackgroundFile::type1Parameters(const QByteArray &map,
                                     const QMultiMap<quint8, quint8> *defaultParams)
{
	int mapSize = map.size(), tilePos = 0;
	const char *constMapData = map.constData();
	Tile1 tile1;
	int sizeOfTile = mapSize - map.lastIndexOf("\xff\x7f");

	if(sizeOfTile != 16) {
		return false;
	}

	while(tilePos + sizeOfTile <= mapSize) {
		memcpy(&tile1, constMapData + tilePos, sizeOfTile);
		tilePos += sizeOfTile;

		if(tile1.X == 0x7fff) {
			if(tilePos + sizeOfTile <= mapSize) {
				qDebug() << "End of type1: remaning data";
			}
			break;
		}

		if(tile1.parameter != 255 &&
		        !allparams.contains(tile1.parameter, tile1.state)) {
			allparams.insert(tile1.parameter, tile1.state);
			// enable parameter only when state = 0
			if(!defaultParams && tile1.state == 0) {
				params.insert(tile1.parameter, tile1.state);
			}
		}
	}

	return true;
}

bool BackgroundFile::type2Parameters(const QByteArray &map,
                                     const QMultiMap<quint8, quint8> *defaultParams)
{
	int mapSize = map.size(), tilePos = 0;
	const char *constMapData = map.constData();
	Tile2 tile2;

	while(tilePos + 16 <= mapSize) {
		memcpy(&tile2, constMapData + tilePos, 16);
		tilePos += 16;

		if(tile2.X == 0x7fff) {
			if(tilePos + 16 <= mapSize) {
				qDebug() << "End of type2: remaning data";
			}
			break;
		}

		// HACK
		if(tile2.blendType >= 60) {
			return type1Parameters(map, defaultParams);
		}

		if(tile2.parameter != 255 &&
		        !allparams.contains(tile2.parameter, tile2.state)) {
			allparams.insert(tile2.parameter, tile2.state);
			// enable parameter only when state = 0
			if(!defaultParams && tile2.state == 0) {
				params.insert(tile2.parameter, tile2.state);
			}
		}
		layers.insert(tile2.layerID, true);
	}

	return true;
}

QImage BackgroundFile::background(bool hideBG) const
{
	int mimSize = mim.size(), palOffset = 4096, srcYWidth = 1664;
	QMultiMap<quint16, Tile> tiles;
	BackgroundBounds bounds;

	if(mimSize == 401408) {
		tiles = type1(bounds, hideBG);
		palOffset = 0;
		srcYWidth = 1536;
	} else if(mimSize == 438272) {
		tiles = type2(bounds, hideBG);
	} else {
		if (mimSize > 0) {
			qDebug() << "Error BackgroundFile::background" << mimSize;
		}
		return FF8Image::errorImage();
	}

	return toImage(palOffset, srcYWidth, tiles, bounds);
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

QMultiMap<quint16, Tile> BackgroundFile::type1(BackgroundBounds &bounds, bool hideBG) const
{
	int mapSize = map.size(), tilePos = 0,
	        sizeOfTile = mapSize - map.lastIndexOf("\xff\x7f");
	Tile1 tileType1;
	Tile tile;
	QMultiMap<quint16, Tile> tiles;
	const char *constMapData = map.constData();

	bounds = BackgroundBounds();

	if(sizeOfTile != 14 && sizeOfTile != 16) {
		qDebug() << "BackgroundFile::type1: invalid map size" << mapSize << sizeOfTile;
		return tiles;
	}

	while(tilePos + sizeOfTile <= mapSize) {
		memcpy(&tileType1, constMapData + tilePos, sizeOfTile);
		tile = Tile::fromTile1(tileType1, sizeOfTile);
		if(tile.X == 0x7fff) {
//			qDebug() << "Fin des tiles" << tilePos << mapSize;
			break;
		}
		//if (tile.state == 128) {
		    /* qDebug() << tileType1.X << tileType1.Y << tileType1.Z
					 << tileType1.srcX << tileType1.srcY
					 << tileType1.texID << tileType1.ZZ1
					 << tileType1.palID << tileType1.parameter
					 << tileType1.state; */
		//}
		if(qAbs(tile.X) < 1000 && qAbs(tile.Y) < 1000) {
			if(tile.X >= 0 && tile.X > bounds.right)
				bounds.right = tile.X;
			else if(tile.X < 0 && -tile.X > bounds.left)
				bounds.left = -tile.X;
			if(tile.Y >= 0 && tile.Y > bounds.bottom)
				bounds.bottom = tile.Y;
			else if(tile.Y < 0 && -tile.Y > bounds.top)
				bounds.top = -tile.Y;

			if(sizeOfTile < 16 || (!hideBG && tile.parameter == 255) ||
			    params.contains(tile.parameter, tile.state)) {
				tiles.insert(4096 - tile.Z, tile);
			}
		} else {
			qDebug() << "type1: out of bounds";
		}
		tilePos += sizeOfTile;
	}

	return tiles;
}

QMultiMap<quint16, Tile> BackgroundFile::type2(BackgroundBounds &bounds, bool hideBG) const
{
	int mapSize = map.size(), tilePos = 0, sizeOfTile = 16;
	Tile2 tileType2;
	Tile tile;
	QMultiMap<quint16, Tile> tiles;
	const char *constMapData = map.constData();

	bounds = BackgroundBounds();

//	QFile debug("C:/Users/vista/Documents/Deling/data/debug.txt");
//	debug.open(QIODevice::WriteOnly);

//	QImage imageDebugPal(QSize(64, 1024), QImage::Format_RGB32);
//	QRgb couleur;
//	x=y=0;
//	for(int i=2048 ; i<6144 ; ++i) {
//		memcpy(&color, &constMimData[i*2], 2);
//		couleur = FF8Image::fromPsColor(color);

//		imageDebugPal.setPixel(QPoint(x*4,y*4), couleur);
//		imageDebugPal.setPixel(QPoint(x*4,y*4+1), couleur);
//		imageDebugPal.setPixel(QPoint(x*4,y*4+2), couleur);
//		imageDebugPal.setPixel(QPoint(x*4,y*4+3), couleur);

//		imageDebugPal.setPixel(QPoint(x*4+1,y*4), couleur);
//		imageDebugPal.setPixel(QPoint(x*4+1,y*4+1), couleur);
//		imageDebugPal.setPixel(QPoint(x*4+1,y*4+2), couleur);
//		imageDebugPal.setPixel(QPoint(x*4+1,y*4+3), couleur);

//		imageDebugPal.setPixel(QPoint(x*4+2,y*4), couleur);
//		imageDebugPal.setPixel(QPoint(x*4+2,y*4+1), couleur);
//		imageDebugPal.setPixel(QPoint(x*4+2,y*4+2), couleur);
//		imageDebugPal.setPixel(QPoint(x*4+2,y*4+3), couleur);

//		imageDebugPal.setPixel(QPoint(x*4+3,y*4), couleur);
//		imageDebugPal.setPixel(QPoint(x*4+3,y*4+1), couleur);
//		imageDebugPal.setPixel(QPoint(x*4+3,y*4+2), couleur);
//		imageDebugPal.setPixel(QPoint(x*4+3,y*4+3), couleur);

//		++x;
//		if(x==16){
//			x=0;
//			++y;
//		}
//	}
//	imageDebugPal.save("C:/Users/vista/Documents/Deling/data/palette.png");

//	QImage imageDebugTex(QSize(1664/**2*/, 256), QImage::Format_RGB32);
//	for(int pal=0 ; pal<16 ; ++pal) {
//		int palStart2 = 4096+pal*512;
//		x=y=0;
//		for(int i=12288 ; i<438272 ; ++i) {
//			memcpy(&color, &constMimData[palStart2+((quint8)mim.at(i)/*&0xF*/)*2], 2);
//			imageDebugTex.setPixel(QPoint(x,y), FF8Image::fromPsColor(color));
//			++x;
////			memcpy(&color, &constMimData[palStart2+((quint8)mim.at(i)>>4)*2], 2);
////			imageDebugTex.setPixel(QPoint(x,y), FF8Image::fromPsColor(color));
////			++x;
//			if(x==1664/**2*/){
//				x=0;
//				++y;
//			}
//		}
//		imageDebugTex.save(QString("C:/Users/vista/Documents/Deling/data/texture%1.png").arg(pal));
//	}

//	qDebug() << "Type 2" << mapSize;

	while(tilePos + sizeOfTile <= mapSize) {
		memcpy(&tileType2, constMapData + tilePos, sizeOfTile);
		tile = Tile::fromTile2(tileType2);
		if(tile.X == 0x7fff) {
//			qDebug() << "Fin des tiles" << tilePos << mapSize;
			break;
		}

		// HACK
		if(tile.blendType >= 60) {
			return type1(bounds, hideBG);
		}

		if(qAbs(tile.X) < 1000 && qAbs(tile.Y) < 1000) {
			if(tile.X >= 0 && tile.X > bounds.right)
				bounds.right = tile.X;
			else if(tile.X < 0 && -tile.X > bounds.left)
				bounds.left = -tile.X;
			if(tile.Y >= 0 && tile.Y > bounds.bottom)
				bounds.bottom = tile.Y;
			else if(tile.Y < 0 && -tile.Y > bounds.top)
				bounds.top = -tile.Y;

			if(((!hideBG && tile.parameter == 255) ||
			    params.contains(tile.parameter, tile.state))
			        && layers.value(tile.layerID)) {

//			debug.write(
//				QString(QString("========== TILE %1 ==========\n").arg(tiles.size(),3)
//				+QString("X=%1 | Y=%2 | Z=%3\n").arg(tile.X,4).arg(tile.Y,4).arg(tile.Z,4)
//				+QString("texID=%1 | deph=%2 | palID=%3\n").arg(tile.texID,2).arg(tile.blend2,2).arg(tile.palID,2)
//				+QString("srcX=%1 | srcY=%2 | layerID=%3\n").arg(tile.srcX,3).arg(tile.srcY,3).arg(tile.layerID,3)
//				+QString("blendType=%1 | parameter=%2 | state=%3\n").arg(tile.blendType,3).arg(tile.parameter,3).arg(tile.state,3)
//				).toLatin1());

				tiles.insert(4096 - tile.Z, tile);
			}
		} else {
			qDebug() << "type2: out of bounds";
		}
		tilePos += sizeOfTile;
	}

	return tiles;
}

QImage BackgroundFile::toImage(int palOffset, int srcYWidth,
                               const QMultiMap<quint16, Tile> &tiles,
                               const BackgroundBounds &bounds) const
{
	int baseX, pos, x, y, palStart,
	        width = bounds.left + bounds.right + 16,
	        height = bounds.top + bounds.bottom + 16;
	qint16 color;
	quint8 index, blendType;
	const char *constMimData = mim.constData();
	QImage image(width, height, QImage::Format_RGB32);
	QRgb *pixels = (QRgb *)image.bits();
	image.fill(0xFF000000);

	foreach(const Tile &tile, tiles) {
		x = 0;
		y = (bounds.top + tile.Y) * width;
		baseX = bounds.left + tile.X;
		palStart = palOffset + tile.palID * 512;
		blendType = tile.blendType;
		pos = palOffset + 8192 + tile.texID * 128 + tile.srcY * srcYWidth;

		if(tile.depth) {
			pos += tile.srcX;

			for(int i=0 ; i<srcYWidth * 16 ; ++i) {
				memcpy(&color, constMimData + palStart + quint8(mim.at(pos+i)) * 2, 2);
				if(color != 0) {
					if(blendType < 4) {
						pixels[baseX + x + y] = BGcolor(color, blendType, pixels[baseX + x + y]);
					} else {
						pixels[baseX + x + y] = BGcolor(color);
					}
				}

				if(x == 15) {
					x = 0;
					i += srcYWidth - 16;
					y += width;
				} else {
					++x;
				}
			}
		} else {
			pos += tile.srcX / 2;

			for(int i=0 ; i<srcYWidth * 16 ; ++i) {
				index = quint8(mim.at(pos+i));
				memcpy(&color, constMimData + palStart + (index & 0xF) *2, 2);
				if(color != 0) {
					if(blendType < 4) {
						pixels[baseX + x + y] = BGcolor(color, blendType, pixels[baseX + x + y]);
					} else {
						pixels[baseX + x + y] = BGcolor(color);
					}
				}
				++x;

				memcpy(&color, constMimData + palStart + (index >> 4) * 2, 2);
				if(color != 0) {
					if(blendType < 4) {
						pixels[baseX + x + y] = BGcolor(color, blendType, pixels[baseX + x + y]);
					} else {
						pixels[baseX + x + y] = BGcolor(color);
					}
				}

				if(x==15) {
					x=0;
					i += srcYWidth - 8;
					y += width;
				} else {
					++x;
				}
			}
		}
	}

	return image;
}

QRgb BackgroundFile::BGcolor(int value, quint8 blendType, QRgb color0)
{
	QRgb color = FF8Image::fromPsColor(value);
	int r = qRed(color), g = qGreen(color), b = qBlue(color);

	switch(blendType) {
	case 0:
		r = (qRed(color0) + r)/2;
		g = (qGreen(color0) + r)/2;
		b = (qBlue(color0) + r)/2;
		break;
	case 1:
		r = qRed(color0) + r;
		if(r>255)	r = 255;
		g = qGreen(color0) + g;
		if(g>255)	g = 255;
		b = qBlue(color0) + b;
		if(b>255)	b = 255;
		break;
	case 2:
		r = qRed(color0) - r;
		if(r<0)	r = 0;
		g = qGreen(color0) - g;
		if(g<0)	g = 0;
		b = qBlue(color0) - b;
		if(b<0)	b = 0;
		break;
	case 3:
		r = qRed(color0) + int(0.25*r);
		if(r>255)	r = 255;
		g = qGreen(color0) + int(0.25*g);
		if(g>255)	g = 255;
		b = qBlue(color0) + int(0.25*b);
		if(b>255)	b = 255;
		break;
	}
	return qRgb(r,g,b);
}
