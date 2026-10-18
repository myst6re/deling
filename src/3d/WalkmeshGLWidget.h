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
#pragma once

#include <QtWidgets>
#include <optional>
#include "Field.h"
#include "Renderer.h"

class WalkmeshGLWidget : public QOpenGLWidget
{
	Q_OBJECT
public:
	// What a left click does
	enum EditMode {
		NoEdit,       // nothing, the view only
		EditWalkmesh, // move, add and delete walkmesh points
		EditExits,    // move exit ends, add and disable exits
		PickArrival   // the view shows an exit's destination field: click its floor to place the arrival
	};

	// Where the field is looked at from
	enum ViewMode {
		GameView, // through the game camera, over the background
		TopView,  // from straight above, without perspective
		FreeView  // turning around the walkmesh with the right button
	};

	// Each colour always means the same thing, whatever the tab; WalkmeshWidget shows the legend
	static constexpr QRgb COLOR_SIDE = 0xFFFFFFFF;     // side between two triangles
	static constexpr QRgb COLOR_WALL = 0xFF6699CC;     // side with nothing across it
	static constexpr QRgb COLOR_BROKEN = 0xFFFF2020;   // triangle the game cannot use
	static constexpr QRgb COLOR_EXIT = 0xFFFF40FF;     // exit line
	static constexpr QRgb COLOR_DOOR = 0xFF00FF00;     // door trigger line
	static constexpr QRgb COLOR_SCRIPT = 0xFFFF00FF;   // line of a script (drawn instead of exits and doors)
	static constexpr QRgb COLOR_SELECTED = 0xFFFF9000; // what the form shows
	static constexpr QRgb COLOR_HOVER = 0xFFFFE040;    // what a click would grab or add

