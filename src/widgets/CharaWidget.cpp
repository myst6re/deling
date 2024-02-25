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

CharaWidget::CharaWidget(QWidget *parent)
	: PageWidget(parent), mainModels(0)
{
}

void CharaWidget::build()
{
	if (isBuilded())	return;

	modelList = new QListWidget(this);
	modelList->setFixedWidth(200);
	modelPreview = new CharaPreview(this);
	modelPreview->setMainModels(mainModels);

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(modelList, 0, 0);
	layout->addWidget(modelPreview, 0, 1);
	layout->setColumnStretch(1, 1);
	layout->setContentsMargins(QMargins());

	connect(modelList, SIGNAL(currentRowChanged(int)), SLOT(setModel(int)));

	PageWidget::build();
}

void CharaWidget::clear()
{
	if (!isFilled())	return;

	modelList->blockSignals(true);
	modelList->clear();
	modelPreview->clear();
	modelList->blockSignals(false);

	PageWidget::clear();
}

void CharaWidget::fill()
{
	if (!isBuilded())	build();
	if (isFilled())		clear();

	if (!hasData() || !data()->hasCharaFile())		return;

	modelList->blockSignals(true);
	for (int i = 0; i < data()->getCharaFile()->modelCount(); ++i) {
		QString name = data()->getCharaFile()->model(i).name();
		modelList->addItem(name.isEmpty() ? tr("(Sans nom)") : name);
	}
	modelList->blockSignals(false);

	modelList->setCurrentRow(0);

	PageWidget::fill();
}

void CharaWidget::setMainModels(QHash<int, CharaModel *> *mainModels)
{
	this->mainModels = mainModels;
	if (isBuilded())
		modelPreview->setMainModels(mainModels);
}

void CharaWidget::setModel(int modelID)
{
	if (!hasData() || !data()->hasCharaFile()
			|| modelID >= data()->getCharaFile()->modelCount()) {
		modelPreview->clear();
		return;
	}

	modelPreview->setModel(data()->getCharaFile()->model(modelID));
}
