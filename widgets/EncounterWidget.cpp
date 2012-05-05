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

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(new QLabel(tr("Formation")), 0, 0);
	layout->addWidget(new QLabel(tr("Fréquence")), 0, 1);

	for(int i=0 ; i<4 ; ++i) {
		formationEdit[i] = new QSpinBox();
		formationEdit[i]->setRange(0, 65535);
		rateEdit[i] = new QSpinBox();
		rateEdit[i]->setRange(0, 255);

		layout->addWidget(formationEdit[i], 1+i, 0);
		layout->addWidget(rateEdit[i], 1+i, 1);
	}

	layout->setRowStretch(5, 1);
	layout->setColumnStretch(2, 1);

	PageWidget::build();
}

void EncounterWidget::clear()
{
	if(!isFilled())	return;

	for(int i=0 ; i<4 ; ++i) {
		formationEdit[i]->setValue(0);
		rateEdit[i]->setValue(0);
	}

	PageWidget::clear();
}

void EncounterWidget::setReadOnly(bool readOnly)
{
	if(isBuilded()) {
		for(int i=0 ; i<4 ; ++i) {
			formationEdit[i]->setReadOnly(readOnly);
			rateEdit[i]->setReadOnly(readOnly);
		}
	}

	PageWidget::setReadOnly(readOnly);
}

void EncounterWidget::setData(Field *field)
{
	if(this->encounterFile != field->getEncounterFile()) {
		clear();
		this->encounterFile = field->getEncounterFile();
	}
}

void EncounterWidget::cleanData()
{
	encounterFile = NULL;
}

void EncounterWidget::fill()
{
	if(!isBuilded())	build();
	if(isFilled())		clear();

	if(encounterFile == NULL)	return;

	for(int i=0 ; i<4 ; ++i) {
		formationEdit[i]->setValue(encounterFile->formation(i));
		rateEdit[i]->setValue(encounterFile->rate(i));
	}

	PageWidget::fill();
}

void EncounterWidget::editFormation(int value)
{
	QObject *s = sender();
	int index;

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

	encounterFile->setFormation(index, value);
}

void EncounterWidget::editRate(int value)
{
	QObject *s = sender();
	int index;

	if(s == rateEdit[0]) {
		index = 0;
	} else if(s == rateEdit[1]) {
		index = 1;
	} else if(s == rateEdit[2]) {
		index = 2;
	} else if(s == rateEdit[3]) {
		index = 3;
	} else {
		qWarning() << "EncounterWidget::editRate Bad sender object";
		return;
	}

	encounterFile->setRate(index, value);
}
