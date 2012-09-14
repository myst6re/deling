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
	PageWidget(parent)
{
}

void WalkmeshWidget::build()
{
	if(isBuilded())		return;

	walkmeshGL = new WalkmeshGLWidget;

	slider1 = new QSlider(this);
	slider2 = new QSlider(this);
	slider3 = new QSlider(this);

	slider1->setRange(-180, 180);
	slider2->setRange(-180, 180);
	slider3->setRange(-180, 180);

	slider1->setValue(0);
	slider2->setValue(0);
	slider3->setValue(0);

	QLabel *keyInfos = new QLabel(tr("Utilisez les touches directionnelles pour déplacer la caméra."));
	keyInfos->setTextFormat(Qt::PlainText);
	keyInfos->setWordWrap(true);

	QPushButton *resetCamera = new QPushButton(tr("Remettre à 0"));

	tabWidget = new QTabWidget(this);
	tabWidget->addTab(buildCameraPage(), tr("Caméra"));
	tabWidget->addTab(buildWalkmeshPage(), tr("Walkmesh"));
	tabWidget->addTab(buildGatewaysPage(), tr("Sorties"));
	tabWidget->addTab(buildDoorsPage(), tr("Portes"));
	tabWidget->addTab(buildCameraRangePage(), tr("Limites caméra"));
	tabWidget->addTab(buildMovieCameraPage(), tr("Caméra cinématique"));
	tabWidget->addTab(buildMiscPage(), tr("Divers"));
	tabWidget->setFixedHeight(250);

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(walkmeshGL, 0, 0, 3, 1);
	layout->addWidget(slider1, 0, 1);
	layout->addWidget(slider2, 0, 2);
	layout->addWidget(slider3, 0, 3);
	layout->addWidget(keyInfos, 1, 1, 1, 3);
	layout->addWidget(resetCamera, 2, 1, 1, 3);
	layout->addWidget(tabWidget, 3, 0, 1, 4);
	layout->setContentsMargins(QMargins());

	connect(slider1, SIGNAL(valueChanged(int)), walkmeshGL, SLOT(setXRotation(int)));
	connect(slider2, SIGNAL(valueChanged(int)), walkmeshGL, SLOT(setYRotation(int)));
	connect(slider3, SIGNAL(valueChanged(int)), walkmeshGL, SLOT(setZRotation(int)));
	connect(resetCamera, SIGNAL(clicked()), SLOT(resetCamera()));

	PageWidget::build();
}

void WalkmeshWidget::resetCamera()
{
	slider1->blockSignals(true);
	slider2->blockSignals(true);
	slider3->blockSignals(true);
	slider1->setValue(0);
	slider2->setValue(0);
	slider3->setValue(0);
	slider1->blockSignals(false);
	slider2->blockSignals(false);
	slider3->blockSignals(false);
	walkmeshGL->resetCamera();
}

QWidget *WalkmeshWidget::buildCameraPage()
{
	QWidget *ret = new QWidget(this);

	ListWidget *listWidget = new ListWidget(ret);
	listWidget->addAction(ListWidget::Add, tr("Ajouter caméra"), this, SLOT(addCamera()));
	listWidget->addAction(ListWidget::Rem, tr("Supprimer caméra"), this, SLOT(removeCamera()));

	caToolbar = listWidget->toolBar();
	camList = listWidget->listWidget();

	caVectorXEdit = new VertexWidget(ret);
	caVectorYEdit = new VertexWidget(ret);
	caVectorZEdit = new VertexWidget(ret);

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

	caZoomEdit = new QSpinBox(ret);
	caZoomEdit->setRange(-32768, 32767);

	QGridLayout *caLayout = new QGridLayout(ret);
	caLayout->addWidget(listWidget, 0, 0, 8, 1);
	caLayout->addWidget(new QLabel(tr("Distance (zoom) :")), 0, 1, 1, 3);
	caLayout->addWidget(caZoomEdit, 0, 4, 1, 2);
	caLayout->addWidget(new QLabel(tr("Axes de la caméra :")), 1, 1, 1, 6);
	caLayout->addWidget(caVectorXEdit, 2, 1, 1, 6);
	caLayout->addWidget(caVectorYEdit, 3, 1, 1, 6);
	caLayout->addWidget(caVectorZEdit, 4, 1, 1, 6);
	caLayout->addWidget(new QLabel(tr("Position de la caméra :")), 5, 1, 1, 6);
	caLayout->addWidget(new QLabel(tr("X")), 6, 1);
	caLayout->addWidget(caSpaceXEdit, 6, 2);
	caLayout->addWidget(new QLabel(tr("Y")), 6, 3);
	caLayout->addWidget(caSpaceYEdit, 6, 4);
	caLayout->addWidget(new QLabel(tr("Z")), 6, 5);
	caLayout->addWidget(caSpaceZEdit, 6, 6);
	caLayout->setRowStretch(7, 1);
	caLayout->setColumnStretch(2, 1);
	caLayout->setColumnStretch(4, 1);
	caLayout->setColumnStretch(6, 1);

	connect(camList, SIGNAL(currentRowChanged(int)), SLOT(setCurrentCamera(int)));

	connect(caVectorXEdit, SIGNAL(valuesChanged(Vertex_s)), SLOT(editCaVector(Vertex_s)));
	connect(caVectorYEdit, SIGNAL(valuesChanged(Vertex_s)), SLOT(editCaVector(Vertex_s)));
	connect(caVectorZEdit, SIGNAL(valuesChanged(Vertex_s)), SLOT(editCaVector(Vertex_s)));

	connect(caSpaceXEdit, SIGNAL(valueChanged(double)), SLOT(editCaPos(double)));
	connect(caSpaceYEdit, SIGNAL(valueChanged(double)), SLOT(editCaPos(double)));
	connect(caSpaceZEdit, SIGNAL(valueChanged(double)), SLOT(editCaPos(double)));

	connect(caZoomEdit, SIGNAL(valueChanged(int)), SLOT(editCaZoom(int)));

	return ret;
}

