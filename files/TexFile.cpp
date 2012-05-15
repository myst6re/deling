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
#include "TexFile.h"

TexFile::TexFile(const QByteArray &data) :
	TextureFile()
{
	open(data);
}

bool TexFile::open(const QByteArray &data)
{
	const char *constData = data.constData();
	int palID=0;
	quint32 l, h, nbPal, entreesPal, bitPerPx;

	// qDebug() << QString("%1 | %2 | %3 | %4 | %5 | %6").arg(QString(data.mid(0,4).toHex())).arg(QString(data.mid(4,4).toHex())).arg(QString(data.mid(20,4).toHex())).arg(QString(data.mid(24,4).toHex())).arg(QString(data.mid(32,4).toHex())).arg(QString(data.mid(36,4).toHex()));
	// qDebug() << QString("%1 | %2 | %3 | %4 | %5").arg(QString(data.mid(40,4).toHex())).arg(QString(data.mid(44,4).toHex())).arg(QString(data.mid(68,4).toHex())).arg(QString(data.mid(72,4).toHex())).arg(QString(data.mid(84,4).toHex()));
	memcpy(&nbPal, &constData[48], 4);
	memcpy(&l, &constData[60], 4);
	memcpy(&h, &constData[64], 4);
	memcpy(&bitPerPx, &constData[104], 4);
	// qDebug() << "Tex.cpp >> var 'l' = " << l << " | var 'h' = " << h;

	_image = QImage(l, h, QImage::Format_ARGB32);
	QRgb *pixels = (QRgb *)_image.bits();

	int size = l*h*bitPerPx, i=0;
	quint32 x=0, y=0;

	if(nbPal > 0)
	{
		memcpy(&entreesPal, &constData[92], 4);
		quint32 index, imageStart = 240+entreesPal*4*nbPal, paletteStart = 240+entreesPal*4*palID;

		while(i<size && _image.valid(x, y))
		{
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

		while(i<size && _image.valid(x, y))
		{
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

	return true;
}

bool TexFile::save(QByteArray &data)
{
	return false;
}
