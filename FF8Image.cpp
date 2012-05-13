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

	if(!data.startsWith(QByteArray("\x10\x00\x00\x00", 4)) || dataSize < 8)		return QPixmap();

	quint8 tag = (quint8)data.at(0);
	quint8 version = (quint8)data.at(1);
	bpp = (quint8)data.at(4) & 3;
	hasPal = ((quint8)data.at(4) >> 3) & 1;

	qDebug() << QString("=== Apercu TIM ===");
	qDebug() << QString("Tag = %1, version = %2, reste = %3").arg(tag).arg(version).arg(QString(data.mid(2,2).toHex()));
	qDebug() << QString("bpp = %1, hasPal = %2, flag = %3, reste = %4").arg(bpp).arg(hasPal).arg((quint8)data.at(4),0,2).arg(QString(data.mid(5,3).toHex()));

	if(hasPal && bpp > 1)		return QPixmap();

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
		if(nbPal==1)	posPal.append(0);
		else if(nbPal>1)
		{
			int pos=0;
			for(int i=1 ; i<=nbPal ; ++i)
			{
				posPal.append(pos);
				pos += pos % palW == 0 ? onePalSize : palW - onePalSize;
			}
		}
		palID %= nbPal;
		qDebug() << QString("-Palette-");
		qDebug() << QString("Size = %1, w = %2, h = %3").arg(palSize).arg(palW).arg(palH);
		qDebug() << QString("NbPal = %1 (valid : %2)").arg(nbPal).arg((palSize-12)%(onePalSize*2));
//		QString str="";
//		foreach(int posp, posPal)
//			str += QString("%1 | ").arg(posp);
//		qDebug() << QString("PosPal = %1").arg(str);
	}

	if((quint32)dataSize < 20+palSize)		return QPixmap();

	memcpy(&imgSize, &constData[8+palSize], 4);
	memcpy(&w, &constData[16+palSize], 2);
	memcpy(&h, &constData[18+palSize], 2);
	if(bpp==0)		w*=4;
	else if(bpp==1)	w*=2;

	qDebug() << QString("-Image-");
	qDebug() << QString("Size = %1, w = %2, h = %3").arg(imgSize).arg(w).arg(h);
	qDebug() << QString("TIM Size = %1").arg(8+palSize+imgSize);

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