QWidget *WalkmeshWidget::buildWalkmeshPage()
{
	QWidget *ret = new QWidget(this);

	ListWidget *listWidget = new ListWidget(ret);
	listWidget->addAction(ListWidget::Add, tr("Ajouter triangle"), this, SLOT(addTriangle()));
	listWidget->addAction(ListWidget::Rem, tr("Supprimer triangle"), this, SLOT(removeTriangle()));

	idToolbar = listWidget->toolBar();
	idList = listWidget->listWidget();

	idVertices[0] = new VertexWidget(ret);
	idVertices[1] = new VertexWidget(ret);
	idVertices[2] = new VertexWidget(ret);

	idAccess[0] = new QSpinBox(ret);
	idAccess[1] = new QSpinBox(ret);
	idAccess[2] = new QSpinBox(ret);

	idAccess[0]->setRange(-32768, 32767);
	idAccess[1]->setRange(-32768, 32767);
	idAccess[2]->setRange(-32768, 32767);

	QHBoxLayout *accessLayout0 = new QHBoxLayout;
	accessLayout0->addWidget(new QLabel(tr("Triangle accessible via la ligne 1-2 :")));
	accessLayout0->addWidget(idAccess[0]);

	QHBoxLayout *accessLayout1 = new QHBoxLayout;
	accessLayout1->addWidget(new QLabel(tr("Triangle accessible via la ligne 2-3 :")));
	accessLayout1->addWidget(idAccess[1]);

	QHBoxLayout *accessLayout2 = new QHBoxLayout;
	accessLayout2->addWidget(new QLabel(tr("Triangle accessible via la ligne 3-1 :")));
	accessLayout2->addWidget(idAccess[2]);

	QGridLayout *layout = new QGridLayout(ret);
	layout->addWidget(listWidget, 0, 0, 7, 1, Qt::AlignLeft);
	layout->addWidget(new QLabel(tr("Point 1 :")), 0, 1);
	layout->addWidget(idVertices[0], 0, 2);
	layout->addWidget(new QLabel(tr("Point 2 :")), 1, 1);
	layout->addWidget(idVertices[1], 1, 2);
	layout->addWidget(new QLabel(tr("Point 3 :")), 2, 1);
	layout->addWidget(idVertices[2], 2, 2);
	layout->addLayout(accessLayout0, 3, 1, 1, 2);
	layout->addLayout(accessLayout1, 4, 1, 1, 2);
	layout->addLayout(accessLayout2, 5, 1, 1, 2);
	layout->setRowStretch(6, 1);

	connect(idList, SIGNAL(currentRowChanged(int)), SLOT(setCurrentId(int)));
	connect(idVertices[0], SIGNAL(valuesChanged(Vertex_s)), SLOT(editIdTriangle(Vertex_s)));
	connect(idVertices[1], SIGNAL(valuesChanged(Vertex_s)), SLOT(editIdTriangle(Vertex_s)));
	connect(idVertices[1], SIGNAL(valuesChanged(Vertex_s)), SLOT(editIdTriangle(Vertex_s)));
	connect(idAccess[0], SIGNAL(valueChanged(int)), SLOT(editIdAccess(int)));
	connect(idAccess[1], SIGNAL(valueChanged(int)), SLOT(editIdAccess(int)));
	connect(idAccess[2], SIGNAL(valueChanged(int)), SLOT(editIdAccess(int)));

	return ret;
}

QWidget *WalkmeshWidget::buildGatewaysPage()
{
	QWidget *ret = new QWidget(this);

	gateList = new QListWidget(ret);
	gateList->setFixedWidth(125);

	exitPoints[0] = new VertexWidget(ret);
	exitPoints[1] = new VertexWidget(ret);
	entryPoint = new VertexWidget(ret);

	fieldId = new QSpinBox(ret);
	fieldId->setRange(0, 65535);

	for(int i=0 ; i<4 ; ++i) {
		unknownGate1[i] = new QSpinBox(ret);
		unknownGate1[i]->setRange(0, 65535);
	}
	unknownGate2 = new HexLineEdit(ret);

	QGridLayout *idsLayout = new QGridLayout;
	idsLayout->addWidget(new QLabel(tr("Id écran :")), 0, 0);
	idsLayout->addWidget(fieldId, 0, 1, 1, 4);
	idsLayout->addWidget(new QLabel(tr("Inconnu 1 :")), 1, 0);
	idsLayout->addWidget(unknownGate1[0], 1, 1);
	idsLayout->addWidget(unknownGate1[1], 1, 2);
	idsLayout->addWidget(unknownGate1[2], 1, 3);
	idsLayout->addWidget(unknownGate1[3], 1, 4);
	idsLayout->addWidget(new QLabel(tr("Inconnu 2 :")), 2, 0);
	idsLayout->addWidget(unknownGate2, 2, 1, 1, 4);

	QGridLayout *layout = new QGridLayout(ret);
	layout->addWidget(gateList, 0, 0, 5, 1, Qt::AlignLeft);
	layout->addWidget(new QLabel(tr("Ligne de sortie :")), 0, 1);
	layout->addWidget(exitPoints[0], 0, 2);
	layout->addWidget(exitPoints[1], 1, 2);
	layout->addWidget(new QLabel(tr("Point de destination :")), 2, 1);
	layout->addWidget(entryPoint, 2, 2);
	layout->addLayout(idsLayout, 3, 1, 1, 2);
	layout->setRowStretch(4, 1);

	connect(gateList, SIGNAL(currentRowChanged(int)), SLOT(setCurrentGateway(int)));
	connect(exitPoints[0], SIGNAL(valuesChanged(Vertex_s)), SLOT(editExitPoint(Vertex_s)));
	connect(exitPoints[1], SIGNAL(valuesChanged(Vertex_s)), SLOT(editExitPoint(Vertex_s)));
	connect(entryPoint, SIGNAL(valuesChanged(Vertex_s)), SLOT(editEntryPoint(Vertex_s)));
	connect(fieldId, SIGNAL(valueChanged(int)), SLOT(editFieldId(int)));
	for(int i=0 ; i<4 ; ++i) {
		connect(unknownGate1[i], SIGNAL(valueChanged(int)), SLOT(editUnknownGate(int)));
	}
	connect(unknownGate2, SIGNAL(dataEdited(QByteArray)), SLOT(editUnknownGate(QByteArray)));

	return ret;
}

