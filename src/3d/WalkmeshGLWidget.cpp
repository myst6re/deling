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
#include <cmath>
#include <limits>

// Walkmesh coordinates are drawn divided by 4096
static QVector3D toScene(const Vertex_sr &point)
{
	return QVector3D(point.x / 4096.0f, point.y / 4096.0f, point.z / 4096.0f);
}

static QVector3D toScene(const Vertex &point)
{
	return QVector3D(point.x / 4096.0f, point.y / 4096.0f, point.z / 4096.0f);
}

static qint16 toCoordinate(float sceneValue)
{
	return qint16(qBound(-32768.0f, std::round(sceneValue * 4096.0f), 32767.0f));
}

static bool isUsed(const Gateway &gate)
{
	return gate.fieldId != GATEWAY_UNUSED;
}

/**
 * A triangle the game cannot use: flipped (never walked on), flat (drops the player to height 0)
 * or past the last id the script can lock
 */
static bool isBroken(const Triangle &triangle, int triangleID)
{
	return IdFile::isFlipped(triangle) || IdFile::isFlat(triangle) || triangleID >= IdFile::MAX_TRIANGLES;
}

/**
 * The triangles the top and free views are framed on: the ones the game can use. Broken
 * triangles are left out, as some reach far away from the floor (glpreo2) and would leave it tiny.
 */
static QList<const Triangle *> framedTriangles(IdFile *idFile)
{
	QList<const Triangle *> usable, all;

	for (int i = 0; i < idFile->triangleCount(); ++i) {
		all.append(&idFile->triangle(i));
		if (!isBroken(idFile->triangle(i), i)) {
			usable.append(&idFile->triangle(i));
		}
	}

	return usable.isEmpty() ? all : usable;
}

static qreal distanceToSegment(const QPointF &p, const QPointF &a, const QPointF &b)
{
	const QPointF ab = b - a;
	const qreal lengthSquared = QPointF::dotProduct(ab, ab);
	const qreal t = lengthSquared > 0.0 ? qBound(0.0, QPointF::dotProduct(p - a, ab) / lengthSquared, 1.0) : 0.0;

	return QLineF(p, a + ab * t).length();
}

WalkmeshGLWidget::WalkmeshGLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      viewZoom(1.0f), viewPanX(0.0f), viewPanY(0.0f), _viewMode(GameView), orbitYaw(0.0f), orbitPitch(45.0f),
      transStep(360.0f), lastKeyPressed(-1),
      camID(0), _selectedTriangle(-1), _selectedDoor(-1), _selectedGate(-1),
      _lineToDrawPoint1(Vertex()), _lineToDrawPoint2(Vertex()),
      fovy(70.0), data(nullptr), curFrame(0), gpuRenderer(nullptr), _drawLine(false),
      _backgroundVisible(true), _editMode(NoEdit), _dragging(false), _panning(false), _orbiting(false), _pickingArrival(false),
      _arrivalX(0), _arrivalY(0), _arrivalTriangle(-1)
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
	clearPointSelection();

	update();
	
	if (gpuRenderer) {
		gpuRenderer->reset();
	}
}

void WalkmeshGLWidget::fill(Field *data)
{
	this->data = data;
	// A field opened from loose files may have a walkmesh and no background at all
	tex = data->hasBackgroundFile() ? data->getBackgroundFile()->background() : QImage();
	clearPointSelection();
	updatePerspective();
	resetCamera();
}

void WalkmeshGLWidget::setEditMode(EditMode mode)
{
	if (mode == _editMode) {
		return;
	}

	_editMode = mode;
	// Highlighting what is under the mouse needs move events with no button held
	setMouseTracking(mode != NoEdit);
	clearPointSelection();
}

void WalkmeshGLWidget::setArrival(qint16 x, qint16 y, int triangle)
{
	_arrivalX = x;
	_arrivalY = y;
	_arrivalTriangle = triangle;
	update();
}

void WalkmeshGLWidget::clearPointSelection()
{
	_hoveredPoint.reset();
	_selectedPoint.reset();
	_draggedPoint.reset();
	_sidePreview.reset();
	_hoveredExitEnd.reset();
	_draggedExitEnd.reset();
	_hoveredExit.reset();
	_exitPreview.reset();
	_hoveredFloor.reset();
	_dragging = false;
	_pickingArrival = false;
	update();
}

/**
 * How much of the widget the game screen occupies, keeping its aspect: the background and the
 * walkmesh both have to be drawn inside this rectangle or they cannot line up.
 */
