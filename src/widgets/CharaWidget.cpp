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
#include "CharaWidget.h"
#include "Field.h"

CharaWidget::CharaWidget(QWidget *parent)
	: PageWidget(parent), _mainModels(nullptr)
{
}

void CharaWidget::build()
{
	if (isBuilded())	return;

	_modelList = new QListWidget(this);
	_modelList->setFixedWidth(200);
	_modelPreview = new CharaPreview(this);
	_loadingTypeChoice = new QComboBox(this);
	_loadingTypeChoice->addItem(tr("Main Character model"), CharaModel::External);
	_loadingTypeChoice->addItem(tr("Local model"), CharaModel::Local);
	_loadingTypeChoice->addItem(tr("Shared texture local model"), CharaModel::LocalSharedTexture);
	_loadingTypeChoice->setDisabled(true);
	_localTextureLoaderIdEdit = new QSpinBox(this);
	_localTextureLoaderIdEdit->setRange(0, 0xFF);
	_defaultLightColorEdit = new ColorDisplay(this);
	_lightColorEdit = new ColorDisplay(this);
	_modelScale = new QSpinBox(this);
	_modelScale->setRange(0, 0xFF);
	_modelId = new QSpinBox(this);
	_modelId->setRange(0, 999);
	_modelId->setPrefix(QString("model\\main_chr\\d00"));
	_modelId->setSuffix(QString(".mch"));

	QVBoxLayout *listLayout = new QVBoxLayout;
	listLayout->addWidget(_modelList, 1);
	listLayout->addWidget(_modelPreview);
	listLayout->setContentsMargins(QMargins());

	_formLayout = new QFormLayout;
	_formLayout->addRow(tr("Color modifier"), _lightColorEdit);
	_formLayout->addRow(tr("Type"), _loadingTypeChoice);
	_formLayout->addRow(tr("Local ID pointer"), _localTextureLoaderIdEdit);
	_formLayout->addRow(tr("Main Model ID"), _modelId);
	_formLayout->addRow(tr("Scale"), _modelScale);
	_formLayout->setContentsMargins(QMargins());

	QHBoxLayout *mainLayout = new QHBoxLayout;
	mainLayout->addLayout(listLayout);
	mainLayout->addLayout(_formLayout, 1);
	mainLayout->setContentsMargins(QMargins());

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(new QLabel(tr("Default color modifier")), 0, 0);
	layout->addWidget(_defaultLightColorEdit, 0, 1);
	layout->setColumnStretch(1, 1);
	layout->addLayout(mainLayout, 1, 0, 1, 2);
	layout->setContentsMargins(QMargins());

	connect(_modelList, SIGNAL(currentRowChanged(int)), SLOT(setModel(int)));
	connect(_defaultLightColorEdit, SIGNAL(colorEdited(int,QRgb)), SLOT(setDefaultLightColor()));
	connect(_lightColorEdit, SIGNAL(colorEdited(int,QRgb)), SLOT(setModelLightColor()));
	connect(_loadingTypeChoice, SIGNAL(currentIndexChanged(int)), SLOT(setModelLoadingType(int)));
	connect(_modelScale, SIGNAL(valueChanged(int)), SLOT(setScaleValue(int)));
	connect(_modelId, SIGNAL(valueChanged(int)), SLOT(setExternalModelId(int)));
	connect(_localTextureLoaderIdEdit, SIGNAL(valueChanged(int)), SLOT(setLocalTextureLoaderId(int)));

	PageWidget::build();

	_defaultLightColorEdit->setFixedHeight(_loadingTypeChoice->height());
	_lightColorEdit->setFixedHeight(_loadingTypeChoice->height());
}

void CharaWidget::clear()
{
	if (!isFilled())	return;

	_modelList->blockSignals(true);
	_modelList->clear();
	_modelPreview->clear();
	_modelList->blockSignals(false);

	PageWidget::clear();
}

void CharaWidget::fill()
{
	if (!isBuilded())	build();
	if (isFilled())		clear();

	if (!hasData() || !data()->hasCharaFile())		return;

	_modelList->blockSignals(true);
	for (int i = 0; i < data()->getCharaFile()->modelCount(); ++i) {
		QString name = data()->getCharaFile()->model(i).name();
		_modelList->addItem(name.isEmpty() ? tr("(No Name)") : name);
	}
	_modelList->blockSignals(false);

	_defaultLightColorEdit->setColors(QList<uint>() << data()->getCharaFile()->defaultLightColor());

	_modelList->setCurrentRow(0);

	PageWidget::fill();
}

void CharaWidget::setMainModels(QHash<int, CharaModel> *mainModels)
{
	_mainModels = mainModels;
}

void CharaWidget::setModel(int modelID)
{
	if (!hasData() || !data()->hasCharaFile()
			|| modelID >= data()->getCharaFile()->modelCount()) {
		_modelPreview->clear();
		return;
	}

	const CharaModel &model = data()->getCharaFile()->model(modelID);
	_modelPreview->setModel(modelID, data()->getCharaFile(), _mainModels);
	_loadingTypeChoice->setCurrentIndex(_loadingTypeChoice->findData(model.loadingType()));
	setModelLoadingType(_loadingTypeChoice->currentIndex());
	_lightColorEdit->setColors(QList<uint>() << (0xFF000000 | model.lightColor()));
	_localTextureLoaderIdEdit->setValue(model.sharedTextureModelId());
	_modelScale->setValue(model.scale());
	_modelId->setValue(model.id());
}

