/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2024 Arzel Jérôme <myst6re@gmail.com>
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
#include "widgets/PageWidget.h"

PageWidget::PageWidget(QWidget *parent) :
	QWidget(parent), builded(false), filled(false), readOnly(false), _field(NULL)
{
	setEnabled(false);
}

bool PageWidget::isBuilded() const
{
	return builded;
}

void PageWidget::build()
{
	if (isBuilded())		return;

	builded = true;
	setReadOnly(readOnly);
}

bool PageWidget::isFilled() const
{
	return filled;
}

void PageWidget::fill()
{
	if (isFilled())		return;

	build();

	setEnabled(true);
	filled = true;
}

void PageWidget::clear()
{
	if (!isFilled())		return;

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

void PageWidget::setData(Field *field)
{
	if (_field != field) {
		clear();
		_field = field;
	}
}

void PageWidget::cleanData()
{
	_field = NULL;
}

bool PageWidget::hasData() const
{
	return _field != NULL;
}

Field *PageWidget::data() const
{
	return _field;
}
