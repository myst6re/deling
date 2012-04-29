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
#include "FF8Image.h"

QPixmap FF8Image::lzs(const QByteArray &data)
{
	//QTime t;t.start();
	int lzs_size = 0, real_size = data.size();
	const char *lzs_constData = data.constData();

	memcpy(&lzs_size, lzs_constData, 4);
	if(lzs_size != real_size - 4)	return errorPixmap();

	const QByteArray &decdata = LZS::decompress(lzs_constData + 4, lzs_size);

	quint16 l, h;
	int size = decdata.size(), i=8;
	uint x=0, y=0;
	quint16 color;
	const char *constData = decdata.constData();

	memcpy(&l, &constData[4], 2);
	memcpy(&h, &constData[6], 2);
	//qDebug() << "Image.cpp >> var 'l' = " << l << " | var 'h' = " << h << " | size = " << (8+h*l*2) << " | realsize = " << size;

	QImage image(l, h, QImage::Format_RGB32);
	QRgb *pixels = (QRgb *)image.bits();

	while(i<size)
	{
		if(!image.valid(x, y))	break;

		memcpy(&color, &constData[i], 2);
		// $a = (color>>15)*127;
		// if(color>>15)
		// image.setPixel(QPoint(x, y), qRgb(0, 255, 0));
		// else
		pixels[x + y*l] = qRgb((color & 31)*COEFF_COLOR, (color>>5 & 31)*COEFF_COLOR, (color>>10 & 31)*COEFF_COLOR);

		++x;
		if(x==l)
		{
			x = 0;
			++y;
		}
		i += 2;
	}

	//qDebug() << t.elapsed();
	return QPixmap::fromImage(image);
}

QPixmap FF8Image::tex(const QByteArray &data, int palID)
{
	const char *constData = data.constData();

	quint32 l, h, nbPal, entreesPal, bitPerPx;

	// qDebug() << QString("%1 | %2 | %3 | %4 | %5 | %6").arg(QString(data.mid(0,4).toHex())).arg(QString(data.mid(4,4).toHex())).arg(QString(data.mid(20,4).toHex())).arg(QString(data.mid(24,4).toHex())).arg(QString(data.mid(32,4).toHex())).arg(QString(data.mid(36,4).toHex()));
	// qDebug() << QString("%1 | %2 | %3 | %4 | %5").arg(QString(data.mid(40,4).toHex())).arg(QString(data.mid(44,4).toHex())).arg(QString(data.mid(68,4).toHex())).arg(QString(data.mid(72,4).toHex())).arg(QString(data.mid(84,4).toHex()));
	memcpy(&nbPal, &constData[48], 4);
	memcpy(&l, &constData[60], 4);
	memcpy(&h, &constData[64], 4);
	memcpy(&bitPerPx, &constData[104], 4);
	// qDebug() << "Tex.cpp >> var 'l' = " << l << " | var 'h' = " << h;

	QImage image(l, h, QImage::Format_ARGB32);
	QRgb *pixels = (QRgb *)image.bits();

	int size = l*h*bitPerPx, i=0;
	quint32 x=0, y=0;

	if(nbPal > 0)
	{
		memcpy(&entreesPal, &constData[92], 4);
		quint32 index, imageStart = 240+entreesPal*4*nbPal, paletteStart = 240+entreesPal*4*palID;

		while(i<size)
		{
			if(!image.valid(x, y))	break;

			index = paletteStart+((quint8)data.at(imageStart+i))*4;
			pixels[x + y*l] = qRgba(data.at(index+2), data.at(index+1), data.at(index), data.at(index+3));

			++x;
			if(x==l)
			{
				x = 0;
				++y;
			}
			++i;
		}
	}
	else
	{
		quint16 color;

		while(i<size)
		{
			if(!image.valid(x, y))	break;

			memcpy(&color, &constData[240+i], 2);
			pixels[x + y*l] = qRgb((color & 31)*COEFF_COLOR, (color>>5 & 31)*COEFF_COLOR, (color>>10 & 31)*COEFF_COLOR);

			++x;
			if(x==l)
			{
				x = 0;
				++y;
			}
			i+=2;
		}
	}

	return QPixmap::fromImage(image);
}

