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
#ifndef ENCOUNTERWIDGET_H
#define ENCOUNTERWIDGET_H

#include <QtGui>
#include "widgets/PageWidget.h"

class EncounterWidget : public PageWidget
{
	Q_OBJECT
public:
	EncounterWidget(QWidget *parent=0);
	void build();
	void clear();
	void setReadOnly(bool readOnly);
	void fill();
	inline QString tabName() const { return tr("Rencontres aléatoires"); }
private slots:
	void editFormation();
	void editRate();
private:
	void fillRateLabel(int value);
	QSpinBox *formationEdit[4];
	QSlider *rateEdit;
	QLabel *rateEditLabel;
};

#endif // ENCOUNTERWIDGET_H
