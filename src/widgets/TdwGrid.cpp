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
#include "TdwGrid.h"

TdwGrid::TdwGrid(QWidget *parent) :
	TdwDisplay(parent)
{
	setFixedSize(sizeHint());
}

TdwGrid::~TdwGrid()
{
}

QSize TdwGrid::sizeHint() const
{
	return QSize(16 * (12 + 1 + 1*2) + 1, 14 * (12 + 1 + 1*2) + 1);
}

QSize TdwGrid::minimumSizeHint() const
{
	return sizeHint();
}

void TdwGrid::paintEvent(QPaintEvent *)
{
	const int lineCountV=17, lineCountH=15, charaSize=12, padding=1, cellSize=charaSize+1+padding*2;
	QLine linesV[lineCountV], linesH[lineCountH];

	for(int i=0 ; i<lineCountV ; ++i) {
		linesV[i].setPoints(QPoint(i*cellSize, 0), QPoint(i*cellSize, height()));
	}

	for(int i=0 ; i<lineCountH ; ++i) {
		linesH[i].setPoints(QPoint(0, i*cellSize), QPoint(width(), i*cellSize));
	}

	QPainter p(this);

	if(isEnabled()) {
		p.setBrush(Qt::black);
		p.drawRect(0, 0, width(), height());
	}

	p.setPen(Qt::gray);

	p.drawLines(linesV, lineCountV);
	p.drawLines(linesH, lineCountH);

	if(_tdwFile) {
		int charCount=_tdwFile->charCount();

		// Draw odd characters (optimization to reduce the number of palette change)
		for(int i=0, x2=0, y2=0 ; i<charCount ; i+=2) {
			p.drawImage(QPoint(1+padding+x2*cellSize, 1+padding+y2*cellSize), _tdwFile->letter(_currentTable, i, _color, true));
			x2+=2;
			if(x2 == 16) {
				++y2;
				x2 = 0;
			}
		}

		// Draw even characters
		for(int i=1, x2=1, y2=0 ; i<charCount ; i+=2) {
			p.drawImage(QPoint(1+padding+x2*cellSize, 1+padding+y2*cellSize), _tdwFile->letter(_currentTable, i, _color, true));
			x2+=2;
			if(x2 == 17) {
				++y2;
				x2 = 1;
			}
		}
	}

	if(isEnabled()) {
		// Draw selection frame
		p.setPen(hasFocus() ? Qt::red : QColor(0xff,0x7f,0x7f));

		QPoint selection = getPos(_letter);
		p.drawLine(QLine(selection, selection + QPoint(0, cellSize)));
		p.drawLine(QLine(selection, selection + QPoint(cellSize, 0)));
		p.drawLine(QLine(selection + QPoint(cellSize, 0), selection + QPoint(cellSize, cellSize)));
		p.drawLine(QLine(selection + QPoint(0, cellSize), selection + QPoint(cellSize, cellSize)));
	}
}

int TdwGrid::getLetter(const QPoint &pos)
{
	return getCell(pos, QSize(15, 15), 16);
}

//void TdwGrid::setTdwFile(TdwFile *tdwFile)
//{
//	//copyGrid = tdwFile->image(_color);
//	TdwDisplay::setTdwFile(tdwFile);
//}

QPoint TdwGrid::getPos(int letter)
{
	const int cellSize = 15;
	if(letter > 16*14)		return QPoint();
	return QPoint((letter % 16) * cellSize, (letter / 16) * cellSize);
}

void TdwGrid::updateLetter(const QRect &rect)
{
	update(QRect(QPoint(2, 2) + getPos(_letter) + rect.topLeft(), rect.size()));
}

//void TdwGrid::reset()
//{

//}

void TdwGrid::mousePressEvent(QMouseEvent *e)
{
	int letter = getLetter(e->pos());
	if(letter < 16*14) {
		setLetter(letter);
		emit letterClicked(letter);
	}
	setFocus();
}

void TdwGrid::keyPressEvent(QKeyEvent *e)
{
	int letter;

	switch(e->key()) {
	case Qt::Key_Left:
		letter = _letter - 1;
		if(letter >= 0) {
			setLetter(letter);
			emit letterClicked(letter);
		}
		break;
	case Qt::Key_Right:
		letter = _letter + 1;
		if(letter < 16*14) {
			setLetter(letter);
			emit letterClicked(letter);
		}
		break;
	case Qt::Key_Up:
		letter = _letter - 16;
		if(letter >= 0) {
			setLetter(letter);
			emit letterClicked(letter);
		}
		break;
	case Qt::Key_Down:
		letter = _letter + 16;
		if(letter < 16*14) {
			setLetter(letter);
			emit letterClicked(letter);
		}
		break;
	}

	QWidget::keyPressEvent(e);
}

void TdwGrid::focusInEvent(QFocusEvent *)
{
	const int cellSize = 15;
	update(QRect(getPos(_letter), QSize(cellSize, cellSize)));
}

void TdwGrid::focusOutEvent(QFocusEvent *)
{
	const int cellSize = 15;
	update(QRect(getPos(_letter), QSize(cellSize, cellSize)));
}
