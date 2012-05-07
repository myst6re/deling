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
#include "WalkmeshGLWidget.h"

WalkmeshGLWidget::WalkmeshGLWidget(QWidget *parent)
	: QGLWidget(parent), dataLoaded(false), distance(-35), xRot(0), yRot(0), zRot(0), walkmeshFile(NULL)
{
}

void WalkmeshGLWidget::clear()
{
	dataLoaded = false;
	walkmeshFile = NULL;
	updateGL();
}

void WalkmeshGLWidget::fill(WalkmeshFile *walkmeshFile)
{
	this->walkmeshFile = walkmeshFile;
	dataLoaded = true;
	updateGL();
}

void WalkmeshGLWidget::initializeGL()
{
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_COLOR_MATERIAL);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void WalkmeshGLWidget::resizeGL(int width, int height)
{
	//if(h>0)
		//gluPerspective(70, (double)(w/h), 1, 1000);
//	gluPerspective(60.0, (float)width/(float)height, 0.0, 1000.0);
//	int side = qMin(width, height);
	glViewport(0, 0, width, height);


//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//#ifdef QT_OPENGL_ES_1
	glOrtho(-1.0, 1.0, -1.0, 1.0, 0.0, 150.0);
//#else
//	glOrtho(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
//#endif
//	glMatrixMode(GL_MODELVIEW);
}

