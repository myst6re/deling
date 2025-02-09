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
#include "WalkmeshGLWidget.h"

WalkmeshGLWidget::WalkmeshGLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      distance(0.0), xRot(0.0f), yRot(0.0f), zRot(0.0f),
      xTrans(0.0f), yTrans(0.0f), transStep(360.0f), lastKeyPressed(-1),
      camID(0), _selectedTriangle(-1), _selectedDoor(-1), _selectedGate(-1),
      _lineToDrawPoint1(Vertex()), _lineToDrawPoint2(Vertex()),
      fovy(70.0), data(nullptr), curFrame(0), gpuRenderer(nullptr), _drawLine(false),
      _backgroundVisible(true)
{
	// setMouseTracking(true);
	// startTimer(100);
}

WalkmeshGLWidget::~WalkmeshGLWidget()
{
	if (gpuRenderer != nullptr) {
		delete gpuRenderer;
	}
}

void WalkmeshGLWidget::timerEvent(QTimerEvent *)
{
	update();
}

void WalkmeshGLWidget::clear()
{
	data = nullptr;
	tex = QImage();
	
	update();
	
	if (gpuRenderer) {
		gpuRenderer->reset();
	}
}

void WalkmeshGLWidget::fill(Field *data)
{
	this->data = data;
	tex = data->getBackgroundFile()->background();
	updatePerspective();
	resetCamera();
}

void WalkmeshGLWidget::computeFov()
{
	if (data && data->hasCaFile()
			&& data->getCaFile()->cameraCount() > 0
			&& camID < data->getCaFile()->cameraCount()) {
		const Camera &cam = data->getCaFile()->camera(camID);
		fovy = (2 * atan(240.0/(2.0 * cam.camera_zoom))) * 57.29577951;
	} else {
		fovy = 70.0;
	}
}

void WalkmeshGLWidget::updatePerspective()
{
	computeFov();
	resizeGL(width(), height());
	update();
}

void WalkmeshGLWidget::initializeGL()
{
}

void WalkmeshGLWidget::resizeGL(int width, int height)
{
	if (gpuRenderer != nullptr) {
		gpuRenderer->setViewport(0, 0, width, height);
	}
}

