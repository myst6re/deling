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
#include "SoundWidget.h"

SoundWidget::SoundWidget() :
	PageWidget()
{
}

void SoundWidget::build()
{
	if(isBuilded())	return;

	list1 = new QListWidget(this);
	list1->setFixedWidth(125);

	// Sfx

	sfxGroup = new QWidget(this);
	sfxGroup->hide();

	sfxValue = new QDoubleSpinBox(sfxGroup);
	sfxValue->setDecimals(0);
	sfxValue->setRange(0, (quint32)-1);

	QHBoxLayout *sfxLayout = new QHBoxLayout(sfxGroup);
	sfxLayout->addWidget(new QLabel(tr("Identifiant :")));
	sfxLayout->addWidget(sfxValue);
	sfxLayout->addStretch();
	sfxLayout->setContentsMargins(QMargins());

	// AkaoList

	akaoGroup = new QWidget(this);
	akaoGroup->hide();

	exportButton = new QPushButton(tr("Exporter"));
	importButton = new QPushButton(tr("Importer"));

	QHBoxLayout *akaoLayout = new QHBoxLayout(akaoGroup);
	akaoLayout->addWidget(exportButton);
	akaoLayout->addWidget(importButton);
	akaoLayout->addStretch();
	akaoLayout->setContentsMargins(QMargins());

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(list1, 0, 0, 3, 1, Qt::AlignLeft);
	layout->addWidget(sfxGroup, 0, 1);
	layout->addWidget(akaoGroup, 1, 1);
	layout->setRowStretch(2, 1);
	layout->setContentsMargins(QMargins());

	connect(list1, SIGNAL(currentRowChanged(int)), SLOT(setCurrentSound(int)));
	connect(sfxValue, SIGNAL(valueChanged(double)), SLOT(editSfxValue(double)));
	connect(exportButton, SIGNAL(clicked()), SLOT(exportAkao()));
	connect(importButton, SIGNAL(clicked()), SLOT(importAkao()));

	PageWidget::build();
}

void SoundWidget::clear()
{
	if(!isFilled())		return;

	list1->clear();
	sfxValue->clear();

	PageWidget::clear();
}

void SoundWidget::setReadOnly(bool ro)
{
	if(isBuilded()) {
		sfxValue->setReadOnly(ro);
		importButton->setDisabled(ro);
	}

	PageWidget::setReadOnly(ro);
}

void SoundWidget::fill()
{
	if(!isBuilded())	build();
	if(isFilled())		clear();

	if(!hasData()) return;

	if(data()->hasSfxFile()) {
		fillList(data()->getSfxFile()->valueCount());
	}
	else if(data()->hasAkaoListFile()) {
		fillList(data()->getAkaoListFile()->akaoCount());
	}

	sfxGroup->setVisible(data()->hasSfxFile());
	akaoGroup->setVisible(data()->hasAkaoListFile());

	PageWidget::fill();
}

void SoundWidget::fillList(int count)
{
	list1->blockSignals(true);
	for(int i=0 ; i<count ; ++i) {
		list1->addItem(tr("Son %1").arg(i));
	}
	list1->blockSignals(false);

	list1->setCurrentRow(0);
}

void SoundWidget::setCurrentSound(int id)
{
	if(!hasData() || id<0)	return;

	if(data()->hasSfxFile()) {
		if(id < data()->getSfxFile()->valueCount()) {
			sfxValue->setValue(data()->getSfxFile()->value(id));
		}
	}
}

void SoundWidget::editSfxValue(double v)
{
	if(data()->hasSfxFile()) {
		int id = list1->currentRow();
		if(id >= 0 && id < data()->getSfxFile()->valueCount()) {
			if(v != data()->getSfxFile()->value(id)) {
				data()->getSfxFile()->setValue(id, v);
				emit modified();
			}
		}
	}
}

void SoundWidget::exportAkao()
{
	if(data()->hasAkaoListFile()) {
		int id = list1->currentRow();
		if(id >= 0 && id < data()->getAkaoListFile()->akaoCount()) {

			QString path = QFileDialog::getSaveFileName(this, tr("Exporter son"), tr("son%1").arg(id), tr("AKAO file (*.akao)"));
			if(path.isNull())		return;

			QFile f(path);
			if(f.open(QIODevice::WriteOnly)) {
				f.write(data()->getAkaoListFile()->akao(id));
				f.close();
			} else {
				QMessageBox::warning(this, tr("Erreur"), tr("Impossible d'exporter le son (%1).").arg(f.errorString()));
			}
		}
	}
}

void SoundWidget::importAkao()
{
	if(data()->hasAkaoListFile()) {
		int id = list1->currentRow();
		if(id >= 0 && id < data()->getAkaoListFile()->akaoCount()) {

			QString path = QFileDialog::getOpenFileName(this, tr("Importer son"), QString(), tr("AKAO file (*.akao)"));
			if(path.isNull())		return;

			QFile f(path);
			if(f.open(QIODevice::ReadOnly)) {
				if(!data()->getAkaoListFile()->setAkao(id, f.readAll())) {
					QMessageBox::warning(this, tr("Erreur"), tr("Fichier invalide."));
				}
				f.close();
			} else {
				QMessageBox::warning(this, tr("Erreur"), tr("Impossible d'importer le son (%1).").arg(f.errorString()));
			}
		}
	}
}