void CharaWidget::setDefaultLightColor()
{
	if (data()->getCharaFile()->defaultLightColor() != _defaultLightColorEdit->colors().first()) {
		data()->getCharaFile()->setDefaultLightColor(_defaultLightColorEdit->colors().first());

		emit modified();

		setModel(_modelList->currentRow());
	}
}

void CharaWidget::setModelLightColor()
{
	int modelID = _modelList->currentRow();

	if (!hasData() || !data()->hasCharaFile()
			|| modelID >= data()->getCharaFile()->modelCount()) {
		return;
	}

	CharaModel &model = data()->getCharaFile()->model(modelID);

	if (model.lightColor() != _lightColorEdit->colors().first()) {
		model.setLightColor(_lightColorEdit->colors().first());

		data()->getCharaFile()->setModified(true);

		emit modified();

		_modelPreview->setModel(modelID, data()->getCharaFile(), _mainModels);
	}
}

void CharaWidget::setModelLoadingType(int index)
{
	int modelID = _modelList->currentRow();

	if (!hasData() || !data()->hasCharaFile()
			|| modelID >= data()->getCharaFile()->modelCount()) {
		return;
	}

	CharaModel &model = data()->getCharaFile()->model(modelID);
	int selectedLoadingType = _loadingTypeChoice->itemData(index).toInt();
	if (selectedLoadingType == CharaModel::External) {
		_formLayout->setRowVisible(_localTextureLoaderIdEdit, false);
		_formLayout->setRowVisible(_modelScale, true);
		_formLayout->setRowVisible(_modelId, true);
	} else if (selectedLoadingType == CharaModel::LocalSharedTexture) {
		_formLayout->setRowVisible(_localTextureLoaderIdEdit, true);
		_formLayout->setRowVisible(_modelScale, false);
		_formLayout->setRowVisible(_modelId, false);
	} else {
		_formLayout->setRowVisible(_localTextureLoaderIdEdit, false);
		_formLayout->setRowVisible(_modelScale, false);
		_formLayout->setRowVisible(_modelId, false);
	}

	if (model.loadingType() != selectedLoadingType) {
		model.setLoadingType(CharaModel::ModelLoadingType(selectedLoadingType));
		data()->getCharaFile()->setModified(true);

		emit modified();
	}
}

void CharaWidget::setScaleValue(int value)
{
	int modelID = _modelList->currentRow();

	if (!hasData() || !data()->hasCharaFile()
			|| modelID >= data()->getCharaFile()->modelCount()) {
		return;
	}

	CharaModel &model = data()->getCharaFile()->model(modelID);
	if (model.loadingType() == CharaModel::External && model.scale() != value) {
		model.setScale(value);
		data()->getCharaFile()->setModified(true);

		emit modified();
	}
}

void CharaWidget::setExternalModelId(int value)
{
	QString t = QString("model\\main_chr\\d%1").arg(value, 3, 10, QChar('0'));
	_modelId->setPrefix(t.left(t.size() - QString::number(value).size()));

	int modelID = _modelList->currentRow();

	if (!hasData() || !data()->hasCharaFile()
			|| modelID >= data()->getCharaFile()->modelCount()) {
		return;
	}

	CharaModel &model = data()->getCharaFile()->model(modelID);
	QString newName = QString("d%1").arg(value, 3, 10, QChar('0'));
	if (model.loadingType() == CharaModel::External && newName != model.name()) {
		model.setName(newName);
		data()->getCharaFile()->setModified(true);

		emit modified();

		if (_modelList->currentItem() != nullptr) {
			_modelList->currentItem()->setText(model.name());
		}
		_modelPreview->setModel(modelID, data()->getCharaFile(), _mainModels);
	}
}

void CharaWidget::setLocalTextureLoaderId(int value)
{
	int modelID = _modelList->currentRow();

	if (!hasData() || !data()->hasCharaFile()
			|| modelID >= data()->getCharaFile()->modelCount()) {
		return;
	}

	if (value < data()->getCharaFile()->modelCount()) {
		_localTextureLoaderIdEdit->setSuffix(QString(" (%1)").arg(data()->getCharaFile()->model(value).name()));
	} else {
		_localTextureLoaderIdEdit->setSuffix(QString());
	}

	CharaModel &model = data()->getCharaFile()->model(modelID);
	if (model.loadingType() == CharaModel::LocalSharedTexture && value != model.sharedTextureModelId() && value != modelID && value < data()->getCharaFile()->modelCount()) {
		model.setSharedTextureModelId(value);
		data()->getCharaFile()->setModified(true);

		emit modified();

		_modelPreview->setModel(modelID, data()->getCharaFile(), _mainModels);
	}
}