void WalkmeshGLWidget::paintGL()
{
	if (!data) {
		return;
	}

	if (gpuRenderer == nullptr) {
		gpuRenderer = new Renderer(this);
	}

	if (gpuRenderer->hasError()) {
		return;
	}

	gpuRenderer->clear();

	if (_backgroundVisible) {
		drawBackground();
	}

	mProjection.setToIdentity();
	mProjection.perspective(fovy, (float)width() / (float)height(), 0.001f, 1000.0f);
	gpuRenderer->bindProjectionMatrix(mProjection);

	QMatrix4x4 mModel;
	mModel.translate(xTrans, yTrans, distance);
	mModel.rotate(xRot, 1.0f, 0.0f, 0.0f);
	mModel.rotate(yRot, 0.0f, 1.0f, 0.0f);
	mModel.rotate(zRot, 0.0f, 0.0f, 1.0f);

	QMatrix4x4 mView;

	if (data->hasCaFile() && data->getCaFile()->cameraCount() > 0 && camID < data->getCaFile()->cameraCount()) {
		const Camera &cam = data->getCaFile()->camera(camID);

		double camAxisXx = cam.camera_axis[0].x / 4096.0;
		double camAxisXy = cam.camera_axis[0].y / 4096.0;
		double camAxisXz = cam.camera_axis[0].z / 4096.0;

		double camAxisYx = -cam.camera_axis[1].x / 4096.0;
		double camAxisYy = -cam.camera_axis[1].y / 4096.0;
		double camAxisYz = -cam.camera_axis[1].z / 4096.0;

		double camAxisZx = cam.camera_axis[2].x / 4096.0;
		double camAxisZy = cam.camera_axis[2].y / 4096.0;
		double camAxisZz = cam.camera_axis[2].z / 4096.0;

		double camPosX = cam.camera_position[0] / 4096.0;
		double camPosY = -cam.camera_position[1] / 4096.0;
		double camPosZ = cam.camera_position[2] / 4096.0;

		double tx = -(camPosX*camAxisXx + camPosY*camAxisYx + camPosZ*camAxisZx);
		double ty = -(camPosX*camAxisXy + camPosY*camAxisYy + camPosZ*camAxisZy);
		double tz = -(camPosX*camAxisXz + camPosY*camAxisYz + camPosZ*camAxisZz);

		const QVector3D eye(tx, ty, tz), center(tx + camAxisZx, ty + camAxisZy, tz + camAxisZz), up(camAxisYx, camAxisYy, camAxisYz);
		mView.lookAt(eye, center, up);
	}

	gpuRenderer->bindModelMatrix(mModel);
	gpuRenderer->bindViewMatrix(mView);

	if (data->hasIdFile()) {
		int i=0;

		for (const Triangle &triangle: data->getIdFile()->getTriangles()) {
			const Access &access = data->getIdFile()->access(i);

			// Vertex info
			QVector3D positionA(triangle.vertices[0].x / 4096.0, triangle.vertices[0].y / 4096.0, triangle.vertices[0].z / 4096.0),
								positionB(triangle.vertices[1].x / 4096.0, triangle.vertices[1].y / 4096.0, triangle.vertices[1].z / 4096.0),
								positionC(triangle.vertices[2].x / 4096.0, triangle.vertices[2].y / 4096.0, triangle.vertices[2].z / 4096.0);
			QRgba64   color1 = QRgba64::fromArgb32((i == _selectedTriangle ? 0xFFFF9000 : (access.a[0] == -1 ? 0xFF6699CC : 0xFFFFFFFF))),
								color2 = QRgba64::fromArgb32((i == _selectedTriangle ? 0xFFFF9000 : (access.a[1] == -1 ? 0xFF6699CC : 0xFFFFFFFF))),
								color3 = QRgba64::fromArgb32((i == _selectedTriangle ? 0xFFFF9000 : (access.a[2] == -1 ? 0xFF6699CC : 0xFFFFFFFF)));
			QVector2D texcoord;

			// Line
			gpuRenderer->bufferVertex(positionA, color1, texcoord);
			gpuRenderer->bufferVertex(positionB, color1, texcoord);

			// Line
			gpuRenderer->bufferVertex(positionB, color2, texcoord);
			gpuRenderer->bufferVertex(positionC, color2, texcoord);

			// Line
			gpuRenderer->bufferVertex(positionC, color3, texcoord);
			gpuRenderer->bufferVertex(positionA, color3, texcoord);

			++i;
		}

		if (!_drawLine && data->hasInfFile()) {
			InfFile *inf = data->getInfFile();

			for (const Gateway &gate: inf->getGateways()) {
				if (gate.fieldId != 0x7FFF) {
					// Vertex info
					QVector3D positionA(gate.exitLine[0].x / 4096.0, gate.exitLine[0].y / 4096.0, gate.exitLine[0].z / 4096.0),
										positionB(gate.exitLine[1].x / 4096.0, gate.exitLine[1].y / 4096.0, gate.exitLine[1].z / 4096.0);
					QRgba64   color = QRgba64::fromArgb32(0xFFFF0000);
					QVector2D texcoord;

					gpuRenderer->bufferVertex(positionA, color, texcoord);
					gpuRenderer->bufferVertex(positionB, color, texcoord);
				}
			}

			for (const Trigger &trigger: inf->getTriggers()) {
				if (trigger.doorID != 0xFF) {
					// Vertex info
					QVector3D positionA(trigger.trigger_line[0].x / 4096.0, trigger.trigger_line[0].y / 4096.0, trigger.trigger_line[0].z / 4096.0),
										positionB(trigger.trigger_line[1].x / 4096.0, trigger.trigger_line[1].y / 4096.0, trigger.trigger_line[1].z / 4096.0);
					QRgba64   color = QRgba64::fromArgb32(0xFF00FF00);
					QVector2D texcoord;

					gpuRenderer->bufferVertex(positionA, color, texcoord);
					gpuRenderer->bufferVertex(positionB, color, texcoord);
				}
			}
		}

		if (_drawLine) {
			// Vertex info
			QVector3D positionA(_lineToDrawPoint1.x / 4096.0, _lineToDrawPoint1.y / 4096.0, _lineToDrawPoint1.z / 4096.0),
								positionB(_lineToDrawPoint2.x / 4096.0, _lineToDrawPoint2.y / 4096.0, _lineToDrawPoint2.z / 4096.0);
			QRgba64   color = QRgba64::fromArgb32(0xFFFF00FF);
			QVector2D texcoord;

			gpuRenderer->bufferVertex(positionA, color, texcoord);
			gpuRenderer->bufferVertex(positionB, color, texcoord);
		}

		gpuRenderer->draw(RendererPrimitiveType::PT_LINES);

		if (_selectedTriangle >= 0 && _selectedTriangle < data->getIdFile()->triangleCount()) {
			const Triangle &triangle = data->getIdFile()->triangle(_selectedTriangle);

			// Vertex info
			QVector3D positionA(triangle.vertices[0].x / 4096.0, triangle.vertices[0].y / 4096.0, triangle.vertices[0].z / 4096.0),
								positionB(triangle.vertices[1].x / 4096.0, triangle.vertices[1].y / 4096.0, triangle.vertices[1].z / 4096.0),
								positionC(triangle.vertices[2].x / 4096.0, triangle.vertices[2].y / 4096.0, triangle.vertices[2].z / 4096.0);
			QRgba64   color = QRgba64::fromArgb32(0xFFFF9000);
			QVector2D texcoord;

			// Line
			gpuRenderer->bufferVertex(positionA, color, texcoord);
			gpuRenderer->bufferVertex(positionB, color, texcoord);
			gpuRenderer->bufferVertex(positionC, color, texcoord);
		}

		if (data->hasInfFile()) {
			if (_selectedGate >= 0 && _selectedGate < 12) {
				const Gateway &gate = data->getInfFile()->getGateway(_selectedGate);
				if (gate.fieldId != 0x7FFF) {
					// Vertex info
					QVector3D positionA(gate.exitLine[0].x / 4096.0, gate.exitLine[0].y / 4096.0, gate.exitLine[0].z / 4096.0),
										positionB(gate.exitLine[1].x / 4096.0, gate.exitLine[1].y / 4096.0, gate.exitLine[1].z / 4096.0);
					QRgba64   color = QRgba64::fromArgb32(0xFFFF0000);
					QVector2D texcoord;

					gpuRenderer->bufferVertex(positionA, color, texcoord);
					gpuRenderer->bufferVertex(positionB, color, texcoord);
				}
			}

			if (_selectedDoor >= 0 && _selectedDoor < 12) {
				const Trigger &trigger = data->getInfFile()->getTrigger(_selectedDoor);
				if (trigger.doorID != 0xFF) {
					// Vertex info
					QVector3D positionA(trigger.trigger_line[0].x / 4096.0, trigger.trigger_line[0].y / 4096.0, trigger.trigger_line[0].z / 4096.0),
										positionB(trigger.trigger_line[1].x / 4096.0, trigger.trigger_line[1].y / 4096.0, trigger.trigger_line[1].z / 4096.0);
					QRgba64   color = QRgba64::fromArgb32(0xFF00FF00);
					QVector2D texcoord;

					gpuRenderer->bufferVertex(positionA, color, texcoord);
					gpuRenderer->bufferVertex(positionB, color, texcoord);
				}
			}
		}

		gpuRenderer->draw(RendererPrimitiveType::PT_POINTS, 7.0f);
	}
}