QWidget *WalkmeshWidget::buildDoorsPage()
{
	QWidget *ret = new QWidget(this);

	doorList = new QListWidget(ret);
	doorList->setFixedWidth(125);

	doorPosition[0] = new VertexWidget(ret);
	doorPosition[1] = new VertexWidget(ret);

	doorUsed = new QCheckBox(tr("Utilisé"));

	doorId = new QSpinBox(ret);
	doorId->setRange(0, 254);

	QGridLayout *idsLayout = new QGridLayout;
	idsLayout->addWidget(doorUsed, 0, 0);
	idsLayout->addWidget(new QLabel(tr("Id porte :")), 0, 1);
	idsLayout->addWidget(doorId, 0, 2);

	QGridLayout *layout = new QGridLayout(ret);
	layout->addWidget(doorList, 0, 0, 4, 1, Qt::AlignLeft);
	layout->addWidget(new QLabel(tr("Ligne déclench. porte :")), 0, 1);
	layout->addWidget(doorPosition[0], 0, 2);
	layout->addWidget(doorPosition[1], 1, 2);
	layout->addLayout(idsLayout, 2, 1, 1, 2);
	layout->setRowStretch(3, 1);

	connect(doorList, SIGNAL(currentRowChanged(int)), SLOT(setCurrentDoor(int)));
	connect(doorPosition[0], SIGNAL(valuesChanged(Vertex_s)), SLOT(editDoorPoint(Vertex_s)));
	connect(doorPosition[1], SIGNAL(valuesChanged(Vertex_s)), SLOT(editDoorPoint(Vertex_s)));
	connect(doorUsed, SIGNAL(toggled(bool)), SLOT(editDoorUsed(bool)));
	connect(doorId, SIGNAL(valueChanged(int)), SLOT(editDoorId(int)));

	return ret;
}

QWidget *WalkmeshWidget::buildCameraRangePage()
{
	QWidget *ret = new QWidget(this);

	rangeList1 = new QListWidget(ret);
	rangeList1->setFixedWidth(125);

	for(int i=0 ; i<8 ; ++i) {
		rangeList1->addItem(tr("Limite caméra %1").arg(i+1));
	}

	rangeList2 = new QListWidget(ret);
	rangeList2->setFixedWidth(125);

	for(int i=0 ; i<2 ; ++i) {
		rangeList2->addItem(tr("Limite écran %1").arg(i+1));
	}

	for(int i=0 ; i<4 ; ++i) {
		rangeEdit1[i] = new QSpinBox(ret);
		rangeEdit1[i]->setRange(-32768, 32767);
		rangeEdit2[i] = new QSpinBox(ret);
		rangeEdit2[i]->setRange(-32768, 32767);
	}

	QGridLayout *layout = new QGridLayout(ret);
	layout->addWidget(rangeList1, 0, 0, 3, 1, Qt::AlignLeft);
	layout->addWidget(rangeList2, 3, 0, 3, 1, Qt::AlignLeft);
	layout->addWidget(new QLabel(tr("Haut")), 0, 1);
	layout->addWidget(rangeEdit1[0], 0, 2);
	layout->addWidget(new QLabel(tr("Bas")), 0, 3);
	layout->addWidget(rangeEdit1[1], 0, 4);
	layout->addWidget(new QLabel(tr("Droite")), 1, 1);
	layout->addWidget(rangeEdit1[2], 1, 2);
	layout->addWidget(new QLabel(tr("Gauche")), 1, 3);
	layout->addWidget(rangeEdit1[3], 1, 4);
	layout->addWidget(new QLabel(tr("Haut")), 3, 1);
	layout->addWidget(rangeEdit2[0], 3, 2);
	layout->addWidget(new QLabel(tr("Bas")), 3, 3);
	layout->addWidget(rangeEdit2[1], 3, 4);
	layout->addWidget(new QLabel(tr("Droite")), 4, 1);
	layout->addWidget(rangeEdit2[2], 4, 2);
	layout->addWidget(new QLabel(tr("Gauche")), 4, 3);
	layout->addWidget(rangeEdit2[3], 4, 4);
	layout->setRowStretch(2, 1);
	layout->setRowStretch(5, 1);
	layout->setColumnStretch(1, 1);
	layout->setColumnStretch(2, 1);
	layout->setColumnStretch(3, 1);
	layout->setColumnStretch(4, 1);

	connect(rangeList1, SIGNAL(currentRowChanged(int)), SLOT(setCurrentRange1(int)));
	connect(rangeList2, SIGNAL(currentRowChanged(int)), SLOT(setCurrentRange2(int)));
	for(int i=0 ; i<4 ; ++i) {
		connect(rangeEdit1[i], SIGNAL(valueChanged(int)), SLOT(editRange(int)));
		connect(rangeEdit2[i], SIGNAL(valueChanged(int)), SLOT(editRange(int)));
	}

	return ret;
}

