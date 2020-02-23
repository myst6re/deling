#include "WorldmapWidget.h"

WorldmapWidget::WorldmapWidget(QWidget *parent, Qt::WindowFlags f) :
    QWidget(parent, f)
{
	const int min = 0, max = 100, minRot = -360, maxRot = 360;

	_scene = new WorldmapGLWidget(this);

	_xTransSlider = new QSlider(Qt::Vertical, this);
	_xTransSlider->setRange(min, max);
	_yTransSlider = new QSlider(Qt::Vertical, this);
	_yTransSlider->setRange(min, max);
	_zTransSlider = new QSlider(Qt::Vertical, this);
	_zTransSlider->setRange(min, max);

	_xRotSlider = new QSlider(Qt::Vertical, this);
	_xRotSlider->setRange(minRot, maxRot);
	_yRotSlider = new QSlider(Qt::Vertical, this);
	_yRotSlider->setRange(minRot, maxRot);
	_zRotSlider = new QSlider(Qt::Vertical, this);
	_zRotSlider->setRange(minRot, maxRot);

	_textureSpinBox = new QSpinBox(this);
	_textureSpinBox->setRange(0, 8);

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(_scene, 0, 0, 2, 1);
	layout->addWidget(_xTransSlider, 0, 1);
	layout->addWidget(_yTransSlider, 0, 2);
	layout->addWidget(_zTransSlider, 0, 3);
	layout->addWidget(_xRotSlider, 0, 4);
	layout->addWidget(_yRotSlider, 0, 5);
	layout->addWidget(_zRotSlider, 0, 6);
	layout->addWidget(_textureSpinBox, 1, 1);
	layout->setColumnStretch(0, 1);

	_xTransSlider->setValue((_scene->xTrans() + 1.0) * _xTransSlider->maximum() / 2.0);
	_yTransSlider->setValue((_scene->yTrans() + 1.0) * _yTransSlider->maximum() / 2.0);
	_zTransSlider->setValue((_scene->zTrans() + 1.0) * _zTransSlider->maximum() / 2.0);

	_xRotSlider->setValue(_scene->xRot());
	_yRotSlider->setValue(_scene->yRot());
	_zRotSlider->setValue(_scene->zRot());

	_textureSpinBox->setValue(_scene->texture());

	connect(_xTransSlider, SIGNAL(sliderMoved(int)), SLOT(setXTrans(int)));
	connect(_yTransSlider, SIGNAL(sliderMoved(int)), SLOT(setYTrans(int)));
	connect(_zTransSlider, SIGNAL(sliderMoved(int)), SLOT(setZTrans(int)));

	connect(_xRotSlider, SIGNAL(sliderMoved(int)), SLOT(setXRot(int)));
	connect(_yRotSlider, SIGNAL(sliderMoved(int)), SLOT(setYRot(int)));
	connect(_zRotSlider, SIGNAL(sliderMoved(int)), SLOT(setZRot(int)));

	connect(_textureSpinBox, SIGNAL(valueChanged(int)), SLOT(setTexture(int)));
}

void WorldmapWidget::setXTrans(int value)
{
	_scene->setXTrans((value * 2.0 / _xTransSlider->maximum()) - 1.0);
}

void WorldmapWidget::setYTrans(int value)
{
	_scene->setYTrans((value * 2.0 / _yTransSlider->maximum()) - 1.0);
}

void WorldmapWidget::setZTrans(int value)
{
	_scene->setZTrans((value * 2.0 / _zTransSlider->maximum()) - 1.0);
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

void WorldmapWidget::setTexture(int value)
{
	_scene->setTexture(value);
}
