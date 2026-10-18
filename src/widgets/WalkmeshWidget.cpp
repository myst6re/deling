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
#include "widgets/WalkmeshWidget.h"
#include "Data.h"
#include "Config.h"
#include "ListWidget.h"
#include "FieldArchive.h"
#include "FieldLoose.h"

/**
 * One undoable walkmesh edit, kept as the whole walkmesh before and after it. A walkmesh is a few
 * hundred triangles at most (about 15 KB), so this is simpler and safer than recording each kind
 * of change together with its inverse.
 */
class WalkmeshEditCommand : public QUndoCommand
{
public:
	WalkmeshEditCommand(WalkmeshWidget *page, const QString &text,
	                    const IdFile::Snapshot &before, const IdFile::Snapshot &after) :
	    QUndoCommand(text), _page(page), _before(before), _after(after)
	{
	}
	void undo() override
	{
		_page->restoreWalkmesh(_before);
	}
	void redo() override
	{
		_page->restoreWalkmesh(_after);
	}
private:
	WalkmeshWidget *_page;
	IdFile::Snapshot _before, _after;
};

// The same for the 12 exits of the .inf file
class GatewaysEditCommand : public QUndoCommand
{
public:
	GatewaysEditCommand(WalkmeshWidget *page, const QString &text,
	                    const QList<Gateway> &before, const QList<Gateway> &after) :
	    QUndoCommand(text), _page(page), _before(before), _after(after)
	{
	}
	void undo() override
	{
		_page->restoreGateways(_before);
	}
	void redo() override
	{
		_page->restoreGateways(_after);
	}
private:
	WalkmeshWidget *_page;
	QList<Gateway> _before, _after;
};

WalkmeshWidget::WalkmeshWidget(QWidget *parent) :
	PageWidget(parent), walkmeshPage(nullptr), gatewaysPage(nullptr), fieldArchive(nullptr),
	viewField(nullptr), viewingDestination(false), looseDestination(nullptr)
{
	walkmeshGL = new WalkmeshGLWidget(this);
	undoStack = new QUndoStack(this);
}

void WalkmeshWidget::build()
{
	if (isBuilded())		return;

	viewMode = new QComboBox(this);
	viewMode->addItem(tr("Game camera"), WalkmeshGLWidget::GameView);
	viewMode->addItem(tr("Top view"), WalkmeshGLWidget::TopView);
	viewMode->addItem(tr("Free 3D view"), WalkmeshGLWidget::FreeView);
	viewMode->setToolTip(tr("The top and free views paint the background onto the walkmesh, "
	                        "as the game camera sees it"));

	viewInfos = new QLabel(this);
	viewInfos->setTextFormat(Qt::PlainText);
	viewInfos->setWordWrap(true);
	viewInfos->setMaximumWidth(260);

	// What the colours of the view mean, with the very colours it uses
	auto legendLine = [](QRgb color, const QString &meaning) {
		return QString("<span style=\"background-color:#303030; color:%1\">&nbsp;&#9632;&nbsp;</span> %2")
		        .arg(QColor(color).name(), meaning.toHtmlEscaped());
	};
	QLabel *legend = new QLabel(QStringList{
	    legendLine(WalkmeshGLWidget::COLOR_SIDE, tr("Side between two triangles")),
	    legendLine(WalkmeshGLWidget::COLOR_WALL, tr("Wall: nothing across this side")),
	    legendLine(WalkmeshGLWidget::COLOR_BROKEN, tr("Triangle the game cannot use (flipped, flat, or past triangle 511)")),
	    legendLine(WalkmeshGLWidget::COLOR_EXIT, tr("Exit (thick line)")),
	    legendLine(WalkmeshGLWidget::COLOR_DOOR, tr("Door trigger")),
	    legendLine(WalkmeshGLWidget::COLOR_SELECTED, tr("Selected: what the form shows")),
	    legendLine(WalkmeshGLWidget::COLOR_HOVER, tr("Under the mouse: what a click grabs, or adds when dashed"))
	}.join("<br>"));
	legend->setTextFormat(Qt::RichText);
	legend->setWordWrap(true);
	legend->setMaximumWidth(260); // wraps instead of widening the column and shrinking the view

	QPushButton *resetCamera = new QPushButton(tr("Reset"));

	showBackground = new QCheckBox(tr("Show background"));
	showBackground->setChecked(Config::value("fieldBackgroundVisible", true).toBool());

	tabWidget = new QTabWidget(this);
	tabWidget->addTab(buildCameraPage(), tr("Camera"));
	tabWidget->addTab(walkmeshPage = buildWalkmeshPage(), tr("Walkmesh"));
	tabWidget->addTab(gatewaysPage = buildGatewaysPage(), tr("Exits"));
	tabWidget->addTab(buildDoorsPage(), tr("Doors"));
	tabWidget->addTab(buildCameraRangePage(), tr("Camera Ranges"));
	tabWidget->addTab(buildMovieCameraPage(), tr("Movie Camera"));
	tabWidget->addTab(buildMiscPage(), tr("Miscellaneous"));
	tabWidget->setFixedHeight(250);

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(walkmeshGL, 0, 0, 5, 1);
	layout->addWidget(viewMode, 0, 1, 1, 3, Qt::AlignTop);
	layout->addWidget(viewInfos, 1, 1, 1, 3);
	layout->addWidget(resetCamera, 2, 1, 1, 3);
	layout->addWidget(showBackground, 3, 1, 1, 3);
	layout->addWidget(legend, 4, 1, 1, 3, Qt::AlignTop);
	layout->addWidget(tabWidget, 5, 0, 1, 4);
	layout->setColumnStretch(0, 1);
	layout->setContentsMargins(QMargins());

	connect(viewMode, &QComboBox::currentIndexChanged, this, &WalkmeshWidget::changeViewMode);
	changeViewMode();
	connect(resetCamera, SIGNAL(clicked()), SLOT(resetCamera()));
	connect(showBackground, SIGNAL(toggled(bool)), walkmeshGL, SLOT(setBackgroundVisible(bool)));

	// Editing the walkmesh and the exits with the mouse
	connect(tabWidget, &QTabWidget::currentChanged, this, &WalkmeshWidget::updateDestinationView);
	connect(walkmeshGL, &WalkmeshGLWidget::pointSelected, this, &WalkmeshWidget::selectTriangleOfPoint);
	connect(walkmeshGL, &WalkmeshGLWidget::pointDragStarted, this, &WalkmeshWidget::startPointDrag);
	connect(walkmeshGL, &WalkmeshGLWidget::pointDragged, this, &WalkmeshWidget::dragPoint);
	connect(walkmeshGL, &WalkmeshGLWidget::pointDragFinished, this, &WalkmeshWidget::finishPointDrag);
	connect(walkmeshGL, &WalkmeshGLWidget::pointAddRequested, this, &WalkmeshWidget::addPoint);
	connect(walkmeshGL, &WalkmeshGLWidget::pointDeleteRequested, this, &WalkmeshWidget::deletePoint);
	connect(walkmeshGL, &WalkmeshGLWidget::exitSelected, this, &WalkmeshWidget::selectExit);
	connect(walkmeshGL, &WalkmeshGLWidget::exitDragStarted, this, &WalkmeshWidget::startExitDrag);
	connect(walkmeshGL, &WalkmeshGLWidget::exitEndDragged, this, &WalkmeshWidget::dragExitEnd);
	connect(walkmeshGL, &WalkmeshGLWidget::exitDragFinished, this, &WalkmeshWidget::finishExitDrag);
	connect(walkmeshGL, &WalkmeshGLWidget::exitAddRequested, this, &WalkmeshWidget::addExit);
	connect(walkmeshGL, &WalkmeshGLWidget::exitDeleteRequested, this, &WalkmeshWidget::deleteExit);
	connect(walkmeshGL, &WalkmeshGLWidget::arrivalPickStarted, this, &WalkmeshWidget::startArrivalPick);
	connect(walkmeshGL, &WalkmeshGLWidget::arrivalPicked, this, &WalkmeshWidget::pickArrival);
	connect(walkmeshGL, &WalkmeshGLWidget::arrivalPickFinished, this, &WalkmeshWidget::finishArrivalPick);
	connect(walkmeshGL, &WalkmeshGLWidget::undoRequested, this, &WalkmeshWidget::undoWalkmeshEdit);
	connect(walkmeshGL, &WalkmeshGLWidget::redoRequested, this, &WalkmeshWidget::redoWalkmeshEdit);

	PageWidget::build();
}