void WalkmeshGLWidget::screenLetterbox(float &sx, float &sy) const
{
	const float widgetAspect = float(width()) / float(height()),
	            screenAspect = float(SCREEN_WIDTH) / float(SCREEN_HEIGHT);

	if (widgetAspect > screenAspect) {
		sx = screenAspect / widgetAspect;   // pillarbox
		sy = 1.0f;
	} else {
		sx = 1.0f;
		sy = widgetAspect / screenAspect;   // letterbox
	}
}
void WalkmeshGLWidget::computeFov()
{
	if (data && data->hasCaFile()
			&& data->getCaFile()->cameraCount() > 0
			&& camID < data->getCaFile()->cameraCount()) {
		const Camera &cam = data->getCaFile()->camera(camID);
		// The game projects onto the 320x224 screen with its centre at (160, 112), so the
		// vertical half-extent is 112 - not the 240 projection-plane height of the PSX GTE.
		fovy = (2 * atan(112.0/cam.camera_zoom)) * 57.29577951;
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

	// The top and free views have no picture behind the walkmesh: a dark ground shows every line
	if (_viewMode == GameView) {
		gpuRenderer->clear();
	} else {
		gpuRenderer->clear(0.12f, 0.12f, 0.12f, 1.0f);
	}

	bindSceneMatrices();

	if (_backgroundVisible && !tex.isNull()) {
		if (_viewMode == GameView) {
			drawBackground();
		} else if (data->hasIdFile()) {
			drawBackgroundOnFloor();
		}
		bindSceneMatrices();
	}

	if (data->hasIdFile()) {
		drawWalkmesh();
	}

	const QVector2D texcoord;

	if (_drawLine) {
		// A script line is shown on its own, without the exits and doors
		gpuRenderer->bufferVertex(toScene(_lineToDrawPoint1), QRgba64::fromArgb32(COLOR_SCRIPT), texcoord);
		gpuRenderer->bufferVertex(toScene(_lineToDrawPoint2), QRgba64::fromArgb32(COLOR_SCRIPT), texcoord);
		gpuRenderer->draw(RendererPrimitiveType::PT_LINES);
	} else if (data->hasInfFile()) {
		drawExitsAndDoors();
	}

	if (_editMode == PickArrival && data->hasIdFile()) {
		drawArrival();
	}
}

void WalkmeshGLWidget::bindSceneMatrices()
{
	gpuRenderer->bindProjectionMatrix(projectionMatrix());
	gpuRenderer->bindViewMatrix(viewMatrix());
	gpuRenderer->bindModelMatrix(QMatrix4x4());
}

void WalkmeshGLWidget::drawWalkmesh()
{
	IdFile *idFile = data->getIdFile();
	const QVector2D texcoord;
	const QMatrix4x4 mvp = sceneToClip();
	// The selected triangle is noise while exits are edited, and belongs to another field while
	// an exit's destination is shown
	const bool showSelectedTriangle = _editMode == NoEdit || _editMode == EditWalkmesh;
	const bool hasSelectedTriangle = showSelectedTriangle && _selectedTriangle >= 0
	                                 && _selectedTriangle < idFile->triangleCount();

	for (int i = 0; i < idFile->triangleCount(); ++i) {
		const Triangle &triangle = idFile->triangle(i);
		const Access &access = idFile->access(i);
		const bool broken = isBroken(triangle, i);

		for (int side = 0; side < 3; ++side) {
			const Vertex_sr &from = triangle.vertices[side], &to = triangle.vertices[(side + 1) % 3];

			// Only the sides between two triangles are thin: walls, broken and selected
			// triangles have to be seen over any background
			if (hasSelectedTriangle && i == _selectedTriangle) {
				bufferWideLine(mvp, toScene(from), toScene(to), COLOR_SELECTED, WIDE_LINE_WIDTH);
			} else if (broken) {
				bufferWideLine(mvp, toScene(from), toScene(to), COLOR_BROKEN, WIDE_LINE_WIDTH);
			} else if (access.a[side] == -1) {
				bufferWideLine(mvp, toScene(from), toScene(to), COLOR_WALL, WIDE_LINE_WIDTH);
			} else {
				bufferLine(from, to, QRgba64::fromArgb32(COLOR_SIDE));
			}
		}
	}

	// The triangle a click would add, dashed until it exists
	if (_sidePreview) {
		const Triangle &source = idFile->triangle(_sidePreview->triangleID);
		const QRgba64 color = QRgba64::fromArgb32(COLOR_HOVER);
		bufferLine(source.vertices[_sidePreview->side], _sidePreview->point, color, true);
		bufferLine(source.vertices[(_sidePreview->side + 1) % 3], _sidePreview->point, color, true);
	}

	gpuRenderer->draw(RendererPrimitiveType::PT_LINES);
	drawWideLines();

	if (hasSelectedTriangle) {
		for (const Vertex_sr &vertex: idFile->triangle(_selectedTriangle).vertices) {
			gpuRenderer->bufferVertex(toScene(vertex), QRgba64::fromArgb32(COLOR_SELECTED), texcoord);
		}
	}

	gpuRenderer->draw(RendererPrimitiveType::PT_POINTS, 7.0f);

	// The selected point, and what a click would grab or add - or where a dragged exit end snaps
	if (_selectedPoint) {
		gpuRenderer->bufferVertex(toScene(*_selectedPoint), QRgba64::fromArgb32(COLOR_SELECTED), texcoord);
	}
	if (_hoveredPoint) {
		gpuRenderer->bufferVertex(toScene(*_hoveredPoint), QRgba64::fromArgb32(COLOR_HOVER), texcoord);
	}
	if (_sidePreview) {
		gpuRenderer->bufferVertex(toScene(_sidePreview->point), QRgba64::fromArgb32(COLOR_HOVER), texcoord);
	}

	gpuRenderer->draw(RendererPrimitiveType::PT_POINTS, 11.0f);
}

void WalkmeshGLWidget::drawExitsAndDoors()
{
	InfFile *inf = data->getInfFile();
	const QVector2D texcoord;
	const bool editingExits = _editMode == EditExits;

	const QMatrix4x4 mvp = sceneToClip();

	// Exits are wider than walls: they usually run along one and have to stand out from it
	for (int gateID = 0; gateID < 12; ++gateID) {
		const Gateway &gate = inf->getGateway(gateID);

		if (!isUsed(gate)) {
			continue;
		}

		const bool hovered = _hoveredExit == gateID || (_hoveredExitEnd && _hoveredExitEnd->gate == gateID);
		const QRgb color = editingExits && hovered ? COLOR_HOVER
		                   : editingExits && gateID == _selectedGate ? COLOR_SELECTED
		                   : COLOR_EXIT;
		bufferWideLine(mvp, toScene(gate.exitLine[0]), toScene(gate.exitLine[1]), color, EXIT_LINE_WIDTH);
	}

	for (const Trigger &trigger: inf->getTriggers()) {
		bufferWideLine(mvp, toScene(trigger.trigger_line[0]), toScene(trigger.trigger_line[1]), COLOR_DOOR, WIDE_LINE_WIDTH);
	}

	drawWideLines();

	// The exit a click would add, along a wall
	if (_exitPreview) {
		bufferLine(_exitPreview->a, _exitPreview->b, QRgba64::fromArgb32(COLOR_HOVER), true);
	}

	gpuRenderer->draw(RendererPrimitiveType::PT_LINES);

	// Exit ends, so they can be seen and grabbed
	for (int gateID = 0; gateID < 12; ++gateID) {
		const Gateway &gate = inf->getGateway(gateID);

		if (isUsed(gate) && (editingExits || gateID == _selectedGate)) {
			gpuRenderer->bufferVertex(toScene(gate.exitLine[0]), QRgba64::fromArgb32(COLOR_EXIT), texcoord);
			gpuRenderer->bufferVertex(toScene(gate.exitLine[1]), QRgba64::fromArgb32(COLOR_EXIT), texcoord);
		}
	}

	if (_selectedDoor >= 0 && _selectedDoor < 12) {
		const Trigger &trigger = inf->getTrigger(_selectedDoor);
		if (trigger.doorID != 0xFF) {
			gpuRenderer->bufferVertex(toScene(trigger.trigger_line[0]), QRgba64::fromArgb32(COLOR_DOOR), texcoord);
			gpuRenderer->bufferVertex(toScene(trigger.trigger_line[1]), QRgba64::fromArgb32(COLOR_DOOR), texcoord);
		}
	}

	gpuRenderer->draw(RendererPrimitiveType::PT_POINTS, 7.0f);

	if (editingExits) {
		if (_selectedGate >= 0 && _selectedGate < 12 && isUsed(inf->getGateway(_selectedGate))) {
			const Gateway &gate = inf->getGateway(_selectedGate);
			gpuRenderer->bufferVertex(toScene(gate.exitLine[0]), QRgba64::fromArgb32(COLOR_SELECTED), texcoord);
			gpuRenderer->bufferVertex(toScene(gate.exitLine[1]), QRgba64::fromArgb32(COLOR_SELECTED), texcoord);
		}
		if (_hoveredExitEnd) {
			const Vertex &end = inf->getGateway(_hoveredExitEnd->gate).exitLine[_hoveredExitEnd->end];
			gpuRenderer->bufferVertex(toScene(end), QRgba64::fromArgb32(COLOR_HOVER), texcoord);
		}
	}

	gpuRenderer->draw(RendererPrimitiveType::PT_POINTS, 11.0f);
}

// Where an exit arrives, shown over its destination field
void WalkmeshGLWidget::drawArrival()
{
	IdFile *idFile = data->getIdFile();
	const QVector2D texcoord;
	const QRgba64 selected = QRgba64::fromArgb32(COLOR_SELECTED);

	if (_arrivalTriangle >= 0 && _arrivalTriangle < idFile->triangleCount()) {
		const Triangle &triangle = idFile->triangle(_arrivalTriangle);

		for (int side = 0; side < 3; ++side) {
			bufferLine(triangle.vertices[side], triangle.vertices[(side + 1) % 3], selected);
		}
		gpuRenderer->draw(RendererPrimitiveType::PT_LINES);

		gpuRenderer->bufferVertex(toScene(arrivalPoint()), selected, texcoord);
	}

	if (_hoveredFloor) {
		gpuRenderer->bufferVertex(toScene(_hoveredFloor->point), QRgba64::fromArgb32(COLOR_HOVER), texcoord);
	}

	gpuRenderer->draw(RendererPrimitiveType::PT_POINTS, 13.0f);
}

/**
 * The height of a triangle's plane at (x, y). Field_Walkmesh_PlaceEntitiesOnLoad computes the
 * arrival height the same way, from the triangle alone.
 */
static float heightOnTriangle(const Triangle &triangle, float x, float y)
{
	const QVector3D a(triangle.vertices[0].x, triangle.vertices[0].y, triangle.vertices[0].z),
	                b(triangle.vertices[1].x, triangle.vertices[1].y, triangle.vertices[1].z),
	                c(triangle.vertices[2].x, triangle.vertices[2].y, triangle.vertices[2].z);
	const QVector3D normal = QVector3D::crossProduct(b - a, c - a);

	if (qFuzzyIsNull(normal.z())) {
		return a.z(); // a flat triangle has no height to give
	}

	return a.z() - (normal.x() * (x - a.x()) + normal.y() * (y - a.y())) / normal.z();
}

Vertex_sr WalkmeshGLWidget::arrivalPoint() const
{
	const Triangle &triangle = data->getIdFile()->triangle(_arrivalTriangle);
	Vertex_sr point = {};

	if (_arrivalX == 0x7FFF) {
		// The game puts the player at the centre of the triangle
		point.x = qint16((triangle.vertices[0].x + triangle.vertices[1].x + triangle.vertices[2].x) / 3);
		point.y = qint16((triangle.vertices[0].y + triangle.vertices[1].y + triangle.vertices[2].y) / 3);
		point.z = qint16((triangle.vertices[0].z + triangle.vertices[1].z + triangle.vertices[2].z) / 3);
	} else {
		point.x = _arrivalX;
		point.y = _arrivalY;
		point.z = qint16(heightOnTriangle(triangle, _arrivalX, _arrivalY));
	}

	return point;
}

// The game's projection onto its 320x224 screen, without the zoom and the letterbox of the widget
QMatrix4x4 WalkmeshGLWidget::gameProjectionMatrix() const
{
	QMatrix4x4 projection;
	projection.perspective(fovy, float(SCREEN_WIDTH) / float(SCREEN_HEIGHT), 0.001f, 1000.0f);

	return projection;
}

QMatrix4x4 WalkmeshGLWidget::projectionMatrix() const
{
	QMatrix4x4 projection = screenMatrix();
	const float aspect = float(width()) / float(qMax(1, height()));
	float radius = 1.0f;
	sceneCentre(radius);
	const float distance = viewDistance(radius);

	switch (_viewMode) {
	case GameView: {
		// The mesh must land in the same rectangle as the background, so project with the
		// SCREEN aspect and letterbox that rectangle into the widget - using the widget's own
		// aspect made the mesh drift sideways from the background on any non-4:3 window.
		float sx = 1.0f, sy = 1.0f;
		screenLetterbox(sx, sy);
		projection.scale(sx, sy, 1.0f);
		projection *= gameProjectionMatrix();
		break;
	}
	case TopView: {
		// No perspective, so distances on the floor look the same near and far; framed on the
		// walkmesh as seen from above, which fills the view even with a long corridor
		float halfWidth = 0.05f, halfHeight = 0.05f;
		const QMatrix4x4 view = viewMatrix();
		if (data && data->hasIdFile()) {
			for (const Triangle *triangle: framedTriangles(data->getIdFile())) {
				for (const Vertex_sr &vertex: triangle->vertices) {
					const QVector3D seen = view.map(toScene(vertex));
					halfWidth = qMax(halfWidth, qAbs(seen.x()));
					halfHeight = qMax(halfHeight, qAbs(seen.y()));
				}
			}
		}
		halfWidth *= 1.05f;
		halfHeight *= 1.05f;
		if (halfWidth < halfHeight * aspect) {
			halfWidth = halfHeight * aspect;
		} else {
			halfHeight = halfWidth / aspect;
		}
		projection.ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, 0.01f, distance + radius * 4.0f);
		break;
	}
	case FreeView:
		projection.perspective(45.0f, aspect, 0.01f, distance + radius * 4.0f);
		break;
	}

	return projection;
}

