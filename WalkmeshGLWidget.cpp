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
	: QGLWidget(parent), /*distance(-35),*/ xRot(0), yRot(0), zRot(0),
	  xTrans(0), yTrans(0), zTrans(0), camID(0), fovy(70.0), data(0), curFrame(0)
{
	setMouseTracking(true);
//	startTimer(100);
}

void WalkmeshGLWidget::timerEvent(QTimerEvent *)
{
	updateGL();
}

void WalkmeshGLWidget::clear()
{
	data = 0;
	updateGL();
}

void WalkmeshGLWidget::fill(Field *data)
{
	this->data = data;

	updatePerspective();
}

void WalkmeshGLWidget::computeFov()
{
	if(data && data->hasCaFile()
			&& data->getCaFile()->cameraCount() > 0
			&& camID < data->getCaFile()->cameraCount()) {
		const CaStruct &cam = data->getCaFile()->camera(camID);
		fovy = (2 * atan(240.0/(2.0 * cam.camera_zoom))) * 57.29577951;
	} else {
		fovy = 70.0;
	}
}

void WalkmeshGLWidget::updatePerspective()
{
	computeFov();
	resizeGL(width(), height());
	updateGL();
}

void WalkmeshGLWidget::initializeGL()
{
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_COLOR_MATERIAL);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	//glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void WalkmeshGLWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(fovy, (double)width/(double)height, 0.001, 1000.0);

	glMatrixMode(GL_MODELVIEW);
}