QPixmap FF8Image::tim(const QByteArray &data, int palID)
{
	//QTime t;t.start();

	quint32 palSize=0, imgSize=0;
	quint16 palW, palH, w, h;
	quint8 bpp;
	const char *constData = data.constData();
	bool hasPal;
	int dataSize = data.size();
	QList<int> posPal;

	if(dataSize < 8)		return QPixmap();

//	quint8 tag = (quint8)data.at(0);
//	quint8 version = (quint8)data.at(1);
	bpp = (quint8)data.at(4) & 3;
	hasPal = ((quint8)data.at(4) >> 3) & 1;

//	qDebug() << QString("=== Apercu TIM ===");
//	qDebug() << QString("Tag = %1, version = %2, reste = %3").arg(tag).arg(version).arg(QString(data.mid(2,2).toHex()));
//	qDebug() << QString("bpp = %1, hasPal = %2, flag = %3, reste = %4").arg(bpp).arg(hasPal).arg((quint8)data.at(4),0,2).arg(QString(data.mid(5,3).toHex()));

	if(hasPal)
	{
		if(dataSize < 20)		return QPixmap();

		memcpy(&palSize, &constData[8], 4);
		memcpy(&palW, &constData[16], 2);
		memcpy(&palH, &constData[18], 2);
		if((quint32)dataSize < 8+palSize/* || palSize != (quint32)palW*palH*2+12*/)	return QPixmap();

		quint16 onePalSize = (bpp==0 ? 16 : 256);
		int nbPal = (palSize-12)/(onePalSize*2);
		nbPal += ((palSize-12)%(onePalSize*2)) !=0 ? nbPal : 0;
		if(nbPal==1)	posPal << 0;
		else if(nbPal>1)
		{
			int pos=0;
			for(int i=1 ; i<=nbPal ; ++i)
			{
				posPal << pos;
				pos += pos % palW == 0 ? onePalSize : palW - onePalSize;
			}
		}
		palID %= nbPal;
//		qDebug() << QString("-Palette-");
//		qDebug() << QString("Size = %1, w = %2, h = %3").arg(palSize).arg(palW).arg(palH);
//		qDebug() << QString("NbPal = %1 (valid : %2)").arg(nbPal).arg((palSize-12)%(onePalSize*2));
//		QString str="";
//		foreach(int posp, posPal)
//		{
//			str += QString("%1 | ").arg(posp);
//		}
//		qDebug() << QString("PosPal = %1").arg(str);
	}

	if((quint32)dataSize < 20+palSize)		return QPixmap();

	memcpy(&imgSize, &constData[8+palSize], 4);
	memcpy(&w, &constData[16+palSize], 2);
	memcpy(&h, &constData[18+palSize], 2);
	if(bpp==0)		w*=4;
	else if(bpp==1)	w*=2;

//	qDebug() << QString("-Image-");
//	qDebug() << QString("Size = %1, w = %2, h = %3").arg(imgSize).arg(w).arg(h);
//	qDebug() << QString("TIM Size = %1").arg(8+palSize+imgSize);

	QImage image(w, h, QImage::Format_RGB32);
	QRgb *pixels = (QRgb *)image.bits();

	int size, i=0;
	quint32 x=0, y=0;
	quint32 color=0;

	if(bpp!=0) {
		size = qMin((quint32)w*h*bpp, dataSize - 8 - palSize);
	} else {
		size = qMin((quint32)w/2*h, dataSize - 8 - palSize);
	}

	if(8 + palSize + size > (quint32)dataSize)
		return QPixmap();

	if(bpp==0)//mag176, icon
	{
		while(i<size)
		{
			if(!image.valid(x, y))	break;

			memcpy(&color, &constData[20+posPal.at(palID)*2+((quint8)data.at(20+palSize+i)&15)*2], 2);
			pixels[x + y*w] = qRgb((color & 31)*COEFF_COLOR, (color>>5 & 31)*COEFF_COLOR, (color>>10 & 31)*COEFF_COLOR);
			++x;
			if(x==w)
			{
				x = 0;
				++y;
			}
			if(!image.valid(x, y))	break;

			memcpy(&color, &constData[20+posPal.at(palID)*2+((quint8)data.at(20+palSize+i)>>4)*2], 2);
			pixels[x + y*w] = qRgb((color & 31)*COEFF_COLOR, (color>>5 & 31)*COEFF_COLOR, (color>>10 & 31)*COEFF_COLOR);
			++x;
			if(x==w)
			{
				x = 0;
				++y;
			}
			++i;
		}
	}
	else if(bpp==1)
	{
		while(i<size)
		{
			if(!image.valid(x, y))	break;

			memcpy(&color, &constData[20+posPal.at(palID)*2+(quint8)data.at(20+palSize+i)*2], 2);
			pixels[x + y*w] = qRgb((color & 31)*COEFF_COLOR, (color>>5 & 31)*COEFF_COLOR, (color>>10 & 31)*COEFF_COLOR);

			++x;
			if(x==w)
			{
				x = 0;
				++y;
			}
			++i;
		}
	}
	else if(bpp==2)
	{
		while(i<size)
		{
			if(!image.valid(x, y))	break;

			memcpy(&color, &constData[20+palSize+i], 2);
			pixels[x + y*w] = qRgb((color & 31)*COEFF_COLOR, (color>>5 & 31)*COEFF_COLOR, (color>>10 & 31)*COEFF_COLOR);

			++x;
			if(x==w)
			{
				x = 0;
				++y;
			}
			i+=2;
		}
	}
	else if(bpp==3)
	{
		while(i<size)
		{
			if(!image.valid(x, y))	break;

			memcpy(&color, &constData[20+palSize+i], 3);
			pixels[x + y*w] = qRgb(color >> 16, (color >> 8) & 0xFF, color & 0xFF);

			++x;
			if(x==w)
			{
				x = 0;
				++y;
			}
			i+=3;
		}
	}

	//qDebug() << t.elapsed();
	return QPixmap::fromImage(image);
}

