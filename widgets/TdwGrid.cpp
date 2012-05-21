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
	QWidget(parent), tdwFile(0), _currentTable(0)
{
	setFixedSize(16 * (12 + 1 + 1*2) + 1, 14 * (12 + 1 + 1*2) + 1);
}

TdwGrid::~TdwGrid()
{
}

void TdwGrid::setTdwFile(TdwFile *tdwFile)
{
	this->tdwFile = tdwFile;
	update();
}

void TdwGrid::clear()
{
	this->tdwFile = 0;
	update();
}

int TdwGrid::currentTable() const
{
	return _currentTable;
}

void TdwGrid::setCurrentTable(int currentTable)
{
	_currentTable = currentTable;
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

	if(tdwFile) {
		int charCount=tdwFile->charCount();

		// Draw odd characters (optimization to reduce the number of palette change)
		for(int i=0, x2=0, y2=0 ; i<charCount ; i+=2) {
			p.drawImage(QPoint(1+padding+x2*cellSize, 1+padding+y2*cellSize), tdwFile->letter(i, 7, true));
			x2+=2;
			if(x2 == 16) {
				++y2;
				x2 = 0;
			}
		}

		// Draw even characters
		for(int i=1, x2=1, y2=0 ; i<charCount ; i+=2) {
			p.drawImage(QPoint(1+padding+x2*cellSize, 1+padding+y2*cellSize), tdwFile->letter(i, 7, true));
			x2+=2;
			if(x2 == 16) {
				++y2;
				x2 = 0;
			}
		}
	}
}