/**
 * The middle of the walkmesh, and the radius of a sphere holding all of it, in scene units:
 * the top and free views turn around it and frame it.
 */
QVector3D WalkmeshGLWidget::sceneCentre(float &radius) const
{
	if (!data || !data->hasIdFile() || data->getIdFile()->triangleCount() == 0) {
		radius = 1.0f;
		return QVector3D();
	}

	QVector3D minimum(1e9f, 1e9f, 1e9f), maximum(-1e9f, -1e9f, -1e9f);

	for (const Triangle *triangle: framedTriangles(data->getIdFile())) {
		for (const Vertex_sr &vertex: triangle->vertices) {
			const QVector3D point = toScene(vertex);
			minimum = QVector3D(qMin(minimum.x(), point.x()), qMin(minimum.y(), point.y()), qMin(minimum.z(), point.z()));
			maximum = QVector3D(qMax(maximum.x(), point.x()), qMax(maximum.y(), point.y()), qMax(maximum.z(), point.z()));
		}
	}

	radius = qMax(0.05f, (maximum - minimum).length() / 2.0f);

	return (minimum + maximum) / 2.0f;
}

// How far the top and free cameras stand from the centre: close enough for the walkmesh to fill
// the free view seen from the side, the wheel zooms out for the rest
float WalkmeshGLWidget::viewDistance(float radius) const
{
	return radius * 1.6f;
}

// Where the game camera stands, in scene units
bool WalkmeshGLWidget::gameCameraEye(QVector3D &eye) const
{
	if (!data || !data->hasCaFile() || camID >= data->getCaFile()->cameraCount()) {
		return false;
	}

	bool invertible = false;
	const QMatrix4x4 cameraToScene = gameViewMatrix().inverted(&invertible);
	eye = cameraToScene.map(QVector3D());

	return invertible;
}