	explicit WalkmeshGLWidget(QWidget *parent = nullptr);
	virtual ~WalkmeshGLWidget() override;
	void clear();
	void fill(Field *data);
	void updatePerspective();
	void setEditMode(EditMode mode);
	void setViewMode(ViewMode mode);
	inline ViewMode viewMode() const {
		return _viewMode;
	}
	void setArrival(qint16 x, qint16 y, int triangle);
	void clearPointSelection();
signals:
	// Edition with the mouse. This widget only works out what the user points at; changing the
	// files is left to the page, so every change goes through its undo stack.
	void pointSelected(const Vertex_sr &point);
	void pointDragStarted();
	void pointDragged(const Vertex_sr &from, const Vertex_sr &to);
	void pointDragFinished();
	void pointAddRequested(int triangleID, int side, const Vertex_sr &point);
	void pointDeleteRequested(const Vertex_sr &point);
	void exitSelected(int gate);
	void exitDragStarted();
	void exitEndDragged(int gate, int end, const Vertex &to);
	void exitDragFinished();
	void exitAddRequested(const Vertex &a, const Vertex &b);
	void exitDeleteRequested(int gate);
	void arrivalPickStarted();
	void arrivalPicked(qint16 x, qint16 y, int triangle);
	void arrivalPickFinished();
	void undoRequested();
	void redoRequested();
public slots:
	void setZoom(int);
	void resetCamera();
	void setCurrentFieldCamera(int camID);
	void setBackgroundVisible(bool show);
	void setSelectedTriangle(int triangle);
	void setSelectedDoor(int door);
	void setSelectedGate(int gate);
	void setLineToDraw(const Vertex vertex[2]);
	void clearLineToDraw();
private:
	// The screen the game projects a field into; its centre (160, 112) is the projection centre
	static const int SCREEN_WIDTH = 320, SCREEN_HEIGHT = 224;
	// How close to a point, in pixels, the mouse has to be to grab it
	static const int PICK_RADIUS = 10;
	// How close to a wall, in pixels, the mouse has to be to add a triangle on it
	static const int ADD_RADIUS = 60;
	// Width in pixels of walls, broken and selected triangles and doors - and of exits
	static constexpr float WIDE_LINE_WIDTH = 3.0f, EXIT_LINE_WIDTH = 6.0f;
	// A corner of a wide line, already in screen coordinates
	struct ScreenVertex {
		QVector3D position;
		QRgba64 color;
	};
	// The triangle a click would add while the mouse is off the floor: a border side and a point
	struct SidePreview {
		int triangleID;
		int side;
		Vertex_sr point;
	};
	// One of the two ends of an exit line
	struct ExitEnd {
		int gate;
		int end;
	};
	// The exit a click would add: along a wall side
	struct ExitPreview {
		Vertex_sr a, b;
	};
	// Where the mouse points on the floor
	struct FloorPoint {
		Vertex_sr point;
		int triangle;
	};
	void screenLetterbox(float &sx, float &sy) const;
	void computeFov();
	void drawBackground();
	void drawBackgroundOnFloor();
	void bindSceneMatrices();
	void bufferWideLine(const QMatrix4x4 &sceneToClip, const QVector3D &from, const QVector3D &to, QRgb color, float pixels);
	void drawWideLines();
	void drawWalkmesh();
	void drawExitsAndDoors();
	void drawArrival();
	QMatrix4x4 projectionMatrix() const;
	QMatrix4x4 viewMatrix() const;
	QMatrix4x4 gameProjectionMatrix() const;
	QMatrix4x4 gameViewMatrix() const;
	QMatrix4x4 screenMatrix() const;
	QVector3D sceneCentre(float &radius) const;
	float viewDistance(float radius) const;
	bool gameCameraEye(QVector3D &eye) const;
	float upSide() const;
	void resetOrbit();
	QMatrix4x4 sceneToClip() const;
	bool toScreen(const QMatrix4x4 &sceneToClip, const Vertex_sr &point, QPointF &screen) const;
	bool mouseRay(const QPoint &pos, QVector3D &nearPoint, QVector3D &farPoint) const;
	bool mouseOnHeight(const QPoint &pos, qint16 planeHeight, Vertex_sr &point) const;
	std::optional<Vertex_sr> pointAt(const QPoint &pos, const std::optional<Vertex_sr> &ignored = std::nullopt) const;
	bool isOnFloor(const QPoint &pos) const;
	std::optional<SidePreview> sidePreviewAt(const QPoint &pos) const;
	std::optional<ExitEnd> exitEndAt(const QPoint &pos) const;
	std::optional<int> exitAt(const QPoint &pos) const;
	std::optional<ExitPreview> exitPreviewAt(const QPoint &pos) const;
	std::optional<FloorPoint> floorAt(const QPoint &pos) const;
	Vertex_sr arrivalPoint() const;
	void updateHover(const QPoint &pos);
	void zoomView(const QPointF &pos, float factor);
	void panView(const QPointF &pixels);
	void pressOnWalkmesh();
	void pressOnExits();
	void bufferLine(const Vertex_sr &from, const Vertex_sr &to, QRgba64 color, bool dashed = false);
	// Zoom and move of the view, in normalized device coordinates (the widget is -1..1)
	float viewZoom, viewPanX, viewPanY;
	ViewMode _viewMode;
	// The free view camera: turned by orbitYaw around the vertical, orbitPitch degrees from it
	float orbitYaw, orbitPitch;
	QList<ScreenVertex> _wideLineVertices;
	float transStep;
	int lastKeyPressed;
	int camID;
	int _selectedTriangle;
	int _selectedDoor;
	int _selectedGate;
	Vertex _lineToDrawPoint1, _lineToDrawPoint2;
	double fovy;
	Field *data;
	QPoint moveStart;
	int curFrame;
	Renderer *gpuRenderer;
	QImage tex;
	bool _drawLine;
	bool _backgroundVisible;
	EditMode _editMode;
	bool _dragging, _panning, _orbiting, _pickingArrival;
	std::optional<Vertex_sr> _hoveredPoint, _selectedPoint, _draggedPoint;
	std::optional<SidePreview> _sidePreview;
	std::optional<ExitEnd> _hoveredExitEnd, _draggedExitEnd;
	std::optional<int> _hoveredExit;
	std::optional<ExitPreview> _exitPreview;
	std::optional<FloorPoint> _hoveredFloor;
	qint16 _arrivalX, _arrivalY;
	int _arrivalTriangle;

protected:
	virtual void timerEvent(QTimerEvent *event) override;
	virtual void initializeGL() override;
	virtual void resizeGL(int w, int h) override;
	virtual void paintGL() override;
	virtual void wheelEvent(QWheelEvent *event) override;
	virtual void mousePressEvent(QMouseEvent *event) override;
	virtual void mouseMoveEvent(QMouseEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent *event) override;
	virtual void leaveEvent(QEvent *event) override;
	virtual void keyPressEvent(QKeyEvent *event) override;
	virtual void focusInEvent(QFocusEvent *event) override;
	virtual void focusOutEvent(QFocusEvent *event) override;
};
