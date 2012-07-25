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
#include "Field.h"

class WalkmeshGLWidget : public QGLWidget
{
	Q_OBJECT
public:
	explicit WalkmeshGLWidget(QWidget *parent=0);
	void clear();
	void fill(Field *data);
	void updatePerspective();
public slots:
	void setXRotation(int);
	void setYRotation(int);
	void setZRotation(int);
//	void setZoom(int);
	void setCurrentFieldCamera(int);
private:
	void computeFov();
	void drawIdLine(const Vertex_sr &vertex1, const Vertex_sr &vertex2, qint16 access);
//	int distance;
	int xRot, yRot, zRot;
	float xTrans, yTrans, zTrans;
	int camID;
	double fovy;
	Field *data;
	int curFrame;

protected:
	virtual void timerEvent(QTimerEvent *);
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();
//	virtual void wheelEvent(QWheelEvent *event);
//	virtual void mousePressEvent(QMouseEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
};

#endif // WALKMESHGLWIDGET_H