/**
 * Which way is up: +1 when the game camera is on the +Z side of the floor, -1 otherwise. The
 * top and free views look from that side, so the field is not seen from below or mirrored.
 */
float WalkmeshGLWidget::upSide() const
{
	QVector3D eye;
	float radius = 1.0f;
	const QVector3D centre = sceneCentre(radius);

	return gameCameraEye(eye) && eye.z() < centre.z() ? -1.0f : 1.0f;
}

QMatrix4x4 WalkmeshGLWidget::viewMatrix() const
{
	float radius = 1.0f;
	const QVector3D centre = sceneCentre(radius);
	const float distance = viewDistance(radius), side = upSide();
	QMatrix4x4 view;

	switch (_viewMode) {
	case GameView:
		return gameViewMatrix();
	case TopView: {
		// Straight down, turned so that what is far from the game camera is at the top, as in the game
		QVector3D eye, forward(0.0f, 1.0f, 0.0f);
		if (gameCameraEye(eye)) {
			const QVector3D horizontal(centre.x() - eye.x(), centre.y() - eye.y(), 0.0f);
			if (horizontal.lengthSquared() > 1e-8f) {
				forward = horizontal.normalized();
			}
		}
		view.lookAt(centre + QVector3D(0.0f, 0.0f, side * distance), centre, forward);
		return view;
	}
	case FreeView: {
		const float yaw = qDegreesToRadians(orbitYaw), pitch = qDegreesToRadians(orbitPitch);
		const QVector3D direction(std::sin(pitch) * std::cos(yaw), std::sin(pitch) * std::sin(yaw), side * std::cos(pitch));
		view.lookAt(centre + direction * distance, centre, QVector3D(0.0f, 0.0f, side));
		return view;
	}
	}

	return view;
}

QMatrix4x4 WalkmeshGLWidget::gameViewMatrix() const
{
	QMatrix4x4 view;

	if (data && data->hasCaFile() && data->getCaFile()->cameraCount() > 0 && camID < data->getCaFile()->cameraCount()) {
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
		view.lookAt(eye, center, up);
	}

	return view;
}

/**
 * Zooming and moving the view act on the picture the game shows, background and walkmesh
 * together, like zooming into a screenshot: moving the walkmesh alone in 3D made it slide off
 * the background it has to match.
 */
QMatrix4x4 WalkmeshGLWidget::screenMatrix() const
{
	QMatrix4x4 screen;
	screen.translate(viewPanX, viewPanY);
	screen.scale(viewZoom, viewZoom, 1.0f);

	return screen;
}

// Everything picking needs goes through the matrices used for drawing, so what a click hits is
// always exactly what is on screen
QMatrix4x4 WalkmeshGLWidget::sceneToClip() const
{
	return projectionMatrix() * viewMatrix();
}

void WalkmeshGLWidget::bufferLine(const Vertex_sr &from, const Vertex_sr &to, QRgba64 color, bool dashed)
{
	const QVector3D a = toScene(from), b = toScene(to);
	const QVector2D texcoord;

	if (!dashed) {
		gpuRenderer->bufferVertex(a, color, texcoord);
		gpuRenderer->bufferVertex(b, color, texcoord);
		return;
	}

	// Cut the line in DASHES pieces and draw every other one
	const int DASHES = 11;
	for (int i = 0; i < DASHES; i += 2) {
		gpuRenderer->bufferVertex(a + (b - a) * (float(i) / DASHES), color, texcoord);
		gpuRenderer->bufferVertex(a + (b - a) * (float(i + 1) / DASHES), color, texcoord);
	}
}

/**
 * A line `pixels` wide, as two triangles facing the screen: OpenGL lines are one pixel wide on
 * most drivers. The triangles are kept in screen coordinates and drawn by drawWideLines().
 */
void WalkmeshGLWidget::bufferWideLine(const QMatrix4x4 &sceneToClip, const QVector3D &from, const QVector3D &to,
                                      QRgb color, float pixels)
{
	const float NEAR_W = 1e-4f;
	QVector4D a = sceneToClip * QVector4D(from, 1.0f), b = sceneToClip * QVector4D(to, 1.0f);

	// Keep only the part in front of the camera
	if (a.w() < NEAR_W && b.w() < NEAR_W) {
		return;
	} else if (a.w() < NEAR_W) {
		a += (b - a) * ((NEAR_W - a.w()) / (b.w() - a.w()));
	} else if (b.w() < NEAR_W) {
		b += (a - b) * ((NEAR_W - b.w()) / (a.w() - b.w()));
	}

	const QVector3D screenA = a.toVector3DAffine(), screenB = b.toVector3DAffine();
	QVector2D direction((screenB.x() - screenA.x()) * width(), (screenB.y() - screenA.y()) * height());
	if (direction.lengthSquared() < 1e-12f) {
		direction = QVector2D(1.0f, 0.0f);
	}
	direction.normalize();
	// Half the width on each side; the screen is 2 units wide for width() pixels
	const QVector3D offset(-direction.y() * pixels / width(), direction.x() * pixels / height(), 0.0f);
	const QRgba64 rgba = QRgba64::fromArgb32(color);

	for (const QVector3D &corner: {screenA - offset, screenA + offset, screenB + offset,
	                               screenA - offset, screenB + offset, screenB - offset}) {
		_wideLineVertices.append(ScreenVertex{corner, rgba});
	}
}

void WalkmeshGLWidget::drawWideLines()
{
	if (_wideLineVertices.isEmpty()) {
		return;
	}

	const QMatrix4x4 identity;
	const QVector2D texcoord;
	gpuRenderer->bindProjectionMatrix(identity);
	gpuRenderer->bindViewMatrix(identity);

	for (const ScreenVertex &vertex: _wideLineVertices) {
		gpuRenderer->bufferVertex(vertex.position, vertex.color, texcoord);
	}
	gpuRenderer->draw(RendererPrimitiveType::PT_TRIANGLES);
	_wideLineVertices.clear();

	bindSceneMatrices();
}

/**
 * The background painted onto the walkmesh, for the top and free views where the flat picture
 * cannot be shown: each point of the floor takes the colour the game camera sees there. What
 * stands in front of the floor in the game (a lamp post, a character) is painted onto it too.
 */
