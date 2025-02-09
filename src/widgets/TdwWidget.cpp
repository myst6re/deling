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
#include "TdwWidget.h"
#include "Field.h"

TdwWidget::TdwWidget(QWidget *parent) :
	PageWidget(parent)
{
}

void TdwWidget::build()
{
	if (isBuilded())		return;

	tdwWidget = new TdwWidget2(true, this);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(tdwWidget);
	layout->addStretch();
	layout->setContentsMargins(QMargins());

	connect(tdwWidget, SIGNAL(letterEdited()), SIGNAL(modified()));

	PageWidget::build();
}

void TdwWidget::clear()
{
	if (!isFilled())		return;

	tdwWidget->clear();

	PageWidget::clear();
}

void TdwWidget::setReadOnly(bool ro)
{
	if (isBuilded()) {
		tdwWidget->setReadOnly(ro);
	}

	PageWidget::setReadOnly(ro);
}

void TdwWidget::fill()
{
	if (!isBuilded())	build();
	if (isFilled())		clear();

	if (!hasData() || !data()->hasTdwFile()) return;

	tdwWidget->setTdwFile(data()->getTdwFile());

	PageWidget::fill();
}

void TdwWidget::focusInEvent(QFocusEvent *)
{
	tdwWidget->setFocus();
}
