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
#include "VertexWidget.h"

class WalkmeshWidget : public PageWidget
{
	Q_OBJECT
public:
	WalkmeshWidget(QWidget *parent=0);
	void clear();
	void fill();
	inline QString tabName() const { return tr("Walkmesh"); }
private slots:
	void editCaVector(const Vertex_s &values);
	void editCaPos(double value);
    void setCurrentGateway(int id);
	void editExitPoint(const Vertex_s &values);
	void editEntryPoint(const Vertex_s &values);
	void editUnknownGate(const QString &u);
	void editUnknownGate2(const QString &u);
    void setCurrentMoviePosition(int id);
	void setMovieCameraPageEnabled(bool enabled);
	void addMovieCameraPosition();
	void removeMovieCameraPosition();
private:
	void build();
	QWidget *buildCameraPage();
	QWidget *buildGatewaysPage();
	QWidget *buildMovieCameraPage();
	void editCaVector(int id, const Vertex_s &values);
	void editCaPos(int id, int value);
	void editExitPoint(int id, const Vertex_s &values);

	WalkmeshGLWidget *walkmeshGL;
	VertexWidget *caVectorXEdit, *caVectorYEdit, *caVectorZEdit;
	QDoubleSpinBox *caSpaceXEdit, *caSpaceYEdit, *caSpaceZEdit;
	QListWidget *gateList, *frameList;
	QLineEdit *unknownGate, *unknownGate2;
	QToolBar *camToolbar;
	QAction *camPlusAction, *camMinusAction;
    VertexWidget *exitPoints[2], *entryPoint;
    VertexWidget *camPoints[4];
};

#endif // WALKMESHWIDGET_H