QPixmap FF8Image::tdw(const QByteArray &data, int palID)
{
	quint32 pos;
	const char *constData = data.constData();

	if(data.size() <= 8) {
		return QPixmap();
	}
	memcpy(&pos, &constData[4], 4);

	return tim(data.mid(pos), palID);
}

int FF8Image::findFirstTim(const QByteArray &data)
{
	int index = -1, dataSize = data.size();
	quint32 palSize, imgSize;
	quint16 w, h;
	quint8 bpp;
	const char *constData = data.constData();
	QByteArray search("\x10\x00\x00\x00",4);

	while( (index = data.indexOf(search, index+4)) != -1 )
	{
		palSize=0;
		if(index+8 > dataSize)		continue;
		bpp = (quint8)data.at(index+4);

//		qDebug() << "bpp" << bpp;

		if(bpp==8 || bpp==9)
		{
			memcpy(&palSize, &constData[index+8], 4);
//			qDebug() << "palSize" << palSize;
			if(index+20 > dataSize)	continue;
			memcpy(&w, &constData[index+16], 2);
			memcpy(&h, &constData[index+18], 2);
//			qDebug() << "w" << w << "h" << h;
			if(palSize != (quint32)w*h*2+12)	continue;
		}
		else if(bpp!=2 && bpp!=3)	continue;

		memcpy(&imgSize, &constData[index+8+palSize], 4);
//		qDebug() << "imgSize" << imgSize;
		if((quint32)(index+20+palSize) > (quint32)dataSize)	continue;
		memcpy(&w, &constData[index+16+palSize], 2);
		memcpy(&h, &constData[index+18+palSize], 2);
//		qDebug() << "w" << w << "h" << h;
		if(((bpp==8 || bpp==9 || bpp==2) && imgSize != (quint32)w*2*h+12) || (bpp==3 && imgSize != (quint32)w*3*h+12))	continue;

		return index;
	}
	return -1;
}

