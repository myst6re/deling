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
#include "widgets/EncounterWidget.h"

EncounterWidget::EncounterWidget(QWidget *parent)
	: PageWidget(parent)
{
}

void EncounterWidget::build()
{
	if(isBuilded())	return;

	QLabel *labelColumn1 = new QLabel(tr("Formations"));
	QLabel *labelColumn2 = new QLabel(tr("Fréquence"));

	QFont font = labelColumn1->font();
	font.setPointSize(font.pointSize()+4);

	labelColumn1->setFont(font);
	labelColumn2->setFont(font);

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(labelColumn1, 0, 0, Qt::AlignCenter);
	layout->addWidget(labelColumn2, 0, 1, 1, 3, Qt::AlignCenter);

	for(int i=0 ; i<4 ; ++i) {
		formationEdit[i] = new QSpinBox();
		formationEdit[i]->setRange(0, 65535);
		if(i==0) {
			rateEdit = new QSlider(Qt::Horizontal);
			rateEdit->setRange(0, 255);
			rateEditLabel = new QLabel();
		}

		int row = 1+i;

		layout->addWidget(formationEdit[i], row, 0);
		if(i==0) {
			layout->addWidget(new QLabel(tr("Basse")), row, 1);
			layout->addWidget(rateEdit, row, 2);
			layout->addWidget(new QLabel(tr("Haute")), row, 3);
			layout->addWidget(rateEditLabel, row+1, 1, 1, 3, Qt::AlignCenter);

			connect(rateEdit, SIGNAL(valueChanged(int)), SLOT(editRate()));
		}

		connect(formationEdit[i], SIGNAL(valueChanged(int)), SLOT(editFormation()));
	}

	layout->setRowStretch(9, 1);
	layout->setColumnStretch(4, 1);

	PageWidget::build();
}

void EncounterWidget::clear()
{
	if(!isFilled())	return;

	for(int i=0 ; i<4 ; ++i) {
		formationEdit[i]->blockSignals(true);
		formationEdit[i]->setValue(0);
		formationEdit[i]->blockSignals(false);
	}
	rateEdit->blockSignals(true);
	rateEdit->setValue(0);
	rateEdit->blockSignals(false);
	fillRateLabel(0);

	PageWidget::clear();
}

void EncounterWidget::setReadOnly(bool readOnly)
{
	if(isBuilded()) {
		for(int i=0 ; i<4 ; ++i) {
			formationEdit[i]->setReadOnly(readOnly);
		}
		rateEdit->setDisabled(readOnly);
	}

	PageWidget::setReadOnly(readOnly);
}

void EncounterWidget::fill()
{
	if(!isBuilded())	build();
	if(isFilled())		clear();

	if(!hasData())	return;

	if(data()->hasMrtFile()) {
		for(int i=0 ; i<4 ; ++i) {
			formationEdit[i]->blockSignals(true);
			formationEdit[i]->setValue(data()->getMrtFile()->formation(i));
			formationEdit[i]->blockSignals(false);
		}
	}

	if(data()->hasRatFile()) {
		rateEdit->blockSignals(true);
		rateEdit->setValue(data()->getRatFile()->rate());
		rateEdit->blockSignals(false);
		fillRateLabel(data()->getRatFile()->rate());
	}

	PageWidget::fill();
}

void EncounterWidget::editFormation()
{
	QObject *s = sender();
	int index, value = ((QSpinBox *)s)->value();

	if(s == formationEdit[0]) {
		index = 0;
	} else if(s == formationEdit[1]) {
		index = 1;
	} else if(s == formationEdit[2]) {
		index = 2;
	} else if(s == formationEdit[3]) {
		index = 3;
	} else {
		qWarning() << "EncounterWidget::editFormation Bad sender object";
		return;
	}

	if(!data()->hasMrtFile()) {
		data()->addMrtFile();
	}
	data()->getMrtFile()->setFormation(index, value);

	emit modified();
}

void EncounterWidget::editRate()
{
	fillRateLabel(rateEdit->value());
	if(!data()->hasRatFile()) {
		data()->addRatFile();
	}
	data()->getRatFile()->setRate(rateEdit->value());

	emit modified();
}

void EncounterWidget::fillRateLabel(int value)
{
	if(value == 0) {
		rateEditLabel->setText(tr("Pas de combats"));
	} else {
		rateEditLabel->setNum(value);
	}
}
