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
#include "FF8Image.h"

class TimFile
{
public:
	TimFile();
	TimFile(const QByteArray &data);
	bool open(const QByteArray &data);
	bool save(QByteArray &data);
	const QImage &image() const;
	int currentColorTable() const;
	QVector<QRgb> colorTable(int id) const;
	void setCurrentColorTable(int id);
	void setColorTable(int id, const QVector<QRgb> &colorTable);
private:
	QImage _image;
	QList< QVector<QRgb> > _colorTables;
	quint16 palW, palH;
	quint8 bpp;
	int _currentColorTable;
};

#endif // TIMFILE_H