QWidget *WalkmeshWidget::buildMovieCameraPage()
{
	QWidget *ret = new QWidget(this);

	ListWidget *listWidget = new ListWidget(ret);
	camPlusAction = listWidget->addAction(ListWidget::Add, tr("Ajouter"), this, SLOT(addMovieCameraPosition()));
	camMinusAction = listWidget->addAction(ListWidget::Rem, tr("Effacer"), this, SLOT(removeMovieCameraPosition()));
	camToolbar = listWidget->toolBar();

	frameList = listWidget->listWidget();
	//	frameList->setDragDropMode(QAbstractItemView::InternalMove);//TODO

	camPoints[0] = new VertexWidget(ret);
	camPoints[1] = new VertexWidget(ret);
	camPoints[2] = new VertexWidget(ret);
	camPoints[3] = new VertexWidget(ret);

	QGridLayout *layout = new QGridLayout(ret);
	layout->addWidget(listWidget, 0, 0, 5, 1);
	layout->addWidget(camPoints[0], 0, 1);
	layout->addWidget(camPoints[1], 1, 1);
	layout->addWidget(camPoints[2], 2, 1);
	layout->addWidget(camPoints[3], 3, 1);
	layout->setRowStretch(4, 1);
	layout->setColumnStretch(1, 1);

	connect(frameList, SIGNAL(currentRowChanged(int)), SLOT(setCurrentMoviePosition(int)));

	return ret;
}

QWidget *WalkmeshWidget::buildMiscPage()
{
	QWidget *ret = new QWidget(this);

	navigation = new OrientationWidget(ret);
	navigation2 = new QSpinBox(ret);
	navigation2->setRange(0, 255);
	navigation2->setWrapping(true);

	unknown = new HexLineEdit(ret);
	cameraFocus = new QSpinBox(ret);
	cameraFocus->setRange(-32768, 32767);

	QGridLayout *layout = new QGridLayout(ret);
	layout->addWidget(new QLabel(tr("Orientation des mouvements :")), 0, 0);
	layout->addWidget(navigation, 0, 1);
	layout->addWidget(navigation2, 0, 2);
	layout->addWidget(new QLabel(tr("Inconnu :")), 1, 0);
	layout->addWidget(unknown, 1, 1, 1, 2);
	layout->addWidget(new QLabel(tr("Hauteur focus caméra sur le personnage :")), 2, 0);
	layout->addWidget(cameraFocus, 2, 1, 1, 2);
	layout->setRowStretch(4, 1);

	connect(navigation, SIGNAL(valueEdited(int)), navigation2, SLOT(setValue(int)));
	connect(navigation2, SIGNAL(valueChanged(int)), SLOT(editNavigation(int)));
	connect(unknown, SIGNAL(dataEdited(QByteArray)), SLOT(editUnknown(QByteArray)));
	connect(cameraFocus, SIGNAL(valueChanged(int)), SLOT(editCameraFocus(int)));

	return ret;
}

void WalkmeshWidget::clear()
{
	if(!isFilled())		return;

	walkmeshGL->clear();

	blockSignals(true);
	camList->clear();
	idList->clear();
	gateList->clear();
	doorList->clear();
	frameList->clear();
	blockSignals(false);

	PageWidget::clear();
}

void WalkmeshWidget::setReadOnly(bool ro)
{
	if(isBuilded()) {
		// CamPage
		caToolbar->setDisabled(ro);
		caVectorXEdit->setReadOnly(ro);
		caVectorYEdit->setReadOnly(ro);
		caVectorZEdit->setReadOnly(ro);
		caSpaceXEdit->setReadOnly(ro);
		caSpaceYEdit->setReadOnly(ro);
		caSpaceZEdit->setReadOnly(ro);
		caZoomEdit->setReadOnly(ro);
		// WalkPage
		idToolbar->setDisabled(ro);
		for(int i=0 ; i<3 ; ++i) {
			idVertices[i]->setReadOnly(ro);
			idAccess[i]->setReadOnly(ro);
		}
		// GatePage
		exitPoints[0]->setReadOnly(ro);
		exitPoints[1]->setReadOnly(ro);
		entryPoint->setReadOnly(ro);
		fieldId->setReadOnly(ro);
		for(int i=0 ; i<4 ; ++i) 	unknownGate1[i]->setReadOnly(ro);
		unknownGate2->setReadOnly(ro);
		// DoorPage
		doorId->setReadOnly(ro);
		doorPosition[0]->setReadOnly(ro);
		doorPosition[1]->setReadOnly(ro);
		doorUsed->setDisabled(ro);
		// CamRangePage
		for(int i=0 ; i<4 ; ++i) {
			rangeEdit1[i]->setReadOnly(ro);
			rangeEdit2[i]->setReadOnly(ro);
		}
		// MoveCamPage
		camToolbar->setDisabled(ro);
		for(int i=0 ; i<4 ; ++i)	camPoints[i]->setReadOnly(ro);
		// MiscPage
		navigation->setReadOnly(ro);
		navigation2->setReadOnly(ro);
		unknown->setReadOnly(ro);
		cameraFocus->setReadOnly(ro);
	}

	PageWidget::setReadOnly(ro);
}

