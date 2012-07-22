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

	QTabWidget *tabWidget = new QTabWidget(this);
	tabWidget->addTab(buildCameraPage(), tr("Caméra"));
	tabWidget->addTab(buildGatewaysPage(), tr("Sorties"));
	tabWidget->addTab(buildMovieCameraPage(), tr("Caméra cinématique"));
	tabWidget->setFixedHeight(200);

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(walkmeshGL, 0, 0);
	layout->addWidget(tabWidget, 1, 0, 1, 4);
	layout->addWidget(slider1, 0, 1);
	layout->addWidget(slider2, 0, 2);
	layout->addWidget(slider3, 0, 3);
//	layout->addWidget(slider4, 0, 4);
	layout->setContentsMargins(QMargins());

	connect(slider1, SIGNAL(valueChanged(int)), walkmeshGL, SLOT(setXRotation(int)));
	connect(slider2, SIGNAL(valueChanged(int)), walkmeshGL, SLOT(setYRotation(int)));
	connect(slider3, SIGNAL(valueChanged(int)), walkmeshGL, SLOT(setZRotation(int)));
//	connect(slider4, SIGNAL(valueChanged(int)), walkmeshGL, SLOT(setZoom(int)));

	PageWidget::build();
}

QWidget *WalkmeshWidget::buildCameraPage()
{
	QWidget *ret = new QWidget(this);

	caVectorX1Edit = new QSpinBox(ret);
	caVectorX1Edit->setRange(-32768, 32767);
	caVectorX2Edit = new QSpinBox(ret);
	caVectorX2Edit->setRange(-32768, 32767);
	caVectorX3Edit = new QSpinBox(ret);
	caVectorX3Edit->setRange(-32768, 32767);

	caVectorY1Edit = new QSpinBox(ret);
	caVectorY1Edit->setRange(-32768, 32767);
	caVectorY2Edit = new QSpinBox(ret);
	caVectorY2Edit->setRange(-32768, 32767);
	caVectorY3Edit = new QSpinBox(ret);
	caVectorY3Edit->setRange(-32768, 32767);

	caVectorZ1Edit = new QSpinBox(ret);
	caVectorZ1Edit->setRange(-32768, 32767);
	caVectorZ2Edit = new QSpinBox(ret);
	caVectorZ2Edit->setRange(-32768, 32767);
	caVectorZ3Edit = new QSpinBox(ret);
	caVectorZ3Edit->setRange(-32768, 32767);

	caSpaceXEdit = new QDoubleSpinBox(ret);
	qreal maxInt = qPow(2,31);
	caSpaceXEdit->setRange(-maxInt, maxInt);
	caSpaceXEdit->setDecimals(0);
	caSpaceYEdit = new QDoubleSpinBox(ret);
	caSpaceYEdit->setRange(-maxInt, maxInt);
	caSpaceYEdit->setDecimals(0);
	caSpaceZEdit = new QDoubleSpinBox(ret);
	caSpaceZEdit->setRange(-maxInt, maxInt);
	caSpaceZEdit->setDecimals(0);

	connect(caVectorX1Edit, SIGNAL(valueChanged(int)), SLOT(editCaVector(int)));
	connect(caVectorX2Edit, SIGNAL(valueChanged(int)), SLOT(editCaVector(int)));
	connect(caVectorX3Edit, SIGNAL(valueChanged(int)), SLOT(editCaVector(int)));
	connect(caVectorY1Edit, SIGNAL(valueChanged(int)), SLOT(editCaVector(int)));
	connect(caVectorY2Edit, SIGNAL(valueChanged(int)), SLOT(editCaVector(int)));
	connect(caVectorY3Edit, SIGNAL(valueChanged(int)), SLOT(editCaVector(int)));
	connect(caVectorZ1Edit, SIGNAL(valueChanged(int)), SLOT(editCaVector(int)));
	connect(caVectorZ2Edit, SIGNAL(valueChanged(int)), SLOT(editCaVector(int)));
	connect(caVectorZ3Edit, SIGNAL(valueChanged(int)), SLOT(editCaVector(int)));

	connect(caSpaceXEdit, SIGNAL(valueChanged(double)), SLOT(editCaPos(double)));
	connect(caSpaceYEdit, SIGNAL(valueChanged(double)), SLOT(editCaPos(double)));
	connect(caSpaceZEdit, SIGNAL(valueChanged(double)), SLOT(editCaPos(double)));

	QGridLayout *caLayout = new QGridLayout(ret);
	caLayout->addWidget(caVectorX1Edit, 0, 0);
	caLayout->addWidget(caVectorX2Edit, 0, 1);
	caLayout->addWidget(caVectorX3Edit, 0, 2);
	caLayout->addWidget(caVectorY1Edit, 1, 0);
	caLayout->addWidget(caVectorY2Edit, 1, 1);
	caLayout->addWidget(caVectorY3Edit, 1, 2);
	caLayout->addWidget(caVectorZ1Edit, 2, 0);
	caLayout->addWidget(caVectorZ2Edit, 2, 1);
	caLayout->addWidget(caVectorZ3Edit, 2, 2);
	caLayout->addWidget(caSpaceXEdit, 3, 0);
	caLayout->addWidget(caSpaceYEdit, 3, 1);
	caLayout->addWidget(caSpaceZEdit, 3, 2);
    caLayout->setRowStretch(4, 1);

	return ret;
}

