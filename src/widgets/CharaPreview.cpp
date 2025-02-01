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
#include "CharaPreview.h"

CharaPreview::CharaPreview(QWidget *parent) :
	BGPreview2(parent), _mainModels(nullptr)
{
	setAutoFillBackground(true);
	setAlignment(Qt::AlignCenter);
	QPalette pal = palette();
	pal.setColor(QPalette::Active, QPalette::Window, Qt::black);
	pal.setColor(QPalette::Inactive, QPalette::Window, Qt::black);
	pal.setColor(QPalette::Disabled, QPalette::Window, pal.color(QPalette::Disabled, QPalette::WindowText));
	setPalette(pal);
}

void CharaPreview::fill(const QPixmap &background)
{
	if (background.width()>width() || background.height()>height()) {
		if (background.height()==height())
			setPixmap(background.scaled(background.width()*width()/background.height(), height(), Qt::KeepAspectRatio));
		else
			setPixmap(background.scaled(width(), height(), Qt::KeepAspectRatio));
	}
	else
		setPixmap(background);
}

void CharaPreview::setMainModels(QHash<int, CharaModel *> *mainModels)
{
	_mainModels = mainModels;
}

void CharaPreview::setModel(const CharaModel &model)
{
	const CharaModel *m;

	if (model.isEmpty() && _mainModels && _mainModels->contains(model.id())) {
		m = _mainModels->value(model.id());
	} else {
		m = &model;
	}

	if (!m->isEmpty()) {
		setName(QString("tex%1").arg(m->id()));
		setPixmap(QPixmap::fromImage(m->texture(0).image()));
	} else {
		clear();
	}
}