void WalkmeshGLWidget::drawBackgroundOnFloor()
{
	if (!data->hasCaFile() || camID >= data->getCaFile()->cameraCount()) {
		return;
	}

	const QMatrix4x4 gameToClip = gameProjectionMatrix() * gameViewMatrix();
	const QRgba64 white = QRgba64::fromArgb32(0xFFFFFFFF);
	const QVector2D pictureSize(tex.width(), tex.height());
	// Where a scene point is in the background picture; false when the game camera does not see it
	auto pictureCoordinates = [&](const QVector3D &point, QVector2D &uv) {
		const QVector4D clip = gameToClip * QVector4D(point, 1.0f);
		if (clip.w() <= 0.0f) {
			return false;
		}
		uv = QVector2D(0.5f + clip.x() / clip.w() * 0.5f * SCREEN_WIDTH / tex.width(),
		               0.5f - clip.y() / clip.w() * 0.5f * SCREEN_HEIGHT / tex.height());
		return uv.x() >= 0.0f && uv.x() <= 1.0f && uv.y() >= 0.0f && uv.y() <= 1.0f;
	};

	for (const Triangle &triangle: data->getIdFile()->getTriangles()) {
		const QVector3D a = toScene(triangle.vertices[0]), b = toScene(triangle.vertices[1]), c = toScene(triangle.vertices[2]);
		QVector2D uvA, uvB, uvC;
		pictureCoordinates(a, uvA);
		pictureCoordinates(b, uvB);
		pictureCoordinates(c, uvC);

		// The picture is a perspective view, which a triangle cannot follow exactly: big triangles
		// are cut in smaller ones, about every 16 pixels of the picture
		const float longest = qMax(qMax(((uvB - uvA) * pictureSize).length(), ((uvC - uvB) * pictureSize).length()),
		                           ((uvA - uvC) * pictureSize).length());
		const int cuts = qBound(1, int(longest / 16.0f), 16);

		for (int i = 0; i < cuts; ++i) {
			for (int j = 0; j < cuts - i; ++j) {
				// The cell (i, j) of a grid spread over the triangle: one small triangle, and a
				// second one except along the far side
				const QPoint cells[2][3] = {
				    {QPoint(i, j), QPoint(i + 1, j), QPoint(i, j + 1)},
				    {QPoint(i + 1, j), QPoint(i + 1, j + 1), QPoint(i, j + 1)}
				};
				const int cellCount = j < cuts - i - 1 ? 2 : 1;

				for (int cell = 0; cell < cellCount; ++cell) {
					QVector3D corners[3];
					QVector2D uvs[3];
					bool seen = true;

					for (int k = 0; k < 3 && seen; ++k) {
						const float u = float(cells[cell][k].x()) / cuts, v = float(cells[cell][k].y()) / cuts;
						corners[k] = a + (b - a) * u + (c - a) * v;
						seen = pictureCoordinates(corners[k], uvs[k]);
					}

					if (seen) {
						for (int k = 0; k < 3; ++k) {
							gpuRenderer->bufferVertex(corners[k], white, uvs[k]);
						}
					}
				}
			}
		}
	}

	gpuRenderer->bindTexture(tex);
	gpuRenderer->draw(RendererPrimitiveType::PT_TRIANGLES);
	// The walkmesh lies exactly on this floor: without this its lines would be hidden half the time
	gpuRenderer->clearDepth();
}

bool WalkmeshGLWidget::toScreen(const QMatrix4x4 &sceneToClip, const Vertex_sr &point, QPointF &screen) const
{
	const QVector4D clip = sceneToClip * QVector4D(toScene(point), 1.0f);

	if (clip.w() <= 0.0f) {
		return false; // behind the camera
	}

	screen = QPointF((clip.x() / clip.w() + 1.0) * 0.5 * width(),
	                 (1.0 - clip.y() / clip.w()) * 0.5 * height());

	return true;
}

// The mouse ray in scene coordinates, from the near plane to the far plane
bool WalkmeshGLWidget::mouseRay(const QPoint &pos, QVector3D &nearPoint, QVector3D &farPoint) const
{
	bool invertible = false;
	const QMatrix4x4 clipToScene = sceneToClip().inverted(&invertible);

	if (!invertible) {
		return false;
	}

	const float ndcX = 2.0f * pos.x() / width() - 1.0f, ndcY = 1.0f - 2.0f * pos.y() / height();
	const QVector4D nearClip = clipToScene * QVector4D(ndcX, ndcY, -1.0f, 1.0f),
	                farClip = clipToScene * QVector4D(ndcX, ndcY, 1.0f, 1.0f);
	nearPoint = nearClip.toVector3D() / nearClip.w();
	farPoint = farClip.toVector3D() / farClip.w();

	return true;
}

/**
 * Where the mouse points on the horizontal plane at `planeHeight`: a point is dragged along the
 * floor at its own height, so it moves over the ground seen through the game camera.
 */
bool WalkmeshGLWidget::mouseOnHeight(const QPoint &pos, qint16 planeHeight, Vertex_sr &point) const
{
	QVector3D nearPoint, farPoint;

	if (!mouseRay(pos, nearPoint, farPoint)) {
		return false;
	}

	const float planeZ = planeHeight / 4096.0f, dz = farPoint.z() - nearPoint.z();

	if (qFuzzyIsNull(dz)) {
		return false; // looking along the plane
	}

	const float t = (planeZ - nearPoint.z()) / dz;

	// Behind the camera or past the far plane: the mouse is above the horizon of that plane,
	// on a wall of the background for instance, and the point would land out of sight
	if (t < 0.0f || t > 1.0f) {
		return false;
	}

	const QVector3D hit = nearPoint + (farPoint - nearPoint) * t;

	// Too far for the 16-bit coordinates of the file: rounding it to the limit would put the
	// point somewhere else than under the mouse
	if (qAbs(hit.x() * 4096.0f) > 32767.0f || qAbs(hit.y() * 4096.0f) > 32767.0f) {
		return false;
	}

	point.x = toCoordinate(hit.x());
	point.y = toCoordinate(hit.y());
	point.z = planeHeight;
	point.res = 0;

	return true;
}

/**
 * The walkmesh point under the mouse: where the mouse ray meets the nearest triangle seen
 * through it, and that triangle's id.
 */
std::optional<WalkmeshGLWidget::FloorPoint> WalkmeshGLWidget::floorAt(const QPoint &pos) const
{
	QVector3D nearPoint, farPoint;

	if (!data || !data->hasIdFile() || !mouseRay(pos, nearPoint, farPoint)) {
		return std::nullopt;
	}

	const QVector3D ray = farPoint - nearPoint;
	std::optional<FloorPoint> nearest;
	float nearestT = std::numeric_limits<float>::max();
	IdFile *idFile = data->getIdFile();

	for (int triangleID = 0; triangleID < idFile->triangleCount(); ++triangleID) {
		const Triangle &triangle = idFile->triangle(triangleID);

		if (IdFile::isFlat(triangle)) {
			continue;
		}

		const QVector3D a = toScene(triangle.vertices[0]), b = toScene(triangle.vertices[1]), c = toScene(triangle.vertices[2]);
		const QVector3D normal = QVector3D::crossProduct(b - a, c - a);
		const float along = QVector3D::dotProduct(normal, ray);

		if (qFuzzyIsNull(along)) {
			continue; // looking along the triangle
		}

		const float t = QVector3D::dotProduct(normal, a - nearPoint) / along;
		if (t < 0.0f || t > nearestT) {
			continue;
		}

		// Inside when the hit is on the same side of the three sides, seen from above
		const QVector3D hit = nearPoint + ray * t;
		auto sideOf = [&hit](const QVector3D &from, const QVector3D &to) {
			return (to.x() - from.x()) * (hit.y() - from.y()) - (to.y() - from.y()) * (hit.x() - from.x());
		};
		const float ab = sideOf(a, b), bc = sideOf(b, c), ca = sideOf(c, a);

		if ((ab >= 0 && bc >= 0 && ca >= 0) || (ab <= 0 && bc <= 0 && ca <= 0)) {
			nearestT = t;
			nearest = FloorPoint{{toCoordinate(hit.x()), toCoordinate(hit.y()), toCoordinate(hit.z()), 0}, triangleID};
		}
	}

	return nearest;
}