void WalkmeshWidget::fill()
{
	if(!isBuilded())	build();
	if(isFilled())		clear();

	if(!hasData() ||
			(!data()->hasCaFile()
			 && !data()->hasIdFile()
			 && !data()->hasInfFile())) return;

	walkmeshGL->fill(data());

	int camCount = 0;

	if(data()->hasCaFile()) {
		camCount = data()->getCaFile()->cameraCount();

		if(camList->count() != camCount) {
			camList->blockSignals(true);
			camList->clear();
			for(int i=0 ; i<camCount ; ++i) {
				camList->addItem(tr("Caméra %1").arg(i));
			}
			camList->blockSignals(false);
		}

		setCurrentCamera(0);
	}
	tabWidget->widget(0)->setEnabled(data()->hasCaFile() && camCount > 0);

	if(data()->hasIdFile()) {
		int triangleCount = data()->getIdFile()->triangleCount();

		if(idList->count() != triangleCount) {
			idList->blockSignals(true);
			idList->clear();
			for(int i=0 ; i<triangleCount ; ++i) {
				idList->addItem(tr("Triangle %1").arg(i));
			}
			idList->blockSignals(false);
		}
		idList->setCurrentRow(0);
		setCurrentId(0);
	}
	tabWidget->widget(1)->setEnabled(data()->hasIdFile());

	if(data()->hasInfFile()) {
		gateList->clear();
		foreach(const Gateway &gateway, data()->getInfFile()->getGateways()) {
			if(gateway.fieldId != 0x7FFF) {
				gateList->addItem(QString("%1 (%2)").arg(Data::maplist().value(gateway.fieldId)).arg(gateway.fieldId));
			} else {
				gateList->addItem(tr("Inutilisé"));
			}
		}
		gateList->setCurrentRow(0);
		setCurrentGateway(0);

		doorList->clear();
		foreach(const Trigger &trigger, data()->getInfFile()->getTriggers(false)) {
			if(trigger.doorID != 0xFF) {
				doorList->addItem(tr("Porte %1").arg(trigger.doorID));
			} else {
				doorList->addItem(tr("Inutilisé"));
			}
		}
		doorList->setCurrentRow(0);
		setCurrentDoor(0);

		rangeList1->setCurrentRow(0);
		setCurrentRange1(0);
		rangeList2->setCurrentRow(0);
		setCurrentRange2(0);

		navigation->setValue(data()->getInfFile()->controlDirection());
		navigation2->setValue(data()->getInfFile()->controlDirection());
		unknown->setData(data()->getInfFile()->unknown());
		cameraFocus->setValue(data()->getInfFile()->cameraFocusHeight());
	}
	tabWidget->widget(2)->setEnabled(data()->hasInfFile());
	tabWidget->widget(3)->setEnabled(data()->hasInfFile());
	tabWidget->widget(4)->setEnabled(data()->hasInfFile());
	tabWidget->widget(6)->setEnabled(data()->hasInfFile());

	if(data()->hasMskFile()) {
		frameList->clear();
		int cameraPositionCount = data()->getMskFile()->cameraPositionCount();
		for(int i=0 ; i<cameraPositionCount ; ++i) {
			frameList->addItem(tr("Position %1").arg(i+1));
		}
		frameList->setCurrentRow(0);
		setCurrentMoviePosition(0);
	}
	tabWidget->widget(5)->setEnabled(data()->hasMskFile());

	PageWidget::fill();
}

int WalkmeshWidget::currentCamera() const
{
	if(!data()->hasCaFile())	return 0;

	int camID = camList->currentRow();
	return camID < 0 || camID >= data()->getCaFile()->cameraCount() ? 0 : camID;
}

void WalkmeshWidget::setCurrentCamera(int camID)
{
	if(!data()->hasCaFile() || camID < 0) {
		return;
	}

	bool hasCamera = camID < data()->getCaFile()->cameraCount();

	if(hasCamera) {
		const Camera &cam = data()->getCaFile()->camera(camID);

		//		qDebug() << cam.camera_axis[0].x << cam.camera_axis[0].y << cam.camera_axis[0].z;
		//		qDebug() << cam.camera_axis[1].x << cam.camera_axis[1].y << cam.camera_axis[1].z;
		//		qDebug() << cam.camera_axis[2].x << cam.camera_axis[2].y << cam.camera_axis[2].z;
		//		qDebug() << cam.camera_position[0] << cam.camera_position[1] << cam.camera_position[2];

		blockSignals(true);
		caVectorXEdit->setValues(cam.camera_axis[0]);
		caVectorYEdit->setValues(cam.camera_axis[1]);
		caVectorZEdit->setValues(cam.camera_axis[2]);

		caSpaceXEdit->setValue(cam.camera_position[0]);
		caSpaceYEdit->setValue(cam.camera_position[1]);
		caSpaceZEdit->setValue(cam.camera_position[2]);

		caZoomEdit->setValue(cam.camera_zoom);

		walkmeshGL->setCurrentFieldCamera(camID);
		blockSignals(false);
	}

	caVectorXEdit->setEnabled(hasCamera);
	caVectorYEdit->setEnabled(hasCamera);
	caVectorZEdit->setEnabled(hasCamera);

	caSpaceXEdit->setEnabled(hasCamera);
	caSpaceYEdit->setEnabled(hasCamera);
	caSpaceZEdit->setEnabled(hasCamera);

	caZoomEdit->setEnabled(hasCamera);

	if(camList->currentRow() != camID) {
		camList->blockSignals(true);
		camList->setCurrentRow(camID);
		camList->blockSignals(false);
	}
}

void WalkmeshWidget::addCamera()
{
	int row = camList->currentRow();

	if(data()->hasCaFile()) {
		Camera ca;
		if(row < data()->getCaFile()->cameraCount()) {
			ca = data()->getCaFile()->camera(row);
		} else {
			ca = Camera();
		}
		data()->getCaFile()->insertCamera(row+1, ca);
		camList->insertItem(row+1, tr("Camera %1").arg(row+1));
		for(int i=row+2 ; i<camList->count() ; ++i) {
			camList->item(i)->setText(tr("Camera %1").arg(i));
		}
		camList->setCurrentRow(row+1);
		emit modified();
	}
}

void WalkmeshWidget::removeCamera()
{
	if(data()->getCaFile()->cameraCount() < 2) return;

	int row = camList->currentRow();

	if(row < 0)		return;

	if(data()->hasCaFile() && row < data()->getCaFile()->cameraCount()) {
		data()->getCaFile()->removeCamera(row);
		delete camList->item(row);
		for(int i=row ; i<camList->count() ; ++i) {
			camList->item(i)->setText(tr("Camera %1").arg(i));
		}
		setCurrentCamera(row);
		emit modified();
	}
}

void WalkmeshWidget::editCaVector(const Vertex_s &values)
{
	QObject *s = sender();

	if(s == caVectorXEdit)			editCaVector(0, values);
	else if(s == caVectorYEdit)		editCaVector(1, values);
	else if(s == caVectorZEdit)		editCaVector(2, values);
}

