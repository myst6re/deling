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
#ifndef TDWPALETTE_H
#define TDWPALETTE_H

#include <QtGui>
#include "files/TdwFile.h"

#define CELL_SIZE		16
#define BORDER_SIZE		1

class TdwPalette : public QWidget
{
	Q_OBJECT
public:
	explicit TdwPalette(QWidget *parent=0);
	virtual ~TdwPalette();
	void setReadOnly(bool ro);
	void setTdwFile(TdwFile *tdw);
	void setCurrentPalette(TdwFile::Color palette);
	QRgb currentColor() const;
public slots:
	void setColor(int id, QRgb color);
	void clear();
	void reset();
signals:
	void colorChanged(int id);
protected:
	virtual QSize sizeHint() const;
	virtual QSize minimumSizeHint() const;
	virtual void paintEvent(QPaintEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
private:
	void setColorCount(quint8 colorCount);
	void setCurrentColor(int id);
	int getColorId(const QPoint &pos);
	quint8 colorCount;
	int _currentColor;
	TdwFile::Color _currentPalette;
	QVector<QRgb> palette, copyPalette;
	bool readOnly;
	TdwFile *tdw;
};

#endif // TDWPALETTE_H