std::optional<Vertex_sr> WalkmeshGLWidget::pointAt(const QPoint &pos, const std::optional<Vertex_sr> &ignored) const
{
	if (!data || !data->hasIdFile()) {
		return std::nullopt;
	}

	const QMatrix4x4 mvp = sceneToClip();
	std::optional<Vertex_sr> nearest;
	qreal nearestDistance = PICK_RADIUS;

	for (const Triangle &triangle: data->getIdFile()->getTriangles()) {
		for (const Vertex_sr &vertex: triangle.vertices) {
			QPointF screen;

			if ((ignored && IdFile::samePoint(vertex, *ignored)) || !toScreen(mvp, vertex, screen)) {
				continue;
			}

			const qreal distance = QLineF(screen, QPointF(pos)).length();
			if (distance <= nearestDistance) {
				nearestDistance = distance;
				nearest = vertex;
			}
		}
	}

	return nearest;
}

bool WalkmeshGLWidget::isOnFloor(const QPoint &pos) const
{
	const QMatrix4x4 mvp = sceneToClip();

	for (const Triangle &triangle: data->getIdFile()->getTriangles()) {
		QPolygonF polygon;
		QPointF screen;
		bool visible = true;

		for (const Vertex_sr &vertex: triangle.vertices) {
			if (!toScreen(mvp, vertex, screen)) {
				visible = false;
				break;
			}
			polygon << screen;
		}

		if (visible && polygon.containsPoint(QPointF(pos), Qt::OddEvenFill)) {
			return true;
		}
	}

	return false;
}

/**
 * The triangle a click would add: on the border side (a wall) nearest to the mouse. Attaching
 * to a side, rather than to the two nearest points, always makes a clean triangle - the two
 * nearest points are not necessarily joined, and a triangle between them could cross others.
 */
std::optional<WalkmeshGLWidget::SidePreview> WalkmeshGLWidget::sidePreviewAt(const QPoint &pos) const
{
	const QMatrix4x4 mvp = sceneToClip();
	IdFile *idFile = data->getIdFile();
	int nearestTriangle = -1, nearestSide = -1;
	qreal nearestDistance = std::numeric_limits<qreal>::max();

	for (int triangleID = 0; triangleID < idFile->triangleCount(); ++triangleID) {
		const Triangle &triangle = idFile->triangle(triangleID);

		for (int side = 0; side < 3; ++side) {
			QPointF a, b;

			if (idFile->access(triangleID).a[side] != -1
			        || !toScreen(mvp, triangle.vertices[side], a)
			        || !toScreen(mvp, triangle.vertices[(side + 1) % 3], b)) {
				continue;
			}

			const qreal distance = distanceToSegment(QPointF(pos), a, b);
			if (distance <= ADD_RADIUS && distance < nearestDistance) {
				nearestDistance = distance;
				nearestTriangle = triangleID;
				nearestSide = side;
			}
		}
	}

	if (nearestTriangle < 0) {
		return std::nullopt;
	}

	// The new point sits at the height of the side it is attached to
	const Triangle &triangle = idFile->triangle(nearestTriangle);
	const qint16 height = qint16((triangle.vertices[nearestSide].z + triangle.vertices[(nearestSide + 1) % 3].z) / 2);
	Vertex_sr point;

	if (!mouseOnHeight(pos, height, point)) {
		return std::nullopt;
	}

	return SidePreview{nearestTriangle, nearestSide, point};
}

std::optional<WalkmeshGLWidget::ExitEnd> WalkmeshGLWidget::exitEndAt(const QPoint &pos) const
{
	if (!data || !data->hasInfFile()) {
		return std::nullopt;
	}

	const QMatrix4x4 mvp = sceneToClip();
	std::optional<ExitEnd> nearest;
	qreal nearestDistance = PICK_RADIUS;

	for (int gateID = 0; gateID < 12; ++gateID) {
		const Gateway &gate = data->getInfFile()->getGateway(gateID);

		for (int end = 0; end < 2 && isUsed(gate); ++end) {
			QPointF screen;

			if (toScreen(mvp, IdFile::fromVertex_s(gate.exitLine[end]), screen)
			        && QLineF(screen, QPointF(pos)).length() <= nearestDistance) {
				nearestDistance = QLineF(screen, QPointF(pos)).length();
				nearest = ExitEnd{gateID, end};
			}
		}
	}

	return nearest;
}

std::optional<int> WalkmeshGLWidget::exitAt(const QPoint &pos) const
{
	if (!data || !data->hasInfFile()) {
		return std::nullopt;
	}

	const QMatrix4x4 mvp = sceneToClip();
	std::optional<int> nearest;
	qreal nearestDistance = PICK_RADIUS;

	for (int gateID = 0; gateID < 12; ++gateID) {
		const Gateway &gate = data->getInfFile()->getGateway(gateID);
		QPointF a, b;

		if (isUsed(gate)
		        && toScreen(mvp, IdFile::fromVertex_s(gate.exitLine[0]), a)
		        && toScreen(mvp, IdFile::fromVertex_s(gate.exitLine[1]), b)
		        && distanceToSegment(QPointF(pos), a, b) <= nearestDistance) {
			nearestDistance = distanceToSegment(QPointF(pos), a, b);
			nearest = gateID;
		}
	}

	return nearest;
}

/**
 * The exit a click would add: along the wall side nearest to the mouse. Only offered while one
 * of the 12 exits of the file is unused, since the file cannot hold more.
 */
