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
#ifndef FF8IMAGE_H
#define FF8IMAGE_H

#include <QtGui>
#include "LZS.h"

#define COEFF_COLOR	8.2258064516129032258064516129032 // 255/31

typedef struct{
	qint16 X, Y;
	quint16 srcX, srcY;
	quint16 Z;
	unsigned texID:4;
	unsigned blend1:1;//ending
	unsigned blend2:3;
	quint8 ZZ1;//ending
	unsigned unused1:6;
	unsigned palID:4;
	unsigned unused2:6;
	quint8 parameter;
	quint8 state;
} Tile1;

typedef struct{
	qint16 X, Y;
	quint16 Z;
	unsigned texID:4;
	unsigned blend1:1;//ending
	unsigned blend2:3;
	quint8 ZZ1;//ending
	unsigned unused1:6;//always 0
	unsigned palID:4;
	unsigned unused2:6;//always 15
	quint8 srcX, srcY;
	unsigned unused3:1;//always 0
	unsigned layerID:7;//0-7
	quint8 blendType;//0-4
	quint8 parameter;
	quint8 state;
} Tile2;

class FF8Image
{
public:
	static QPixmap lzs(const QByteArray &data);
	static QPixmap tex(const QByteArray &data, int palID=0);
	static QPixmap tim(const QByteArray &data, int palID=0);
	static QPixmap tdw(const QByteArray &data, int palID=0);

	static int findFirstTim(const QByteArray &data);
	static int findTims(const QByteArray &data);
	static QPixmap errorPixmap();

	static QPixmap background(const QByteArray &mapData, const QByteArray &mimData);
	static QPixmap type1(const QByteArray &mapData, const QByteArray &mimData/*, const QMultiMap<quint8, quint8> &params*/);
	static QPixmap type2(const QByteArray &mapData, const QByteArray &mimData, const QMultiMap<quint8, quint8> &params, const QMap<quint8, bool> &layers);
private:
	static QRgb BGcolor(int value, quint8 blendType=4, QRgb color0=0);
};

#endif // FF8IMAGE_H
