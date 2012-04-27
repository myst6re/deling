#include "BGThread.h"

void BGThread::run()
{
	int mimSize = mimData.size();
	QPixmap image;

	if(mimSize == 401408)
	{
//		qDebug() << "type1";
		image = type1();
	}
	else if(mimSize == 438272)
	{
//		qDebug() << "type2";
		QTime t;
		t.start();
		image = type2();
		qDebug() << "génération bg : " << t.elapsed();
	}
	else {
		qDebug() << "taille mim invalide";
	}
	emit BG(image);
}

QPixmap BGThread::type1()
{/*
	int mapSize = mapData.size(), tilePos=0;
	Tile1 tile;
	QMultiMap<quint16, Tile1> tiles;
//	QFile debug("C:/Users/vista/Documents/Deling/data/debug.txt");
//	debug.open(QIODevice::WriteOnly);

//	QImage imageDebugPal(QSize(64, 1024), QImage::Format_RGB32);
//	int x=0, y=0;
//	quint16 colore;
//	QRgb couleure;
//	for(int i=0 ; i<4096 ; ++i) {
//		memcpy(&colore, mimData.mid(i*2, 2), 2);
//		couleure = qRgb((colore & 31)*8.2258, (colore>>5 & 31)*8.2258, (colore>>10 & 31)*8.2258);
//
//		imageDebugPal.setPixel(QPoint(x*4,y*4), couleure);
//		imageDebugPal.setPixel(QPoint(x*4,y*4+1), couleure);
//		imageDebugPal.setPixel(QPoint(x*4,y*4+2), couleure);
//		imageDebugPal.setPixel(QPoint(x*4,y*4+3), couleure);
//
//		imageDebugPal.setPixel(QPoint(x*4+1,y*4), couleure);
//		imageDebugPal.setPixel(QPoint(x*4+1,y*4+1), couleure);
//		imageDebugPal.setPixel(QPoint(x*4+1,y*4+2), couleure);
//		imageDebugPal.setPixel(QPoint(x*4+1,y*4+3), couleure);
//
//		imageDebugPal.setPixel(QPoint(x*4+2,y*4), couleure);
//		imageDebugPal.setPixel(QPoint(x*4+2,y*4+1), couleure);
//		imageDebugPal.setPixel(QPoint(x*4+2,y*4+2), couleure);
//		imageDebugPal.setPixel(QPoint(x*4+2,y*4+3), couleure);
//
//		imageDebugPal.setPixel(QPoint(x*4+3,y*4), couleure);
//		imageDebugPal.setPixel(QPoint(x*4+3,y*4+1), couleure);
//		imageDebugPal.setPixel(QPoint(x*4+3,y*4+2), couleure);
//		imageDebugPal.setPixel(QPoint(x*4+3,y*4+3), couleure);
//
//		++x;
//		if(x==16){
//			x=0;
//			++y;
//		}
//	}
//	imageDebugPal.save("C:/Users/vista/Documents/Deling/data/palette.png");
//
//	QImage imageDebugTex(QSize(1536, 256), QImage::Format_RGB32);
//	for(int pal=0 ; pal<16 ; ++pal) {
//		int palStart2 = pal*512;
//		x=y=0;
//		for(int i=8192 ; i<401408 ; ++i) {
//			memcpy(&colore, mimData.mid(palStart2+((quint8)mimData.at(i))*2, 2), 2);
//			imageDebugTex.setPixel(QPoint(x,y), qRgb((colore & 31)*8.2258, (colore>>5 & 31)*8.2258, (colore>>10 & 31)*8.2258));
//			++x;
//			if(x==1536){
//				x=0;
//				++y;
//			}
//		}
//		imageDebugTex.save(QString("C:/Users/vista/Documents/Deling/data/texture%1.png").arg(pal));
//	}

	int largeurMin=0, largeurMax=0, hauteurMin=0, hauteurMax=0, sizeOfTile = mapSize-mapData.lastIndexOf("\xff\x7f");
	if(mapSize%sizeOfTile != 0)	return QPixmap();

	while(tilePos+15 < mapSize)
	{
		memcpy(&tile, mapData.mid(tilePos,sizeOfTile), sizeOfTile);
//		debug.write(QString("========== TILE %1 ==========\r\n").arg(tiles.size(),3).toLatin1());
//		debug.write(QString("X=%1 | Y=%2 | Z=%3\r\n").arg(tile.X,4).arg(tile.Y,4).arg(tile.Z,4).toLatin1());
//		debug.write(QString("texID=%1 | zz2=%2 | palID=%3\r\n").arg(tile.texID,2).arg(tile.blend2,2).arg(tile.palID,2).toLatin1());
//		debug.write(QString("srcX=%1 | srcY=%2\r\n").arg(tile.srcX,3).arg(tile.srcY,3).toLatin1());
		if(tile.X == 0x7fff) break;
		if(tile.X>-1000 && tile.X<1000 && tile.Y>-1000 && tile.Y<1000) {
			if(tile.X >= 0 && tile.X > largeurMax)
				largeurMax = tile.X;
			else if(tile.X < 0 && -tile.X > largeurMin)
				largeurMin = -tile.X;
			if(tile.Y >= 0 && tile.Y > hauteurMax)
				hauteurMax = tile.Y;
			else if(tile.Y < 0 && -tile.Y > hauteurMin)
				hauteurMin = -tile.Y;
			tiles.insert(4096-tile.Z, tile);
		}

		tilePos += sizeOfTile;
	}

	QImage image(QSize(largeurMin+largeurMax+16, hauteurMin+hauteurMax+16), QImage::Format_RGB32);
	image.fill(0xFF000000);

	int baseX, pos, right, top, palStart;
	qint16 color;

	foreach(Tile1 tile, tiles)
	{
		pos = 8192 + tile.texID*128 + tile.srcX + 1536*tile.srcY;
		right = 0;
		top = hauteurMin + tile.Y;
		baseX = largeurMin + tile.X;
		palStart = tile.palID*512;

		for(int i=0 ; i<24576 ; ++i) {
			memcpy(&color, mimData.mid(palStart+((quint8)mimData.at(pos+i))*2, 2), 2);
			if(color!=0)
				image.setPixel(QPoint(baseX+right, top), qRgb((color & 31)*8.2258, (color>>5 & 31)*8.2258, (color>>10 & 31)*8.2258));

			++right;
			if(right==16){
				right=0;
				i += 1520;
				++top;
			}
		}
	}

	return QPixmap::fromImage(image);*/
}

