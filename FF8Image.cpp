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

quint16 FF8Image::toPsColor(const QRgb &color)
{
	return (qRound(qRed(color)/COEFF_COLOR) & 31) | ((qRound(qGreen(color)/COEFF_COLOR) & 31) << 5) | ((qRound(qBlue(color)/COEFF_COLOR) & 31) << 10) | ((qAlpha(color)==255) << 15);
}

QRgb FF8Image::fromPsColor(quint16 color, bool useAlpha)
{
	return qRgba(qRound((color & 31)*COEFF_COLOR), qRound((color>>5 & 31)*COEFF_COLOR), qRound((color>>10 & 31)*COEFF_COLOR), color == 0 && useAlpha ? 0 : 255);
}

QPixmap FF8Image::lzs(const QByteArray &data)
{
	//QTime t;t.start();
	int lzs_size = 0, real_size = data.size();
	const char *lzs_constData = data.constData();

	memcpy(&lzs_size, lzs_constData, 4);
	if(lzs_size != real_size - 4)	return errorPixmap();

	const QByteArray &decdata = LZS::decompressAll(lzs_constData + 4, lzs_size);

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

	while(i<size && image.valid(x, y))
	{
		memcpy(&color, &constData[i], 2);
		// $a = (color>>15)*127;
		// if(color>>15)
		// image.setPixel(QPoint(x, y), qRgb(0, 255, 0));
		// else
		pixels[x + y*l] = fromPsColor(color);

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

void FF8Image::error(QPaintDevice *pd)
{
	QFont font;
	font.setPixelSize(44);
	QString text = QObject::tr("Erreur");
	int textWidth = QFontMetrics(font).width(text);
	int textHeight = QFontMetrics(font).height();

	QPainter p(pd);
	p.setBrush(Qt::black);
	p.drawRect(0, 0, pd->width(), pd->height());
	p.setPen(Qt::white);
	p.setFont(font);
	p.drawStaticText((320-textWidth)/2, (224-textHeight)/2, QStaticText(text));
	p.end();
}

QImage FF8Image::errorImage()
{
	QImage errorPix(320, 224, QImage::Format_RGB32);

	error(&errorPix);

	return errorPix;
}

QPixmap FF8Image::errorPixmap()
{
	QPixmap errorPix(320, 224);

	error(&errorPix);

	return errorPix;
}
