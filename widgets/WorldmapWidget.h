#ifndef WORLDMAPWIDGET_H
#define WORLDMAPWIDGET_H

#include <QtWidgets>
#include "widgets/WorldmapGLWidget.h"

class WorldmapWidget : public QWidget
{
	Q_OBJECT
public:
	explicit WorldmapWidget(QWidget *parent = Q_NULLPTR,
	                        Qt::WindowFlags f = Qt::WindowFlags());
	inline void setMap(const Map *map) {
		_scene->setMap(map);
	}
	inline const Map *map() const {
		return _scene->map();
	}
	inline WorldmapGLWidget *scene() const {
		return _scene;
	}
private slots:
	void setXTrans(int value);
	void setYTrans(int value);
	void setZTrans(int value);
	void setXRot(int value);
	void setYRot(int value);
	void setZRot(int value);
private:
	WorldmapGLWidget *_scene;
	QSlider *_xTransSlider, *_yTransSlider, *_zTransSlider;
	QSlider *_xRotSlider, *_yRotSlider, *_zRotSlider;
};

#endif // WORLDMAPWIDGET_H
