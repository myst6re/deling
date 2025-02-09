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
#include "WorldmapGLWidget.h"

WorldmapGLWidget::WorldmapGLWidget(QWidget *parent,
                                   Qt::WindowFlags f) :
    QOpenGLWidget(parent, f), _map(nullptr),
    _distance(-0.714248f), _xRot(-90.0f), _yRot(180.0f), _zRot(180.0f),
    _xTrans(-0.5f), _yTrans(0.5f), _transStep(360.0f), _lastKeyPressed(-1),
    _texture(-1), _segmentGroupId(-1), _segmentId(-1), _blockId(-1),
    _groundType(-1), _polyId(-1), _clutId(-1), _limits(QRect(0, 0, 32, 24)),
    _megaTexture(nullptr), gpuRenderer(nullptr), _segmentFiltering(Map::NoFiltering)
{
	setMouseTracking(true);
}

WorldmapGLWidget::~WorldmapGLWidget()
{
	if (gpuRenderer) {
		delete gpuRenderer;
	}

	makeCurrent();
	buf.destroy();
	
	if (_megaTexture) {
		delete _megaTexture;
	}
}

void WorldmapGLWidget::resetCamera()
{
	_xRot = -90.0f;
	_yRot = 180.0f;
	_zRot = 180.0f;
	update();
}

void WorldmapGLWidget::setMap(Map *map)
{
	_map = map;

	importVertices();
	update();
}

void WorldmapGLWidget::setLimits(const QRect &rect)
{
	_limits = rect;
	importVertices();
	update();
}

void WorldmapGLWidget::setXTrans(float trans)
{
	_xTrans = trans;
	update();
}

void WorldmapGLWidget::setYTrans(float trans)
{
	_yTrans = trans;
	update();
}

void WorldmapGLWidget::setZTrans(float trans)
{
	_distance = trans;
	update();
}

void WorldmapGLWidget::setXRot(float rot)
{
	_xRot = rot;
	update();
}

void WorldmapGLWidget::setYRot(float rot)
{
	_yRot = rot;
	update();
}

void WorldmapGLWidget::setZRot(float rot)
{
	_zRot = rot;
	update();
}

void WorldmapGLWidget::setTexture(int texture)
{
	_texture = texture;
	update();
}

void WorldmapGLWidget::setSegmentGroupId(int segmentGroupId)
{
	_segmentGroupId = segmentGroupId;
	update();
}

void WorldmapGLWidget::setSegmentId(int segmentId)
{
	_segmentId = segmentId;
	update();
}

void WorldmapGLWidget::setBlockId(int blockId)
{
	_blockId = blockId;
	update();
}

void WorldmapGLWidget::setGroundType(int groundType)
{
	_groundType = groundType;
	update();
}

void WorldmapGLWidget::setPolyId(int polyId)
{
	_polyId = polyId;
	update();
}

void WorldmapGLWidget::setClutId(int clutId)
{
	_clutId = clutId;
	update();
}

void WorldmapGLWidget::setSegmentFiltering(Map::SegmentFiltering filtering)
{
	_segmentFiltering = filtering;
	
	importVertices();
	update();
}

void WorldmapGLWidget::dumpCurrent()
{
	const MapPoly &poly = _map->segments().at(_segmentId).blocks().at(_blockId).polygons().at(_polyId);
	qDebug() << QString::number(poly.flags1(), 16) << QString::number(poly.flags2(), 16)
	         << poly.groundType() << "texPage" << poly.texPage() << "clutId" << poly.clutId() << "hasTexture" << poly.hasTexture() << "isMonochrome" << poly.isMonochrome();
	for (const TexCoord &coord: poly.texCoords()) {
		qDebug() << "texcoord" << coord.x << coord.y;
	}	
	for (const Vertex &vertex: poly.vertices()) {
		qDebug() << "vertex" << vertex.x << vertex.y << vertex.z;
	}
}

