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
#include "widgets/BackgroundWidget.h"

BackgroundWidget::BackgroundWidget(QWidget *parent)
	: PageWidget(parent)
{
}

void BackgroundWidget::build()
{
	if(isBuilded())	return;

	QScrollArea *scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);

	QPalette pal = scrollArea->palette();
	pal.setColor(QPalette::Active, QPalette::Window, Qt::black);
	pal.setColor(QPalette::Inactive, QPalette::Window, Qt::black);
	pal.setColor(QPalette::Disabled, QPalette::Window, pal.color(QPalette::Disabled, QPalette::Text));
	scrollArea->setPalette(pal);

	image = new BGPreview2();
	image->setAlignment(Qt::AlignCenter);
	scrollArea->setWidget(image);

	QWidget *page1 = new QWidget(this);

	parametersWidget = new QComboBox(page1);
	parametersWidget->setMinimumWidth(150);
	parametersWidget->setEnabled(false);
	statesWidget = new QListWidget(page1);
	statesWidget->setFixedWidth(150);
	layersWidget = new QListWidget(page1);
	layersWidget->setFixedWidth(150);

	hideBack = new QCheckBox(tr("Cacher background"), page1);
	hideBack->setChecked(false);

	QVBoxLayout *layout1 = new QVBoxLayout(page1);
	layout1->addWidget(parametersWidget);
	layout1->addWidget(statesWidget);
	layout1->addWidget(layersWidget);
	layout1->addWidget(hideBack);
	layout1->setContentsMargins(QMargins());

	QWidget *page2 = new QWidget(this);
	currentTile = new QSpinBox(page2);
	currentTile->setMinimum(0);
	tileX = new QSpinBox(page2);
	tileX->setRange(-32768, 32767);
	tileY = new QSpinBox(page2);
	tileY->setRange(-32768, 32767);
	tileZ = new QSpinBox(page2);
	tileZ->setRange(0, 65535);
	tileTexID = new QSpinBox(page2);
	tileTexID->setRange(0, 16);
	tilePalID = new QSpinBox(page2);
	tilePalID->setRange(0, 16);
	tileBlend = new QSpinBox(page2);
	tileBlend->setRange(0, 3);
	tileDraw = new QCheckBox(page2);
	tileDepth = new QSpinBox(page2);
	tileDepth->setRange(0, 3);
	tileSrcX = new QSpinBox(page2);
	tileSrcX->setRange(0, 255);
	tileSrcY = new QSpinBox(page2);
	tileSrcY->setRange(0, 255);
	tileLayerID = new QSpinBox(page2);
	tileLayerID->setRange(0, 255);
	tileBlendType = new QSpinBox(page2);
	tileBlendType->setRange(0, 255);
	tileParameter = new QSpinBox(page2);
	tileParameter->setRange(0, 255);
	tileState = new QSpinBox(page2);
	tileState->setRange(0, 255);

	QFormLayout *layout2 = new QFormLayout(page2);
	layout2->addRow(tr("Tile ID"), currentTile);
	layout2->addRow(new QFrame(page2));
	layout2->addRow(tr("Destination X"), tileX);
	layout2->addRow(tr("Destination Y"), tileY);
	layout2->addRow(tr("Destination Z"), tileZ);
	layout2->addRow(tr("Source X"), tileSrcX);
	layout2->addRow(tr("Source Y"), tileSrcY);
	layout2->addRow(tr("Texture"), tileTexID);
	layout2->addRow(tr("Palette"), tilePalID);
	layout2->addRow(tr("Transparence"), tileBlend);
	layout2->addRow(tr("Dessiner"), tileDraw);
	layout2->addRow(tr("Type transparence"), tileBlendType);
	layout2->addRow(tr("Type couleur"), tileDepth);
	layout2->addRow(tr("Couche"), tileLayerID);
	layout2->addRow(tr("Paramètre"), tileParameter);
	layout2->addRow(tr("État"), tileState);
	layout2->setContentsMargins(QMargins());

	stackedLayout = new QStackedLayout;
	stackedLayout->addWidget(page1);
	stackedLayout->addWidget(page2);

	QTabBar *tabBar = new QTabBar(this);
	tabBar->addTab(tr("Paramètres"));
	tabBar->addTab(tr("Tuiles"));

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(scrollArea, 0, 0, 2, 1);
	layout->addWidget(tabBar, 0, 1);
	layout->addLayout(stackedLayout, 1, 1);
	layout->setColumnStretch(0, 1);
	layout->setContentsMargins(QMargins());

	connect(parametersWidget, SIGNAL(currentIndexChanged(int)), SLOT(parameterChanged(int)));
	connect(statesWidget, SIGNAL(itemChanged(QListWidgetItem*)), SLOT(enableState(QListWidgetItem*)));
	connect(layersWidget, SIGNAL(itemChanged(QListWidgetItem*)), SLOT(enableLayer(QListWidgetItem*)));
