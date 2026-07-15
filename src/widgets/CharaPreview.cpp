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

#include "files/CharaOneFile.h"

CharaPreview::CharaPreview(QWidget *parent) :
	BGPreview2(parent)
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

void CharaPreview::setModel(int modelID, CharaOneFile *charaOne, QHash<int, CharaModel> *mainModels)
{
	const CharaModel &model = charaOne->model(modelID);
	const CharaModel *m;

	if (model.loadingType() == CharaModel::External && mainModels != nullptr && mainModels->contains(model.id())) {
		m = &(*mainModels)[model.id()];
	} else if (model.loadingType() == CharaModel::LocalSharedTexture && model.sharedTextureModelId() < charaOne->modelCount()) {
		m = &charaOne->model(model.sharedTextureModelId());
	} else {
		m = &model;
	}

	if (!m->textures().isEmpty()) {
		setName(QString("%1-tex%2").arg(m->name()).arg(m->id()));
		setPixmap(QPixmap::fromImage(setImageColors(m->texture(0).image(), model.lightColor())));
	} else {
		clear();
	}
}

QImage CharaPreview::setImageColors(const QImage &image, quint32 colorModifier)
{
	QList<uint> colorTable = image.colorTable();
	const int redModifier = qRed(colorModifier) - 128,
	    greenModifier = qGreen(colorModifier) - 128,
	    blueModifier = qBlue(colorModifier) - 128;

	for (qsizetype i = 0; i < colorTable.size(); ++i) {
		const uint color = colorTable.at(i);
		if (color != 0) {
			int r = qRed(color) + redModifier, g = qGreen(color) + greenModifier, b = qBlue(color) + blueModifier;
			if (r < 0) {
				r = 0;
			}
			if (r > 255) {
				r = 255;
			}
			if (g < 0) {
				g = 0;
			}
			if (g > 255) {
				g = 255;
			}
			if (b < 0) {
				b = 0;
			}
			if (b > 255) {
				b = 255;
			}
			colorTable[i] = qRgba(r, g, b, qAlpha(color));
		}
	}

	QImage ret = image;
	ret.setColorTable(colorTable);

	return ret;
}