void WorldmapGLWidget::initializeGL()
{
	if (gpuRenderer == nullptr) {
		gpuRenderer = new Renderer(this);
		importVertices();
	}
}

static quint16 normalizeY(qint16 y)
{
	return quint16(-(y - 128));
}

static QOpenGLTexture *textureFromImage(const QImage &image)
{
	QOpenGLTexture *texture = new QOpenGLTexture(image);
	texture->setMinificationFilter(QOpenGLTexture::NearestMipMapLinear);
	texture->setMagnificationFilter(QOpenGLTexture::Nearest);
	texture->setAutoMipMapGenerationEnabled(false);

	return texture;
}

void WorldmapGLWidget::importVertices()
{
	if (nullptr == _map || gpuRenderer == nullptr) {
		return;
	}
	
	QImage megaImage = _map->megaImage();
	if (_megaTexture) {
		delete _megaTexture;
	}
	_megaTexture = textureFromImage(megaImage);

	const int segmentPerLine = 32, blocksPerLine = 4,
	        diffSize = _limits.width() - _limits.height();
	const float scaleVect = 2048.0f, scaleTexX = float(_megaTexture->width() - 1), scaleTexY = float(_megaTexture->height() - 1), scale = _limits.width() * blocksPerLine;
	const float xShift = -_limits.x() * blocksPerLine + (diffSize < 0 ? -diffSize : 0) * blocksPerLine / 2.0f;
	const float zShift = -_limits.y() * blocksPerLine + (diffSize > 0 ? diffSize : 0) * blocksPerLine / 2.0f;
	int xs = 0, ys = 0;

	QList<MapSegment> segments = _map->segments(_segmentFiltering);
	QRgba64 color = QRgba64::fromRgba(0xFF, 0xFF, 0xFF, 0xFF);
	foreach (const MapSegment &segment, segments) {
		int xb = 0, yb = 0;
		foreach (const MapBlock &block, segment.blocks()) {
			foreach (const MapPoly &poly, block.polygons()) {
				const int x = xs * blocksPerLine + xb, z = ys * blocksPerLine + yb;

				if (poly.vertices().size() != 3) {
					qWarning() << "Wrong vertices size" << poly.vertices().size();
					return;
				}
				
				const int pageX = poly.texPage() / 5, pageY = poly.texPage() % 5;

				for (quint8 i = 0; i < 3; ++i) {
					const Vertex &v = poly.vertex(i);
					const TexCoord &tc = poly.texCoord(i);
					
					QVector3D position((xShift + x + v.x / scaleVect) / scale,
					                   normalizeY(v.y) / scaleVect / scale,
					                   (zShift + z - v.z / scaleVect) / scale);
					QVector2D texcoord;
					
					if (poly.isRoadTexture()) {
						texcoord.setX((4 * 256 + tc.x) / scaleTexX);
						texcoord.setY((1 * 256 + tc.y) / scaleTexY);
					} else if (poly.isWaterTexture()) {
						texcoord.setX((4 * 256 + tc.x) / scaleTexX);
						texcoord.setY((0 * 256 + tc.y) / scaleTexY);
					} else {
						texcoord.setX((pageX * 256 + tc.x) / scaleTexX);
						texcoord.setY((pageY * 256 + tc.y) / scaleTexY);
					}
					gpuRenderer->bufferVertex(position, color, texcoord);
				}
			}

			xb += 1;

			if (xb >= blocksPerLine) {
				xb = 0;
				yb += 1;
			}
		}

		xs += 1;

		if (xs >= segmentPerLine) {
			xs = 0;
			ys += 1;
		}
	}
}

void WorldmapGLWidget::resizeGL(int width, int height)
{
	if (gpuRenderer != nullptr) {
		gpuRenderer->setViewport(0, 0, width, height);
	}

	_matrixProj.setToIdentity();
	_matrixProj.perspective(70.0f, GLfloat(width) / GLfloat(height), 0.000001f, 1000.0f);
}

