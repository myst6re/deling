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
#include "widgets/MiscWidget.h"

MiscWidget::MiscWidget(QWidget *parent)
	: PageWidget(parent)
{
}

void MiscWidget::build()
{
	if(isBuilded())	return;

	QLabel *nameLbl = new QLabel(tr("Nom :"), this);
	nameEdit = new QLineEdit(this);
    nameEdit->setMaxLength(8);
	QLabel *pmpLbl = new QLabel(tr("PMP :"), this);
	pmpEdit = new QLineEdit(this);
	QLabel *pmdLbl = new QLabel(tr("PMD :"), this);
	pmdEdit = new QLineEdit(this);
	QLabel *pvpLbl = new QLabel(tr("PVP (1) :"), this);
	pvpEdit = new QDoubleSpinBox(this);
	pvpEdit->setDecimals(0);
	pvpEdit->setRange(0, (quint32)-1);
	QLabel *pvpLbl2 = new QLabel(tr("PVP (2) :"), this);
	pvpEdit2 = new QSpinBox(this);
	pvpEdit2->setRange(0, (quint16)-1);

	pmpGroup = new QGroupBox(tr("PMP : données particules"), this);
	pmpView = new QLabel(pmpGroup);
	pmpPaletteView = new QLabel(pmpGroup);
	pmpPaletteBox = new QComboBox(pmpGroup);
	for(int i=0 ; i<16 ; ++i)
		pmpPaletteBox->addItem(tr("Palette %1").arg(i));
	pmpDephBox = new QComboBox(pmpGroup);
	pmpDephBox->addItem("4", 4);
	pmpDephBox->addItem("8", 8);
	pmpDephBox->addItem("16", 16);
	QGridLayout *pmpLayout = new QGridLayout(pmpGroup);
	pmpLayout->addWidget(pmpPaletteBox, 0, 0);
	pmpLayout->addWidget(pmpDephBox, 0, 1);
	pmpLayout->addWidget(pmpView, 1, 0);
	pmpLayout->addWidget(pmpPaletteView, 1, 1);

	connect(nameEdit, SIGNAL(textEdited(QString)), SLOT(editName(QString)));
	connect(pmpEdit, SIGNAL(textEdited(QString)), SLOT(editPmp(QString)));
	connect(pmdEdit, SIGNAL(textEdited(QString)), SLOT(editPmd(QString)));
	connect(pvpEdit, SIGNAL(valueChanged(double)), SLOT(editPvp(double)));
	connect(pvpEdit2, SIGNAL(valueChanged(int)), SLOT(editPvp2(int)));
	connect(pmpPaletteBox, SIGNAL(currentIndexChanged(int)), SLOT(updatePmpView()));
	connect(pmpDephBox, SIGNAL(currentIndexChanged(int)), SLOT(updatePmpView()));

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(nameLbl, 0, 0);
	layout->addWidget(nameEdit, 0, 1);
	layout->addWidget(pmpLbl, 1, 0);
	layout->addWidget(pmpEdit, 1, 1);
	layout->addWidget(pmdLbl, 2, 0);
	layout->addWidget(pmdEdit, 2, 1);
	layout->addWidget(pvpLbl, 3, 0);
	layout->addWidget(pvpEdit, 3, 1);
	layout->addWidget(pvpLbl2, 4, 0);
	layout->addWidget(pvpEdit2, 4, 1);
	layout->addWidget(pmpGroup, 5, 0, 1, 2);
	layout->setRowStretch(6, 1);
	layout->setColumnStretch(1, 1);
	layout->setContentsMargins(QMargins());

	PageWidget::build();
}

void MiscWidget::clear()
{
	if(!isFilled())	return;

	nameEdit->clear();
	pmpEdit->clear();
	pmpView->clear();
	pmpPaletteView->clear();
	pmdEdit->clear();
	pvpEdit->clear();
	pvpEdit2->clear();

	PageWidget::clear();
}

void MiscWidget::setReadOnly(bool readOnly)
{
	if(isBuilded()) {
		nameEdit->setReadOnly(readOnly);
		pmpEdit->setReadOnly(readOnly);
		pmdEdit->setReadOnly(readOnly);
		pvpEdit->setReadOnly(readOnly);
		pvpEdit2->setReadOnly(readOnly);
	}

	PageWidget::setReadOnly(readOnly);
}

void MiscWidget::fill()
{
	if(!isBuilded())	build();
	if(isFilled())		clear();

	if(!hasData())	return;

	if(data()->hasInfFile()) {
		nameEdit->setText(data()->getInfFile()->getMapName());
		pvpEdit2->setValue(data()->getInfFile()->pvp());
	}
	nameEdit->setEnabled(data()->hasInfFile());
	pvpEdit2->setEnabled(data()->hasInfFile());

	if(data()->hasPmpFile()) {
		pmpEdit->setText(data()->getPmpFile()->getPmpData().toHex());
		QPixmap pal = QPixmap::fromImage(data()->getPmpFile()->palette());
		if(!pal.isNull()) {
			pal = pal.scaledToWidth(256);
		}
		pmpPaletteView->setPixmap(pal);
	}
	pmpEdit->setEnabled(data()->hasPmpFile());
	updatePmpView();

	if(data()->hasPmdFile()) {
		pmdEdit->setText(data()->getPmdFile()->getPmdData().toHex());
	}
	pmdEdit->setEnabled(data()->hasPmdFile());

	if(data()->hasPvpFile()) {
		pvpEdit->setValue(data()->getPvpFile()->value());
	}
	pvpEdit->setEnabled(data()->hasPvpFile());

	PageWidget::fill();
}

void MiscWidget::editName(const QString &name)
{
	if(data()->hasInfFile()) {
		data()->getInfFile()->setMapName(name);
		emit modified();
	}
}

void MiscWidget::editPmp(const QString &pmp)
{
	if(data()->hasPmpFile()) {
		data()->getPmpFile()->setPmpData(
					QByteArray::fromHex(pmp.toLatin1())
					.leftJustified(data()->getPmpFile()->getPmpData().size(), '\x00', true));
		emit modified();
		pmpEdit->setText(data()->getPmpFile()->getPmpData().toHex());
	}
}

void MiscWidget::editPmd(const QString &pmd)
{
	if(data()->hasPmdFile()) {
		data()->getPmdFile()->setPmdData(
					QByteArray::fromHex(pmd.toLatin1())
					.leftJustified(data()->getPmdFile()->getPmdData().size(), '\x00', true));
		emit modified();
		pmdEdit->setText(data()->getPmdFile()->getPmdData().toHex());
	}
}

void MiscWidget::editPvp(double value)
{
	if(!data()->hasPvpFile()) {
		data()->addPvpFile();
	}
	if(data()->getPvpFile()->value() != value) {
		data()->getPvpFile()->setValue(value);
		emit modified();
	}
}

void MiscWidget::editPvp2(int value)
{
	if(data()->hasInfFile()) {
		if(data()->getInfFile()->pvp() != value) {
			data()->getInfFile()->setPvp(value);
			emit modified();
		}
	}
}

void MiscWidget::updatePmpView()
{
	if(hasData() && data()->hasPmpFile()) {
		int palID = pmpPaletteBox->currentIndex(), deph=pmpDephBox->itemData(pmpDephBox->currentIndex()).toInt();

		pmpView->setPixmap(QPixmap::fromImage(data()->getPmpFile()->image(deph, palID)));
	}
	pmpGroup->setEnabled(pmpView->pixmap() && !pmpView->pixmap()->isNull());
}