QPixmap BGThread::type2()
{
	/*int mapSize = mapData.size(), tilePos=0;
	Tile2 tile;
	QMultiMap<quint16, Tile2> tiles;
//	QFile debug("C:/Users/vista/Documents/Deling/data/debug.txt");
//	debug.open(QIODevice::WriteOnly);
//
//	QImage imageDebugPal(QSize(64, 1024), QImage::Format_RGB32);
//	int x=0, y=0;
//	quint16 colore;
//	QRgb couleure;
//	for(int i=2048 ; i<6144 ; ++i) {
//		memcpy(&colore, mimData.mid(i*2, 2), 2);
//		couleure = qRgb((colore & 31)*8.2258, (colore>>5 & 31)*8.2258, (colore>>10 & 31)*8.2258);
//
//		imageDebugPal.setPixel(QPoint(x*4,y*4), couleure);
//		imageDebugPal.setPixel(QPoint(x*4,y*4+1), couleure);
//		imageDebugPal.setPixel(QPoint(x*4,y*4+2), couleure);
//		imageDebugPal.setPixel(QPoint(x*4,y*4+3), couleure);
//
//		imageDebugPal.setPixel(QPoint(x*4+1,y*4), couleure);
//		imageDebugPal.setPixel(QPoint(x*4+1,y*4+1), couleure);
//		imageDebugPal.setPixel(QPoint(x*4+1,y*4+2), couleure);
//		imageDebugPal.setPixel(QPoint(x*4+1,y*4+3), couleure);
//
//		imageDebugPal.setPixel(QPoint(x*4+2,y*4), couleure);
//		imageDebugPal.setPixel(QPoint(x*4+2,y*4+1), couleure);
//		imageDebugPal.setPixel(QPoint(x*4+2,y*4+2), couleure);
//		imageDebugPal.setPixel(QPoint(x*4+2,y*4+3), couleure);
//
//		imageDebugPal.setPixel(QPoint(x*4+3,y*4), couleure);
//		imageDebugPal.setPixel(QPoint(x*4+3,y*4+1), couleure);
//		imageDebugPal.setPixel(QPoint(x*4+3,y*4+2), couleure);
//		imageDebugPal.setPixel(QPoint(x*4+3,y*4+3), couleure);
//
//		++x;
//		if(x==16){
//			x=0;
//			++y;
//		}
//	}
//	imageDebugPal.save("C:/Users/vista/Documents/Deling/data/palette.png");
//
//	QImage imageDebugTex(QSize(1664, 256), QImage::Format_RGB32);
//	for(int pal=0 ; pal<16 ; ++pal) {
//		int palStart2 = 4096+pal*512;
//		x=y=0;
//		for(int i=12288 ; i<438272 ; ++i) {
//			memcpy(&colore, mimData.mid(palStart2+((quint8)mimData.at(i))*2, 2), 2);
//			imageDebugTex.setPixel(QPoint(x,y), qRgb((colore & 31)*8.2258, (colore>>5 & 31)*8.2258, (colore>>10 & 31)*8.2258));
//			++x;
//			if(x==1664){
//				x=0;
//				++y;
//			}
//		}
//		imageDebugTex.save(QString("C:/Users/vista/Documents/Deling/data/texture%1.png").arg(pal));
//	}

	int largeurMin=0, largeurMax=0, hauteurMin=0, hauteurMax=0;

	while(tilePos+15 < mapSize)
	{
		memcpy(&tile, mapData.mid(tilePos,16), 16);
//		debug.write(QString("========== TILE %1 ==========\r\n").arg(tiles.size(),3).toLatin1());
//		debug.write(QString("X=%1 | Y=%2 | Z=%3\r\n").arg(tile.X,4).arg(tile.Y,4).arg(tile.Z,4).toLatin1());
//		debug.write(QString("texID=%1 | zz2=%2 | palID=%3\r\n").arg(tile.texID,2).arg(tile.blend2,2).arg(tile.palID,2).toLatin1());
//		debug.write(QString("srcX=%1 | srcY=%2 | layerID=%3\r\n").arg(tile.srcX,3).arg(tile.srcY,3).arg(tile.layerID,3).toLatin1());
//		debug.write(QString("blendType=%1 | parameter=%2 | state=%3\r\n").arg(tile.blendType,3).arg(tile.parameter,3).arg(tile.state,3).toLatin1());
		if(tile.X == 0x7fff) break;
		if(tile.X >= 0 && tile.X > largeurMax)
			largeurMax = tile.X;
		else if(tile.X < 0 && -tile.X > largeurMin)
			largeurMin = -tile.X;
		if(tile.Y >= 0 && tile.Y > hauteurMax)
			hauteurMax = tile.Y;
		else if(tile.Y < 0 && -tile.Y > hauteurMin)
			hauteurMin = -tile.Y;
		tiles.insert(4096-tile.Z, tile);

		tilePos += 16;
	}

	QImage image(QSize(largeurMin+largeurMax+16, hauteurMin+hauteurMax+16), QImage::Format_RGB32);
	image.fill(0xFF000000);

	int baseX, pos, right, top, palStart, r, g, b;
	qint16 color;
	QRgb couleur0, couleur1;

	foreach(Tile2 tile, tiles)
	{
		if(tile.layerID>0 || tile.blend2>4)	continue;
		pos = 12288 + tile.texID*128 + tile.srcX + 1664*tile.srcY;
		right = 0;
		top = hauteurMin + tile.Y;
		baseX = largeurMin + tile.X;
		palStart = 4096+tile.palID*512;

		for(int i=0 ; i<26624 ; ++i) {
			memcpy(&color, mimData.mid(palStart+((quint8)mimData.at(pos+i))*2, 2), 2);
			if(color!=0) {
				if(tile.blendType!=4 && tile.blendType!=2) {
					couleur0 = image.pixel(QPoint(baseX + right, top));
					couleur1 = qRgb((color & 31)*8.2258, (color>>5 & 31)*8.2258, (color>>10 & 31)*8.2258);
					switch(tile.blendType) {
					case 1:
						r = qRed(couleur0) + qRed(couleur1);
						if(r>255)	r = 255;
						g = qGreen(couleur0) + qGreen(couleur1);
						if(g>255)	g = 255;
						b = qBlue(couleur0) + qBlue(couleur1);
						if(b>255)	b = 255;
//						r=255;
//						g=b=0;
						break;
					case 2:
						r = qRed(couleur0) - qRed(couleur1);
						if(r<0)	r = 0;
						g = qGreen(couleur0) - qGreen(couleur1);
						if(g<0)	g = 0;
						b = qBlue(couleur0) - qBlue(couleur1);
						if(b<0)	b = 0;
//						r=255;
//						g=b=0;
						break;
					case 3:
						r = qRed(couleur0) + 0.25*qRed(couleur1);
						if(r>255)	r = 255;
						g = qGreen(couleur0) + 0.25*qGreen(couleur1);
						if(g>255)	g = 255;
						b = qBlue(couleur0) + 0.25*qBlue(couleur1);
						if(b>255)	b = 255;
//						r=255;
//						g=b=0;
						break;
					default://0
						r = (qRed(couleur0) + qRed(couleur1))/2;
						g = (qGreen(couleur0) + qGreen(couleur1))/2;
						b = (qBlue(couleur0) + qBlue(couleur1))/2;
						break;
					}
					image.setPixel(QPoint(baseX + right, top), qRgb(r,g,b));
				}
				else {
					image.setPixel(QPoint(baseX + right, top), qRgb((color & 31)*8.2258, (color>>5 & 31)*8.2258, (color>>10 & 31)*8.2258));
				}
//				if(tile.ZZ6>0) {
//					image.setPixel(QPoint(baseX + right, top), qRgb(255, 0, 0));
//				}
			}

			++right;
			if(right==16){
				right=0;
				i += 1648;
				++top;
			}
		}
	}

	return QPixmap::fromImage(image);*/
}
