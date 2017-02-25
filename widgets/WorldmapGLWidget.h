#ifndef WORLDMAPGLWIDGET_H
#define WORLDMAPGLWIDGET_H

#include <QtWidgets>
#include <QtOpenGL>
#include "game/worldmap/Map.h"

class WorldmapGLWidget : public QGLWidget
{
public:
	explicit WorldmapGLWidget(QWidget *parent = Q_NULLPTR,
	                          const QGLWidget *shareWidget = Q_NULLPTR,
	                          Qt::WindowFlags f = Qt::WindowFlags());
	explicit WorldmapGLWidget(QGLContext *context,
	                          QWidget *parent = Q_NULLPTR,
	                          const QGLWidget *shareWidget = Q_NULLPTR,
	                          Qt::WindowFlags f = Qt::WindowFlags());
	explicit WorldmapGLWidget(const QGLFormat &format,
	                          QWidget *parent = Q_NULLPTR,
	                          const QGLWidget *shareWidget = Q_NULLPTR,
	                          Qt::WindowFlags f = Qt::WindowFlags());

	void setMap(const Map *map);
	inline const Map *map() const {
		return _map;
	}
	void setLimits(const QRect &rect);
	// QImage toImage(int w, int h);
	void setXTrans(float trans);
	inline float xTrans() const {
		return _xTrans;
	}
	void setYTrans(float trans);
	inline float yTrans() const {
		return _yTrans;
	}
	void setZTrans(float trans);
	inline float zTrans() const {
		return _distance;
	}
	void setXRot(float rot);
	inline float xRot() const {
		return _xRot;
	}
	void setYRot(float rot);
	inline float yRot() const {
		return _yRot;
	}
	void setZRot(float rot);
	inline float zRot() const {
		return _zRot;
	}
protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();
	virtual void wheelEvent(QWheelEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void focusInEvent(QFocusEvent *event);
	virtual void focusOutEvent(QFocusEvent *event);
private:
	const Map *_map;
	QRect _limits;
	double _distance;
	float _xRot, _yRot, _zRot;
	float _xTrans, _yTrans, _transStep;
	int _lastKeyPressed;
	QPoint _moveStart;
};

#endif // WORLDMAPGLWIDGET_H
