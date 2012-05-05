/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2012 Arzel Jérôme <myst6re@gmail.com>
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
#include "widgets/WalkmeshWidget.h"

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
	if(!isFilled())		return;

	if(walkmeshGL != NULL)
		walkmeshGL->clear();

	PageWidget::clear();
}

void WalkmeshWidget::fill()
{
	if(!isBuilded())	build();
	if(isFilled())		clear();

	if(!hasData() || !data()->hasWalkmeshFile()) return;

//	qDebug() << walkmeshFile->camAxis(0).x << walkmeshFile->camAxis(0).y << walkmeshFile->camAxis(0).z;
//	qDebug() << walkmeshFile->camAxis(1).x << walkmeshFile->camAxis(1).y << walkmeshFile->camAxis(1).z;
//	qDebug() << walkmeshFile->camAxis(2).x << walkmeshFile->camAxis(2).y << walkmeshFile->camAxis(2).z;
//	qDebug() << walkmeshFile->camPos(0) << walkmeshFile->camPos(1) << walkmeshFile->camPos(2);

	walkmeshGL->fill(data()->getWalkmeshFile());

	PageWidget::fill();
}