void WalkmeshGLWidget::drawBackground()
{
	if (data->getBackgroundFile())
	{
		RendererVertex vertices[] = {
		    {
		        {-1.0f, -1.0f, 1.0f, 1.0f},
		        {1.0f, 1.0f, 1.0f, 1.0f},
		        {0.0f, 1.0f},
		    },
		    {
		        {-1.0f, 1.0f, 1.0f, 1.0f},
		        {1.0f, 1.0f, 1.0f, 1.0f},
		        {0.0f, 0.0f},
		    },
		    {
		        {1.0f, -1.0f, 1.0f, 1.0f},
		        {1.0f, 1.0f, 1.0f, 1.0f},
		        {1.0f, 1.0f},
		    },
		    {
		        {1.0f, 1.0f, 1.0f, 1.0f},
		        {1.0f, 1.0f, 1.0f, 1.0f},
		        {1.0f, 0.0f},
		    }
		};

		uint32_t indices[] = {
		    0, 1, 2,
		    1, 3, 2
		};

		QMatrix4x4 mBG;

		gpuRenderer->bindProjectionMatrix(mBG);
		gpuRenderer->bindViewMatrix(mBG);
		gpuRenderer->bindModelMatrix(mBG);

		gpuRenderer->bindVertex(vertices, 4);
		gpuRenderer->bindIndex(indices, 6);
		gpuRenderer->bindTexture(tex);
		gpuRenderer->draw(RendererPrimitiveType::PT_TRIANGLES);
	}
}

