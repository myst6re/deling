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
#include "WorldmapWidget.h"
#include "Field.h"

WorldmapWidget::WorldmapWidget(QWidget *parent) :
      PageWidget(parent)
{
}

void WorldmapWidget::build()
{
	if (isBuilded()) {
		return;
	}
	
	_scene = new WorldmapGLWidget(this);
	const int minRot = -360, maxRot = 360;
	
	_xRotSlider = new QSlider(Qt::Vertical, this);
	_xRotSlider->setRange(minRot, maxRot);
	_yRotSlider = new QSlider(Qt::Vertical, this);
	_yRotSlider->setRange(minRot, maxRot);
	_zRotSlider = new QSlider(Qt::Vertical, this);
	_zRotSlider->setRange(minRot, maxRot);
	
	QPushButton *resetButton = new QPushButton(tr("Reset"), this);
	
	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(_scene, 0, 0, 2, 1);
	layout->addWidget(_xRotSlider, 0, 1);
	layout->addWidget(_yRotSlider, 0, 2);
	layout->addWidget(_zRotSlider, 0, 3);
	layout->addWidget(resetButton, 1, 1, 1, 3);
	layout->setRowStretch(0, 1);
	layout->setColumnStretch(0, 1);
	layout->setContentsMargins(QMargins());
	
	_xRotSlider->setValue(_scene->xRot());
	_yRotSlider->setValue(_scene->yRot());
	_zRotSlider->setValue(_scene->zRot());
	
	connect(_xRotSlider, SIGNAL(sliderMoved(int)), SLOT(setXRot(int)));
	connect(_yRotSlider, SIGNAL(sliderMoved(int)), SLOT(setYRot(int)));
	connect(_zRotSlider, SIGNAL(sliderMoved(int)), SLOT(setZRot(int)));
	connect(resetButton, SIGNAL(clicked()), SLOT(resetCamera()));
	
	PageWidget::build();
}

void WorldmapWidget::clear()
{
	if (!isFilled()) {
		return;
	}
	
	_scene->setMap(nullptr);
	
	PageWidget::clear();
}

void WorldmapWidget::fill()
{
	if (!isBuilded())	build();
	if (isFilled())		clear();
	
	if (!data()->hasWorldmapFile()) {
		return;
	}

	_scene->setMap(data()->getWorldmapFile());
	_scene->setZTrans(-0.714249f);
	_scene->setFocus();
	
	PageWidget::fill();
}

void WorldmapWidget::setXRot(int value)
{
	_scene->setXRot(value);
}

void WorldmapWidget::setYRot(int value)
{
	_scene->setYRot(value);
}

void WorldmapWidget::setZRot(int value)
{
	_scene->setZRot(value);
}

void WorldmapWidget::resetCamera()
{
	_scene->resetCamera();	
	_xRotSlider->setValue(_scene->xRot());
	_yRotSlider->setValue(_scene->yRot());
	_zRotSlider->setValue(_scene->zRot());
}
