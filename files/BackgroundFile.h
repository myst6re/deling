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
#ifndef BACKGROUNDFILE_H
#define BACKGROUNDFILE_H

#include <QtCore>
#include <QPixmap>
#include "FF8Image.h"

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

class BackgroundFile
{
public:
	BackgroundFile();
	bool open(const QByteArray &map, const QByteArray &mim);
	bool isModified();

	QPixmap background();

	QMultiMap<quint8, quint8> params;
	QMultiMap<quint8, quint8> allparams;
	QMap<quint8, bool> layers;
private:
	QPixmap type1();
	QPixmap type2();
	static QRgb BGcolor(int value, quint8 blendType=4, QRgb color0=0);
	static QByteArray map, mim;
	bool modified;
};

#endif // BACKGROUNDFILE_H
