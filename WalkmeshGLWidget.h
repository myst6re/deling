#ifndef WALKMESHGLWIDGET_H
#define WALKMESHGLWIDGET_H

#include <QtGui>
#include <QGLWidget>
#include <GL/glu.h>
#include "WalkmeshFile.h"

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
