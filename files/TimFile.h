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
#ifndef TIMFILE_H
#define TIMFILE_H

#include <QtCore>
#include <QImage>
#include "files/TextureFile.h"

class TimFile : public TextureFile
{
public:
	TimFile() : TextureFile() {}
	explicit TimFile(const QByteArray &data);
	TimFile(const TextureFile &texture, quint8 bpp, quint16 palX, quint16 palY, quint16 palW, quint16 palH, quint16 imgX, quint16 imgY);
	bool open(const QByteArray &data);
	bool save(QByteArray &data);
	inline QPoint imgPos() const {
		quint16 x = imgX;

		if (bpp == 0) {
			x *= 4;
		} else if (bpp == 1) {
			x *= 2;
		}

		return QPoint(x, imgY);
	}
	inline QPoint palPos() const {
		return QPoint(palX, palY);
	}
	inline QSize imgSize() const {
		return image().size();
	}
	inline QSize palSize() const {
		return QSize(palW, palH);
	}
	inline QRect imgRect() const {
		return QRect(imgPos(), imgSize());
	}
	inline QRect palRect() const {
		return QRect(palPos(), palSize());
	}
	inline quint8 depth() const {
		if (bpp == 0) {
			return 4;
		}
		return bpp * 8;
	}
	QImage palImage() const;
private:
	quint8 bpp;
	quint16 palX, palY;
	quint16 palW, palH;
	quint16 imgX, imgY;
};

#endif // TIMFILE_H