QWidget *WalkmeshWidget::buildGatewaysPage()
{
	QWidget *ret = new QWidget(this);

	gateList = new QListWidget(ret);
	gateList->setFixedWidth(150);

    exitPoints[0] = new VertexWidget(ret);
    exitPoints[1] = new VertexWidget(ret);
    entryPoint = new VertexWidget(ret);

	QGridLayout *layout = new QGridLayout(ret);
    layout->addWidget(gateList, 0, 0, 2, 1);
    layout->addWidget(new QLabel(tr("Ligne de sortie")), 0, 1, Qt::AlignTop);
    layout->addWidget(exitPoints[0], 0, 2, Qt::AlignTop);
    layout->addWidget(exitPoints[1], 0, 3, Qt::AlignTop);
    layout->addWidget(new QLabel(tr("Point de destination")), 1, 1, 1, 2, Qt::AlignTop);
    layout->addWidget(entryPoint, 1, 3, Qt::AlignTop);
    layout->setRowStretch(1, 1);

    connect(gateList, SIGNAL(currentRowChanged(int)), SLOT(setCurrentGateway(int)));

	return ret;
}

QWidget *WalkmeshWidget::buildMovieCameraPage()
{
	QWidget *ret = new QWidget(this);

	frameList = new QListWidget(ret);
	frameList->setFixedWidth(150);

    camPoints[0] = new VertexWidget(ret);
    camPoints[1] = new VertexWidget(ret);
    camPoints[2] = new VertexWidget(ret);
    camPoints[3] = new VertexWidget(ret);

	QGridLayout *layout = new QGridLayout(ret);
    layout->addWidget(frameList, 0, 0, 4, 1);
    layout->addWidget(camPoints[0], 0, 1);
    layout->addWidget(camPoints[1], 1, 1);
    layout->addWidget(camPoints[2], 2, 1);
    layout->addWidget(camPoints[3], 3, 1);

    connect(frameList, SIGNAL(currentRowChanged(int)), SLOT(setCurrentMoviePosition(int)));

	return ret;
}

void WalkmeshWidget::clear()
{
	if(!isFilled())		return;

	if(walkmeshGL != NULL)
		walkmeshGL->clear();

	gateList->clear();
	frameList->clear();

	PageWidget::clear();
}