//	connect(statesWidget, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(switchItem(QListWidgetItem*)));
//	connect(layersWidget, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(switchItem(QListWidgetItem*)));
	connect(hideBack, SIGNAL(toggled(bool)), SLOT(setHideBack(bool)));
	connect(tabBar, SIGNAL(currentChanged(int)), stackedLayout, SLOT(setCurrentIndex(int)));
	connect(tabBar, SIGNAL(currentChanged(int)), this, SLOT(setPage(int)));
	connect(currentTile, SIGNAL(valueChanged(int)), this, SLOT(setCurrentTile(int)));
	connect(tileX, SIGNAL(editingFinished()), this, SLOT(setTileX()));
	connect(tileY, SIGNAL(editingFinished()), this, SLOT(setTileY()));
	connect(tileZ, SIGNAL(editingFinished()), this, SLOT(setTileZ()));
	connect(tileSrcX, SIGNAL(editingFinished()), this, SLOT(setTileSrcX()));
	connect(tileSrcY, SIGNAL(editingFinished()), this, SLOT(setTileSrcY()));
	connect(tileTexID, SIGNAL(editingFinished()), this, SLOT(setTileTexID()));
	connect(tilePalID, SIGNAL(editingFinished()), this, SLOT(setTilePalID()));
	connect(tileBlend, SIGNAL(editingFinished()), this, SLOT(setTileBlend()));
	connect(tileDraw, SIGNAL(released()), this, SLOT(setTileDraw()));
	connect(tileBlendType, SIGNAL(editingFinished()), this, SLOT(setTileBlendType()));
	connect(tileDepth, SIGNAL(editingFinished()), this, SLOT(setTileDepth()));
	connect(tileLayerID, SIGNAL(editingFinished()), this, SLOT(setTileLayerID()));
	connect(tileParameter, SIGNAL(editingFinished()), this, SLOT(setTileParameter()));
	connect(tileState, SIGNAL(editingFinished()), this, SLOT(setTileState()));

	PageWidget::build();
}

void BackgroundWidget::clear()
{
	if(!isFilled())	return;

	parametersWidget->clear();
	statesWidget->clear();
	layersWidget->clear();
	image->clear();
	currentTile->clear();
	tileX->clear();
	tileY->clear();
	tileZ->clear();
	tileTexID->clear();
	tilePalID->clear();
	tileBlend->clear();
	tileDraw->setChecked(false);
	tileDepth->clear();
	tileSrcX->clear();
	tileSrcY->clear();
	tileLayerID->clear();
	tileBlendType->clear();
	tileParameter->clear();
	tileState->clear();

	PageWidget::clear();
}


