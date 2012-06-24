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
	QStringList colors;
	colors << tr("Gris foncé") << tr("Gris") << tr("Jaune") << tr("Rouge") << tr("Vert") << tr("Bleu") << tr("Violet") << tr("Blanc");
	selectPal->addItems(colors);
	selectPal->setCurrentIndex(7);

	selectTable = new QComboBox(this);

	QPushButton *fromImage1 = new QPushButton(tr("À partir d'une image..."));
	QPushButton *fromImage2 = new QPushButton(tr("À partir d'une image..."));
//	QPushButton *resetButton1 = new QPushButton(tr("Annuler les modifications"));//TODO
	resetButton2 = new QPushButton(tr("Annuler les modifications"));
	resetButton2->setEnabled(false);

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(tdwGrid, 0, 0, Qt::AlignRight);
	layout->addWidget(tdwLetter, 0, 1, Qt::AlignLeft);
	layout->addWidget(fromImage1, 1, 0, Qt::AlignLeft);
	layout->addWidget(fromImage2, 1, 1, Qt::AlignRight);
//	layout->addWidget(resetButton1, 2, 0, Qt::AlignLeft);
	layout->addWidget(resetButton2, 2, 1, Qt::AlignRight);
	layout->addWidget(selectPal, 3, 0, Qt::AlignRight);
	layout->addWidget(selectTable, 3, 1, Qt::AlignLeft);
	layout->setRowStretch(4, 1);
	layout->setColumnStretch(2, 1);
	layout->setContentsMargins(QMargins());

	connect(selectPal, SIGNAL(currentIndexChanged(int)), SLOT(setColor(int)));
	connect(selectTable, SIGNAL(currentIndexChanged(int)), SLOT(setTable(int)));
	connect(tdwGrid, SIGNAL(letterClicked(int)), SLOT(setLetter(int)));
	connect(tdwLetter, SIGNAL(imageChanged(QRect)), tdwGrid, SLOT(updateLetter(QRect)));
	connect(tdwLetter, SIGNAL(imageChanged(QRect)), SLOT(setModified()));
//	connect(resetButton1, SIGNAL(clicked()), SLOT(reset()));
	connect(resetButton2, SIGNAL(clicked()), SLOT(resetLetter()));

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
	if(selectTable->count() != tdw->tableCount()) {
		selectTable->clear();
		for(int i=1 ; i<=tdw->tableCount() ; ++i) {
			selectTable->addItem(tr("Table %1").arg(i));
		}
		selectTable->setEnabled(selectTable->count() > 1);
	}

	PageWidget::fill();
}

void TdwWidget::setColor(int i)
{
	if(!hasData() || !data()->hasTdwFile()) return;

	tdwGrid->setColor((TdwFile::Color)i);
	tdwLetter->setColor((TdwFile::Color)i);
}

void TdwWidget::setTable(int i)
{
	if(!hasData() || !data()->hasTdwFile()) return;

	tdwGrid->setCurrentTable(i);
	tdwLetter->setCurrentTable(i);
}

void TdwWidget::setLetter(int i)
{
	if(!hasData() || !data()->hasTdwFile()) return;

	tdwLetter->setLetter(i);
	resetButton2->setEnabled(false);
}

void TdwWidget::reset()
{
	if(!hasData() || !data()->hasTdwFile()) return;

//	tdwGrid->reset();//TODO
	tdwLetter->update();
}

void TdwWidget::resetLetter()
{
	if(!hasData() || !data()->hasTdwFile()) return;

	tdwLetter->reset();
	tdwGrid->update();
	resetButton2->setEnabled(false);
}

void TdwWidget::setModified()
{
	emit modified();
	resetButton2->setEnabled(true);
}
