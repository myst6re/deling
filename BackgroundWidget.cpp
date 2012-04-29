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
#include "BackgroundWidget.h"

BackgroundWidget::BackgroundWidget(QWidget *parent)
	: PageWidget(parent)
{
//	build();
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

	parametersWidget = new QComboBox(this);
	parametersWidget->setMinimumWidth(150);
	parametersWidget->setEnabled(false);
	statesWidget = new QListWidget(this);
	statesWidget->setFixedWidth(150);
	layersWidget = new QListWidget(this);
	layersWidget->setFixedWidth(150);

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(scrollArea, 0, 0, 3, 1);
	layout->addWidget(parametersWidget, 0, 1);
	layout->addWidget(statesWidget, 1, 1);
	layout->addWidget(layersWidget, 2, 1);
	layout->setContentsMargins(QMargins());

	connect(parametersWidget, SIGNAL(currentIndexChanged(int)), SLOT(parameterChanged(int)));
	connect(statesWidget, SIGNAL(itemChanged(QListWidgetItem*)), SLOT(enableState(QListWidgetItem*)));
	connect(layersWidget, SIGNAL(itemChanged(QListWidgetItem*)), SLOT(enableLayer(QListWidgetItem*)));
//	connect(statesWidget, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(switchItem(QListWidgetItem*)));
//	connect(layersWidget, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(switchItem(QListWidgetItem*)));

	PageWidget::build();
}

void BackgroundWidget::clear()
{
	if(!isBuilded())	return;

	parametersWidget->clear();
	statesWidget->clear();
	layersWidget->clear();
	allparams.clear();
	params.clear();
	layers.clear();
	map.clear();
	mim.clear();
	image->clear();

	PageWidget::clear();
}

//bool BackgroundWidget::filled()
//{
//	return isEnabled();
//}

void BackgroundWidget::parameterChanged(int index)
{
	int parameter = parametersWidget->itemData(index).toInt();
	QList<quint8> states = allparams.values(parameter);
	qSort(states);
	QListWidgetItem *item;

	statesWidget->clear();
	foreach(quint8 state, states)
	{
		item = new QListWidgetItem(tr("État %1").arg(state));
		item->setData(Qt::UserRole, state);
		item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		item->setCheckState(params.contains(parameter, state) ? Qt::Checked : Qt::Unchecked);
		statesWidget->addItem(item);
	}
}

void BackgroundWidget::enableState(QListWidgetItem *item)
{
	bool enabled = item->data(Qt::CheckStateRole).toBool();
	int parameter = parametersWidget->itemData(parametersWidget->currentIndex()).toInt(), state = item->data(Qt::UserRole).toInt();

	if(enabled)
		params.insert(parameter, state);
	else
		params.remove(parameter, state);

	fill();
}

void BackgroundWidget::enableLayer(QListWidgetItem *item)
{
	bool enabled = item->data(Qt::CheckStateRole).toBool();
	int layer = item->data(Qt::UserRole).toInt();

	layers.insert(layer, enabled);

	fill();
}

/*void BackgroundWidget::switchItem(QListWidgetItem *item)
{
	item->setCheckState(item->checkState()==Qt::Unchecked ? Qt::Checked : Qt::Unchecked);
}*/

QPixmap BackgroundWidget::generate(const QString &name, const QByteArray &map, const QByteArray &mim)
{
	PageWidget::fill();

	this->map = map;
	this->mim = mim;
	fillWidgets();
	fill();
	image->setName(name);
	return *(image->pixmap());
}

QPixmap BackgroundWidget::error()
{
	PageWidget::fill();

	image->setPixmap(FF8Image::errorPixmap());
	setEnabled(false);
	return *(image->pixmap());
}

void BackgroundWidget::fillWidgets()
{
	int mimSize = mim.size(), mapSize = map.size(), tilePos=0;
	const char *constMapData = map.constData();
	allparams.clear();
	params.clear();
	layers.clear();

	if(mimSize == 401408) {
//		Tile1 tile1;
//		int sizeOfTile = mapSize-map.lastIndexOf("\xff\x7f");

//		if(mapSize%sizeOfTile != 0)	return;

//		while(tilePos+15 < mapSize)
//		{
//			memcpy(&tile1, &constMapData[tilePos], sizeOfTile);
//			if(tile1.X == 0x7fff) break;
//			tilePos += sizeOfTile;

//			if(tile1.parameter!=255 && !allparams.contains(tile1.parameter, tile1.state))
//			{
//				allparams.insert(tile1.parameter, tile1.state);
//				if(tile1.state==0)
//					params.insert(tile1.parameter, tile1.state);
//			}
//		}
		statesWidget->setEnabled(false);
		layersWidget->setEnabled(false);
	} else if(mimSize == 438272) {
		Tile2 tile2;

		while(tilePos+15 < mapSize)
		{
			memcpy(&tile2, &constMapData[tilePos], 16);
			if(tile2.X == 0x7fff) break;

			if(tile2.parameter!=255 && !allparams.contains(tile2.parameter, tile2.state))
			{
				allparams.insert(tile2.parameter, tile2.state);
				if(tile2.state==0)
					params.insert(tile2.parameter, tile2.state);
			}
			layers.insert(tile2.layerID, true);
			tilePos += 16;
		}
		statesWidget->setEnabled(true);
		layersWidget->setEnabled(true);
	}

	parametersWidget->clear();
	QList<quint8> parameters = allparams.uniqueKeys();
	foreach(quint8 parameter, parameters)
		parametersWidget->addItem(tr("Paramètre %1").arg(parameter), parameter);

	QList<quint8> layerIDs = layers.keys();
	QListWidgetItem *item;

	layersWidget->clear();
	foreach(quint8 layerID, layerIDs)
	{
		item = new QListWidgetItem(tr("Couche %1").arg(layerID));
		item->setData(Qt::UserRole, layerID);
		item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		item->setCheckState(Qt::Checked);
		layersWidget->addItem(item);
	}

	parametersWidget->setEnabled(parametersWidget->count()>1);

	setEnabled(true);
}

void BackgroundWidget::fill()
{
	int mimSize = mim.size();

	if(mimSize == 401408)
		image->setPixmap(FF8Image::type1(map, mim/*, params*/));
	else if(mimSize == 438272)
		image->setPixmap(FF8Image::type2(map, mim, params, layers));
	else
		image->setPixmap(FF8Image::errorPixmap());
}
