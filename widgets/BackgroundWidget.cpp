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
	: PageWidget(parent), backgroundFile(NULL)
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
	if(!isFilled())	return;

	parametersWidget->clear();
	statesWidget->clear();
	layersWidget->clear();
	image->clear();

	PageWidget::clear();
}

void BackgroundWidget::parameterChanged(int index)
{
	if(backgroundFile == NULL)		return;

	int parameter = parametersWidget->itemData(index).toInt();
	QList<quint8> states = backgroundFile->allparams.values(parameter);
	qSort(states);
	QListWidgetItem *item;

	statesWidget->clear();
	foreach(quint8 state, states)
	{
		item = new QListWidgetItem(tr("État %1").arg(state));
		item->setData(Qt::UserRole, state);
		item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		item->setCheckState(backgroundFile->params.contains(parameter, state) ? Qt::Checked : Qt::Unchecked);
		statesWidget->addItem(item);
	}
}

void BackgroundWidget::enableState(QListWidgetItem *item)
{
	if(backgroundFile == NULL)		return;

	bool enabled = item->data(Qt::CheckStateRole).toBool();
	int parameter = parametersWidget->itemData(parametersWidget->currentIndex()).toInt(), state = item->data(Qt::UserRole).toInt();

	if(enabled)
		backgroundFile->params.insert(parameter, state);
	else
		backgroundFile->params.remove(parameter, state);

	image->setPixmap(backgroundFile->background());
}

void BackgroundWidget::enableLayer(QListWidgetItem *item)
{
	if(backgroundFile == NULL)		return;

	bool enabled = item->data(Qt::CheckStateRole).toBool();
	int layer = item->data(Qt::UserRole).toInt();

	backgroundFile->layers.insert(layer, enabled);

	image->setPixmap(backgroundFile->background());
}

/*void BackgroundWidget::switchItem(QListWidgetItem *item)
{
	item->setCheckState(item->checkState()==Qt::Unchecked ? Qt::Checked : Qt::Unchecked);
}*/

void BackgroundWidget::setData(Field *field)
{
	if(this->backgroundFile != field->getBackgroundFile()) {
		clear();
		this->backgroundFile = field->getBackgroundFile();
	}

	if(!isBuilded())	build();

	image->setName(field->name());
}

void BackgroundWidget::cleanData()
{
	backgroundFile = NULL;
}

void BackgroundWidget::fill()
{
	if(!isBuilded())	build();
	if(isFilled())		clear();

	if(backgroundFile == NULL)		return;

	image->setPixmap(backgroundFile->background());

	parametersWidget->clear();
	QList<quint8> parameters = backgroundFile->allparams.uniqueKeys();
	foreach(quint8 parameter, parameters)
		parametersWidget->addItem(tr("Paramètre %1").arg(parameter), parameter);

	QList<quint8> layerIDs = backgroundFile->layers.keys();
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

	layersWidget->setEnabled(layersWidget->count()>0);
	parametersWidget->setEnabled(parametersWidget->count()>1);
	statesWidget->setEnabled(parametersWidget->count()>0);

	PageWidget::fill();
}
