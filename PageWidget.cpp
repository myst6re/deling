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
#include "PageWidget.h"

PageWidget::PageWidget(QWidget *parent) :
	QWidget(parent), builded(false), filled(false), readOnly(false)
{
}

bool PageWidget::isBuilded() const
{
	return builded;
}

void PageWidget::build()
{
	builded = true;
	setReadOnly(readOnly);
}

bool PageWidget::isFilled() const
{
	return filled;
}

void PageWidget::fill()
{
	if(!isBuilded())	build();

	setEnabled(true);
	filled = true;
}

void PageWidget::clear()
{
	setEnabled(false);
	filled = false;
}

void PageWidget::setReadOnly(bool ro)
{
	readOnly = ro;
}

bool PageWidget::isReadOnly() const
{
	return readOnly;
}
