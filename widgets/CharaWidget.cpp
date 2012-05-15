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
#include "CharaWidget.h"

CharaWidget::CharaWidget(QWidget *parent)
	: PageWidget(parent), mainModels(0)
{
}

void CharaWidget::build()
{
	if(isBuilded())	return;

	modelList = new QListWidget(this);
	modelList->setFixedWidth(200);
	texLabel = new BGPreview2(this);

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(modelList, 0, 0, Qt::AlignLeft);
	layout->addWidget(texLabel, 0, 1);
	layout->setContentsMargins(QMargins());

	connect(modelList, SIGNAL(currentRowChanged(int)), SLOT(setModel(int)));

	PageWidget::build();
}

void CharaWidget::clear()
{
	if(!isFilled())	return;

	modelList->blockSignals(true);
	modelList->clear();
	texLabel->clear();
	modelList->blockSignals(false);

	PageWidget::clear();
}

void CharaWidget::fill()
{
	if(!isBuilded())	build();
	if(isFilled())		clear();

	if(!hasData() || !data()->hasCharaFile())		return;

	modelList->blockSignals(true);
	for(int i=0 ; i<data()->getCharaFile()->modelCount() ; ++i) {
		modelList->addItem(data()->getCharaFile()->model(i)->name());
	}
	modelList->blockSignals(false);

	PageWidget::fill();
}

void CharaWidget::setMainModels(QHash<int, CharaModel *> *mainModels)
{
	this->mainModels = mainModels;
}

void CharaWidget::setModel(int modelID)
{
	if(!hasData() || !data()->hasCharaFile()
			|| modelID >= data()->getCharaFile()->modelCount()) {
		texLabel->clear();
		return;
	}

	CharaModel *model = data()->getCharaFile()->model(modelID);
	if(model->isEmpty() && mainModels && mainModels->contains(model->id())) {
		model = mainModels->value(model->id());
	}

	if(!model->isEmpty()) {
		texLabel->setName(QString("tex%1").arg(modelID));
		texLabel->setPixmap(QPixmap::fromImage(model->texture(0).image()));
	} else {
		texLabel->clear();
	}
}
