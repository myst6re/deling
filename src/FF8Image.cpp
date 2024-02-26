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
#include "FF8Image.h"
#include "LZS.h"

QPixmap FF8Image::lzs(const QByteArray &data)
{
	//QElapsedTimer t;t.start();
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

	/* quint16 u1, u2;
	memcpy(&u1, &constData[0], 2);
	memcpy(&u2, &constData[2], 2);
	qDebug() << "image" << u1 << u2; */

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

QByteArray FF8Image::toLzs(const QImage &image, quint16 u1, quint16 u2)
{
	QByteArray data;
	int x = 0, y = 0;
	quint16 w = image.width(), h = image.height();

	data.append((char *)&u1, 2);
	data.append((char *)&u2, 2);

	data.append((char *)&w, 2);
	data.append((char *)&h, 2);
	
	const QRgb *pixels = (QRgb *)image.constBits();

	while(image.valid(x, y)) {
		quint16 color = toPsColor(pixels[x + y * w]);
		data.append((char *)&color, 2);

		++x;
		if(x == w) {
			x = 0;
			++y;
		}
	}

	data = LZS::compress(data);
	int lzs_size = data.size();

	return data.prepend((char *)&lzs_size, 4);
}

QList<int> FF8Image::findTims(const QByteArray &data)
{
	int indexInData = -1, dataSize = data.size();
	qint32 palSize, imgSize;
	quint16 w, h;
	quint8 bpp;
	const char *constData = data.constData();
	QByteArray search("\x10\x00\x00\x00", 4);
	QList<int> ret;

	while((indexInData = data.indexOf(search, indexInData+4)) != -1) {
		palSize = 0;
		if(indexInData + 8 > dataSize) {
			continue;
		}
		bpp = quint8(data.at(indexInData + 4));

//		qDebug() << "bpp" << bpp;

		if(bpp == 8 || bpp == 9) {
			memcpy(&palSize, &constData[indexInData + 8], 4);
//			qDebug() << "palSize" << palSize;
			if(indexInData + 20 > dataSize) {
				continue;
			}
			memcpy(&w, &constData[indexInData + 16], 2);
			memcpy(&h, &constData[indexInData + 18], 2);
//			qDebug() << "w" << w << "h" << h;
			if(palSize != w * h * 2 + 12) {
				continue;
			}
		}
		else if(bpp != 2 && bpp != 3) {
			continue;
		}

		memcpy(&imgSize, &constData[indexInData + 8 + palSize], 4);
//		qDebug() << "imgSize" << imgSize;
		if(indexInData + 20 + palSize > dataSize) {
			continue;
		}
		memcpy(&w, &constData[indexInData+16+palSize], 2);
		memcpy(&h, &constData[indexInData+18+palSize], 2);
//		qDebug() << "w" << w << "h" << h;
		if(((bpp == 8 || bpp == 9 || bpp == 2) && imgSize != w * 2 * h + 12)
		        || (bpp == 3 && imgSize != w * 3 * h + 12)) {
			continue;
		}

		ret.append(indexInData);
	}

	return ret;
}

void FF8Image::error(QPaintDevice *pd)
{
	QFont font;
	font.setPixelSize(44);
	QString text = QObject::tr("Erreur");
	int textWidth = QFontMetrics(font).horizontalAdvance(text);
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