std::optional<WalkmeshGLWidget::ExitPreview> WalkmeshGLWidget::exitPreviewAt(const QPoint &pos) const
{
	if (!data || !data->hasInfFile() || !data->hasIdFile()) {
		return std::nullopt;
	}

	bool hasUnusedExit = false;
	for (const Gateway &gate: data->getInfFile()->getGateways()) {
		hasUnusedExit = hasUnusedExit || !isUsed(gate);
	}
	if (!hasUnusedExit) {
		return std::nullopt;
	}

	const QMatrix4x4 mvp = sceneToClip();
	IdFile *idFile = data->getIdFile();
	std::optional<ExitPreview> nearest;
	qreal nearestDistance = PICK_RADIUS;

	for (int triangleID = 0; triangleID < idFile->triangleCount(); ++triangleID) {
		const Triangle &triangle = idFile->triangle(triangleID);

		for (int side = 0; side < 3; ++side) {
			const Vertex_sr &from = triangle.vertices[side], &to = triangle.vertices[(side + 1) % 3];
			QPointF a, b;

			if (idFile->access(triangleID).a[side] == -1
			        && toScreen(mvp, from, a) && toScreen(mvp, to, b)
			        && distanceToSegment(QPointF(pos), a, b) <= nearestDistance) {
				nearestDistance = distanceToSegment(QPointF(pos), a, b);
				nearest = ExitPreview{from, to};
			}
		}
	}

	return nearest;
}

void WalkmeshGLWidget::updateHover(const QPoint &pos)
{
	_hoveredPoint.reset();
	_sidePreview.reset();
	_hoveredExitEnd.reset();
	_hoveredExit.reset();
	_exitPreview.reset();
	_hoveredFloor.reset();

	if (!data) {
		return;
	}

	switch (_editMode) {
	case EditWalkmesh:
		if (data->hasIdFile()) {
			_hoveredPoint = pointAt(pos);

			// Grabbing a point wins, and adding is only offered off the floor, so clicking on the
			// floor never creates a triangle by accident
			if (!_hoveredPoint && !isOnFloor(pos)) {
				_sidePreview = sidePreviewAt(pos);
			}
		}
		break;
	case EditExits:
		// Grabbing an end wins over selecting a line, which wins over adding an exit
		_hoveredExitEnd = exitEndAt(pos);
		if (!_hoveredExitEnd) {
			_hoveredExit = exitAt(pos);
		}
		if (!_hoveredExitEnd && !_hoveredExit) {
			_exitPreview = exitPreviewAt(pos);
		}
		break;
	case PickArrival:
		_hoveredFloor = floorAt(pos);
		break;
	case NoEdit:
		break;
	}

	update();
}

void WalkmeshGLWidget::drawBackground()
{
	if (data->getBackgroundFile())
	{
		// Map the image so its central SCREEN_WIDTH x SCREEN_HEIGHT region covers exactly the
		// same rectangle the walkmesh is projected into; a bigger (scrolling) background then
		// simply extends past it instead of being squeezed to fit.
		float sx = 1.0f, sy = 1.0f;
		screenLetterbox(sx, sy);
		const float bx = tex.isNull() ? sx : sx * float(tex.width()) / float(SCREEN_WIDTH),
		            by = tex.isNull() ? sy : sy * float(tex.height()) / float(SCREEN_HEIGHT);

		RendererVertex vertices[] = {
		    {
		        {-bx, -by, 1.0f, 1.0f},
		        {1.0f, 1.0f, 1.0f, 1.0f},
		        {0.0f, 1.0f},
		    },
		    {
		        {-bx, by, 1.0f, 1.0f},
		        {1.0f, 1.0f, 1.0f, 1.0f},
		        {0.0f, 0.0f},
		    },
		    {
		        {bx, -by, 1.0f, 1.0f},
		        {1.0f, 1.0f, 1.0f, 1.0f},
		        {1.0f, 1.0f},
		    },
		    {
		        {bx, by, 1.0f, 1.0f},
		        {1.0f, 1.0f, 1.0f, 1.0f},
		        {1.0f, 0.0f},
		    }
		};

		uint32_t indices[] = {
		    0, 1, 2,
		    1, 3, 2
		};

		QMatrix4x4 mBG;

		gpuRenderer->bindProjectionMatrix(screenMatrix());
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
	// angleDelta() is what a regular mouse wheel reports: pixelDelta() stays null for one on
	// Windows, and its horizontal component was read, so the wheel did nothing there
	zoomView(event->position(), event->angleDelta().y() > 0 ? 1.25f : 0.8f);
}

// Zoom by factor, keeping what is under the mouse where it is
void WalkmeshGLWidget::zoomView(const QPointF &pos, float factor)
{
	const float newZoom = qBound(0.25f, viewZoom * factor, 32.0f);
	const float ndcX = 2.0f * float(pos.x()) / width() - 1.0f, ndcY = 1.0f - 2.0f * float(pos.y()) / height();

	// The point under the mouse is at ndc = zoom * p + pan before and after
	viewPanX = ndcX - (ndcX - viewPanX) * newZoom / viewZoom;
	viewPanY = ndcY - (ndcY - viewPanY) * newZoom / viewZoom;
	viewZoom = newZoom;
	updateHover(pos.toPoint());
}

void WalkmeshGLWidget::panView(const QPointF &pixels)
{
	viewPanX += 2.0f * float(pixels.x()) / width();
	viewPanY -= 2.0f * float(pixels.y()) / height();
	update();
}

void WalkmeshGLWidget::mousePressEvent(QMouseEvent *event)
{
	setFocus();
	if (event->button() == Qt::MiddleButton)
	{
		resetCamera();
	}
	else if (event->button() == Qt::RightButton)
	{
		// The view moves with the right button, leaving the left one to edit the walkmesh; in
		// the free view it turns around the walkmesh, and moves with Shift
		moveStart = event->pos();
		if (_viewMode == FreeView && !(event->modifiers() & Qt::ShiftModifier)) {
			_orbiting = true;
		} else {
			_panning = true;
		}
	}
	else if (event->button() == Qt::LeftButton)
	{
		updateHover(event->pos());

		switch (_editMode) {
		case EditWalkmesh:
			pressOnWalkmesh();
			break;
		case EditExits:
			pressOnExits();
			break;
		case PickArrival:
			if (_hoveredFloor) {
				_pickingArrival = true;
				emit arrivalPickStarted();
				emit arrivalPicked(_hoveredFloor->point.x, _hoveredFloor->point.y, _hoveredFloor->triangle);
			}
			break;
		case NoEdit:
			break;
		}

		update();
	}
}

void WalkmeshGLWidget::pressOnWalkmesh()
{
	if (_hoveredPoint) {
		_selectedPoint = _draggedPoint = _hoveredPoint;
		_dragging = true;
		emit pointSelected(*_selectedPoint);
		emit pointDragStarted();
	} else if (_sidePreview) {
		const SidePreview preview = *_sidePreview;
		_sidePreview.reset();
		_selectedPoint = preview.point;
		emit pointAddRequested(preview.triangleID, preview.side, preview.point);
		emit pointSelected(preview.point);
	} else {
		_selectedPoint.reset();
	}
}

void WalkmeshGLWidget::pressOnExits()
{
	if (_hoveredExitEnd) {
		_draggedExitEnd = _hoveredExitEnd;
		emit exitSelected(_draggedExitEnd->gate);
		emit exitDragStarted();
	} else if (_hoveredExit) {
		emit exitSelected(*_hoveredExit);
	} else if (_exitPreview) {
		const ExitPreview preview = *_exitPreview;
		_exitPreview.reset();
		emit exitAddRequested(IdFile::toVertex_s(preview.a), IdFile::toVertex_s(preview.b));
	}
}

void WalkmeshGLWidget::mouseMoveEvent(QMouseEvent *event)
{
	// buttons(), not button(): for a move event button() is always Qt::NoButton, which is why
	// dragging never moved the view before
	if (_panning && (event->buttons() & Qt::RightButton)) {
		panView(event->pos() - moveStart);
		moveStart = event->pos();
	} else if (_orbiting && (event->buttons() & Qt::RightButton)) {
		const QPoint moved = event->pos() - moveStart;
		orbitYaw -= moved.x() * 0.4f;
		orbitPitch = qBound(1.0f, orbitPitch - moved.y() * 0.4f, 179.0f);
		moveStart = event->pos();
		update();
	} else if (_dragging && (event->buttons() & Qt::LeftButton)) {
		Vertex_sr target;

		if (mouseOnHeight(event->pos(), _draggedPoint->z, target) && !IdFile::samePoint(target, *_draggedPoint)) {
			emit pointDragged(*_draggedPoint, target);
			_selectedPoint = _draggedPoint = target;
		}

		// Releasing near another point merges the two: show which one, so it never happens
		// by surprise in a dense part of the walkmesh
		_hoveredPoint = pointAt(event->pos(), _draggedPoint);
		update();
	} else if (_draggedExitEnd && (event->buttons() & Qt::LeftButton)) {
		const Vertex &end = data->getInfFile()->getGateway(_draggedExitEnd->gate).exitLine[_draggedExitEnd->end];
		Vertex_sr target;

		// An exit end moves at its own height, like a walkmesh point
		if (mouseOnHeight(event->pos(), end.z, target)) {
			emit exitEndDragged(_draggedExitEnd->gate, _draggedExitEnd->end, IdFile::toVertex_s(target));
		}

		// The walkmesh point it would snap to on release
		_hoveredPoint = pointAt(event->pos());
		update();
	} else if (_pickingArrival && (event->buttons() & Qt::LeftButton)) {
		_hoveredFloor = floorAt(event->pos());
		if (_hoveredFloor) {
			emit arrivalPicked(_hoveredFloor->point.x, _hoveredFloor->point.y, _hoveredFloor->triangle);
		}
		update();
	} else {
		updateHover(event->pos());
	}
}

void WalkmeshGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::RightButton) {
		_panning = false;
		_orbiting = false;
	} else if (event->button() == Qt::LeftButton && _dragging) {
		// Dropped onto another point: snap to it, which merges the two
		const std::optional<Vertex_sr> target = pointAt(event->pos(), _draggedPoint);
		if (target) {
			emit pointDragged(*_draggedPoint, *target);
			_selectedPoint = target;
		}

		_dragging = false;
		_draggedPoint.reset();
		emit pointDragFinished();
		updateHover(event->pos());
	} else if (event->button() == Qt::LeftButton && _draggedExitEnd) {
		// Exits usually run along walls: dropped near a walkmesh point, the end goes exactly on it
		const std::optional<Vertex_sr> target = pointAt(event->pos());
		if (target) {
			emit exitEndDragged(_draggedExitEnd->gate, _draggedExitEnd->end, IdFile::toVertex_s(*target));
		}

		_draggedExitEnd.reset();
		emit exitDragFinished();
		updateHover(event->pos());
	} else if (event->button() == Qt::LeftButton && _pickingArrival) {
		_pickingArrival = false;
		emit arrivalPickFinished();
		updateHover(event->pos());
	}
}

