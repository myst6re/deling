#include "WorldmapGLWidget.h"

WorldmapGLWidget::WorldmapGLWidget(QWidget *parent,
                                   const QGLWidget *shareWidget,
                                   Qt::WindowFlags f) :
    QGLWidget(parent, shareWidget, f), _map(Q_NULLPTR),
    _distance(-0.714248f), _xRot(-90.0f), _yRot(180.0f), _zRot(180.0f),
    _xTrans(-0.5f), _yTrans(0.5f), _transStep(360.0f), _lastKeyPressed(-1),
    _limits(QRect(0, 0, 32, 24))
{
}

WorldmapGLWidget::WorldmapGLWidget(QGLContext *context,
                                   QWidget *parent,
                                   const QGLWidget *shareWidget,
                                   Qt::WindowFlags f) :
    QGLWidget(context, parent, shareWidget, f), _map(Q_NULLPTR),
    _distance(-0.714248f), _xRot(-90.0f), _yRot(180.0f), _zRot(180.0f),
    _xTrans(-0.5f), _yTrans(0.5f), _transStep(360.0f), _lastKeyPressed(-1),
    _limits(QRect(0, 0, 32, 24))
{
}

WorldmapGLWidget::WorldmapGLWidget(const QGLFormat &format,
                                   QWidget *parent,
                                   const QGLWidget *shareWidget,
                                   Qt::WindowFlags f) :
    QGLWidget(format, parent, shareWidget, f), _map(Q_NULLPTR),
    _distance(-0.714248f), _xRot(-90.0f), _yRot(180.0f), _zRot(180.0f),
    _xTrans(-0.5f), _yTrans(0.5f), _transStep(360.0f), _lastKeyPressed(-1),
    _limits(QRect(0, 0, 32, 24))
{
}
/*
QImage WorldmapGLWidget::toImage(int w, int h)
{
	QSize sz = size();
	if ((w > 0) && (h > 0))
		sz = QSize(w, h);

	QImage pm;
	if (context()->isValid()) {
		context()->makeCurrent();
		QGLFramebufferObject fbo(sz, QGLFramebufferObject::CombinedDepthStencil);
		fbo.bind();
		context()->setInitialized(false);
		uint prevDefaultFbo = d->glcx->d_ptr->default_fbo;
		d->glcx->d_ptr->default_fbo = fbo.handle();
		d->glcx->d_ptr->readback_target_size = sz;
		updateGL();
		fbo.release();
		pm = fbo.toImage();
		d->glcx->d_ptr->default_fbo = prevDefaultFbo;
		d->glcx->setInitialized(false);
		d->glcx->d_ptr->readback_target_size = QSize();
	}

	saveGLState();
	const int nrPics = 360 / DEGREES_BETWEEN_PICTURES;
	for (int i = 0; i < nrPics; i++) {
		catchFbo->bind();
		glColorMask(true, true, true, true);
		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_MULTISAMPLE);
		glLoadIdentity();

		GLfloat x = GLfloat(PICTURE_SIZE) / PICTURE_SIZE;
		glFrustum(-x, +x, -1.0, +1.0, 1.0, 1000.0);
		glViewport(0, 0, PICTURE_SIZE, PICTURE_SIZE);

		drawScreenshot(i);
		catchFbo->release();

		QImage catchImage = catchFbo->toImage();
		catchImage.save("object/test" + QString::number(i) + ".png");
	}
	glDisable(GL_MULTISAMPLE);

	restoreGLState();
}
*/
void WorldmapGLWidget::setMap(const Map *map)
{
	_map = map;
	_colorRegions[0] = qRgb(128, 0, 0);
	//_colorRegions[1] = qRgb(128, 0, 128);
	//_colorRegions[2] = qRgb(128, 0, 0);
	/* _colorRegions[3] = qRgb(0, 128, 0);
	_colorRegions[4] = qRgb(0, 0, 255);
	_colorRegions[6] = qRgb(128, 128, 128);
	_colorRegions[15] = qRgb(0, 128, 128);
	_colorRegions[19] = qRgb(128, 128, 0); */
	/* _colorRegions[5] = qRgb(255, 0, 255);
	_colorRegions[6] = qRgb(255, 255, 255);
	_colorRegions[7] = qRgb(128, 0, 0);
	_colorRegions[8] = qRgb(0, 128, 0);
	_colorRegions[9] = qRgb(0, 0, 128);
	_colorRegions[10] = qRgb(128, 128, 0);
	_colorRegions[11] = qRgb(0, 128, 128);
	_colorRegions[12] = qRgb(128, 0, 128);
	_colorRegions[13] = qRgb(128, 128, 128); */
	updateGL();
}

void WorldmapGLWidget::setLimits(const QRect &rect)
{
	_limits = rect;
	updateGL();
}

void WorldmapGLWidget::setXTrans(float trans)
{
	_xTrans = trans;
	updateGL();
}

void WorldmapGLWidget::setYTrans(float trans)
{
	_yTrans = trans;
	updateGL();
}

void WorldmapGLWidget::setZTrans(double trans)
{
	_distance = trans;
	updateGL();
}

