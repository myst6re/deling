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

class FF8Image
{
public:
	static quint16 toPsColor(const QRgb &color);
	static QRgb fromPsColor(quint16 color, bool useAlpha=false);
	static QPixmap lzs(const QByteArray &data);

	static int findFirstTim(const QByteArray &data);
	static int findTims(const QByteArray &data);

	static QImage errorImage();
	static QPixmap errorPixmap();
private:
	static void error(QPaintDevice *pd);
};

#endif // FF8IMAGE_H