int FF8Image::findTims(const QByteArray &data)
{
	int index = -1, oldIndex = 0, dataSize = data.size();
	quint32 palSize, imgSize;
	quint16 w, h, i=0;
	quint8 bpp;
	const char *constData = data.constData();

	while( (index = data.indexOf(QByteArray("\x10\x00\x00\x00",4), index+1)) != -1 )
	{
		palSize=0;
		if(index+4 >= dataSize)		continue;
		bpp = (quint8)data.at(index+4);

		if(bpp==8 || bpp==9)
		{
			memcpy(&palSize, &constData[index+8], 4);
			if(index+20 >= dataSize)	continue;
			memcpy(&w, &constData[index+16], 2);
			memcpy(&h, &constData[index+18], 2);
			if(palSize != (quint32)w*h*2+12)	continue;
		}
		else if(bpp!=2 && bpp!=3)	continue;

		memcpy(&imgSize, &constData[index+8+palSize], 4);
		if((quint32)index+20+palSize >= (quint32)dataSize)	continue;
		memcpy(&w, &constData[index+16+palSize], 2);
		memcpy(&h, &constData[index+18+palSize], 2);
		if(((bpp==8 || bpp==9 || bpp==2) && imgSize != (quint32)w*2*h+12) || (bpp==3 && imgSize != (quint32)w*3*h+12))	continue;

//		QFile fic(QString("C:/wamp/www/ff8/fils/fre/world/dat/wmsetfr%1.tim").arg(i++));
//		fic.open(QIODevice::WriteOnly);
//		fic.write(data.mid(index, 8+palSize+imgSize));
//		fic.close();
		qDebug() << "espace" << (index-oldIndex) << "position" << QString().setNum(oldIndex, 16);
		qDebug() << QString().setNum(index, 16) << "(taille=" << (8+palSize+imgSize) << ")";
		oldIndex = index+8+palSize+imgSize;
//		Tim::image(data.mid(index, 8+palSize+imgSize), QString("C:/wamp/www/ff8/fils/fre/world/esk/texl%1.png").arg(i++));
	}
	qDebug() << "espace" << (dataSize-oldIndex) << "position" << QString().setNum(oldIndex, 16);

	return i+1;
}

QPixmap FF8Image::background(const QByteArray &mapData, const QByteArray &mimData)
{
	int mimSize = mimData.size();

	if(mimSize == 401408)
		return type1(mapData, mimData/*, params*/);
	else if(mimSize == 438272) {
		QMultiMap<quint8, quint8> params;
		QMap<quint8, bool> layers;
		return type2(mapData, mimData, params, layers);
	}
	else
		return FF8Image::errorPixmap();
}

QPixmap FF8Image::type1(const QByteArray &mapData, const QByteArray &mimData/*, const QMultiMap<quint8, quint8> &params*/)
{
	int mapSize = mapData.size(), tilePos=0;
	Tile1 tile;
	QMultiMap<quint16, Tile1> tiles;
	const char *constMimData = mimData.constData(), *constMapData = mapData.constData();

	int largeurMin=0, largeurMax=0, hauteurMin=0, hauteurMax=0, sizeOfTile = mapSize-mapData.lastIndexOf("\xff\x7f");
	if(mapSize%sizeOfTile != 0)	return QPixmap();

//	qDebug() << "Type 1";

	while(tilePos+15 < mapSize)
	{
		memcpy(&tile, &constMapData[tilePos], sizeOfTile);
		if(tile.X == 0x7fff) {
//			qDebug() << "Fin des tiles" << tilePos << mapSize;
			break;
		}
		tilePos += sizeOfTile;
		if(qAbs(tile.X)<1000 && qAbs(tile.Y)<1000) {
			if(tile.X >= 0 && tile.X > largeurMax)
				largeurMax = tile.X;
			else if(tile.X < 0 && -tile.X > largeurMin)
				largeurMin = -tile.X;
			if(tile.Y >= 0 && tile.Y > hauteurMax)
				hauteurMax = tile.Y;
			else if(tile.Y < 0 && -tile.Y > hauteurMin)
				hauteurMin = -tile.Y;

//			if(tile.parameter==255 || params.isEmpty() || params.contains(tile.parameter, tile.state))
//			{
				tiles.insert(4096-tile.Z, tile);
//			}
		}
	}

	int width = largeurMin+largeurMax+16;
	QImage image(width, hauteurMin+hauteurMax+16, QImage::Format_RGB32);
	QRgb *pixels = (QRgb *)image.bits();
	image.fill(0xFF000000);

	int baseX, pos, x, y, palStart;
	qint16 color;

	foreach(const Tile1 &tile, tiles)
	{
		pos = 8192 + tile.texID*128 + tile.srcX + 1536*tile.srcY;
		x = 0;
		y = (hauteurMin + tile.Y) * width;
		baseX = largeurMin + tile.X;
		palStart = tile.palID*512;

		for(int i=0 ; i<24576 ; ++i) {
			memcpy(&color, &constMimData[palStart+((quint8)mimData.at(pos+i))*2], 2);
			if(color!=0)
				pixels[baseX+x + y] = qRgb((color & 31)*COEFF_COLOR, (color>>5 & 31)*COEFF_COLOR, (color>>10 & 31)*COEFF_COLOR);

			if(x==15) {
				x=0;
				i += 1520;
				y += width;
			}
			else
				++x;
		}
	}

	return QPixmap::fromImage(image);
}