void WalkmeshWidget::fill()
{
	if(!isBuilded())	build();
	if(isFilled())		clear();

	if(!hasData() || !data()->hasWalkmeshFile()) return;

	if(data()->hasWalkmeshFile()) {
//		qDebug() << walkmeshFile->camAxis(0).x << walkmeshFile->camAxis(0).y << walkmeshFile->camAxis(0).z;
//		qDebug() << walkmeshFile->camAxis(1).x << walkmeshFile->camAxis(1).y << walkmeshFile->camAxis(1).z;
//		qDebug() << walkmeshFile->camAxis(2).x << walkmeshFile->camAxis(2).y << walkmeshFile->camAxis(2).z;
//		qDebug() << walkmeshFile->camPos(0) << walkmeshFile->camPos(1) << walkmeshFile->camPos(2);

		walkmeshGL->fill(data()->getWalkmeshFile());

		caVectorX1Edit->setValue(data()->getWalkmeshFile()->camAxis(0).x);
		caVectorX2Edit->setValue(data()->getWalkmeshFile()->camAxis(0).y);
		caVectorX3Edit->setValue(data()->getWalkmeshFile()->camAxis(0).z);
		caVectorY1Edit->setValue(data()->getWalkmeshFile()->camAxis(1).x);
		caVectorY2Edit->setValue(data()->getWalkmeshFile()->camAxis(1).y);
		caVectorY3Edit->setValue(data()->getWalkmeshFile()->camAxis(1).z);
		caVectorZ1Edit->setValue(data()->getWalkmeshFile()->camAxis(2).x);
		caVectorZ2Edit->setValue(data()->getWalkmeshFile()->camAxis(2).y);
		caVectorZ3Edit->setValue(data()->getWalkmeshFile()->camAxis(2).z);

		caSpaceXEdit->setValue(data()->getWalkmeshFile()->camPos(0));
		caSpaceYEdit->setValue(data()->getWalkmeshFile()->camPos(1));
		caSpaceZEdit->setValue(data()->getWalkmeshFile()->camPos(2));
	}

    if(data()->hasInfFile()) {
		gateList->clear();
        foreach(const Gateway &gateway, data()->getInfFile()->getGateways()) {
            gateList->addItem(QString("%1 (%2)").arg(Data::maplist().value(gateway.fieldId)).arg(gateway.fieldId));
		}
        gateList->setCurrentRow(0);
	}

	if(data()->hasMskFile()) {
		frameList->clear();
		int cameraPositionCount = data()->getMskFile()->cameraPositionCount();
		for(int i=0 ; i<cameraPositionCount ; ++i) {
			frameList->addItem(QString("Position %1").arg(i+1));
		}
        frameList->setCurrentRow(0);
	}

	PageWidget::fill();
}

void WalkmeshWidget::editCaVector(int value)
{
	QObject *s = sender();

	if(s == caVectorX1Edit)			editCaVector(0, 0, value);
	else if(s == caVectorX2Edit)	editCaVector(0, 1, value);
	else if(s == caVectorX3Edit)	editCaVector(0, 2, value);
	else if(s == caVectorY1Edit)	editCaVector(1, 0, value);
	else if(s == caVectorY2Edit)	editCaVector(1, 1, value);
	else if(s == caVectorY3Edit)	editCaVector(1, 2, value);
	else if(s == caVectorZ1Edit)	editCaVector(2, 0, value);
	else if(s == caVectorZ2Edit)	editCaVector(2, 1, value);
	else if(s == caVectorZ3Edit)	editCaVector(2, 2, value);
}

void WalkmeshWidget::editCaVector(int id, int id2, int value)
{
	if(data()->hasWalkmeshFile()) {
		Vertex_s v = data()->getWalkmeshFile()->camAxis(id), oldV;
		oldV = v;
		switch(id2) {
		case 0:
			v.x = value;
			break;
		case 1:
			v.y = value;
			break;
		case 2:
			v.z = value;
			break;
		}
		if(oldV.x != v.x || oldV.y != v.y || oldV.z != v.z) {
			data()->getWalkmeshFile()->setCamAxis(id, v);
			emit modified();
		}
	}
}

void WalkmeshWidget::editCaPos(double value)
{
	QObject *s = sender();

	if(s == caSpaceXEdit)			editCaPos(0, value);
	else if(s == caSpaceYEdit)		editCaPos(1, value);
	else if(s == caSpaceZEdit)		editCaPos(2, value);
}

void WalkmeshWidget::editCaPos(int id, int value)
{
	if(data()->hasWalkmeshFile()) {
		if(data()->getWalkmeshFile()->camPos(id) != value) {
			data()->getWalkmeshFile()->setCamPos(id, value);
			emit modified();
		}
	}
}

void WalkmeshWidget::setCurrentGateway(int id)
{
    if(!data()->hasInfFile() || id < 0)    return;

    InfFile *inf = data()->getInfFile();
    QList<Gateway> gateways = inf->getGateways();
    if(gateways.size() <= id)    return;

    Gateway gateway = gateways.at(id);

    exitPoints[0]->setValues(gateway.exitLine[0]);
    exitPoints[1]->setValues(gateway.exitLine[1]);
    entryPoint->setValues(gateway.destinationPoint);
}

void WalkmeshWidget::setCurrentMoviePosition(int id)
{
    if(!data()->hasMskFile() || id < 0)    return;

    MskFile *msk = data()->getMskFile();
    if(msk->cameraPositionCount() <= id)    return;

    camPoints[0]->setValues(msk->cameraPosition(id)[0]);
    camPoints[1]->setValues(msk->cameraPosition(id)[1]);
    camPoints[2]->setValues(msk->cameraPosition(id)[2]);
    camPoints[3]->setValues(msk->cameraPosition(id)[3]);
}
