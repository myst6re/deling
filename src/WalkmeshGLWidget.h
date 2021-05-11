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

#ifdef OPENGLLOL

#include <QtWidgets>
#include "Field.h"
#include "Renderer.h"

class WalkmeshGLWidget : public QOpenGLWidget
{
	Q_OBJECT
public:
	explicit WalkmeshGLWidget(QWidget *parent=nullptr);
	void clear();
	void fill(Field *data);
	void updatePerspective();
public slots:
	void setXRotation(int);
	void setYRotation(int);
	void setZRotation(int);
	void setZoom(int);
	void resetCamera();
	void setCurrentFieldCamera(int camID);
	void setSelectedTriangle(int triangle);
	void setSelectedDoor(int door);
	void setSelectedGate(int gate);
	void setLineToDraw(const Vertex_s vertex[2]);
	void clearLineToDraw();
private:
	void computeFov();
	void drawBackground();
	double distance;
	float xRot, yRot, zRot;
	float xTrans, yTrans, transStep;
	int lastKeyPressed;
	int camID;
	int _selectedTriangle;
	int _selectedDoor;
	int _selectedGate;
	bool _drawLine;
	Vertex_s _lineToDrawPoint1, _lineToDrawPoint2;
	double fovy;
	Field *data;
	QPoint moveStart;
	int curFrame;
	//Renderer *gpuRenderer;
	QMatrix4x4 mProjection;

protected:
	virtual void timerEvent(QTimerEvent *);
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();
	virtual void wheelEvent(QWheelEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void focusInEvent(QFocusEvent *event);
	virtual void focusOutEvent(QFocusEvent *event);
};

#endif

#endif // WALKMESHGLWIDGET_H
