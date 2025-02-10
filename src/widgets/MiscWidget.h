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

class MiscWidget : public PageWidget
{
	Q_OBJECT
public:
	MiscWidget(QWidget *parent = nullptr);
	void build();
	void clear();
	void setReadOnly(bool readOnly);
	void fill();
	inline QString tabName() const { return tr("Miscellaneous"); }
private slots:
	void editName(const QString &);
	void editPmp(const QString &);
	void editPmd(const QString &);
	void editPvp(double);
	void editPvp2(int);
	void updatePmpView();
private:
	QGroupBox *pmpGroup;
	QLabel *pmpView, *pmpPaletteView;
	QComboBox *pmpPaletteBox, *pmpDephBox;
	QLineEdit *nameEdit, *pmpEdit, *pmdEdit;
	QDoubleSpinBox *pvpEdit;
	QSpinBox *pvpEdit2;
};
