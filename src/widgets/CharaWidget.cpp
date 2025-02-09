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
	_modelPreview->setMainModels(_mainModels);

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(_modelList, 0, 0);
	layout->addWidget(_modelPreview, 0, 1);
	layout->setColumnStretch(1, 1);
	layout->setContentsMargins(QMargins());

	connect(_modelList, SIGNAL(currentRowChanged(int)), SLOT(setModel(int)));

	PageWidget::build();
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
		_modelList->addItem(name.isEmpty() ? tr("(Sans nom)") : name);
	}
	_modelList->blockSignals(false);

	_modelList->setCurrentRow(0);

	PageWidget::fill();
}

void CharaWidget::setMainModels(QHash<int, CharaModel *> *mainModels)
{
	_mainModels = mainModels;
	if (isBuilded()) {
		_modelPreview->setMainModels(mainModels);
	}
}

void CharaWidget::setModel(int modelID)
{
	if (!hasData() || !data()->hasCharaFile()
			|| modelID >= data()->getCharaFile()->modelCount()) {
		_modelPreview->clear();
		return;
	}

	_modelPreview->setModel(data()->getCharaFile()->model(modelID));
}
