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
#include "TdwWidget.h"

TdwWidget::TdwWidget(QWidget *parent) :
	PageWidget(parent)
{
}

void TdwWidget::build()
{
	if(isBuilded())		return;

	tdwGrid = new TdwGrid(this);
	tdwLetter = new TdwLetter(this);
	selectPal = new QComboBox(this);

	for(int i=1 ; i<=8 ; ++i) {
		selectPal->addItem(tr("Couleur %1").arg(i));
	}

	selectPal->setCurrentIndex(7);

	QPushButton *resetButton = new QPushButton(tr("Annuler les modifications"));

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(tdwGrid, 0, 0, Qt::AlignRight);
	layout->addWidget(tdwLetter, 0, 1, Qt::AlignLeft);
	layout->addWidget(resetButton, 1, 1, Qt::AlignLeft);
	layout->addWidget(selectPal, 2, 0, 1, 2, Qt::AlignCenter);
	layout->setRowStretch(2, 1);
	layout->setContentsMargins(QMargins());

	connect(selectPal, SIGNAL(currentIndexChanged(int)), SLOT(setColor(int)));
	connect(tdwGrid, SIGNAL(letterClicked(int)), tdwLetter, SLOT(setLetter(int)));
	connect(tdwLetter, SIGNAL(imageChanged(QRect)), tdwGrid, SLOT(updateLetter(QRect)));
	connect(resetButton, SIGNAL(clicked()), SLOT(reset()));

	PageWidget::build();
}

void TdwWidget::clear()
{
	if(!isFilled())		return;

	tdwGrid->clear();
	tdwLetter->clear();

	PageWidget::clear();
}

void TdwWidget::fill()
{
	if(!isBuilded())	build();
	if(isFilled())		clear();

	if(!hasData() || !data()->hasTdwFile()) return;

	TdwFile *tdw = data()->getTdwFile();
	tdwGrid->setTdwFile(tdw);
	tdwGrid->setLetter(0);
	tdwLetter->setTdwFile(tdw);
	tdwLetter->setLetter(0);

	PageWidget::fill();
}

void TdwWidget::setColor(int i)
{
	if(!hasData() || !data()->hasTdwFile()) return;

	tdwGrid->setColor(i);
	tdwLetter->setColor(i);
}

void TdwWidget::reset()
{
	if(!hasData() || !data()->hasTdwFile()) return;

	tdwLetter->reset();
	tdwGrid->update();
}
