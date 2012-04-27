#include "WalkmeshWidget.h"

WalkmeshWidget::WalkmeshWidget(QWidget *parent) :
	PageWidget(parent), walkmeshGL(NULL)
{
}

void WalkmeshWidget::build()
{
	if(isBuilded())		return;

	walkmeshGL = new WalkmeshGLWidget;

	QSlider *slider1 = new QSlider(this);
	QSlider *slider2 = new QSlider(this);
	QSlider *slider3 = new QSlider(this);
//	QSlider *slider4 = new QSlider(this);

	slider1->setRange(0, 360 * 16);
	slider2->setRange(0, 360 * 16);
	slider3->setRange(0, 360 * 16);
//	slider4->setRange(-4096, 4096);

	slider1->setValue(0);
	slider2->setValue(0);
	slider3->setValue(0);
//	slider4->setValue(0);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(walkmeshGL);
	layout->addWidget(slider1);
	layout->addWidget(slider2);
	layout->addWidget(slider3);
//	layout->addWidget(slider4);
	layout->setContentsMargins(QMargins());

	connect(slider1, SIGNAL(valueChanged(int)), walkmeshGL, SLOT(setXRotation(int)));
	connect(slider2, SIGNAL(valueChanged(int)), walkmeshGL, SLOT(setYRotation(int)));
	connect(slider3, SIGNAL(valueChanged(int)), walkmeshGL, SLOT(setZRotation(int)));
//	connect(slider4, SIGNAL(valueChanged(int)), walkmeshGL, SLOT(setZoom(int)));

	PageWidget::build();
}

void WalkmeshWidget::clear()
{
	if(!isBuilded())	return;

	walkmeshFile = NULL;
	if(walkmeshGL != NULL)
		walkmeshGL->clear();

	PageWidget::clear();
}

void WalkmeshWidget::fill(WalkmeshFile *walkmeshFile)
{
	PageWidget::fill();

	this->walkmeshFile = walkmeshFile;
//	qDebug() << walkmeshFile->camAxis(0).x << walkmeshFile->camAxis(0).y << walkmeshFile->camAxis(0).z;
//	qDebug() << walkmeshFile->camAxis(1).x << walkmeshFile->camAxis(1).y << walkmeshFile->camAxis(1).z;
//	qDebug() << walkmeshFile->camAxis(2).x << walkmeshFile->camAxis(2).y << walkmeshFile->camAxis(2).z;
//	qDebug() << walkmeshFile->camPos(0) << walkmeshFile->camPos(1) << walkmeshFile->camPos(2);

	walkmeshGL->fill(walkmeshFile);
}