void BackgroundWidget::setReadOnly(bool ro)
{
	PageWidget::setReadOnly(ro);

	if(!isBuilded())	return;

	tileX->setReadOnly(ro);
	tileY->setReadOnly(ro);
	tileZ->setReadOnly(ro);
	tileTexID->setReadOnly(ro);
	tilePalID->setReadOnly(ro);
	tileBlend->setReadOnly(ro);
	tileDraw->setDisabled(ro);
	tileDepth->setReadOnly(ro);
	tileSrcX->setReadOnly(ro);
	tileSrcY->setReadOnly(ro);
	tileLayerID->setReadOnly(ro);
	tileBlendType->setReadOnly(ro);
	tileParameter->setReadOnly(ro);
	tileState->setReadOnly(ro);
}

void BackgroundWidget::parameterChanged(int index)
{
	if(!hasData() || !data()->hasBackgroundFile())		return;

	int parameter = parametersWidget->itemData(index).toInt();
	QList<quint8> states = data()->getBackgroundFile()->allparams.values(parameter);
	qSort(states);
	QListWidgetItem *item;

	statesWidget->clear();
	foreach(quint8 state, states)
	{
		item = new QListWidgetItem(tr("État %1").arg(state));
		item->setData(Qt::UserRole, state);
		item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		item->setCheckState(data()->getBackgroundFile()->params.contains(parameter, state) ? Qt::Checked : Qt::Unchecked);
		statesWidget->addItem(item);
	}
}

void BackgroundWidget::enableState(QListWidgetItem *item)
{
	if(!hasData() || !data()->hasBackgroundFile())		return;

	bool enabled = item->data(Qt::CheckStateRole).toBool();
	int parameter = parametersWidget->itemData(parametersWidget->currentIndex()).toInt(), state = item->data(Qt::UserRole).toInt();

	if(enabled)
		data()->getBackgroundFile()->params.insert(parameter, state);
	else
		data()->getBackgroundFile()->params.remove(parameter, state);

	updateBackground();
}

void BackgroundWidget::enableLayer(QListWidgetItem *item)
{
	if(!hasData() || !data()->hasBackgroundFile())		return;

	bool enabled = item->data(Qt::CheckStateRole).toBool();
	int layer = item->data(Qt::UserRole).toInt();

	data()->getBackgroundFile()->layers.insert(layer, enabled);

	updateBackground();
}

/*void BackgroundWidget::switchItem(QListWidgetItem *item)
{
	item->setCheckState(item->checkState()==Qt::Unchecked ? Qt::Checked : Qt::Unchecked);
}*/

void BackgroundWidget::setHideBack(bool)
{
	if(!hasData() || !data()->hasBackgroundFile())		return;

	updateBackground();
}

void BackgroundWidget::updateBackground()
{
	image->setPixmap(QPixmap::fromImage(data()->getBackgroundFile()->background(hideBack->isChecked())));
}

void BackgroundWidget::setPage(int index)
{
	setCurrentTile(index != 1 ? -1 : 0);
}

void BackgroundWidget::setCurrentTile(int index)
{
	if(!hasData() || !data()->hasBackgroundFile()) {
		return;
	}

	data()->getBackgroundFile()->setVisibleTile(index);
	if (index >= 0) {
		Tile t = data()->getBackgroundFile()->tile(quint16(index));
		tileX->setValue(t.X);
		tileY->setValue(t.Y);
		tileZ->setValue(t.Z);
		tileTexID->setValue(t.texID);
		tileDraw->setChecked(t.draw);
		tileBlend->setValue(t.blend);
		tileDepth->setValue(t.depth);
		tilePalID->setValue(t.palID);
		tileSrcX->setValue(t.srcX);
		tileSrcY->setValue(t.srcY);
		tileLayerID->setValue(t.layerID);
		tileBlendType->setValue(t.blendType);
		tileParameter->setValue(t.parameter);
		tileState->setValue(t.state);
	}
	updateBackground();
}

void BackgroundWidget::setTileX()
{
	if(!hasData() || !data()->hasBackgroundFile()) {
		return;
	}

	int index = currentTile->value();

	if (index < 0) {
		return;
	}

	Tile t = data()->getBackgroundFile()->tile(quint16(index));
	if (t.X != quint8(tileX->value())) {
		t.X = qint16(tileX->value());
		data()->getBackgroundFile()->setTile(quint16(index), t);

		emit modified();
	}
}