void WalkmeshGLWidget::paintGL()
{
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if(!dataLoaded || walkmeshFile==NULL)	return;

//	gluLookAt(distance,0,0,0,0,0,0,1,0);

	double camAxisXx = walkmeshFile->camAxis(0).x/4096.0;
	double camAxisXy = walkmeshFile->camAxis(0).y/4096.0;
	double camAxisXz = walkmeshFile->camAxis(0).z/4096.0;

	double camAxisYx = walkmeshFile->camAxis(1).x/4096.0;
	double camAxisYy = walkmeshFile->camAxis(1).y/4096.0;
	double camAxisYz = walkmeshFile->camAxis(1).z/4096.0;

	double camAxisZx = walkmeshFile->camAxis(2).x/4096.0;
	double camAxisZy = walkmeshFile->camAxis(2).y/4096.0;
	double camAxisZz = walkmeshFile->camAxis(2).z/4096.0;

	double camPosX = walkmeshFile->camPos(0)/4096.0;
	double camPosY = walkmeshFile->camPos(1)/4096.0;
	double camPosZ = walkmeshFile->camPos(2)/4096.0;

	double posCamX = -(camPosX*camAxisXx + camPosY*camAxisYx + camPosZ*camAxisZx);
	double posCamY = -(camPosX*camAxisXy + camPosY*camAxisYy + camPosZ*camAxisZy);
	double posCamZ = -(camPosX*camAxisXz + camPosY*camAxisYz + camPosZ*camAxisZz);
//	qDebug() << posCamX << posCamY << posCamZ;
//	glTranslatef(walkmeshFile->camPos(0)/4096.0, walkmeshFile->camPos(1)/4096.0, walkmeshFile->camPos(2)/4096.0);

//	glScalef(-1.0, 1.0, 1.0);
//	gluLookAt(posCamX, posCamY, posCamZ, posCamX + camAxisZx, posCamY + camAxisZy, posCamZ + camAxisZz, camAxisYx, camAxisYy, camAxisYz);
    //gluLookAt(posCamX, posCamY, posCamZ, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);

//	qDebug() << "xRot" << (walkmeshFile->camAxis(2).y*45)/4096.0 << "yRot" << (walkmeshFile->camAxis(2).x*45)/4096.0 << "zRot" << (walkmeshFile->camAxis(0).y*45)/4096.0;

//	glRotatef(xRot, walkmeshFile->camAxis(0).x/4096.0, 0.0, 0.0);
//	glRotatef(yRot, 0.0, -walkmeshFile->camAxis(1).y/4096.0, 0.0);
//	glRotatef(zRot, 0.0, 0.0, -walkmeshFile->camAxis(2).z/4096.0);
	glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
	glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
	glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);

	/*glRotatef(xRot, posCamX/4096.0, 0.0, 0.0);
	glRotatef(yRot, 0.0, posCamY/4096.0, 0.0);
	glRotatef(zRot, 0.0, 0.0, posCamZ/4096.0);
*/
	/*glRotatef(xRot, walkmeshFile->camAxis(0).x/4096.0, walkmeshFile->camAxis(0).y/4096.0, walkmeshFile->camAxis(0).z/4096.0);
	glRotatef(yRot, walkmeshFile->camAxis(1).x/4096.0, walkmeshFile->camAxis(1).y/4096.0, walkmeshFile->camAxis(1).z/4096.0);
	glRotatef(zRot, walkmeshFile->camAxis(2).x/4096.0, walkmeshFile->camAxis(2).y/4096.0, walkmeshFile->camAxis(2).z/4096.0);*/
	/*glBegin(GL_POINTS);
		glColor3ub(0xFF, 0xFF, 0xFF);
		glVertex3f(posCamX, posCamY, posCamZ);
	glEnd();*/

	//glTranslatef(xtrans, ytrans, ztrans);

	glBegin(GL_LINES);
		/*glColor3ub(0, 255, 255);
		glVertex3f(posCamX/4096.0, posCamY/4096.0, posCamZ/4096.0);
		glVertex3f(walkmeshFile->camAxis(0).x/4096.0, walkmeshFile->camAxis(0).y/4096.0, walkmeshFile->camAxis(0).z/4096.0);
		glVertex3f(posCamX/4096.0, posCamY/4096.0, posCamZ/4096.0);
		glVertex3f(walkmeshFile->camAxis(1).x/4096.0, walkmeshFile->camAxis(1).y/4096.0, walkmeshFile->camAxis(1).z/4096.0);
		glVertex3f(posCamX/4096.0, posCamY/4096.0, posCamZ/4096.0);
		glVertex3f(walkmeshFile->camAxis(2).x/4096.0, walkmeshFile->camAxis(2).y/4096.0, walkmeshFile->camAxis(2).z/4096.0);*/
		glColor3ub(0, 0, 255);
		glVertex3f(-1.0, 0.0, 0.0);
		glVertex3f(1.0, 0.0, 0.0);
		glColor3ub(0, 255, 0);
		glVertex3f(0.0, -1.0, 0.0);
		glVertex3f(0.0, 1.0, 0.0);
		glColor3ub(255, 0, 0);
		glVertex3f(0.0, 0.0, -1.0);
		glVertex3f(0.0, 0.0, 1.0);
	glEnd();

	const QList<Triangle> &triangles = walkmeshFile->getTriangles();
	Vertex_sr vertex;

	glColor3ub(0xFF, 0xFF, 0xFF);

	glBegin(GL_TRIANGLES);

	foreach(const Triangle &triangle, triangles) {
		vertex = triangle.vertices[0];
		glVertex3f(vertex.x/4096.0, vertex.y/4096.0, vertex.z/4096.0);
		vertex = triangle.vertices[1];
		glVertex3f(vertex.x/4096.0, vertex.y/4096.0, vertex.z/4096.0);
		vertex = triangle.vertices[2];
		glVertex3f(vertex.x/4096.0, vertex.y/4096.0, vertex.z/4096.0);
	}

	glEnd();
}

/* void WalkmeshGLWidget::wheelEvent(QWheelEvent *event)
{
	distance += event->delta()/120;
	updateGL();
}

void WalkmeshGLWidget::mousePressEvent(QMouseEvent *event)
{
	if(event->button()==Qt::MidButton)
	{
		distance = -35;
		updateGL();
	}
} */

static void qNormalizeAngle(int &angle)
{
	while (angle < 0)
		angle += 360 * 16;
	while (angle > 360 * 16)
		angle -= 360 * 16;
}

void WalkmeshGLWidget::setXRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != xRot) {
		xRot = angle;
		updateGL();
	}
}

void WalkmeshGLWidget::setYRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != yRot) {
		yRot = angle;
		updateGL();
	}
}

void WalkmeshGLWidget::setZRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != zRot) {
		zRot = angle;
		updateGL();
	}
}

void WalkmeshGLWidget::setZoom(int zoom)
{
	distance = zoom;
	updateGL();
}
