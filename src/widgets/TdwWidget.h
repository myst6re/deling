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
#pragma once

#include <QtWidgets>
#include "widgets/PageWidget.h"
#include "widgets/TdwWidget2.h"

class TdwWidget : public PageWidget
{
	Q_OBJECT
public:
	TdwWidget(QWidget *parent = nullptr);
	void clear();
	void setReadOnly(bool ro);
	void fill();
	inline QString tabName() const { return tr("Additional characters"); }
private:
	void build();
	TdwWidget2 *tdwWidget;
protected:
	void focusInEvent(QFocusEvent *);

};