void BackgroundWidget::setTileY()
{
	if(!hasData() || !data()->hasBackgroundFile()) {
		return;
	}

	int index = currentTile->value();

	if (index < 0) {
		return;
	}

	Tile t = data()->getBackgroundFile()->tile(quint16(index));
	if (t.Y != quint8(tileY->value())) {
		t.Y = qint16(tileY->value());
		data()->getBackgroundFile()->setTile(quint16(index), t);

		emit modified();
	}
}

void BackgroundWidget::setTileZ()
{
	if(!hasData() || !data()->hasBackgroundFile()) {
		return;
	}

	int index = currentTile->value();

	if (index < 0) {
		return;
	}

	Tile t = data()->getBackgroundFile()->tile(quint16(index));
	if (t.Z != quint8(tileZ->value())) {
		t.Z = quint16(tileZ->value());
		data()->getBackgroundFile()->setTile(quint16(index), t);

		emit modified();
	}
}

void BackgroundWidget::setTileTexID()
{
	if(!hasData() || !data()->hasBackgroundFile()) {
		return;
	}

	int index = currentTile->value();

	if (index < 0) {
		return;
	}

	Tile t = data()->getBackgroundFile()->tile(quint16(index));
	if (t.texID != quint8(tileTexID->value())) {
		t.texID = quint8(tileTexID->value());
		data()->getBackgroundFile()->setTile(quint16(index), t);

		emit modified();
	}
}

void BackgroundWidget::setTileBlend()
{
	if(!hasData() || !data()->hasBackgroundFile()) {
		return;
	}

	int index = currentTile->value();

	if (index < 0) {
		return;
	}

	Tile t = data()->getBackgroundFile()->tile(quint16(index));
	if (t.blend != quint8(tileBlend->value())) {
		t.blend = quint8(tileBlend->value());
		data()->getBackgroundFile()->setTile(quint16(index), t);

		emit modified();
	}
}

void BackgroundWidget::setTilePalID()
{
	if(!hasData() || !data()->hasBackgroundFile()) {
		return;
	}

	int index = currentTile->value();

	if (index < 0) {
		return;
	}

	Tile t = data()->getBackgroundFile()->tile(quint16(index));
	if (t.palID != quint8(tilePalID->value())) {
		t.palID = quint8(tilePalID->value());
		data()->getBackgroundFile()->setTile(quint16(index), t);

		emit modified();
	}
}

void BackgroundWidget::setTileDraw()
{
	if(!hasData() || !data()->hasBackgroundFile()) {
		return;
	}

	int index = currentTile->value();

	if (index < 0) {
		return;
	}

	Tile t = data()->getBackgroundFile()->tile(quint16(index));
	if (t.draw != tileDraw->isChecked()) {
		t.draw = tileDraw->isChecked();
		data()->getBackgroundFile()->setTile(quint16(index), t);

		emit modified();
	}
}

void BackgroundWidget::setTileDepth()
{
	if(!hasData() || !data()->hasBackgroundFile()) {
		return;
	}

	int index = currentTile->value();

	if (index < 0) {
		return;
	}

	Tile t = data()->getBackgroundFile()->tile(quint16(index));
	if (t.depth != tileDepth->value()) {
		t.depth = quint8(tileDepth->value());
		data()->getBackgroundFile()->setTile(quint16(index), t);

		emit modified();
	}
}

void BackgroundWidget::setTileSrcX()
{
	if(!hasData() || !data()->hasBackgroundFile()) {
		return;
	}

	int index = currentTile->value();

	if (index < 0) {
		return;
	}

	Tile t = data()->getBackgroundFile()->tile(quint16(index));
	if (t.srcX != quint16(tileSrcX->value())) {
		t.srcX = quint16(tileSrcX->value());
		data()->getBackgroundFile()->setTile(quint16(index), t);

		emit modified();
	}
}