void WalkmeshWidget::resetCamera()
{
	walkmeshGL->resetCamera();
}

void WalkmeshWidget::changeViewMode()
{
	const auto mode = WalkmeshGLWidget::ViewMode(viewMode->currentData().toInt());

	walkmeshGL->setViewMode(mode);

	switch (mode) {
	case WalkmeshGLWidget::GameView:
		viewInfos->setText(tr("Right-drag or the arrow keys move the view, the wheel zooms, "
		                      "a middle click resets it."));
		break;
	case WalkmeshGLWidget::TopView:
		viewInfos->setText(tr("Seen from above, the far side of the game camera at the top. "
		                      "Right-drag moves the view, the wheel zooms, a middle click resets it."));
		break;
	case WalkmeshGLWidget::FreeView:
		viewInfos->setText(tr("Right-drag turns around the walkmesh, Shift+right-drag moves the view, "
		                      "the wheel zooms, a middle click resets it."));
		break;
	}
}

QWidget *WalkmeshWidget::buildCameraPage()
{
	QWidget *ret = new QWidget(this);

	ListWidget *listWidget = new ListWidget(ret);
	listWidget->addAction(ListWidget::Add, tr("Add camera"), this, SLOT(addCamera()));
	listWidget->addAction(ListWidget::Rem, tr("Remove camera"), this, SLOT(removeCamera()));

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
	caLayout->addWidget(new QLabel(tr("Zoom:")), 0, 1, 1, 3);
	caLayout->addWidget(caZoomEdit, 0, 4, 1, 2);
	caLayout->addWidget(new QLabel(tr("Camera axis:")), 1, 1, 1, 6);
	caLayout->addWidget(caVectorXEdit, 2, 1, 1, 6);
	caLayout->addWidget(caVectorYEdit, 3, 1, 1, 6);
	caLayout->addWidget(caVectorZEdit, 4, 1, 1, 6);
	caLayout->addWidget(new QLabel(tr("Camera position:")), 5, 1, 1, 6);
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

	connect(caVectorXEdit, SIGNAL(valuesChanged(Vertex)), SLOT(editCaVector(Vertex)));
	connect(caVectorYEdit, SIGNAL(valuesChanged(Vertex)), SLOT(editCaVector(Vertex)));
	connect(caVectorZEdit, SIGNAL(valuesChanged(Vertex)), SLOT(editCaVector(Vertex)));

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
	listWidget->addAction(ListWidget::Add, tr("Add triangle"), this, SLOT(addTriangle()));
	listWidget->addAction(ListWidget::Rem, tr("Remove triangle"), this, SLOT(removeTriangle()));

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

	// Which triangle lies across each side is worked out from the points triangles share, and
	// redone after every edit, so these are only shown
	for (QSpinBox *access: idAccess) {
		access->setReadOnly(true);
		access->setButtonSymbols(QAbstractSpinBox::NoButtons);
		access->setToolTip(tr("Computed from the points the triangles share (-1: wall)"));
	}

	QHBoxLayout *accessLayout0 = new QHBoxLayout;
	accessLayout0->addWidget(new QLabel(tr("Triangle accessible via the line 1-2:")));
	accessLayout0->addWidget(idAccess[0]);

	QHBoxLayout *accessLayout1 = new QHBoxLayout;
	accessLayout1->addWidget(new QLabel(tr("Triangle accessible via la ligne 2-3:")));
	accessLayout1->addWidget(idAccess[1]);

	QHBoxLayout *accessLayout2 = new QHBoxLayout;
	accessLayout2->addWidget(new QLabel(tr("Triangle accessible via la ligne 3-1:")));
	accessLayout2->addWidget(idAccess[2]);

	QLabel *editInfos = new QLabel(tr("Drag a point to move it. Click outside the floor to add a triangle. "
	                                  "Delete removes the selected point. Right-drag moves the view, "
	                                  "Ctrl+Z / Ctrl+Y undo and redo."));
	editInfos->setTextFormat(Qt::PlainText);
	editInfos->setWordWrap(true);

	QGridLayout *layout = new QGridLayout(ret);
	layout->addWidget(listWidget, 0, 0, 8, 1, Qt::AlignLeft);
	layout->addWidget(new QLabel(tr("Point 1:")), 0, 1);
	layout->addWidget(idVertices[0], 0, 2);
	layout->addWidget(new QLabel(tr("Point 2:")), 1, 1);
	layout->addWidget(idVertices[1], 1, 2);
	layout->addWidget(new QLabel(tr("Point 3:")), 2, 1);
	layout->addWidget(idVertices[2], 2, 2);
	layout->addLayout(accessLayout0, 3, 1, 1, 2);
	layout->addLayout(accessLayout1, 4, 1, 1, 2);
	layout->addLayout(accessLayout2, 5, 1, 1, 2);
	layout->addWidget(editInfos, 6, 1, 1, 2);
	layout->setRowStretch(7, 1);

	connect(idList, SIGNAL(currentRowChanged(int)), SLOT(setCurrentId(int)));
	connect(idVertices[0], SIGNAL(valuesChanged(Vertex)), SLOT(editIdTriangle(Vertex)));
	connect(idVertices[1], SIGNAL(valuesChanged(Vertex)), SLOT(editIdTriangle(Vertex)));
	connect(idVertices[2], SIGNAL(valuesChanged(Vertex)), SLOT(editIdTriangle(Vertex)));
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

	fieldId = new QSpinBox(ret);
	fieldId->setRange(0, 65535);
	fieldId->setToolTip(tr("Line of the field in maplist. 32767: unused exit"));
	destinationName = new QLabel(ret);

	// Where the player stands in the destination field
	destinationX = new QSpinBox(ret);
	destinationX->setRange(-32768, 32767);
	destinationX->setToolTip(tr("32767: centre of the triangle"));
	destinationY = new QSpinBox(ret);
	destinationY->setRange(-32768, 32767);
	destinationTriangle = new QSpinBox(ret);
	destinationTriangle->setRange(-32768, 32767);
	destinationTriangle->setToolTip(tr("Triangle of the destination field's walkmesh"));
	destinationFacing = new QSpinBox(ret);
	destinationFacing->setRange(0, 255);
	destinationFacing->setWrapping(true);

	showDestination = new QCheckBox(tr("Show destination"), ret);
	showDestination->setToolTip(tr("Show the destination field in the view, and click its floor to choose where the player arrives"));
	destinationStatus = new QLabel(ret);
	destinationStatus->setTextFormat(Qt::PlainText);
	destinationStatus->setWordWrap(true);

	for (int i = 0; i < 4; ++i) {
		unknownGate1[i] = new QSpinBox(ret);
		unknownGate1[i]->setRange(0, 65535);
	}
	unknownGate2 = new HexLineEdit(ret);

	QLabel *exitInfos = new QLabel(tr("Drag an exit end to move it: dropped near a walkmesh point, it goes on it. "
	                                  "Click near a wall to add an exit there, leading where the selected exit leads. "
	                                  "Delete disables the selected exit."), ret);
	exitInfos->setTextFormat(Qt::PlainText);
	exitInfos->setWordWrap(true);

	QHBoxLayout *destinationLayout = new QHBoxLayout;
	destinationLayout->addWidget(fieldId);
	destinationLayout->addWidget(destinationName, 1);
	destinationLayout->addWidget(showDestination);

	QHBoxLayout *arrivalLayout = new QHBoxLayout;
	arrivalLayout->addWidget(new QLabel(tr("X")));
	arrivalLayout->addWidget(destinationX, 1);
	arrivalLayout->addWidget(new QLabel(tr("Y")));
	arrivalLayout->addWidget(destinationY, 1);
	arrivalLayout->addWidget(new QLabel(tr("Triangle")));
	arrivalLayout->addWidget(destinationTriangle, 1);
	arrivalLayout->addWidget(new QLabel(tr("Facing")));
	arrivalLayout->addWidget(destinationFacing, 1);

	QHBoxLayout *unknownLayout = new QHBoxLayout;
	for (int i = 0; i < 4; ++i) {
		unknownLayout->addWidget(unknownGate1[i], 1);
	}
	unknownLayout->addWidget(unknownGate2, 1);

	QGridLayout *layout = new QGridLayout(ret);
	layout->addWidget(gateList, 0, 0, 8, 1, Qt::AlignLeft);
	layout->addWidget(new QLabel(tr("Exit line:")), 0, 1);
	layout->addWidget(exitPoints[0], 0, 2);
	layout->addWidget(exitPoints[1], 1, 2);
	layout->addWidget(new QLabel(tr("Destination:")), 2, 1);
	layout->addLayout(destinationLayout, 2, 2);
	layout->addWidget(new QLabel(tr("Arrival:")), 3, 1);
	layout->addLayout(arrivalLayout, 3, 2);
	layout->addWidget(new QLabel(tr("Unknown:")), 4, 1);
	layout->addLayout(unknownLayout, 4, 2);
	layout->addWidget(destinationStatus, 5, 1, 1, 2);
	layout->addWidget(exitInfos, 6, 1, 1, 2);
	layout->setRowStretch(7, 1);
	layout->setColumnStretch(2, 1);

	connect(gateList, SIGNAL(currentRowChanged(int)), SLOT(setCurrentGateway(int)));
	connect(exitPoints[0], SIGNAL(valuesChanged(Vertex)), SLOT(editExitPoint(Vertex)));
	connect(exitPoints[1], SIGNAL(valuesChanged(Vertex)), SLOT(editExitPoint(Vertex)));
	connect(fieldId, SIGNAL(valueChanged(int)), SLOT(editFieldId(int)));
	connect(destinationX, SIGNAL(valueChanged(int)), SLOT(editArrival()));
	connect(destinationY, SIGNAL(valueChanged(int)), SLOT(editArrival()));
	connect(destinationTriangle, SIGNAL(valueChanged(int)), SLOT(editArrival()));
	connect(destinationFacing, SIGNAL(valueChanged(int)), SLOT(editArrival()));
	connect(showDestination, SIGNAL(toggled(bool)), SLOT(updateDestinationView()));
	for (int i = 0; i < 4; ++i) {
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

	doorUsed = new QCheckBox(tr("Used"));

	doorId = new QSpinBox(ret);
	doorId->setRange(0, 254);

	QGridLayout *idsLayout = new QGridLayout;
	idsLayout->addWidget(doorUsed, 0, 0);
	idsLayout->addWidget(new QLabel(tr("Door ID:")), 0, 1);
	idsLayout->addWidget(doorId, 0, 2);

	QGridLayout *layout = new QGridLayout(ret);
	layout->addWidget(doorList, 0, 0, 4, 1, Qt::AlignLeft);
	layout->addWidget(new QLabel(tr("Trigger Line Door:")), 0, 1);
	layout->addWidget(doorPosition[0], 0, 2);
	layout->addWidget(doorPosition[1], 1, 2);
	layout->addLayout(idsLayout, 2, 1, 1, 2);
	layout->setRowStretch(3, 1);

	connect(doorList, SIGNAL(currentRowChanged(int)), SLOT(setCurrentDoor(int)));
	connect(doorPosition[0], SIGNAL(valuesChanged(Vertex)), SLOT(editDoorPoint(Vertex)));
	connect(doorPosition[1], SIGNAL(valuesChanged(Vertex)), SLOT(editDoorPoint(Vertex)));
	connect(doorUsed, SIGNAL(toggled(bool)), SLOT(editDoorUsed(bool)));
	connect(doorId, SIGNAL(valueChanged(int)), SLOT(editDoorId(int)));

	return ret;
}

QWidget *WalkmeshWidget::buildCameraRangePage()
{
	QWidget *ret = new QWidget(this);

	rangeList1 = new QListWidget(ret);

	for (int i = 0; i < 8; ++i) {
		rangeList1->addItem(tr("Camera Range %1").arg(i+1));
	}

	rangeList2 = new QListWidget(ret);

	for (int i = 0; i < 2; ++i) {
		rangeList2->addItem(tr("Screen Range %1").arg(i+1));
	}

	for (int i = 0; i < 4; ++i) {
		rangeEdit1[i] = new QSpinBox(ret);
		rangeEdit1[i]->setRange(-32768, 32767);
		rangeEdit2[i] = new QSpinBox(ret);
		rangeEdit2[i]->setRange(-32768, 32767);
	}

	QGridLayout *layout = new QGridLayout(ret);
	layout->addWidget(rangeList1, 0, 0, 3, 1, Qt::AlignLeft);
	layout->addWidget(rangeList2, 3, 0, 3, 1, Qt::AlignLeft);
	layout->addWidget(new QLabel(tr("Top")), 0, 1);
	layout->addWidget(rangeEdit1[0], 0, 2);
	layout->addWidget(new QLabel(tr("Bottom")), 0, 3);
	layout->addWidget(rangeEdit1[1], 0, 4);
	layout->addWidget(new QLabel(tr("Right")), 1, 1);
	layout->addWidget(rangeEdit1[2], 1, 2);
	layout->addWidget(new QLabel(tr("Left")), 1, 3);
	layout->addWidget(rangeEdit1[3], 1, 4);
	layout->addWidget(new QLabel(tr("Top")), 3, 1);
	layout->addWidget(rangeEdit2[0], 3, 2);
	layout->addWidget(new QLabel(tr("Bottom")), 3, 3);
	layout->addWidget(rangeEdit2[1], 3, 4);
	layout->addWidget(new QLabel(tr("Right")), 4, 1);
	layout->addWidget(rangeEdit2[2], 4, 2);
	layout->addWidget(new QLabel(tr("Left")), 4, 3);
	layout->addWidget(rangeEdit2[3], 4, 4);
	layout->setRowStretch(2, 1);
	layout->setRowStretch(5, 1);
	layout->setColumnStretch(1, 1);
	layout->setColumnStretch(2, 1);
	layout->setColumnStretch(3, 1);
	layout->setColumnStretch(4, 1);

	connect(rangeList1, SIGNAL(currentRowChanged(int)), SLOT(setCurrentRange1(int)));
	connect(rangeList2, SIGNAL(currentRowChanged(int)), SLOT(setCurrentRange2(int)));
	for (int i = 0; i < 4; ++i) {
		connect(rangeEdit1[i], SIGNAL(valueChanged(int)), SLOT(editRange(int)));
		connect(rangeEdit2[i], SIGNAL(valueChanged(int)), SLOT(editRange(int)));
	}

	return ret;
}

QWidget *WalkmeshWidget::buildMovieCameraPage()
{
	QWidget *ret = new QWidget(this);

	ListWidget *listWidget = new ListWidget(ret);
	camPlusAction = listWidget->addAction(ListWidget::Add, tr("Add"), this, SLOT(addMovieCameraPosition()));
	camMinusAction = listWidget->addAction(ListWidget::Rem, tr("Remove"), this, SLOT(removeMovieCameraPosition()));
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
	layout->addWidget(new QLabel(tr("Movements orientation:")), 0, 0);
	layout->addWidget(navigation, 0, 1);
	layout->addWidget(navigation2, 0, 2);
	layout->addWidget(new QLabel(tr("Unknown:")), 1, 0);
	layout->addWidget(unknown, 1, 1, 1, 2);
	layout->addWidget(new QLabel(tr("Camera Focus Height on the playable character:")), 2, 0);
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
	if (!isFilled())		return;

	walkmeshGL->clear();
	undoStack->clear();
	viewField = nullptr;
	viewingDestination = false;
	delete looseDestination;
	looseDestination = nullptr;

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
	if (isBuilded()) {
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
		for (int i = 0; i < 3; ++i) {
			idVertices[i]->setReadOnly(ro);
			// idAccess stays read-only: adjacency is computed, never typed
		}
		// GatePage
		exitPoints[0]->setReadOnly(ro);
		exitPoints[1]->setReadOnly(ro);
		destinationX->setReadOnly(ro);
		destinationY->setReadOnly(ro);
		destinationTriangle->setReadOnly(ro);
		destinationFacing->setReadOnly(ro);
		fieldId->setReadOnly(ro);
		for (int i=0 ; i<4 ; ++i) 	unknownGate1[i]->setReadOnly(ro);
		unknownGate2->setReadOnly(ro);
		// DoorPage
		doorId->setReadOnly(ro);
		doorPosition[0]->setReadOnly(ro);
		doorPosition[1]->setReadOnly(ro);
		doorUsed->setDisabled(ro);
		// CamRangePage
		for (int i = 0; i < 4; ++i) {
			rangeEdit1[i]->setReadOnly(ro);
			rangeEdit2[i]->setReadOnly(ro);
		}
		// MoveCamPage
		camToolbar->setDisabled(ro);
		for (int i=0 ; i<4 ; ++i)	camPoints[i]->setReadOnly(ro);
		// MiscPage
		navigation->setReadOnly(ro);
		navigation2->setReadOnly(ro);
		unknown->setReadOnly(ro);
		cameraFocus->setReadOnly(ro);
	}

	PageWidget::setReadOnly(ro);

	if (isBuilded()) {
		updateEditable();
	}
}

void WalkmeshWidget::fill()
{
	if (!isBuilded())	build();
	if (isFilled())		clear();

	if (!hasData() ||
			(!data()->hasCaFile()
			 && !data()->hasIdFile()
			 && !data()->hasInfFile())) return;

	walkmeshGL->fill(data());
	viewField = data();
	viewingDestination = false;
	// Nothing to show or hide when the field was opened without its map/mim
	showBackground->setEnabled(data()->hasBackgroundFile());

	int camCount = 0;

	if (data()->hasCaFile()) {
		camCount = data()->getCaFile()->cameraCount();

		if (camList->count() != camCount) {
			camList->blockSignals(true);
			camList->clear();
			for (int i = 0; i < camCount; ++i) {
				camList->addItem(tr("Camera %1").arg(i));
			}
			camList->blockSignals(false);
		}

		setCurrentCamera(0);
	}
	tabWidget->widget(0)->setEnabled(data()->hasCaFile() && camCount > 0);

	if (data()->hasIdFile()) {
		fillTriangleList();
		idList->setCurrentRow(0);
		setCurrentId(0);
	}
	tabWidget->widget(1)->setEnabled(data()->hasIdFile());

	if (data()->hasInfFile()) {
		gateList->clear();
		fillGatewayList();
		gateList->setCurrentRow(0);
		setCurrentGateway(0);

		doorList->clear();
		for (const Trigger &trigger: data()->getInfFile()->getTriggers(false)) {
			if (trigger.doorID != 0xFF) {
				doorList->addItem(tr("Door %1").arg(trigger.doorID));
			} else {
				doorList->addItem(tr("Unused"));
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

	if (data()->hasMskFile()) {
		frameList->clear();
		int cameraPositionCount = data()->getMskFile()->cameraPositionCount();
		for (int i = 0; i < cameraPositionCount; ++i) {
			frameList->addItem(tr("Position %1").arg(i+1));
		}
		frameList->setCurrentRow(0);
		setCurrentMoviePosition(0);
	}
//	tabWidget->widget(5)->setEnabled(data()->hasMskFile());

	PageWidget::fill();
	updateDestinationView();
}

void WalkmeshWidget::fillTriangleList()
{
	const int triangleCount = data()->getIdFile()->triangleCount();

	if (idList->count() == triangleCount) {
		return; // the items are only numbered, there is nothing else to refresh
	}

	const int row = idList->currentRow();

	idList->blockSignals(true);
	idList->clear();
	for (int i = 0; i < triangleCount; ++i) {
		idList->addItem(tr("Triangle %1").arg(i));
	}
	idList->setCurrentRow(qMin(row, triangleCount - 1));
	idList->blockSignals(false);
}

int WalkmeshWidget::currentCamera() const
{
	if (!data()->hasCaFile())	return 0;

	int camID = camList->currentRow();
	return camID < 0 || camID >= data()->getCaFile()->cameraCount() ? 0 : camID;
}

void WalkmeshWidget::setCurrentCamera(int camID)
{
	if (!data()->hasCaFile() || camID < 0) {
		return;
	}

	bool hasCamera = camID < data()->getCaFile()->cameraCount();

	if (hasCamera) {
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

	if (camList->currentRow() != camID) {
		camList->blockSignals(true);
		camList->setCurrentRow(camID);
		camList->blockSignals(false);
	}
}

void WalkmeshWidget::addCamera()
{
	int row = camList->currentRow();

	if (data()->hasCaFile()) {
		Camera ca;
		if (row < data()->getCaFile()->cameraCount()) {
			ca = data()->getCaFile()->camera(row);
		} else {
			ca = Camera();
		}
		data()->getCaFile()->insertCamera(row+1, ca);
		camList->insertItem(row+1, tr("Camera %1").arg(row+1));
		for (int i = row + 2; i < camList->count(); ++i) {
			camList->item(i)->setText(tr("Camera %1").arg(i));
		}
		camList->setCurrentRow(row+1);
		emit modified();
	}
}

void WalkmeshWidget::removeCamera()
{
	if (data()->getCaFile()->cameraCount() < 2) return;

	int row = camList->currentRow();

	if (row < 0)		return;

	if (data()->hasCaFile() && row < data()->getCaFile()->cameraCount()) {
		data()->getCaFile()->removeCamera(row);
		delete camList->item(row);
		for (int i = row; i < camList->count(); ++i) {
			camList->item(i)->setText(tr("Camera %1").arg(i));
		}
		setCurrentCamera(row);
		emit modified();
	}
}

void WalkmeshWidget::editCaVector(const Vertex &values)
{
	QObject *s = sender();

	if (s == caVectorXEdit)			editCaVector(0, values);
	else if (s == caVectorYEdit)		editCaVector(1, values);
	else if (s == caVectorZEdit)		editCaVector(2, values);
}

void WalkmeshWidget::editCaVector(int id, const Vertex &values)
{
	if (data()->hasCaFile() && data()->getCaFile()->cameraCount() > 0) {
		const int camID = currentCamera();
		Camera cam = data()->getCaFile()->camera(camID);
		Vertex oldV = cam.camera_axis[id];

		if (oldV.x != values.x || oldV.y != values.y || oldV.z != values.z) {
			cam.camera_axis[id] = values;
			data()->getCaFile()->setCamera(camID, cam);
			walkmeshGL->update();
			emit modified();
		}
	}
}

void WalkmeshWidget::editCaPos(double value)
{
	QObject *s = sender();

	if (s == caSpaceXEdit)			editCaPos(0, value);
	else if (s == caSpaceYEdit)		editCaPos(1, value);
	else if (s == caSpaceZEdit)		editCaPos(2, value);
}

void WalkmeshWidget::editCaPos(int id, double value)
{
	if (data()->hasCaFile() && data()->getCaFile()->cameraCount() > 0) {
		const int camID = currentCamera();
		Camera cam = data()->getCaFile()->camera(camID);
		if (cam.camera_position[id] != (qint32)value) {
			cam.camera_position[id] = value;
			data()->getCaFile()->setCamera(camID, cam);
			walkmeshGL->update();
			emit modified();
		}
	}
}

void WalkmeshWidget::editCaZoom(int value)
{
	if (data()->hasCaFile() && data()->getCaFile()->cameraCount() > 0) {
		const int camID = currentCamera();
		Camera cam = data()->getCaFile()->camera(camID);
		if (cam.camera_zoom != value) {
			cam.camera_zoom = value;
			data()->getCaFile()->setCamera(camID, cam);
			walkmeshGL->updatePerspective();
			emit modified();
		}
	}
}

void WalkmeshWidget::setCurrentId(int i)
{
	if (!data()->hasIdFile() || i < 0)	return;

	IdFile *id = data()->getIdFile();
	if (id->triangleCount() <= i)	return;

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
	const int row = idList->currentRow();

	applyWalkmeshEdit(tr("Add triangle"), [&](IdFile *idFile) {
		const bool hasRow = row >= 0 && row < idFile->triangleCount();
		idFile->insertTriangle(row + 1, hasRow ? idFile->triangle(row) : Triangle(),
		                       hasRow ? idFile->access(row) : Access());
		return true;
	});
	idList->setCurrentRow(row + 1);
}

void WalkmeshWidget::removeTriangle()
{
	const int row = idList->currentRow();

	applyWalkmeshEdit(tr("Remove triangle"), [&](IdFile *idFile) {
		if (row < 0 || row >= idFile->triangleCount()) {
			return false;
		}
		idFile->removeTriangles({row});
		return true;
	});
}

void WalkmeshWidget::selectTriangleOfPoint(const Vertex_sr &point)
{
	if (!hasData() || !data()->hasIdFile()) {
		return;
	}

	// Keep the current triangle when it uses the point, so the form does not jump around
	const QList<int> triangleIDs = data()->getIdFile()->trianglesUsingPoint(point);
	if (!triangleIDs.isEmpty() && !triangleIDs.contains(idList->currentRow())) {
		idList->setCurrentRow(triangleIDs.first());
	}
}

// A drag is one undo step: the walkmesh is saved when it starts, and the edit recorded when the
// point is released, however many times it moved in between
void WalkmeshWidget::startPointDrag()
{
	dragBefore = data()->getIdFile()->snapshot();
}

void WalkmeshWidget::dragPoint(const Vertex_sr &from, const Vertex_sr &to)
{
	data()->getIdFile()->movePoint(from, to);
	setCurrentId(idList->currentRow()); // keeps the numbers in the form following the drag
	walkmeshGL->update();
}

void WalkmeshWidget::finishPointDrag()
{
	pushWalkmeshEdit(tr("Move point"), dragBefore);
}

void WalkmeshWidget::addPoint(int triangleID, int side, const Vertex_sr &point)
{
	int newTriangleID = -1;

	applyWalkmeshEdit(tr("Add point"), [&](IdFile *idFile) {
		newTriangleID = idFile->addTriangleOnSide(triangleID, side, point);
		return newTriangleID >= 0;
	});

	if (newTriangleID < 0) {
		walkmeshGL->clearPointSelection(); // the point was on the side's line: nothing to add
	}
}

void WalkmeshWidget::deletePoint(const Vertex_sr &point)
{
	applyWalkmeshEdit(tr("Delete point"), [&](IdFile *idFile) {
		const QList<int> triangleIDs = idFile->trianglesUsingPoint(point);
		if (triangleIDs.isEmpty()) {
			return false;
		}
		idFile->removeTriangles(triangleIDs);
		return true;
	});
}

void WalkmeshWidget::undoWalkmeshEdit()
{
	walkmeshGL->clearPointSelection(); // the selected point may not exist in the restored walkmesh
	undoStack->undo();
}

void WalkmeshWidget::redoWalkmeshEdit()
{
	walkmeshGL->clearPointSelection();
	undoStack->redo();
}

// The same view is shown on every tab: a click edits the walkmesh on the Walkmesh tab, the exits
// on the Exits tab, and nothing elsewhere
void WalkmeshWidget::updateEditable()
{
	WalkmeshGLWidget::EditMode mode = WalkmeshGLWidget::NoEdit;

	if (hasData() && !isReadOnly()) {
		if (viewingDestination) {
			mode = WalkmeshGLWidget::PickArrival;
		} else if (tabWidget->currentWidget() == walkmeshPage && data()->hasIdFile()) {
			mode = WalkmeshGLWidget::EditWalkmesh;
		} else if (tabWidget->currentWidget() == gatewaysPage && data()->hasInfFile()) {
			mode = WalkmeshGLWidget::EditExits;
		}
	}

	walkmeshGL->setEditMode(mode);
}

void WalkmeshWidget::applyWalkmeshEdit(const QString &text, const std::function<bool(IdFile *)> &edit)
{
	if (!hasData() || !data()->hasIdFile()) {
		return;
	}

	const IdFile::Snapshot before = data()->getIdFile()->snapshot();

	if (edit(data()->getIdFile())) {
		pushWalkmeshEdit(text, before);
	}
}

void WalkmeshWidget::pushWalkmeshEdit(const QString &text, const IdFile::Snapshot &before)
{
	IdFile *idFile = data()->getIdFile();

	// Which triangle is next to which follows from the points they share, so it is worked out
	// again after every edit instead of being typed
	idFile->rebuildAccess();

	const IdFile::Snapshot after = idFile->snapshot();
	if (after == before) {
		return;
	}

	undoStack->push(new WalkmeshEditCommand(this, text, before, after)); // push() applies it
}

void WalkmeshWidget::restoreWalkmesh(const IdFile::Snapshot &snapshot)
{
	if (!hasData() || !data()->hasIdFile()) {
		return;
	}

	data()->getIdFile()->restore(snapshot);
	fillTriangleList();

	if (idList->count() > 0) {
		setCurrentId(qMax(0, idList->currentRow()));
	}

	walkmeshGL->update();
	emit modified();
}

void WalkmeshWidget::editIdTriangle(const Vertex &values)
{
	QObject *s = sender();

	if (s == idVertices[0])			editIdTriangle(0, values);
	else if (s == idVertices[1])		editIdTriangle(1, values);
	else if (s == idVertices[2])		editIdTriangle(2, values);
}

void WalkmeshWidget::editIdTriangle(int id, const Vertex &values)
{
	if (!data()->hasIdFile()) {
		return;
	}

	const int triangleID = idList->currentRow();
	if (triangleID < 0 || triangleID >= data()->getIdFile()->triangleCount()) {
		return;
	}

	const Vertex_sr from = data()->getIdFile()->triangle(triangleID).vertices[id],
	                to = IdFile::fromVertex_s(values);

	// Also filters out the form refreshing itself, since setValues() emits valuesChanged()
	if (IdFile::samePoint(from, to)) {
		return;
	}

	// A point typed in moves like a dragged one: every triangle sharing it follows
	applyWalkmeshEdit(tr("Move point"), [&](IdFile *idFile) {
		idFile->movePoint(from, to);
		return true;
	});
}

void WalkmeshWidget::editIdAccess(int value)
{
	QObject *s = sender();

	if (s == idAccess[0])			editIdAccess(0, value);
	else if (s == idAccess[1])		editIdAccess(1, value);
	else if (s == idAccess[2])		editIdAccess(2, value);
}

void WalkmeshWidget::editIdAccess(int id, int value)
{
	if (data()->hasIdFile()) {
		const int triangleID = idList->currentRow();
		if (triangleID > -1 && triangleID < data()->getIdFile()->triangleCount()) {
			Access old = data()->getIdFile()->access(triangleID);
			qint16 oldV = old.a[id];
			if (oldV != value) {
				old.a[id] = value;
				data()->getIdFile()->setAccess(triangleID, old);
				walkmeshGL->update();
				emit modified();
			}
		}
	}
}

void WalkmeshWidget::setCurrentGateway(int id)
{
	if (!hasData() || !data()->hasInfFile() || id < 0 || id >= 12) {
		return;
	}

	const Gateway &gateway = data()->getInfFile()->getGateway(id);

	{
		// Filling the form is not an edit: each box would otherwise record the gateway with
		// the boxes after it still showing the previous exit
		QSignalBlocker b0(exitPoints[0]), b1(exitPoints[1]), b2(fieldId), b3(destinationX), b4(destinationY),
		               b5(destinationTriangle), b6(destinationFacing), b7(unknownGate1[0]), b8(unknownGate1[1]),
		               b9(unknownGate1[2]), b10(unknownGate1[3]), b11(unknownGate2);

		exitPoints[0]->setValues(gateway.exitLine[0]);
		exitPoints[1]->setValues(gateway.exitLine[1]);
		fieldId->setValue(gateway.fieldId);
		destinationX->setValue(gateway.destinationX);
		destinationY->setValue(gateway.destinationY);
		destinationTriangle->setValue(gateway.destinationTriangle);
		destinationFacing->setValue(gateway.destinationFacing);
		for (int i = 0; i < 4; ++i) {
			unknownGate1[i]->setValue(gateway.unknown1[i]);
		}
		unknownGate2->setData(QByteArray((char *)gateway.unknown2, 3));
	}

	destinationName->setText(fieldName(gateway.fieldId));
	walkmeshGL->setSelectedGate(id);
	updateDestinationView();
}

QString WalkmeshWidget::fieldName(int fieldId) const
{
	if (fieldId == GATEWAY_UNUSED) {
		return tr("Unused");
	}

	const QStringList &mapList = fieldArchive != nullptr ? fieldArchive->mapList() : Data::maplist();
	const QString name = mapList.value(fieldId, tr("Unknown field"));

	return fieldId < GATEWAY_FIRST_FIELD ? tr("%1 (world map)").arg(name) : name;
}

// This page's field id, which is its line in maplist
int WalkmeshWidget::currentFieldId() const
{
	const QStringList &mapList = fieldArchive != nullptr ? fieldArchive->mapList() : Data::maplist();

	return mapList.indexOf(data()->name());
}

void WalkmeshWidget::fillGatewayList()
{
	InfFile *inf = data()->getInfFile();

	for (int i = 0; i < 12; ++i) {
		const Gateway &gateway = inf->getGateway(i);
		const QString text = gateway.fieldId == GATEWAY_UNUSED ? tr("Unused")
		                     : QString("%1 (%2)").arg(fieldName(gateway.fieldId)).arg(gateway.fieldId);

		if (i < gateList->count()) {
			gateList->item(i)->setText(text);
		} else {
			gateList->addItem(text);
		}
	}
}

void WalkmeshWidget::setFieldArchive(FieldArchive *fieldArchive)
{
	this->fieldArchive = fieldArchive;
}

/**
 * The field an exit leads to, with what the view needs loaded: from the archive when there is
 * one, otherwise from the folders next to the loose files (see FieldLoose::openNeighbour).
 */
Field *WalkmeshWidget::destinationField(int fieldId)
{
	if (fieldArchive != nullptr) {
		Field *field = fieldArchive->getFieldFromMapId(fieldId);
		if (field != nullptr && field != data()) {
			fieldArchive->openFull(field);
		}
		return field;
	}

	FieldLoose *loose = dynamic_cast<FieldLoose *>(data());
	if (loose == nullptr) {
		return nullptr;
	}

	const QString name = Data::maplist().value(fieldId);
	if (name == loose->name()) {
		return loose;
	}
	if (looseDestination == nullptr || looseDestination->name() != name) {
		delete looseDestination;
		looseDestination = FieldLoose::openNeighbour(loose->paths(), name);
	}

	return looseDestination;
}

void WalkmeshWidget::showInView(Field *field, bool isDestination)
{
	if (field != viewField) {
		Field *previous = viewField;

		walkmeshGL->fill(field);
		// A destination is shown through its first camera; this field through the chosen one
		walkmeshGL->setCurrentFieldCamera(isDestination ? 0 : currentCamera());
		viewField = field;

		// The background of a destination from the archive is big and only needed while it is
		// shown: the archive reads it again next time (MainWindow::fillPage does the same)
		if (fieldArchive != nullptr && previous != nullptr && previous != data() && !previous->isModified()) {
			previous->deleteFile(Field::Background);
		}
	}

	viewingDestination = isDestination;
}

/**
 * With "Show destination" ticked on the Exits tab, the view shows the field the selected exit
 * leads to, where a click places the arrival; otherwise it shows this page's field.
 */
void WalkmeshWidget::updateDestinationView()
{
	if (!hasData() || !isBuilded()) {
		return;
	}

	const int gateId = gateList->currentRow();
	Field *destination = nullptr;
	QString status;

	if (showDestination->isChecked() && tabWidget->currentWidget() == gatewaysPage
	        && data()->hasInfFile() && gateId >= 0 && gateId < 12) {
		const Gateway &gateway = data()->getInfFile()->getGateway(gateId);

		if (gateway.fieldId == GATEWAY_UNUSED) {
			status = tr("This exit is unused.");
		} else if (gateway.fieldId < GATEWAY_FIRST_FIELD) {
			status = tr("This exit leads to the world map.");
		} else {
			destination = destinationField(gateway.fieldId);

			if (destination == nullptr || !destination->hasIdFile()) {
				destination = nullptr;
				status = tr("The walkmesh of %1 was not found: open field.fs, or keep the field folders "
				            "next to each other as in mapdata.").arg(fieldName(gateway.fieldId));
			} else if (gateway.destinationTriangle < 0
			           || gateway.destinationTriangle >= destination->getIdFile()->triangleCount()) {
				status = tr("Triangle %1 does not exist in %2: the player would arrive nowhere. "
				            "Click the floor to choose a place.").arg(gateway.destinationTriangle).arg(fieldName(gateway.fieldId));
			} else {
				status = tr("Click the floor of %1 to choose where the player arrives.").arg(fieldName(gateway.fieldId));
			}

			if (destination != nullptr) {
				walkmeshGL->setArrival(gateway.destinationX, gateway.destinationY, gateway.destinationTriangle);
			}
		}
	}

	if (data()->hasCaFile() || data()->hasIdFile() || data()->hasInfFile()) {
		showInView(destination != nullptr ? destination : data(), destination != nullptr);
	}

	destinationStatus->setText(status);
	updateEditable();
}

void WalkmeshWidget::setCurrentDoor(int id)
{
	if (!data()->hasInfFile() || id < 0)    return;

	InfFile *inf = data()->getInfFile();
	if (12 <= id)    return;

	const Trigger &trigger = inf->getTrigger(id);

	doorPosition[0]->setValues(trigger.trigger_line[0]);
	doorPosition[1]->setValues(trigger.trigger_line[1]);
	if (trigger.doorID == 0xFF) {
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

void WalkmeshWidget::editExitPoint(const Vertex &values)
{
	QObject *s = sender();

	if (s == exitPoints[0])			editExitPoint(0, values);
	else if (s == exitPoints[1])		editExitPoint(1, values);
}

void WalkmeshWidget::editExitPoint(int id, const Vertex &values)
{
	applyGatewayEdit(tr("Move exit"), [&](Gateway &gateway) {
		gateway.exitLine[id] = values;
	});
}

void WalkmeshWidget::editArrival()
{
	applyGatewayEdit(tr("Move arrival"), [&](Gateway &gateway) {
		gateway.destinationX = qint16(destinationX->value());
		gateway.destinationY = qint16(destinationY->value());
		gateway.destinationTriangle = qint16(destinationTriangle->value());
		gateway.destinationFacing = quint8(destinationFacing->value());
	});
}

void WalkmeshWidget::selectExit(int gate)
{
	gateList->setCurrentRow(gate);
}

// Like a walkmesh point, a drag is one undo step
void WalkmeshWidget::startExitDrag()
{
	gatewaysBefore = data()->getInfFile()->getGateways();
}

void WalkmeshWidget::dragExitEnd(int gate, int end, const Vertex &to)
{
	Gateway gateway = data()->getInfFile()->getGateway(gate);
	gateway.exitLine[end] = to;
	data()->getInfFile()->setGateway(gate, gateway);

	if (gate == gateList->currentRow()) {
		QSignalBlocker blocker(exitPoints[end]);
		exitPoints[end]->setValues(to);
	}

	walkmeshGL->update();
}

void WalkmeshWidget::finishExitDrag()
{
	pushGatewaysEdit(tr("Move exit"), gatewaysBefore);
}

void WalkmeshWidget::addExit(const Vertex &a, const Vertex &b)
{
	InfFile *inf = data()->getInfFile();
	const QList<Gateway> before = inf->getGateways();
	const int selected = gateList->currentRow();
	int slot = -1;

	for (int i = 0; i < before.size() && slot < 0; ++i) {
		if (before.at(i).fieldId == GATEWAY_UNUSED) {
			slot = i;
		}
	}

	if (slot < 0) {
		return; // the file holds 12 exits, all used
	}

	Gateway gateway = {};

	// Exits often come in groups leading to the same place (a wide passage cut in several
	// lines), so a new exit leads where the selected one does; otherwise back into this field
	if (selected >= 0 && selected < before.size() && before.at(selected).fieldId != GATEWAY_UNUSED) {
		gateway = before.at(selected);
	} else {
		const int thisField = currentFieldId();
		gateway.fieldId = quint16(thisField >= 0 ? thisField : GATEWAY_FIRST_FIELD);
		gateway.destinationX = gateway.destinationY = 0x7FFF; // the centre of the triangle
		gateway.destinationTriangle = 0;
	}

	gateway.exitLine[0] = a;
	gateway.exitLine[1] = b;
	inf->setGateway(slot, gateway);
	pushGatewaysEdit(tr("Add exit"), before);
	gateList->setCurrentRow(slot);
}

void WalkmeshWidget::deleteExit(int gate)
{
	InfFile *inf = data()->getInfFile();
	const QList<Gateway> before = inf->getGateways();
	Gateway gateway = before.at(gate);

	// The game skips an exit by its field id alone; the rest is kept, so undoing is not needed
	// to get it back: typing the field id again is enough
	gateway.fieldId = GATEWAY_UNUSED;
	inf->setGateway(gate, gateway);
	pushGatewaysEdit(tr("Disable exit"), before);
}

void WalkmeshWidget::startArrivalPick()
{
	gatewaysBefore = data()->getInfFile()->getGateways();
}

void WalkmeshWidget::pickArrival(qint16 x, qint16 y, int triangle)
{
	const int gate = gateList->currentRow();
	Gateway gateway = data()->getInfFile()->getGateway(gate);

	gateway.destinationX = x;
	gateway.destinationY = y;
	gateway.destinationTriangle = qint16(triangle);
	data()->getInfFile()->setGateway(gate, gateway);

	QSignalBlocker b0(destinationX), b1(destinationY), b2(destinationTriangle);
	destinationX->setValue(x);
	destinationY->setValue(y);
	destinationTriangle->setValue(triangle);

	walkmeshGL->setArrival(x, y, triangle);
}

void WalkmeshWidget::finishArrivalPick()
{
	pushGatewaysEdit(tr("Move arrival"), gatewaysBefore);
}

void WalkmeshWidget::applyGatewayEdit(const QString &text, const std::function<void(Gateway &)> &edit)
{
	const int gateId = gateList->currentRow();

	if (!hasData() || !data()->hasInfFile() || gateId < 0 || gateId >= 12) {
		return;
	}

	const QList<Gateway> before = data()->getInfFile()->getGateways();
	Gateway gateway = before.at(gateId);
	edit(gateway);

	if (InfFile::sameGateway(gateway, before.at(gateId))) {
		return;
	}

	data()->getInfFile()->setGateway(gateId, gateway);
	pushGatewaysEdit(text, before);
}

void WalkmeshWidget::pushGatewaysEdit(const QString &text, const QList<Gateway> &before)
{
	const QList<Gateway> after = data()->getInfFile()->getGateways();
	bool changed = false;

	for (int i = 0; i < after.size(); ++i) {
		changed = changed || !InfFile::sameGateway(after.at(i), before.at(i));
	}

	if (changed) {
		undoStack->push(new GatewaysEditCommand(this, text, before, after)); // push() applies it
	}
}

void WalkmeshWidget::restoreGateways(const QList<Gateway> &gateways)
{
	if (!hasData() || !data()->hasInfFile()) {
		return;
	}

	data()->getInfFile()->setGateways(gateways);
	fillGatewayList();
	setCurrentGateway(qMax(0, gateList->currentRow())); // refreshes the form and the destination
	walkmeshGL->update();
	emit modified();
}

void WalkmeshWidget::editDoorPoint(const Vertex &values)
{
	QObject *s = sender();

	if (s == doorPosition[0])			editDoorPoint(0, values);
	else if (s == doorPosition[1])		editDoorPoint(1, values);
}

void WalkmeshWidget::editDoorPoint(int id, const Vertex &values)
{
	if (data()->hasInfFile()) {
		int gateId = gateList->currentRow();
		Trigger old = data()->getInfFile()->getTrigger(gateId);
		Vertex oldVertex = old.trigger_line[id];
		if (oldVertex.x != values.x || oldVertex.y != values.y || oldVertex.z != values.z) {
			old.trigger_line[id] = values;
			data()->getInfFile()->setTrigger(gateId, old);
			walkmeshGL->update();
			emit modified();
		}
	}
}

void WalkmeshWidget::editUnknownGate(int val)
{
	QObject *s = sender();

	if (s == unknownGate1[0])			editUnknownGate(0, val);
	else if (s == unknownGate1[1])		editUnknownGate(1, val);
	else if (s == unknownGate1[2])		editUnknownGate(2, val);
	else if (s == unknownGate1[3])		editUnknownGate(3, val);
}

void WalkmeshWidget::editUnknownGate(int id, int val)
{
	applyGatewayEdit(tr("Edit exit"), [&](Gateway &gateway) {
		gateway.unknown1[id] = quint16(val);
	});
}

void WalkmeshWidget::editUnknownGate(const QByteArray &u)
{
	applyGatewayEdit(tr("Edit exit"), [&](Gateway &gateway) {
		memcpy(gateway.unknown2, u.leftJustified(3, '\0', true).constData(), 3);
	});
}

void WalkmeshWidget::editFieldId(int v)
{
	applyGatewayEdit(tr("Change exit destination"), [&](Gateway &gateway) {
		gateway.fieldId = quint16(v);
	});
}

void WalkmeshWidget::editDoorUsed(bool enable)
{
	doorId->setEnabled(enable);

	editDoorId(enable ? doorId->value() : 0xFF);
}

void WalkmeshWidget::editDoorId(int v)
{
	if (data()->hasInfFile()) {
		int gateId = doorList->currentRow();
		Trigger old = data()->getInfFile()->getTrigger(gateId);
		v = doorUsed->isChecked() ? doorId->value() : 0xFF;
		if (old.doorID != v) {
			old.doorID = v;
			data()->getInfFile()->setTrigger(gateId, old);
			if (v != 0xFF) {
				doorList->currentItem()->setText(tr("Door %1").arg(v));
			} else {
				doorList->currentItem()->setText(tr("Unused"));
			}

			walkmeshGL->update();
			emit modified();
		}
	}
}

void WalkmeshWidget::setCurrentRange1(int id)
{
	if (!data()->hasInfFile() || id < 0)    return;

	InfFile *inf = data()->getInfFile();
	if (8 <= id)    return;

	const Range &range = inf->cameraRange(id);

	rangeEdit1[0]->setValue(range.top);
	rangeEdit1[1]->setValue(range.bottom);
	rangeEdit1[2]->setValue(range.right);
	rangeEdit1[3]->setValue(range.left);
}

void WalkmeshWidget::setCurrentRange2(int id)
{
	if (!data()->hasInfFile() || id < 0)    return;

	InfFile *inf = data()->getInfFile();
	if (2 <= id)    return;

	const Range &range = inf->screenRange(id);

	rangeEdit2[0]->setValue(range.top);
	rangeEdit2[1]->setValue(range.bottom);
	rangeEdit2[2]->setValue(range.right);
	rangeEdit2[3]->setValue(range.left);
}

void WalkmeshWidget::editRange(int v)
{
	QObject *s = sender();

	if (s == rangeEdit1[0])			editRange1(0, v);
	else if (s == rangeEdit1[1])		editRange1(1, v);
	else if (s == rangeEdit1[2])		editRange1(2, v);
	else if (s == rangeEdit1[3])		editRange1(3, v);
	else if (s == rangeEdit2[0])		editRange2(0, v);
	else if (s == rangeEdit2[1])		editRange2(1, v);
	else if (s == rangeEdit2[2])		editRange2(2, v);
	else if (s == rangeEdit2[3])		editRange2(3, v);
}

void WalkmeshWidget::editRange1(int id, int v)
{
	if (data()->hasInfFile()) {
		const int currentRange = rangeList1->currentRow();
		Range old = data()->getInfFile()->cameraRange(currentRange);
		qint16 oldv=0;

		switch (id) {
		case 0:	oldv = old.top;		break;
		case 1:	oldv = old.bottom;	break;
		case 2:	oldv = old.right;	break;
		case 3:	oldv = old.left;	break;
		}

		if (oldv != v) {
			switch (id) {
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
	if (data()->hasInfFile()) {
		const int currentRange = rangeList2->currentRow();
		Range old = data()->getInfFile()->screenRange(currentRange);
		qint16 oldv=0;

		switch (id) {
		case 0:	oldv = old.top;		break;
		case 1:	oldv = old.bottom;	break;
		case 2:	oldv = old.right;	break;
		case 3:	oldv = old.left;	break;
		}

		if (oldv != v) {
			switch (id) {
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
	if (!data()->hasMskFile() || id < 0) {
		setMovieCameraPageEnabled(false);
		return;
	}

	MskFile *msk = data()->getMskFile();
	if (msk->cameraPositionCount() <= id) {
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
	if (!data()->hasMskFile()) {
		data()->addMskFile();
	}

	int row = frameList->currentRow();
	if (row < 0)	row = 0;

	Vertex *camPos = new Vertex[4];

	memset(camPos, 0, sizeof(Vertex) * 4);

	data()->getMskFile()->insertCameraPosition(row+1, camPos);

	frameList->addItem(tr("Position %1").arg(row+1));
	for (int i = row + 1; i < frameList->count(); ++i) {
		frameList->item(i)->setText(tr("Position %1").arg(i+1));
	}

	emit modified();

	frameList->setCurrentRow(row+1);
}

void WalkmeshWidget::removeMovieCameraPosition()
{
	if (!data()->hasMskFile()) {
		data()->addMskFile();
	}

	int row = frameList->currentRow();
	if (row < 0)	row = 0;

	data()->getMskFile()->removeCameraPosition(row);

	delete frameList->item(row);
	for (int i = row; i < frameList->count(); ++i) {
		frameList->item(i)->setText(tr("Position %1").arg(i+1));
	}

	emit modified();
}

void WalkmeshWidget::editNavigation(int v)
{
	if (data()->hasInfFile()) {
		int old = data()->getInfFile()->controlDirection();
		if (old != v) {
			navigation->setValue(v);
			data()->getInfFile()->setControlDirection(v);
			emit modified();
		}
	}
}

void WalkmeshWidget::editUnknown(const QByteArray &u)
{
	if (data()->hasInfFile()) {
		if (u != data()->getInfFile()->unknown()) {
			data()->getInfFile()->setUnknown(u);
			emit modified();
		}
	}
}

void WalkmeshWidget::editCameraFocus(int value)
{
	if (data()->hasInfFile()) {
		if (value != data()->getInfFile()->cameraFocusHeight()) {
			data()->getInfFile()->setCameraFocusHeight(value);
			emit modified();
		}
	}
}

void WalkmeshWidget::focusInEvent(QFocusEvent *e)
{
	if (isBuilded())	walkmeshGL->setFocus();
	QWidget::focusInEvent(e);
}

void WalkmeshWidget::focusOutEvent(QFocusEvent *e)
{
	if (isBuilded())	walkmeshGL->clearFocus();
	QWidget::focusOutEvent(e);
}
