#include "MapFile.h"

QByteArray MapFile::mim = QByteArray();
QByteArray MapFile::map = QByteArray();

MapFile::MapFile() :
	modified(false)
{
}

bool MapFile::open(const QByteArray &map, const QByteArray &mim)
{
	int mimSize = mim.size(), mapSize = map.size(), tilePos=0;
	const char *constMapData = map.constData();

	allparams.clear();
	params.clear();
	layers.clear();

	if(mimSize == 401408) {
//		Tile1 tile1;
//		int sizeOfTile = mapSize-map.lastIndexOf("\xff\x7f");

//		if(mapSize%sizeOfTile != 0)	return;

//		while(tilePos+15 < mapSize)
//		{
//			memcpy(&tile1, &constMapData[tilePos], sizeOfTile);
//			if(tile1.X == 0x7fff) break;
//			tilePos += sizeOfTile;

//			if(tile1.parameter!=255 && !allparams.contains(tile1.parameter, tile1.state))
//			{
//				allparams.insert(tile1.parameter, tile1.state);
//				if(tile1.state==0)
//					params.insert(tile1.parameter, tile1.state);
//			}
//		}
	} else if(mimSize == 438272) {
		Tile2 tile2;

		while(tilePos+15 < mapSize)
		{
			memcpy(&tile2, &constMapData[tilePos], 16);
			if(tile2.X == 0x7fff) break;

			if(tile2.parameter!=255 && !allparams.contains(tile2.parameter, tile2.state))
			{
				allparams.insert(tile2.parameter, tile2.state);
				if(tile2.state==0)
					params.insert(tile2.parameter, tile2.state);
			}
			layers.insert(tile2.layerID, true);
			tilePos += 16;
		}
	}

	this->map = map;
	this->mim = mim;

	return true;
}

bool MapFile::isModified()
{
	return modified;
}

QPixmap MapFile::background()
{
	int mimSize = mim.size();

	if(mimSize == 401408)
		return type1();
	else if(mimSize == 438272)
		return type2();
	else
		return FF8Image::errorPixmap();
}

QPixmap MapFile::type1()
{
	int mapSize = map.size(), tilePos=0;
	Tile1 tile;
	QMultiMap<quint16, Tile1> tiles;
	const char *constMimData = mim.constData(), *constMapData = map.constData();

	int largeurMin=0, largeurMax=0, hauteurMin=0, hauteurMax=0, sizeOfTile = mapSize-map.lastIndexOf("\xff\x7f");
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
			memcpy(&color, &constMimData[palStart+((quint8)mim.at(pos+i))*2], 2);
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

QPixmap MapFile::type2()
{
	int mapSize = map.size(), tilePos=0, baseX, pos, x, y, palStart, largeurMin=0, largeurMax=0, hauteurMin=0, hauteurMax=0;
	Tile1 tileType1;
	Tile2 tile;
	QMultiMap<quint16, Tile2> tiles;
	qint16 color;
	quint8 index, blendType;
	const char *constMimData = mim.constData(), *constMapData = map.constData();
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
//			memcpy(&color, &constMimData[palStart2+((quint8)mim.at(i)/*&0xF*/)*2], 2);
//			imageDebugTex.setPixel(QPoint(x,y), qRgb((color & 31)*COEFF_COLOR, (color>>5 & 31)*COEFF_COLOR, (color>>10 & 31)*COEFF_COLOR));
//			++x;
////			memcpy(&color, &constMimData[palStart2+((quint8)mim.at(i)>>4)*2], 2);
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
				memcpy(&color, &constMimData[palStart+((quint8)mim.at(pos+i))*2], 2);
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
				index = (quint8)mim.at(pos+i);
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

QRgb MapFile::BGcolor(int value, quint8 blendType, QRgb color0)
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