QPixmap FF8Image::type2(const QByteArray &mapData, const QByteArray &mimData, const QMultiMap<quint8, quint8> &params, const QMap<quint8, bool> &layers)
{
	int mapSize = mapData.size(), tilePos=0, baseX, pos, x, y, palStart, largeurMin=0, largeurMax=0, hauteurMin=0, hauteurMax=0;
	Tile1 tileType1;
	Tile2 tile;
	QMultiMap<quint16, Tile2> tiles;
	qint16 color;
	quint8 index, blendType;
	const char *constMimData = mimData.constData(), *constMapData = mapData.constData();
//	QFile debug("C:/Users/vista/Documents/Deling/data/debug.txt");
//	debug.open(QIODevice::WriteOnly);

//	QImage imageDebugPal(QSize(64, 1024), QImage::Format_RGB32);
//	QRgb couleur;
//	x=y=0;
//	for(int i=2048 ; i<6144 ; ++i) {
//		memcpy(&color, &constMimData[i*2], 2);
//		couleur = qRgb((color & 31)*COEFF_COLOR, (color>>5 & 31)*COEFF_COLOR, (color>>10 & 31)*COEFF_COLOR);

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
//			memcpy(&color, &constMimData[palStart2+((quint8)mimData.at(i)/*&0xF*/)*2], 2);
//			imageDebugTex.setPixel(QPoint(x,y), qRgb((color & 31)*COEFF_COLOR, (color>>5 & 31)*COEFF_COLOR, (color>>10 & 31)*COEFF_COLOR));
//			++x;
////			memcpy(&color, &constMimData[palStart2+((quint8)mimData.at(i)>>4)*2], 2);
////			imageDebugTex.setPixel(QPoint(x,y), qRgb((color & 31)*COEFF_COLOR, (color>>5 & 31)*COEFF_COLOR, (color>>10 & 31)*COEFF_COLOR));
////			++x;
//			if(x==1664/**2*/){
//				x=0;
//				++y;
//			}
//		}
//		imageDebugTex.save(QString("C:/Users/vista/Documents/Deling/data/texture%1.png").arg(pal));
//	}

//	qDebug() << "Type 2" << mapSize;

	while(tilePos+15 < mapSize)
	{
		memcpy(&tile, &constMapData[tilePos], 16);
		if(tile.X == 0x7fff) {
//			qDebug() << "Fin des tiles" << tilePos << mapSize;
			break;
		}

		if(qAbs(tile.X)<1000 && qAbs(tile.Y)<1000) {
			if(tile.X >= 0 && tile.X > largeurMax)
				largeurMax = tile.X;
			else if(tile.X < 0 && -tile.X > largeurMin)
				largeurMin = -tile.X;
			if(tile.Y >= 0 && tile.Y > hauteurMax)
				hauteurMax = tile.Y;
			else if(tile.Y < 0 && -tile.Y > hauteurMin)
				hauteurMin = -tile.Y;

			if((tile.parameter==255 || params.isEmpty() || params.contains(tile.parameter, tile.state)) && (layers.value(tile.layerID) || layers.isEmpty()))
			{
				// HACK
				if(tile.blendType>=60)
				{
					memcpy(&tileType1, &constMapData[tilePos], 16);
					tile.X = tileType1.X;
					tile.Y = tileType1.Y;
					tile.Z = tileType1.Z;
					tile.texID = tileType1.texID;
					tile.blend1 = tileType1.blend1;
					tile.blend2 = tileType1.blend2;
					tile.blendType = 4;
					tile.layerID = 0;
					tile.palID = tileType1.palID;
					tile.srcX = tileType1.srcX;
					tile.srcY = tileType1.srcY;
					tile.parameter = tileType1.parameter;
					tile.state = tileType1.state;
				}

	//			debug.write(
	//				QString(QString("========== TILE %1 ==========\n").arg(tiles.size(),3)
	//				+QString("X=%1 | Y=%2 | Z=%3\n").arg(tile.X,4).arg(tile.Y,4).arg(tile.Z,4)
	//				+QString("texID=%1 | zz2=%2 | palID=%3\n").arg(tile.texID,2).arg(tile.blend2,2).arg(tile.palID,2)
	//				+QString("srcX=%1 | srcY=%2 | layerID=%3\n").arg(tile.srcX,3).arg(tile.srcY,3).arg(tile.layerID,3)
	//				+QString("blendType=%1 | parameter=%2 | state=%3\n").arg(tile.blendType,3).arg(tile.parameter,3).arg(tile.state,3)
	//				).toLatin1());
				tiles.insert(4096-tile.Z, tile);
			}
		}
		tilePos += 16;
	}

	int width = largeurMin+largeurMax+16;
	QImage image(width, hauteurMin+hauteurMax+16, QImage::Format_RGB32);
	image.fill(0xFF000000);
	QRgb *pixels = (QRgb *)image.bits();

	foreach(const Tile2 &tile, tiles)
	{
		x = 0;
		y = (hauteurMin + tile.Y) * width;
		baseX = largeurMin + tile.X;
		palStart = 4096+tile.palID*512;
		blendType = tile.blendType;

		if(tile.blend2>=4) {
			pos = 12288 + tile.texID*128 + tile.srcX + 1664*tile.srcY;
			for(int i=0 ; i<26624 ; ++i) {
				memcpy(&color, &constMimData[palStart+((quint8)mimData.at(pos+i))*2], 2);
				if(color!=0) {
					if(blendType<4)
						pixels[baseX + x + y] = BGcolor(color, blendType, pixels[baseX + x + y]);
					else
						pixels[baseX + x + y] = BGcolor(color);
				}

				if(x==15) {
					x=0;
					i += 1648;
					y += width;
				}
				else
					++x;
			}
		}
		else {
			pos = 12288 + tile.texID*128 + tile.srcX/2 + 1664*tile.srcY;
			for(int i=0 ; i<26624 ; ++i) {
				index = (quint8)mimData.at(pos+i);
				memcpy(&color, &constMimData[palStart+(index&0xF)*2], 2);
				if(color!=0) {
					if(blendType<4)
						pixels[baseX + x + y] = BGcolor(color, blendType, pixels[baseX + x + y]);
					else
						pixels[baseX + x + y] = BGcolor(color);
				}
				++x;

				memcpy(&color, &constMimData[palStart+(index>>4)*2], 2);
				if(color!=0) {
					if(blendType<4)
						pixels[baseX + x + y] = BGcolor(color, blendType, pixels[baseX + x + y]);
					else
						pixels[baseX + x + y] = BGcolor(color);
				}

				if(x==15) {
					x=0;
					i += 1656;
					y += width;
				}
				else
					++x;
			}
		}
	}

	return QPixmap::fromImage(image);
}

QRgb FF8Image::BGcolor(int value, quint8 blendType, QRgb color0)
{
	int r = (value & 31)*COEFF_COLOR, g = (value>>5 & 31)*COEFF_COLOR, b = (value>>10 & 31)*COEFF_COLOR;

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
		r = qRed(color0) + 0.25*r;
		if(r>255)	r = 255;
		g = qGreen(color0) + 0.25*g;
		if(g>255)	g = 255;
		b = qBlue(color0) + 0.25*b;
		if(b>255)	b = 255;
		break;
	}
	return qRgb(r,g,b);
}

QPixmap FF8Image::errorPixmap()
{
	QPixmap errorPix(320, 224);
	errorPix.fill(Qt::black);
	QFont font;
	font.setPixelSize(44);
	QString text = QObject::tr("Erreur");
	int textWidth = QFontMetrics(font).width(text);
	int textHeight = QFontMetrics(font).height();

	QPainter p(&errorPix);
	p.setPen(Qt::white);
	p.setFont(font);
	p.drawStaticText((320-textWidth)/2, (224-textHeight)/2, QStaticText(text));
	p.end();

	return errorPix;
}