void WorldmapGLWidget::setXRot(float rot)
{
	_xRot = rot;
	updateGL();
}

void WorldmapGLWidget::setYRot(float rot)
{
	_yRot = rot;
	updateGL();
}

void WorldmapGLWidget::setZRot(float rot)
{
	_zRot = rot;
	updateGL();
}

void WorldmapGLWidget::initializeGL()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glEnable(GL_MULTISAMPLE);
}

static GLdouble deg2rad(GLdouble deg)
{
	const GLdouble pi = 3.1415926535897932384626433832795;
	return deg / 360 * pi;
}

static void perspectiveGL(GLdouble fovY, GLdouble aspect,
                          GLdouble zNear, GLdouble zFar)
{
	GLdouble fW, fH;

	fH = tan(deg2rad(fovY)) * zNear;
	fW = fH * aspect;

	glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}

void WorldmapGLWidget::resizeGL(int w, int h)
{
	glViewport(0, 0, (GLint)w, (GLint)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	perspectiveGL(70.0, (double)w/(double)h, 0.000001, 1000.0);

	glMatrixMode(GL_MODELVIEW);
}

static quint16 normalizeY(qint16 y)
{
	return -(y - 128);
}

void WorldmapGLWidget::paintGL()
{
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if (Q_NULLPTR == _map) {
		return;
	}

	qDebug() << _xTrans << _yTrans << _distance << _xRot << _yRot << _zRot;

	glTranslatef(_xTrans, _yTrans, _distance);
	glRotatef(_xRot, 1.0, 0.0, 0.0);
	glRotatef(_yRot, 0.0, 1.0, 0.0);
	glRotatef(_zRot, 0.0, 0.0, 1.0);

	glBegin(GL_POINTS);

	glColor3ub(0xFF, 0x00, 0x00);

	glVertex3f(0.0f,
	           0.0f,
	           0.0f);
	glEnd();

	glBegin(GL_TRIANGLES);

	glColor3ub(0xFF, 0xFF, 0xFF);

	const int segmentPerLine = 32, blocksPerLine = 4,
	        diffSize = _limits.width() - _limits.height();
	const float scaleVect = 2048.0f, scale = _limits.width() * blocksPerLine;
	const float xShift = -_limits.x() * blocksPerLine + (diffSize < 0 ? -diffSize : 0) * blocksPerLine / 2.0f;
	const float zShift = -_limits.y() * blocksPerLine + (diffSize > 0 ? diffSize : 0) * blocksPerLine / 2.0f;
	QList<quint8> encounterRegions = _map->encounterRegions();
	int xs = 0, ys = 0;
	foreach (const MapSegment &segment, _map->segments()) {
		quint8 region = encounterRegions.at(ys * segmentPerLine + xs);
		QList<WmEncounter> encounters = _map->encounters(region);
		QSet<quint8> grounds;

		foreach (const WmEncounter &enc, encounters) {
			grounds.insert(enc.ground());
		}

		qDebug() << grounds;

		// xs = ys = -1.0f;
		// if (_limits.isNull() || _limits.contains(xs, ys)) {
		    int xb = 0, yb = 0;
			foreach (const MapBlock &block, segment.blocks()) {
				foreach (const MapPoly &poly, block.polygons()) {
					const Vertex &v0 = poly.vertex(0),
					             &v1 = poly.vertex(1),
					             &v2 = poly.vertex(2);
					const int x = xs * blocksPerLine + xb, z = ys * blocksPerLine + yb;
					QRgb c = groundColor(poly.groundType(), region, grounds);

					glColor3ub(qRed(c), qGreen(c), qBlue(c));

					glVertex3f((xShift + x + v0.x/scaleVect) / scale,
					           normalizeY(v0.y)/scaleVect / scale,
					           (zShift + z - v0.z/scaleVect) / scale);
					glVertex3f((xShift + x + v1.x/scaleVect) / scale,
					           normalizeY(v1.y)/scaleVect / scale,
					           (zShift + z - v1.z/scaleVect) / scale);
					glVertex3f((xShift + x + v2.x/scaleVect) / scale,
					           normalizeY(v2.y)/scaleVect / scale,
					           (zShift + z - v2.z/scaleVect) / scale);
				}
				xb += 1;
				if (xb >= blocksPerLine) {
					xb = 0;
					yb += 1;
				}
			}
		// }
		xs += 1;
		if (xs >= segmentPerLine) {
			xs = 0;
			ys += 1;
		}
	}

	glEnd();
}

void WorldmapGLWidget::wheelEvent(QWheelEvent *event)
{
	setFocus();
	qDebug() << event->delta();
	_distance += event->delta() / 4096.0;
	updateGL();
}

void WorldmapGLWidget::mousePressEvent(QMouseEvent *event)
{
	setFocus();
	_moveStart = event->pos();
	if(event->button() == Qt::MidButton)
	{
		_distance = -35;
		updateGL();
	}
}

void WorldmapGLWidget::mouseMoveEvent(QMouseEvent *event)
{
	/* if(event->modifiers() == Qt::CTRL) {
		if(event->button() == Qt::LeftButton)
		{
			_xRot += fmod((event->pos().x() - _moveStart.x()) / 4096.0f, 360.0f);
		}
		else if(event->button() == Qt::RightButton)
		{
			_yRot -= fmod((event->pos().y() - _moveStart.y()) / 4096.0f, 360.0f);
		}
	} else if(event->button() == Qt::LeftButton) {
		_xTrans += (event->pos().x() - _moveStart.x()) / 4096.0;
		_yTrans -= (event->pos().y() - _moveStart.y()) / 4096.0;
	}
	_moveStart = event->pos();
	updateGL(); */
}

void WorldmapGLWidget::keyPressEvent(QKeyEvent *event)
{
	qDebug() << "key press";
	if(_lastKeyPressed == event->key()
	        && (event->key() == Qt::Key_Left
	            || event->key() == Qt::Key_Right
	            || event->key() == Qt::Key_Down
	            || event->key() == Qt::Key_Up)) {
		if(_transStep > 100.0f) {
			_transStep *= 0.90f; // accelerator
		}
	} else {
		_transStep = 360.0f;
	}
	_lastKeyPressed = event->key();

	switch(event->key())
	{
	case Qt::Key_Left:
		_xTrans += 1.0f/_transStep;
		updateGL();
		break;
	case Qt::Key_Right:
		_xTrans -= 1.0f/_transStep;
		updateGL();
		break;
	case Qt::Key_Down:
		_yTrans += 1.0f/_transStep;
		updateGL();
		break;
	case Qt::Key_Up:
		_yTrans -= 1.0f/_transStep;
		updateGL();
		break;
	case Qt::Key_7:
		_xRot += 0.1f;
		updateGL();
		break;
	case Qt::Key_1:
		_xRot -= 0.1f;
		updateGL();
		break;
	case Qt::Key_8:
		_yRot += 0.1f;
		updateGL();
		break;
	case Qt::Key_2:
		_yRot -= 0.1f;
		updateGL();
		break;
	case Qt::Key_9:
		_zRot += 0.1f;
		updateGL();
		break;
	case Qt::Key_3:
		_zRot -= 0.1f;
		updateGL();
		break;
	default:
		QWidget::keyPressEvent(event);
		return;
	}
}

void WorldmapGLWidget::focusInEvent(QFocusEvent *event)
{
	grabKeyboard();
	QWidget::focusInEvent(event);
}

void WorldmapGLWidget::focusOutEvent(QFocusEvent *event)
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

QRgb WorldmapGLWidget::groundColor(quint8 groundType, quint8 region,
                                   const QSet<quint8> &grounds)
{
	QRgb c;

	switch(groundType) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		c = qRgb(57, 71, 45);
		break;
	case 6:
	case 15:
	case 24:
		c = qRgb(75, 80, 55);
		break;
	case 7:
		c = qRgb(106, 78, 63);
		break;
	case 8:
		c = qRgb(145, 124, 109);
		break;
	case 9:
		c = qRgb(144, 157, 164);
		break;
	case 10:
		//c = qRgb(122, 143, 158);
		//c = qRgb(0xD0, 0xD0, 0xBA); // yellow
		c = qRgb(0xA1, 0xA1, 0x8C);
		break;
	case 14:
		c = qRgb(116, 100, 90);
		break;
	case 16:
	case 25:
		c = qRgb(103, 85, 72);
		break;
	case 17:
	case 18:
	case 23:
		c = qRgb(57, 60, 53);
		break;
	case 27:
		c = qRgb(112, 97, 86);
		break;
	case 28:
		c = qRgb(69, 65, 64);
		break;
	case 29:
		c = qRgb(133, 108, 91);
		break;
	case 31:
		c = qRgb(53, 74, 75);
		break;
	case 32:
		c = qRgb(56, 88, 99);
		break;
	case 33:
		c = qRgb(40, 65, 81);
		break;
	case 34:
		c = qRgb(35, 60, 75);// 56, 88, 99
		break;
	default:
		const quint8 grey = 55 + groundType * 200 / 34;
		c = qRgb(grey, grey, grey);
		break;
	}

	if (/* grounds.contains(groundType) &&*/groundType >= 29 || !_colorRegions.contains(region)) {
		//QRgb cRegion = _colorRegions.value(region, Qt::transparent);
		QRgb cRegion = qRgb(128, 128, 128);
		/* int r = qRed(cRegion) * .5 + qRed(c),
			g = qGreen(cRegion) * .5 + qGreen(c),
			b = qBlue(cRegion) * .5 + qBlue(c); */
		int r = qRed(c) - qRed(cRegion) * .33,
		    g = qGreen(c) - qGreen(cRegion) * .33,
		    b = qBlue(c) - qBlue(cRegion) * .33;

		if (r > 255) r = 255;
		if (g > 255) g = 255;
		if (b > 255) b = 255;
		if (r < 0) r = 0;
		if (g < 0) g = 0;
		if (b < 0) b = 0;

		c = qRgb(r, g, b);
	}

	return c;
}
