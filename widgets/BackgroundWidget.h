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
#ifndef BACKGROUNDWIDGET_H
#define BACKGROUNDWIDGET_H

#include <QtWidgets>
#include "widgets/PageWidget.h"
#include "BGPreview.h"
#include "BGPreview2.h"
#include "FF8Image.h"

class BackgroundWidget : public PageWidget
{
	Q_OBJECT
public:
	BackgroundWidget(QWidget *parent=0);
	void clear();
	void setReadOnly(bool ro);
	void fill();
	inline QString tabName() const { return tr("Décors"); }
private slots:
	void parameterChanged(int index);
	void enableState(QListWidgetItem *item);
	void enableLayer(QListWidgetItem *item);
//	void switchItem(QListWidgetItem *item);
	void setHideBack(bool);
	void setPage(int index);
	void setCurrentTile(int index);
	void setTileX();
	void setTileY();
	void setTileZ();
	void setTileTexID();
	void setTileBlend();
	void setTilePalID();
	void setTileDraw();
	void setTileDepth();
	void setTileSrcX();
	void setTileSrcY();
	void setTileLayerID();
	void setTileBlendType();
	void setTileParameter();
	void setTileState();
private:
	void updateBackground();
	void build();

	BGPreview2 *image;
	QComboBox *parametersWidget;
	QListWidget *statesWidget, *layersWidget;
	QCheckBox *hideBack;
	QSpinBox *currentTile, *tileDepth;
	QSpinBox *tileX, *tileY, *tileZ;
	QSpinBox *tileTexID, *tileBlend, *tilePalID;
	QCheckBox *tileDraw;
	QSpinBox *tileSrcX, *tileSrcY, *tileLayerID;
	QSpinBox *tileBlendType, *tileParameter, *tileState;
	QStackedLayout *stackedLayout;
};

#endif // BACKGROUNDWIDGET_H