void BackgroundWidget::setTileSrcY()
{
	if(!hasData() || !data()->hasBackgroundFile()) {
		return;
	}

	int index = currentTile->value();

	if (index < 0) {
		return;
	}

	Tile t = data()->getBackgroundFile()->tile(quint16(index));
	if (t.srcY != quint16(tileSrcY->value())) {
		t.srcY = quint16(tileSrcY->value());
		data()->getBackgroundFile()->setTile(quint16(index), t);

		emit modified();
	}
}

void BackgroundWidget::setTileLayerID()
{
	if(!hasData() || !data()->hasBackgroundFile()) {
		return;
	}

	int index = currentTile->value();

	if (index < 0) {
		return;
	}

	Tile t = data()->getBackgroundFile()->tile(quint16(index));
	if (t.layerID != quint8(tileLayerID->value())) {
		t.layerID = quint8(tileLayerID->value());
		data()->getBackgroundFile()->setTile(quint16(index), t);

		emit modified();
	}
}

void BackgroundWidget::setTileBlendType()
{
	if(!hasData() || !data()->hasBackgroundFile()) {
		return;
	}

	int index = currentTile->value();

	if (index < 0) {
		return;
	}

	Tile t = data()->getBackgroundFile()->tile(quint16(index));
	if (t.blendType != quint8(tileBlendType->value())) {
		t.blendType = quint8(tileBlendType->value());
		data()->getBackgroundFile()->setTile(quint16(index), t);

		emit modified();
	}
}

void BackgroundWidget::setTileParameter()
{
	if(!hasData() || !data()->hasBackgroundFile()) {
		return;
	}

	int index = currentTile->value();

	if (index < 0) {
		return;
	}

	Tile t = data()->getBackgroundFile()->tile(quint16(index));
	if (t.parameter != quint8(tileParameter->value())) {
		t.parameter = quint8(tileParameter->value());
		data()->getBackgroundFile()->setTile(quint16(index), t);

		emit modified();
	}
}

void BackgroundWidget::setTileState()
{
	if(!hasData() || !data()->hasBackgroundFile()) {
		return;
	}

	int index = currentTile->value();

	if (index < 0) {
		return;
	}

	Tile t = data()->getBackgroundFile()->tile(quint16(index));
	if (t.state != quint8(tileState->value())) {
		t.state = quint8(tileState->value());
		data()->getBackgroundFile()->setTile(quint16(index), t);

		emit modified();
	}
}

void BackgroundWidget::fill()
{
	if(!isBuilded())	build();
	if(isFilled())		clear();

	if(!hasData() || !data()->hasBackgroundFile())		return;

	image->setName(data()->name());
	updateBackground();

	BackgroundFile *bgFile = data()->getBackgroundFile();

	parametersWidget->clear();
	QList<quint8> parameters = bgFile->allparams.uniqueKeys();
	foreach(quint8 parameter, parameters)
		parametersWidget->addItem(tr("Paramètre %1").arg(parameter), parameter);

	QList<quint8> layerIDs = bgFile->layers.keys();
	QListWidgetItem *item;

	layersWidget->clear();
	foreach(quint8 layerID, layerIDs)
	{
		item = new QListWidgetItem(tr("Couche %1").arg(layerID));
		item->setData(Qt::UserRole, layerID);
		item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		item->setCheckState(bgFile->layers.value(layerID, false) ? Qt::Checked : Qt::Unchecked);
		layersWidget->addItem(item);
	}

	layersWidget->setEnabled(layersWidget->count()>0);
	parametersWidget->setEnabled(parametersWidget->count()>1);
	statesWidget->setEnabled(parametersWidget->count()>0);
	hideBack->setEnabled(parametersWidget->count()>0);
	hideBack->setChecked(false);

	if (stackedLayout->currentIndex() == 1) {
		currentTile->setValue(0);
		setCurrentTile(0);
	} else {
		currentTile->clear();
	}
	currentTile->setMaximum(bgFile->tiles().size() - 1);

	PageWidget::fill();
}