void WorldmapGLWidget::paintGL()
{
	gpuRenderer->clear();

	if (nullptr == _map || gpuRenderer->hasError()) {
		return;
	}
	gpuRenderer->bindProjectionMatrix(_matrixProj);

	if (_distance > -0.011124f) {
		_distance = -0.011124f;
	} else if (_distance < -1.78358f) {
		_distance = -1.78358f;
	}
	
	if (_xTrans > 0.0115338f) {
		_xTrans = 0.0115338f;
	} else if (_xTrans < -1.01512f) {
		_xTrans = -1.01512f;
	}
	
	if (_yTrans < 0.116807f) {
		_yTrans = 0.116807f;
	} else if (_yTrans > 0.892654f) {
		_yTrans = 0.892654f;
	}

	QMatrix4x4 mModel;
	mModel.translate(_xTrans, _yTrans, _distance);
	mModel.rotate(_xRot, 1.0f, 0.0f, 0.0f);
	mModel.rotate(_yRot, 0.0f, 1.0f, 0.0f);
	mModel.rotate(_zRot, 0.0f, 0.0f, 1.0f);
	
	QMatrix4x4 mView;

	gpuRenderer->bindModelMatrix(mModel);
	gpuRenderer->bindViewMatrix(mView);
	gpuRenderer->bindTexture(_megaTexture);
	
	gpuRenderer->draw(RendererPrimitiveType::PT_TRIANGLES, 1.0f, false);
}

void WorldmapGLWidget::wheelEvent(QWheelEvent *event)
{
	setFocus();
	_distance += double(event->angleDelta().y()) / 8192.0;
	update();
}

void WorldmapGLWidget::mousePressEvent(QMouseEvent *event)
{
	setFocus();

	if (event->button() == Qt::MiddleButton) {
		_distance = -0.714248f;
		update();
	} else if (event->button() == Qt::LeftButton) {
		_moveStart = event->position();
	}
}

void WorldmapGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
	_moveStart = QPointF();
}

void WorldmapGLWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (_moveStart.isNull()) {
		return;
	}

	QPointF diff = event->position() - _moveStart;
	bool toUpdate = false;
	
	if (abs(diff.x()) >= 4.0f) {
		_xTrans += (diff.x() > 0.0f ? 1.0f : -1.0f) / 360.0f;
		toUpdate = true;
	}
	if (abs(diff.y()) >= 4.0f) {
		_yTrans -= (diff.y() > 0.0f ? 1.0f : -1.0f) / 360.0f;
		toUpdate = true;
	}
	if (toUpdate) {
		update();
	}
}

void WorldmapGLWidget::keyPressEvent(QKeyEvent *event)
{
	if(_lastKeyPressed == event->key()
	        && (event->key() == Qt::Key_Left
	            || event->key() == Qt::Key_Right
	            || event->key() == Qt::Key_Down
	            || event->key() == Qt::Key_Up)) {
		if(_transStep > 100.0f) {
			_transStep *= 0.90f; // accelerator
		}
	} else {
		_transStep = 180.0f;
	}
	_lastKeyPressed = event->key();

	switch(event->key())
	{
	case Qt::Key_Left:
		_xTrans += 1.0f / _transStep;
		update();
		break;
	case Qt::Key_Right:
		_xTrans -= 1.0f / _transStep;
		update();
		break;
	case Qt::Key_Down:
		_yTrans += 1.0f / _transStep;
		update();
		break;
	case Qt::Key_Up:
		_yTrans -= 1.0f / _transStep;
		update();
		break;
	case Qt::Key_7:
		_xRot += 0.1f;
		update();
		break;
	case Qt::Key_1:
		_xRot -= 0.1f;
		update();
		break;
	case Qt::Key_8:
		_yRot += 0.1f;
		update();
		break;
	case Qt::Key_2:
		_yRot -= 0.1f;
		update();
		break;
	case Qt::Key_9:
		_zRot += 0.1f;
		update();
		break;
	case Qt::Key_3:
		_zRot -= 0.1f;
		update();
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
