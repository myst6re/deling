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
	TdwDisplay(parent), startDrag(false)
{
	setFixedSize(16*PIXEL_SIZE, 12*PIXEL_SIZE);
	setMouseTracking(true);
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
		p.drawImage(QPoint(0, 0), tdwFile->letter(_currentTable, _letter, _color, true).scaled(QSize(12*PIXEL_SIZE, 12*PIXEL_SIZE), Qt::KeepAspectRatio));
		const quint8 *charWidth = tdwFile->charWidth(_currentTable);
		int linePos = charWidth[_letter] * PIXEL_SIZE;
		p.setPen(Qt::red);
		p.drawLine(QPoint(linePos, 0), QPoint(linePos, height()));
	}
}

QPoint TdwLetter::getPixel(const QPoint &pos)
{
	return getCellPos(pos, QSize(PIXEL_SIZE, PIXEL_SIZE));
}

void TdwLetter::mouseMoveEvent(QMouseEvent *e)
{
	const QPoint &mousePos = e->pos();
	const quint8 *charWidth = tdwFile->charWidth(_currentTable);
	int linePos = charWidth[_letter] * PIXEL_SIZE;

	if(startDrag) {
		int newLinePos = mousePos.x() / PIXEL_SIZE;
		if(linePos / PIXEL_SIZE != newLinePos && newLinePos < 16) {
			tdwFile->setCharWidth(_currentTable, _letter, newLinePos);
			update();
		}
	} else {
		if(mousePos.x() >= linePos - 1 && mousePos.x() <= linePos + 1) {
			if(cursor().shape() != Qt::SplitHCursor) {
				setCursor(Qt::SplitHCursor);
			}
		} else {
			if(mousePos.x() < 12 * PIXEL_SIZE && mousePos.y() < 12 * PIXEL_SIZE) {
				if(cursor().shape() != Qt::PointingHandCursor) {
					setCursor(Qt::PointingHandCursor);
				}
			} else if(cursor().shape() != Qt::ArrowCursor) {
				setCursor(Qt::ArrowCursor);
			}
		}
	}
}

void TdwLetter::mousePressEvent(QMouseEvent *e)
{
	QPoint pixel = getPixel(e->pos());

	const quint8 *charWidth = tdwFile->charWidth(_currentTable);
	int linePos = charWidth[_letter];

	if(pixel.x() >= linePos - 1 && pixel.x() <= linePos + 1) {
		startDrag = true;
	} else if(pixel.x() < 12 && pixel.y() < 12 && tdwFile->setLetterPixelIndex(_currentTable, _letter, pixel, (tdwFile->letterPixelIndex(_currentTable, _letter, pixel) + 1) % 4)) {
		update(QRect(pixel * PIXEL_SIZE, QSize(PIXEL_SIZE, PIXEL_SIZE)));
		emit imageChanged(QRect(pixel, QSize(1, 1)));
	}
}

void TdwLetter::mouseReleaseEvent(QMouseEvent *)
{
	startDrag = false;
}
