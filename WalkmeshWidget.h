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
#ifndef WALKMESHWIDGET_H
#define WALKMESHWIDGET_H

#include <QtGui>
#include "PageWidget.h"
#include "WalkmeshFile.h"
#include "WalkmeshGLWidget.h"

class WalkmeshWidget : public PageWidget
{
public:
	WalkmeshWidget(QWidget *parent=0);
	void clear();
	void setData(Field *field);
	void cleanData();
	void fill();
	inline QString tabName() const { return tr("Walkmesh"); }
private:
	void build();
	WalkmeshFile *walkmeshFile;
	WalkmeshGLWidget *walkmeshGL;
};

#endif // WALKMESHWIDGET_H
