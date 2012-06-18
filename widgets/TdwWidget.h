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
#ifndef TDWWIDGET_H
#define TDWWIDGET_H

#include <QtGui>
#include "widgets/PageWidget.h"
#include "widgets/TdwGrid.h"
#include "widgets/TdwLetter.h"

class TdwWidget : public PageWidget
{
	Q_OBJECT
public:
	TdwWidget(QWidget *parent=0);
	void clear();
	void fill();
	inline QString tabName() const { return tr("Caractères additionnels"); }
public slots:
	void setColor(int i);
	void reset();
private:
	void build();
	TdwGrid *tdwGrid;
	TdwLetter *tdwLetter;
	QComboBox *selectPal;

};

#endif // TDWWIDGET_H
