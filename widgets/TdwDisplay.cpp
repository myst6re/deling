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
#include "TdwDisplay.h"

TdwDisplay::TdwDisplay(QWidget *parent) :
	QWidget(parent), tdwFile(0), _color(TdwFile::White), _currentTable(0), _letter(0)
{
}

TdwDisplay::~TdwDisplay()
{
}

void TdwDisplay::setTdwFile(TdwFile *tdwFile)
{
	this->tdwFile = tdwFile;
	update();
}

void TdwDisplay::clear()
{
	this->tdwFile = 0;
	update();
}

int TdwDisplay::currentTable() const
{
	return _currentTable;
}

void TdwDisplay::setCurrentTable(int currentTable)
{
	_currentTable = currentTable;
}

void TdwDisplay::setColor(TdwFile::Color color)
{
	_color = color;
	update();
}

void TdwDisplay::setLetter(int letter)
{
	_letter = letter;
	update();
}

QPoint TdwDisplay::getCellPos(const QPoint &pos, const QSize &cellSize, int colCount, int rowCount)
{
	int col = qMin(pos.x() / cellSize.width(), colCount-1), row = qMin(pos.y() / cellSize.height(), rowCount-1);

	return QPoint(col, row);
}

int TdwDisplay::getCell(const QPoint &pos, const QSize &cellSize, int colCount, int rowCount)
{
	QPoint cellPos = getCellPos(pos, cellSize, colCount, rowCount);

	return cellPos.y() * colCount + cellPos.x();
}
