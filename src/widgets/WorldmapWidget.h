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
#include "widgets/WorldmapGLWidget.h"

class WorldmapWidget : public QWidget
{
	Q_OBJECT
public:
	explicit WorldmapWidget(QWidget *parent = Q_NULLPTR,
	                        Qt::WindowFlags f = Qt::WindowFlags());
	inline void setMap(Map *map) {
		if (map) {
			_segmentSpinBox->setMaximum(map->segments().size());
		}
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
	QSpinBox *_textureSpinBox, *_segmentGroupSpinBox, *_segmentSpinBox;
	QSpinBox *_blockSpinBox, *_groundTypeSpinBox, *_polyIdSpinBox, *_clutIdSpinBox;
};
