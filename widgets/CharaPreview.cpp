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
#include "CharaPreview.h"

CharaPreview::CharaPreview(QWidget *parent) :
	BGPreview(parent), mainModels(0)
{
	createContents();
}

void CharaPreview::createContents()
{
	label = new BGPreview2();
}

void CharaPreview::setMainModels(QHash<int, CharaModel *> *mainModels)
{
	this->mainModels = mainModels;
}

void CharaPreview::setModel(CharaModel *model)
{
	if(!model)		return;

	if(model->isEmpty() && mainModels && mainModels->contains(model->id())) {
		model = mainModels->value(model->id());
	}

	if(!model->isEmpty()) {
		createContents();
		((BGPreview2 *)label)->setName(QString("tex%1").arg(model->id()));
		label->setPixmap(QPixmap::fromImage(model->texture(0).image()));
		setWidget(label);
	} else {
		clear();
	}
}