void WalkmeshGLWidget::paintGL()
{
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if(!data)	return;

	/*if(data->hasBackgroundFile()) {

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0,width(),0,-height());
		glPushMatrix();
//		glScalef(1, 1, 1);

		glEnable(GL_TEXTURE_2D);
		glDepthMask(false);
		GLuint texIDBg = bindTexture(data->getBackgroundFile()->background(), GL_TEXTURE_2D, GL_RGB, QGLContext::MipmapBindOption);
//		drawTexture(QPointF(0, 0), texIDBg);
		glBegin(GL_QUADS);
		glTexCoord2f(0,1);  glVertex2f(0,1);
		glTexCoord2f(0,0);  glVertex2f(0,0);
		glTexCoord2f(1,0);  glVertex2f(1,0);
		glTexCoord2f(1,1);  glVertex2f(1,1);
		glEnd();
		glDepthMask(true);
		deleteTexture(texIDBg);
		glDisable(GL_TEXTURE_2D);

		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

	}*/

	if(data->hasCaFile() && data->getCaFile()->cameraCount() > 0 && camID < data->getCaFile()->cameraCount()) {
		const CaStruct &cam = data->getCaFile()->camera(camID);

		double camAxisXx = cam.camera_axis[0].x/4096.0;
		double camAxisXy = cam.camera_axis[0].y/4096.0;
		double camAxisXz = cam.camera_axis[0].z/4096.0;

		double camAxisYx = -cam.camera_axis[1].x/4096.0;
		double camAxisYy = -cam.camera_axis[1].y/4096.0;
		double camAxisYz = -cam.camera_axis[1].z/4096.0;

		double camAxisZx = cam.camera_axis[2].x/4096.0;
		double camAxisZy = cam.camera_axis[2].y/4096.0;
		double camAxisZz = cam.camera_axis[2].z/4096.0;

		double camPosX = cam.camera_position[0]/4096.0;
		double camPosY = -cam.camera_position[1]/4096.0;
		double camPosZ = cam.camera_position[2]/4096.0;

		double tx = -(camPosX*camAxisXx + camPosY*camAxisYx + camPosZ*camAxisZx);
		double ty = -(camPosX*camAxisXy + camPosY*camAxisYy + camPosZ*camAxisZy);
		double tz = -(camPosX*camAxisXz + camPosY*camAxisYz + camPosZ*camAxisZz);

		gluLookAt(tx+xTrans, ty+yTrans, tz+zTrans, tx + camAxisZx, ty + camAxisZy, tz + camAxisZz, camAxisYx, camAxisYy, camAxisYz);
	}

	if(data->hasIdFile()) {

		/*if(data->hasMskFile() && data->getMskFile()->cameraPositionCount() > 0) {
			Vertex_s *verts = data->getMskFile()->cameraPosition(curFrame % data->getMskFile()->cameraPositionCount());

			double camAxisXx = verts[0].x/4096.0;
			double camAxisXy = verts[0].y/4096.0;
			double camAxisXz = verts[0].z/4096.0;

			double camAxisYx = -verts[1].x/4096.0;
			double camAxisYy = -verts[1].y/4096.0;
			double camAxisYz = -verts[1].z/4096.0;

			double camAxisZx = verts[2].x/4096.0;
			double camAxisZy = verts[2].y/4096.0;
			double camAxisZz = verts[2].z/4096.0;

			double camPosX = verts[3].x/4096.0;
			double camPosY = -verts[3].y/4096.0;
			double camPosZ = verts[3].z/4096.0;

			double tx = -(camPosX*camAxisXx + camPosY*camAxisYx + camPosZ*camAxisZx);
			double ty = -(camPosX*camAxisXy + camPosY*camAxisYy + camPosZ*camAxisZy);
			double tz = -(camPosX*camAxisXz + camPosY*camAxisYz + camPosZ*camAxisZz);

			gluLookAt(tx+xTrans, ty+yTrans, tz+zTrans, tx + camAxisZx, ty + camAxisZy, tz + camAxisZz, camAxisYx, camAxisYy, camAxisYz);
			curFrame = (curFrame + 1) % data->getMskFile()->cameraPositionCount();
		}*/

		glRotatef(xRot / 16.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(yRot / 16.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(zRot / 16.0f, 0.0f, 0.0f, 1.0f);

		/*glBegin(GL_LINES);
			glColor3ub(0, 0, 255);
			glVertex3f(-1.0f, 0.0f, 0.0f);
			glVertex3f(1.0f, 0.0f, 0.0f);
			glColor3ub(0, 255, 0);
			glVertex3f(0.0f, -1.0f, 0.0f);
			glVertex3f(0.0f, 1.0f, 0.0f);
			glColor3ub(255, 0, 0);
			glVertex3f(0.0f, 0.0f, -1.0f);
			glVertex3f(0.0f, 0.0f, 1.0f);
		glEnd();*/

		glColor3ub(0xFF, 0xFF, 0xFF);

		glBegin(GL_LINES);

		int i=0;

		foreach(const Triangle &triangle, data->getIdFile()->getTriangles()) {
			const Access &access = data->getIdFile()->access(i++);

			drawIdLine(triangle.vertices[0], triangle.vertices[1], access.a1);
			drawIdLine(triangle.vertices[1], triangle.vertices[2], access.a2);
			drawIdLine(triangle.vertices[2], triangle.vertices[0], access.a3);
		}

		glEnd();

		if(data->hasInfFile()) {
			InfFile *inf = data->getInfFile();

			glBegin(GL_LINES);

			glColor3ub(0xFF, 0x00, 0x00);

			foreach(const Gateway &door, inf->getGateways()) {
				Vertex_s vertex;
				vertex = door.exitLine[0];
				glVertex3f(vertex.x/4096.0f, vertex.y/4096.0f, vertex.z/4096.0f);
				vertex = door.exitLine[1];
				glVertex3f(vertex.x/4096.0f, vertex.y/4096.0f, vertex.z/4096.0f);
			}

			glColor3ub(0x00, 0xFF, 0x00);

			foreach(const Trigger &trigger, inf->getTriggers()) {
				Vertex_s vertex;
				vertex = trigger.trigger_line[0];
				glVertex3f(vertex.x/4096.0f, vertex.y/4096.0f, vertex.z/4096.0f);
				vertex = trigger.trigger_line[1];
				glVertex3f(vertex.x/4096.0f, vertex.y/4096.0f, vertex.z/4096.0f);
			}

			glEnd();
		}
	}
}

void WalkmeshGLWidget::drawIdLine(const Vertex_sr &vertex1, const Vertex_sr &vertex2, qint16 access)
{
	if(access == -1) {
		glColor3ub(0x00, 0x00, 0xFF);
	} else {
		glColor3ub(0xFF, 0xFF, 0xFF);
	}

	glVertex3f(vertex1.x/4096.0f, vertex1.y/4096.0f, vertex1.z/4096.0f);
	glVertex3f(vertex2.x/4096.0f, vertex2.y/4096.0f, vertex2.z/4096.0f);
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

/*void WalkmeshGLWidget::setZoom(int zoom)
{
	distance = zoom;
	updateGL();
}*/

void WalkmeshGLWidget::setCurrentFieldCamera(int camID)
{
	this->camID = camID;
	updatePerspective();
}

void WalkmeshGLWidget::keyPressEvent(QKeyEvent *event)
{
	switch(event->key()) {
	case Qt::Key_Left:
		xTrans += 128 / 4096.0f;
		updateGL();
		break;
	case Qt::Key_Right:
		xTrans -= 128 / 4096.0f;
		updateGL();
		break;
	case Qt::Key_Up:
		zTrans -= 128 / 4096.0f;
		updateGL();
		break;
	case Qt::Key_Down:
		zTrans += 128 / 4096.0f;
		updateGL();
		break;
	}
}

void WalkmeshGLWidget::mousePressEvent(QMouseEvent *event)
{
	setFocus();
}

void WalkmeshGLWidget::mouseMoveEvent(QMouseEvent *event)
{
	QPointF pos = event->posF();
}
