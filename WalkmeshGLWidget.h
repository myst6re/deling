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
#ifndef WALKMESHGLWIDGET_H
#define WALKMESHGLWIDGET_H

#include <QtGui>
#include <QGLWidget>
#include <GL/glu.h>
#include "files/WalkmeshFile.h"

class WalkmeshGLWidget : public QGLWidget
{
	Q_OBJECT
public:
	WalkmeshGLWidget(QWidget *parent=0);
	void clear();
	void fill(WalkmeshFile *walkmeshFile);
public slots:
	void setXRotation(int);
	void setYRotation(int);
	void setZRotation(int);
	void setZoom(int);
private:
	bool dataLoaded;
	int distance;
	int xRot, yRot, zRot;
	WalkmeshFile *walkmeshFile;

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
//	void wheelEvent(QWheelEvent *event);
//	void mousePressEvent(QMouseEvent *event);
};

#endif // WALKMESHGLWIDGET_H
