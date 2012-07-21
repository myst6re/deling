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
#include "widgets/PageWidget.h"
#include "files/WalkmeshFile.h"
#include "WalkmeshGLWidget.h"

class WalkmeshWidget : public PageWidget
{
	Q_OBJECT
public:
	WalkmeshWidget(QWidget *parent=0);
	void clear();
	void fill();
	inline QString tabName() const { return tr("Walkmesh"); }
private slots:
	void editCaVector(int value);
	void editCaPos(double value);
private:
	void build();
	QWidget *buildCameraPage();
	QWidget *buildGatewaysPage();
	QWidget *buildMovieCameraPage();
	void editCaVector(int id, int id2, int value);
	void editCaPos(int id, int value);

	WalkmeshGLWidget *walkmeshGL;
	QSpinBox *caVectorX1Edit, *caVectorX2Edit, *caVectorX3Edit, *caVectorY1Edit, *caVectorY2Edit, *caVectorY3Edit, *caVectorZ1Edit, *caVectorZ2Edit, *caVectorZ3Edit;
	QDoubleSpinBox *caSpaceXEdit, *caSpaceYEdit, *caSpaceZEdit;
	QListWidget *gateList, *frameList;
};

#endif // WALKMESHWIDGET_H
