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
#include "TdwLetter.h"

TdwLetter::TdwLetter(QWidget *parent) :
	TdwDisplay(parent)
{
	setFixedSize(12*21, 12*21);
}

TdwLetter::~TdwLetter()
{
}

void TdwLetter::setLetter(int letter)
{
	copyLetter = tdwFile->letter(_currentTable, letter, _color, true);
	TdwDisplay::setLetter(letter);
}

void TdwLetter::reset()
{
	if(copyLetter.isNull())		return;
	tdwFile->setLetter(_currentTable, _letter, copyLetter);
	update();
}

void TdwLetter::paintEvent(QPaintEvent *)
{
	QPainter p(this);

	if(isEnabled()) {
		p.setBrush(Qt::black);
		p.drawRect(0, 0, width(), height());
	}

	if(tdwFile) {
		p.drawImage(QPoint(0, 0), tdwFile->letter(_currentTable, _letter, _color, true).scaled(QSize(12*21, 12*21), Qt::KeepAspectRatio));
	}
}

QPoint TdwLetter::getPixel(const QPoint &pos)
{
	return getCellPos(pos, QSize(21, 21), 12, 12);
}

void TdwLetter::mousePressEvent(QMouseEvent *e)
{
	QPoint pixel = getPixel(e->pos());
	if(tdwFile->setLetterPixelIndex(_currentTable, _letter, pixel, (tdwFile->letterPixelIndex(_currentTable, _letter, pixel) + 1) % 4)) {
		update(QRect(pixel * 21, QSize(21, 21)));
		emit imageChanged(QRect(pixel, QSize(1, 1)));
	}
}
