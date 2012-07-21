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
#include "TdwPalette.h"

TdwPalette::TdwPalette(QWidget *parent) :
	QWidget(parent), colorCount(16), _currentColor(0),
	_currentPalette(TdwFile::White), readOnly(false), tdw(0)
{
}

TdwPalette::~TdwPalette()
{
}

QSize TdwPalette::sizeHint() const
{
	return QSize(colorCount * (CELL_SIZE + BORDER_SIZE) + BORDER_SIZE, CELL_SIZE + 2 * BORDER_SIZE);
}

QSize TdwPalette::minimumSizeHint() const
{
	return sizeHint();
}

void TdwPalette::setReadOnly(bool ro)
{
	readOnly = ro;
}

void TdwPalette::setColorCount(quint8 colorCount)
{
	this->colorCount = qMin(colorCount, quint8(16));
	setFixedSize(sizeHint());
}

void TdwPalette::setTdwFile(TdwFile *tdw)
{
	setColorCount(tdw->isOptimizedVersion() ? 4 : 16);

	this->tdw = tdw;
	setCurrentPalette(_currentPalette);
}

void TdwPalette::setCurrentPalette(TdwFile::Color palette)
{
	_currentPalette = palette;
	copyPalette = tdw->image(_currentPalette).colorTable();
	this->palette = copyPalette;

	update();
}

void TdwPalette::setColor(int id, QRgb color)
{
	palette.replace(id, color);

	update(BORDER_SIZE + id * (CELL_SIZE + BORDER_SIZE), BORDER_SIZE, CELL_SIZE, CELL_SIZE);
}

QRgb TdwPalette::currentColor() const
{
	return palette.at(_currentColor);
}

void TdwPalette::setCurrentColor(int id)
{
	_currentColor = id;
	emit colorChanged(_currentColor);

	update();
}

void TdwPalette::clear()
{
	copyPalette.clear();
	palette.clear();
	update();
}

void TdwPalette::reset()
{
	if(copyPalette.isEmpty())		return;
	palette = copyPalette;
	update();
}

void TdwPalette::paintEvent(QPaintEvent *)
{
	QPainter p(this);

	if(isEnabled()) {
		p.fillRect(0, 0, width(), height(), Qt::black);
	}

	int id=0;

	foreach(const QRgb &color, palette) {
		if(!readOnly && id == _currentColor) {
			p.fillRect(QRect(id * (CELL_SIZE + BORDER_SIZE), 0, CELL_SIZE + 2 * BORDER_SIZE, CELL_SIZE + 2 * BORDER_SIZE), Qt::red);
		} else {
			p.setPen(Qt::black);
		}

		p.fillRect(BORDER_SIZE + id * (CELL_SIZE + BORDER_SIZE), BORDER_SIZE, CELL_SIZE, CELL_SIZE, QColor(color));

		++id;
	}
}

int TdwPalette::getColorId(const QPoint &pos)
{
	return pos.x() / (CELL_SIZE + BORDER_SIZE);
}

void TdwPalette::mousePressEvent(QMouseEvent *e)
{
	if(readOnly || palette.isEmpty())	return;

	setCurrentColor(getColorId(e->pos()));
}