void WalkmeshWidget::editCaVector(int id, const Vertex_s &values)
{
	if(data()->hasCaFile() && data()->getCaFile()->cameraCount() > 0) {
		const int camID = currentCamera();
		Camera cam = data()->getCaFile()->camera(camID);
		Vertex_s oldV = cam.camera_axis[id];

		if(oldV.x != values.x || oldV.y != values.y || oldV.z != values.z) {
			cam.camera_axis[id] = values;
			data()->getCaFile()->setCamera(camID, cam);
			walkmeshGL->updateGL();
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

void WalkmeshWidget::editCaPos(int id, double value)
{
	if(data()->hasCaFile() && data()->getCaFile()->cameraCount() > 0) {
		const int camID = currentCamera();
		Camera cam = data()->getCaFile()->camera(camID);
		if(cam.camera_position[id] != (qint32)value) {
			cam.camera_position[id] = value;
			data()->getCaFile()->setCamera(camID, cam);
			walkmeshGL->updateGL();
			emit modified();
		}
	}
}

void WalkmeshWidget::editCaZoom(int value)
{
	if(data()->hasCaFile() && data()->getCaFile()->cameraCount() > 0) {
		const int camID = currentCamera();
		Camera cam = data()->getCaFile()->camera(camID);
		if(cam.camera_zoom != value) {
			cam.camera_zoom = value;
			data()->getCaFile()->setCamera(camID, cam);
			walkmeshGL->updatePerspective();
			emit modified();
		}
	}
}

void WalkmeshWidget::setCurrentId(int i)
{
	if(!data()->hasIdFile() || i < 0)	return;

	IdFile *id = data()->getIdFile();
	if(id->triangleCount() <= i)	return;

	const Triangle &triangle = id->triangle(i);
	const Access &access = id->access(i);

	idVertices[0]->setValues(IdFile::toVertex_s(triangle.vertices[0]));
	idVertices[1]->setValues(IdFile::toVertex_s(triangle.vertices[1]));
	idVertices[2]->setValues(IdFile::toVertex_s(triangle.vertices[2]));

	idAccess[0]->setValue(access.a[0]);
	idAccess[1]->setValue(access.a[1]);
	idAccess[2]->setValue(access.a[2]);

	walkmeshGL->setSelectedTriangle(i);
}

void WalkmeshWidget::addTriangle()
{
	int row = idList->currentRow();

	if(data()->hasIdFile()) {
		Triangle tri;
		Access acc;
		if(row < data()->getIdFile()->triangleCount()) {
			tri = data()->getIdFile()->triangle(row);
			acc = data()->getIdFile()->access(row);
		} else {
			tri = Triangle();
			acc = Access();
		}
		data()->getIdFile()->insertTriangle(row+1, tri, acc);
		idList->insertItem(row+1, tr("Triangle %1").arg(row+1));
		for(int i=row+2 ; i<idList->count() ; ++i) {
			idList->item(i)->setText(tr("Triangle %1").arg(i));
		}
		idList->setCurrentRow(row+1);
		emit modified();
	}
}

void WalkmeshWidget::removeTriangle()
{
	int row = idList->currentRow();

	if(row < 0)		return;

	if(data()->hasIdFile() && row < data()->getIdFile()->triangleCount()) {
		data()->getIdFile()->removeTriangle(row);
		delete idList->item(row);
		for(int i=row ; i<idList->count() ; ++i) {
			idList->item(i)->setText(tr("Triangle %1").arg(i));
		}
		setCurrentId(row);
		emit modified();
	}
}

void WalkmeshWidget::editIdTriangle(const Vertex_s &values)
{
	QObject *s = sender();

	if(s == idVertices[0])			editIdTriangle(0, values);
	else if(s == idVertices[1])		editIdTriangle(1, values);
	else if(s == idVertices[2])		editIdTriangle(2, values);
}

void WalkmeshWidget::editIdTriangle(int id, const Vertex_s &values)
{
	if(data()->hasIdFile()) {
		const int triangleID = idList->currentRow();
		if(triangleID > -1 && triangleID < data()->getIdFile()->triangleCount()) {
			Triangle old = data()->getIdFile()->triangle(triangleID);
			Vertex_sr &oldV = old.vertices[id];
			if(oldV.x != values.x || oldV.y != values.y || oldV.z != values.z) {
				oldV = IdFile::fromVertex_s(values);
				data()->getIdFile()->setTriangle(triangleID, old);
				walkmeshGL->updateGL();
				emit modified();
			}
		}
	}
}

void WalkmeshWidget::editIdAccess(int value)
{
	QObject *s = sender();

	if(s == idAccess[0])			editIdAccess(0, value);
	else if(s == idAccess[1])		editIdAccess(1, value);
	else if(s == idAccess[2])		editIdAccess(2, value);
}

void WalkmeshWidget::editIdAccess(int id, int value)
{
	if(data()->hasIdFile()) {
		const int triangleID = idList->currentRow();
		if(triangleID > -1 && triangleID < data()->getIdFile()->triangleCount()) {
			Access old = data()->getIdFile()->access(triangleID);
			qint16 oldV = old.a[id];
			if(oldV != value) {
				old.a[id] = value;
				data()->getIdFile()->setAccess(triangleID, old);
				walkmeshGL->updateGL();
				emit modified();
			}
		}
	}
}

void WalkmeshWidget::setCurrentGateway(int id)
{
	if(!data()->hasInfFile() || id < 0)    return;

	InfFile *inf = data()->getInfFile();
	if(12 <= id)    return;

	const Gateway &gateway = inf->getGateway(id);

	exitPoints[0]->setValues(gateway.exitLine[0]);
	exitPoints[1]->setValues(gateway.exitLine[1]);
	entryPoint->setValues(gateway.destinationPoint);
	fieldId->setValue(gateway.fieldId);
	unknownGate1[0]->setValue(gateway.unknown1[0]);
	unknownGate1[1]->setValue(gateway.unknown1[1]);
	unknownGate1[2]->setValue(gateway.unknown1[2]);
	unknownGate1[3]->setValue(gateway.unknown1[3]);

	unknownGate2->setData(QByteArray((char *)&gateway.unknown2, 4));

	walkmeshGL->setSelectedGate(id);
}

void WalkmeshWidget::setCurrentDoor(int id)
{
	if(!data()->hasInfFile() || id < 0)    return;

	InfFile *inf = data()->getInfFile();
	if(12 <= id)    return;

	const Trigger &trigger = inf->getTrigger(id);

	doorPosition[0]->setValues(trigger.trigger_line[0]);
	doorPosition[1]->setValues(trigger.trigger_line[1]);
	if(trigger.doorID == 0xFF) {
		doorId->setValue(0);
		doorId->setEnabled(false);
		doorUsed->setChecked(false);
	} else {
		doorId->setValue(trigger.doorID);
		doorId->setEnabled(true);
		doorUsed->setChecked(true);
	}
	walkmeshGL->setSelectedDoor(id);
}

void WalkmeshWidget::editExitPoint(const Vertex_s &values)
{
	QObject *s = sender();

	if(s == exitPoints[0])			editExitPoint(0, values);
	else if(s == exitPoints[1])		editExitPoint(1, values);
}

void WalkmeshWidget::editExitPoint(int id, const Vertex_s &values)
{
	if(data()->hasInfFile()) {
		int gateId = gateList->currentRow();
		Gateway old = data()->getInfFile()->getGateway(gateId);
		Vertex_s oldVertex = old.exitLine[id];
		if(oldVertex.x != values.x || oldVertex.y != values.y || oldVertex.z != values.z) {
			old.exitLine[id] = values;
			data()->getInfFile()->setGateway(gateId, old);
			walkmeshGL->updateGL();
			emit modified();
		}
	}
}

void WalkmeshWidget::editEntryPoint(const Vertex_s &values)
{
	if(data()->hasInfFile()) {
		int gateId = gateList->currentRow();
		Gateway old = data()->getInfFile()->getGateway(gateId);
		Vertex_s oldVertex = old.destinationPoint;
		if(oldVertex.x != values.x || oldVertex.y != values.y || oldVertex.z != values.z) {
			old.destinationPoint = values;
			data()->getInfFile()->setGateway(gateId, old);
			emit modified();
		}
	}
}

void WalkmeshWidget::editDoorPoint(const Vertex_s &values)
{
	QObject *s = sender();

	if(s == doorPosition[0])			editDoorPoint(0, values);
	else if(s == doorPosition[1])		editDoorPoint(1, values);
}

void WalkmeshWidget::editDoorPoint(int id, const Vertex_s &values)
{
	if(data()->hasInfFile()) {
		int gateId = gateList->currentRow();
		Trigger old = data()->getInfFile()->getTrigger(gateId);
		Vertex_s oldVertex = old.trigger_line[id];
		if(oldVertex.x != values.x || oldVertex.y != values.y || oldVertex.z != values.z) {
			old.trigger_line[id] = values;
			data()->getInfFile()->setTrigger(gateId, old);
			walkmeshGL->updateGL();
			emit modified();
		}
	}
}

void WalkmeshWidget::editUnknownGate(int val)
{
	QObject *s = sender();

	if(s == unknownGate1[0])			editUnknownGate(0, val);
	else if(s == unknownGate1[1])		editUnknownGate(1, val);
	else if(s == unknownGate1[2])		editUnknownGate(2, val);
	else if(s == unknownGate1[3])		editUnknownGate(3, val);
}

void WalkmeshWidget::editUnknownGate(int id, int val)
{
	if(data()->hasInfFile()) {
		int gateId = gateList->currentRow();
		Gateway old = data()->getInfFile()->getGateway(gateId);
		if(old.unknown1[id] != val) {
			old.unknown1[id] = val;
			data()->getInfFile()->setGateway(gateId, old);
			emit modified();
		}
	}
}

void WalkmeshWidget::editUnknownGate(const QByteArray &u)
{
	if(data()->hasInfFile()) {
		int gateId = gateList->currentRow();
		const char *uData = u.constData();
		Gateway old = data()->getInfFile()->getGateway(gateId);
		memcpy(&old.unknown2, uData, 4);
		if(old.unknown2 != data()->getInfFile()->getGateway(gateId).unknown2) {
			data()->getInfFile()->setGateway(gateId, old);
			emit modified();
		}
	}
}

void WalkmeshWidget::editFieldId(int v)
{
	if(data()->hasInfFile()) {
		int gateId = gateList->currentRow();
		Gateway old = data()->getInfFile()->getGateway(gateId);
		if(old.fieldId != v) {
			old.fieldId = v;
			data()->getInfFile()->setGateway(gateId, old);
			if(v != 0x7FFF) {
				gateList->currentItem()->setText(QString("%1 (%2)").arg(Data::maplist().value(v)).arg(v));
			} else {
				gateList->currentItem()->setText(tr("Inutilisé"));
			}

			emit modified();
		}
	}
}

void WalkmeshWidget::editDoorUsed(bool enable)
{
	doorId->setEnabled(enable);

	editDoorId(enable ? doorId->value() : 0xFF);
}

void WalkmeshWidget::editDoorId(int v)
{
	if(data()->hasInfFile()) {
		int gateId = doorList->currentRow();
		Trigger old = data()->getInfFile()->getTrigger(gateId);
		v = doorUsed->isChecked() ? doorId->value() : 0xFF;
		if(old.doorID != v) {
			old.doorID = v;
			data()->getInfFile()->setTrigger(gateId, old);
			if(v != 0xFF) {
				doorList->currentItem()->setText(tr("Porte %1").arg(v));
			} else {
				doorList->currentItem()->setText(tr("Inutilisé"));
			}

			walkmeshGL->updateGL();
			emit modified();
		}
	}
}

void WalkmeshWidget::setCurrentRange1(int id)
{
	if(!data()->hasInfFile() || id < 0)    return;

	InfFile *inf = data()->getInfFile();
	if(8 <= id)    return;

	const Range &range = inf->cameraRange(id);

	rangeEdit1[0]->setValue(range.top);
	rangeEdit1[1]->setValue(range.bottom);
	rangeEdit1[2]->setValue(range.right);
	rangeEdit1[3]->setValue(range.left);
}

void WalkmeshWidget::setCurrentRange2(int id)
{
	if(!data()->hasInfFile() || id < 0)    return;

	InfFile *inf = data()->getInfFile();
	if(2 <= id)    return;

	const Range &range = inf->screenRange(id);

	rangeEdit2[0]->setValue(range.top);
	rangeEdit2[1]->setValue(range.bottom);
	rangeEdit2[2]->setValue(range.right);
	rangeEdit2[3]->setValue(range.left);
}

void WalkmeshWidget::editRange(int v)
{
	QObject *s = sender();

	if(s == rangeEdit1[0])			editRange1(0, v);
	else if(s == rangeEdit1[1])		editRange1(1, v);
	else if(s == rangeEdit1[2])		editRange1(2, v);
	else if(s == rangeEdit1[3])		editRange1(3, v);
	else if(s == rangeEdit2[0])		editRange2(0, v);
	else if(s == rangeEdit2[1])		editRange2(1, v);
	else if(s == rangeEdit2[2])		editRange2(2, v);
	else if(s == rangeEdit2[3])		editRange2(3, v);
}

void WalkmeshWidget::editRange1(int id, int v)
{
	if(data()->hasInfFile()) {
		const int currentRange = rangeList1->currentRow();
		Range old = data()->getInfFile()->cameraRange(currentRange);
		qint16 oldv=0;

		switch(id) {
		case 0:	oldv = old.top;		break;
		case 1:	oldv = old.bottom;	break;
		case 2:	oldv = old.right;	break;
		case 3:	oldv = old.left;	break;
		}

		if(oldv != v) {
			switch(id) {
			case 0:	old.top = v;	break;
			case 1:	old.bottom = v;	break;
			case 2:	old.right = v;	break;
			case 3:	old.left = v;	break;
			}
			data()->getInfFile()->setCameraRange(currentRange, old);
			emit modified();
		}
	}
}

void WalkmeshWidget::editRange2(int id, int v)
{
	if(data()->hasInfFile()) {
		const int currentRange = rangeList2->currentRow();
		Range old = data()->getInfFile()->screenRange(currentRange);
		qint16 oldv=0;

		switch(id) {
		case 0:	oldv = old.top;		break;
		case 1:	oldv = old.bottom;	break;
		case 2:	oldv = old.right;	break;
		case 3:	oldv = old.left;	break;
		}

		if(oldv != v) {
			switch(id) {
			case 0:	old.top = v;	break;
			case 1:	old.bottom = v;	break;
			case 2:	old.right = v;	break;
			case 3:	old.left = v;	break;
			}
			data()->getInfFile()->setScreenRange(currentRange, old);
			emit modified();
		}
	}
}

void WalkmeshWidget::setCurrentMoviePosition(int id)
{
	if(!data()->hasMskFile() || id < 0) {
		setMovieCameraPageEnabled(false);
		return;
	}

	MskFile *msk = data()->getMskFile();
	if(msk->cameraPositionCount() <= id) {
		setMovieCameraPageEnabled(false);
		return;
	}

	setMovieCameraPageEnabled(true);

	camPoints[0]->setValues(msk->cameraPosition(id)[0]);
	camPoints[1]->setValues(msk->cameraPosition(id)[1]);
	camPoints[2]->setValues(msk->cameraPosition(id)[2]);
	camPoints[3]->setValues(msk->cameraPosition(id)[3]);
}

void WalkmeshWidget::setMovieCameraPageEnabled(bool enabled)
{
	camPoints[0]->setEnabled(enabled);
	camPoints[1]->setEnabled(enabled);
	camPoints[2]->setEnabled(enabled);
	camPoints[3]->setEnabled(enabled);
}

void WalkmeshWidget::addMovieCameraPosition()
{
	if(!data()->hasMskFile()) return;

	int row = frameList->currentRow();
	if(row < 0)	row = 0;

	Vertex_s *camPos = new Vertex_s[4];

	memset(camPos, 0, sizeof(Vertex_s) * 4);

	data()->getMskFile()->insertCameraPosition(row+1, camPos);

	frameList->addItem(tr("Position %1").arg(row+1));
	for(int i=row+1 ; i<frameList->count() ; ++i) {
		frameList->item(i)->setText(tr("Position %1").arg(i+1));
	}

	emit modified();

	frameList->setCurrentRow(row+1);
}

void WalkmeshWidget::removeMovieCameraPosition()
{
	if(!data()->hasMskFile()) return;

	int row = frameList->currentRow();
	if(row < 0)	row = 0;

	data()->getMskFile()->removeCameraPosition(row);

	delete frameList->item(row);
	for(int i=row ; i<frameList->count() ; ++i) {
		frameList->item(i)->setText(tr("Position %1").arg(i+1));
	}

	emit modified();
}

void WalkmeshWidget::editNavigation(int v)
{
	if(data()->hasInfFile()) {
		int old = data()->getInfFile()->controlDirection();
		if(old != v) {
			navigation->setValue(v);
			data()->getInfFile()->setControlDirection(v);
			emit modified();
		}
	}
}

void WalkmeshWidget::editUnknown(const QByteArray &u)
{
	if(data()->hasInfFile()) {
		if(u != data()->getInfFile()->unknown()) {
			data()->getInfFile()->setUnknown(u);
			emit modified();
		}
	}
}

void WalkmeshWidget::editCameraFocus(int value)
{
	if(data()->hasInfFile()) {
		if(value != data()->getInfFile()->cameraFocusHeight()) {
			data()->getInfFile()->setCameraFocusHeight(value);
			emit modified();
		}
	}
}

void WalkmeshWidget::focusInEvent(QFocusEvent *e)
{
	if(isBuilded())	walkmeshGL->setFocus();
	QWidget::focusInEvent(e);
}

void WalkmeshWidget::focusOutEvent(QFocusEvent *e)
{
	if(isBuilded())	walkmeshGL->clearFocus();
	QWidget::focusOutEvent(e);
}