void WalkmeshGLWidget::leaveEvent(QEvent *event)
{
	_hoveredPoint.reset();
	_sidePreview.reset();
	_hoveredExitEnd.reset();
	_hoveredExit.reset();
	_exitPreview.reset();
	_hoveredFloor.reset();
	update();
	QOpenGLWidget::leaveEvent(event);
}

void WalkmeshGLWidget::keyPressEvent(QKeyEvent *event)
{
	// This widget grabs the keyboard while it has the focus, so shortcuts defined elsewhere would
	// never fire: the editing keys are handled here
	if (event->matches(QKeySequence::Undo)) {
		emit undoRequested();
		return;
	}
	if (event->matches(QKeySequence::Redo)) {
		emit redoRequested();
		return;
	}
	if (event->key() == Qt::Key_Delete) {
		if (_editMode == EditWalkmesh && _selectedPoint) {
			const Vertex_sr point = *_selectedPoint;
			clearPointSelection();
			emit pointDeleteRequested(point);
		} else if (_editMode == EditExits && _selectedGate >= 0 && _selectedGate < 12) {
			clearPointSelection();
			emit exitDeleteRequested(_selectedGate);
		}
		return;
	}
	if (event->key() == Qt::Key_Escape) {
		clearPointSelection();
		return;
	}

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
		panView(QPointF(width() * 0.5f / transStep, 0.0f));
		break;
	case Qt::Key_Right:
		panView(QPointF(-width() * 0.5f / transStep, 0.0f));
		break;
	case Qt::Key_Down:
		panView(QPointF(0.0f, -height() * 0.5f / transStep));
		break;
	case Qt::Key_Up:
		panView(QPointF(0.0f, height() * 0.5f / transStep));
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

void WalkmeshGLWidget::setZoom(int zoom)
{
	viewZoom = qBound(0.25f, zoom / 4096.0f, 32.0f);
	update();
}

void WalkmeshGLWidget::resetCamera()
{
	viewZoom = 1.0f;
	viewPanX = viewPanY = 0.0f;
	resetOrbit();
	update();
}

// The free view starts where the game camera is, so switching to it does not lose the field
void WalkmeshGLWidget::resetOrbit()
{
	QVector3D eye;
	float radius = 1.0f;
	const QVector3D centre = sceneCentre(radius);

	orbitYaw = -90.0f;
	orbitPitch = 45.0f;

	if (gameCameraEye(eye) && (eye - centre).lengthSquared() > 1e-8f) {
		const QVector3D direction = (eye - centre).normalized();
		orbitYaw = qRadiansToDegrees(std::atan2(direction.y(), direction.x()));
		orbitPitch = qBound(1.0f, qRadiansToDegrees(std::acos(qBound(-1.0f, direction.z() * upSide(), 1.0f))), 179.0f);
	}
}

void WalkmeshGLWidget::setViewMode(ViewMode mode)
{
	if (mode == _viewMode) {
		return;
	}

	_viewMode = mode;
	_panning = _orbiting = false;
	viewZoom = 1.0f;
	viewPanX = viewPanY = 0.0f;
	resetOrbit();
	clearPointSelection();
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