void WalkmeshGLWidget::wheelEvent(QWheelEvent *event)
{
	setFocus();
	distance += event->pixelDelta().x() / 4096.0;
	update();
}

void WalkmeshGLWidget::mousePressEvent(QMouseEvent *event)
{
	setFocus();
	if (event->button() == Qt::MiddleButton)
	{
		distance = -35;
		update();
	}
	else if (event->button() == Qt::LeftButton)
	{
		moveStart = event->pos();
	}
}

void WalkmeshGLWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		xTrans += (event->pos().x() - moveStart.x()) / 4096.0;
		yTrans -= (event->pos().y() - moveStart.y()) / 4096.0;
		moveStart = event->pos();
		update();
	}
}

void WalkmeshGLWidget::keyPressEvent(QKeyEvent *event)
{
	if (lastKeyPressed == event->key()
			&& (event->key() == Qt::Key_Left
				|| event->key() == Qt::Key_Right
				|| event->key() == Qt::Key_Down
				|| event->key() == Qt::Key_Up)) {
		if (transStep > 100.0f) {
			transStep *= 0.90f; // accelerator
		}
	} else {
		transStep = 360.0f;
	}
	lastKeyPressed = event->key();

	switch (event->key())
	{
	case Qt::Key_Left:
		xTrans += 1.0f/transStep;
		update();
		break;
	case Qt::Key_Right:
		xTrans -= 1.0f/transStep;
		update();
		break;
	case Qt::Key_Down:
		yTrans += 1.0f/transStep;
		update();
		break;
	case Qt::Key_Up:
		yTrans -= 1.0f/transStep;
		update();
		break;
	default:
		QWidget::keyPressEvent(event);
		return;
	}
}

void WalkmeshGLWidget::focusInEvent(QFocusEvent *event)
{
	grabKeyboard();
	QWidget::focusInEvent(event);
}

void WalkmeshGLWidget::focusOutEvent(QFocusEvent *event)
{
	releaseKeyboard();
	QWidget::focusOutEvent(event);
}

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
		update();
	}
}

void WalkmeshGLWidget::setYRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != yRot) {
		yRot = angle;
		update();
	}
}

void WalkmeshGLWidget::setZRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != zRot) {
		zRot = angle;
		update();
	}
}

void WalkmeshGLWidget::setZoom(int zoom)
{
	distance = zoom / 4096.0;
}

void WalkmeshGLWidget::resetCamera()
{
	distance = 0;
	zRot = yRot = xRot = 0;
	xTrans = yTrans = 0;
	update();
}

void WalkmeshGLWidget::setCurrentFieldCamera(int camID)
{
	this->camID = camID;
	updatePerspective();
}

void WalkmeshGLWidget::setSelectedTriangle(int triangle)
{
	_selectedTriangle = triangle;
	update();
}

void WalkmeshGLWidget::setSelectedDoor(int door)
{
	_selectedDoor = door;
	update();
}

void WalkmeshGLWidget::setSelectedGate(int gate)
{
	_selectedGate = gate;
	update();
}

void WalkmeshGLWidget::setLineToDraw(const Vertex vertex[2])
{
	_lineToDrawPoint1 = vertex[0];
	_lineToDrawPoint2 = vertex[1];
	_drawLine = true;
	update();
}

void WalkmeshGLWidget::clearLineToDraw()
{
	_drawLine = false;
	update();
}

void WalkmeshGLWidget::setBackgroundVisible(bool show)
{
	if (_backgroundVisible != show) {
		_backgroundVisible = show;
		update();
	}
}
