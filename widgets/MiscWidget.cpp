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
	QLabel *pmpLbl = new QLabel(tr("PMP :"), this);
	pmpEdit = new QLineEdit(this);
	QLabel *pmdLbl = new QLabel(tr("PMD :"), this);
	pmdEdit = new QLineEdit(this);
	QLabel *pvpLbl = new QLabel(tr("PVP :"), this);
	pvpEdit = new QLineEdit(this);
	gateList = new QListWidget(this);

	caVectorX1Edit = new QSpinBox(this);
	caVectorX1Edit->setRange(-32768, 32767);
	caVectorX2Edit = new QSpinBox(this);
	caVectorX2Edit->setRange(-32768, 32767);
	caVectorX3Edit = new QSpinBox(this);
	caVectorX3Edit->setRange(-32768, 32767);

	caVectorY1Edit = new QSpinBox(this);
	caVectorY1Edit->setRange(-32768, 32767);
	caVectorY2Edit = new QSpinBox(this);
	caVectorY2Edit->setRange(-32768, 32767);
	caVectorY3Edit = new QSpinBox(this);
	caVectorY3Edit->setRange(-32768, 32767);

	caVectorZ1Edit = new QSpinBox(this);
	caVectorZ1Edit->setRange(-32768, 32767);
	caVectorZ2Edit = new QSpinBox(this);
	caVectorZ2Edit->setRange(-32768, 32767);
	caVectorZ3Edit = new QSpinBox(this);
	caVectorZ3Edit->setRange(-32768, 32767);

	caSpaceXEdit = new QDoubleSpinBox(this);
	qreal maxInt = qPow(2,31);
	caSpaceXEdit->setRange(-maxInt, maxInt);
	caSpaceXEdit->setDecimals(0);
	caSpaceYEdit = new QDoubleSpinBox(this);
	caSpaceYEdit->setRange(-maxInt, maxInt);
	caSpaceYEdit->setDecimals(0);
	caSpaceZEdit = new QDoubleSpinBox(this);
	caSpaceZEdit->setRange(-maxInt, maxInt);
	caSpaceZEdit->setDecimals(0);

	connect(caVectorX1Edit, SIGNAL(valueChanged(int)), SLOT(editCaVector(int)));
	connect(caVectorX2Edit, SIGNAL(valueChanged(int)), SLOT(editCaVector(int)));
	connect(caVectorX3Edit, SIGNAL(valueChanged(int)), SLOT(editCaVector(int)));
	connect(caVectorY1Edit, SIGNAL(valueChanged(int)), SLOT(editCaVector(int)));
	connect(caVectorY2Edit, SIGNAL(valueChanged(int)), SLOT(editCaVector(int)));
	connect(caVectorY3Edit, SIGNAL(valueChanged(int)), SLOT(editCaVector(int)));
	connect(caVectorZ1Edit, SIGNAL(valueChanged(int)), SLOT(editCaVector(int)));
	connect(caVectorZ2Edit, SIGNAL(valueChanged(int)), SLOT(editCaVector(int)));
	connect(caVectorZ3Edit, SIGNAL(valueChanged(int)), SLOT(editCaVector(int)));

	connect(caSpaceXEdit, SIGNAL(valueChanged(double)), SLOT(editCaPos(double)));
	connect(caSpaceYEdit, SIGNAL(valueChanged(double)), SLOT(editCaPos(double)));
	connect(caSpaceZEdit, SIGNAL(valueChanged(double)), SLOT(editCaPos(double)));

	QGridLayout *caLayout = new QGridLayout;
	caLayout->addWidget(caVectorX1Edit, 0, 0);
	caLayout->addWidget(caVectorX2Edit, 0, 1);
	caLayout->addWidget(caVectorX3Edit, 0, 2);
	caLayout->addWidget(caVectorY1Edit, 1, 0);
	caLayout->addWidget(caVectorY2Edit, 1, 1);
	caLayout->addWidget(caVectorY3Edit, 1, 2);
	caLayout->addWidget(caVectorZ1Edit, 2, 0);
	caLayout->addWidget(caVectorZ2Edit, 2, 1);
	caLayout->addWidget(caVectorZ3Edit, 2, 2);
	caLayout->addWidget(caSpaceXEdit, 3, 0);
	caLayout->addWidget(caSpaceYEdit, 3, 1);
	caLayout->addWidget(caSpaceZEdit, 3, 2);

	connect(nameEdit, SIGNAL(textEdited(QString)), SLOT(editName(QString)));
	connect(pmpEdit, SIGNAL(textEdited(QString)), SLOT(editPmp(QString)));
	connect(pmdEdit, SIGNAL(textEdited(QString)), SLOT(editPmd(QString)));
	connect(pvpEdit, SIGNAL(textEdited(QString)), SLOT(editPvp(QString)));

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(nameLbl, 0, 0);
	layout->addWidget(nameEdit, 0, 1);
	layout->addWidget(pmpLbl, 1, 0);
	layout->addWidget(pmpEdit, 1, 1);
	layout->addWidget(pmdLbl, 2, 0);
	layout->addWidget(pmdEdit, 2, 1);
	layout->addWidget(pvpLbl, 3, 0);
	layout->addWidget(pvpEdit, 3, 1);
	layout->addWidget(gateList, 4, 0, 1, 2, Qt::AlignLeft);
	layout->addLayout(caLayout, 5, 0, 1, 2);
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
	pmdEdit->clear();
	pvpEdit->clear();
	gateList->clear();

	PageWidget::clear();
}

void MiscWidget::setReadOnly(bool readOnly)
{
	if(isBuilded()) {
		nameEdit->setReadOnly(readOnly);
		pmpEdit->setReadOnly(readOnly);
		pmdEdit->setReadOnly(readOnly);
		pvpEdit->setReadOnly(readOnly);
	}

	PageWidget::setReadOnly(readOnly);
}

void MiscWidget::fill()
{
	if(!isBuilded())	build();
	if(isFilled())		clear();

	if(!hasData() || (!data()->hasMiscFile() && !data()->hasWalkmeshFile()))	return;

	if(data()->hasMiscFile()) {
		nameEdit->setText(data()->getMiscFile()->getMapName());
		pmpEdit->setText(data()->getMiscFile()->getPmpData().toHex());
		pmdEdit->setText(data()->getMiscFile()->getPmdData().toHex());
		pvpEdit->setText(data()->getMiscFile()->getPvpData().toHex());
		gateList->clear();
		foreach(quint16 mapId, data()->getMiscFile()->getGateways()) {
			gateList->addItem(QString::number(mapId));
		}
	}

	if(data()->hasWalkmeshFile()) {
		caVectorX1Edit->setValue(data()->getWalkmeshFile()->camAxis(0).x);
		caVectorX2Edit->setValue(data()->getWalkmeshFile()->camAxis(0).y);
		caVectorX3Edit->setValue(data()->getWalkmeshFile()->camAxis(0).z);
		caVectorY1Edit->setValue(data()->getWalkmeshFile()->camAxis(1).x);
		caVectorY2Edit->setValue(data()->getWalkmeshFile()->camAxis(1).y);
		caVectorY3Edit->setValue(data()->getWalkmeshFile()->camAxis(1).z);
		caVectorZ1Edit->setValue(data()->getWalkmeshFile()->camAxis(2).x);
		caVectorZ2Edit->setValue(data()->getWalkmeshFile()->camAxis(2).y);
		caVectorZ3Edit->setValue(data()->getWalkmeshFile()->camAxis(2).z);

		caSpaceXEdit->setValue(data()->getWalkmeshFile()->camPos(0));
		caSpaceYEdit->setValue(data()->getWalkmeshFile()->camPos(1));
		caSpaceZEdit->setValue(data()->getWalkmeshFile()->camPos(2));
	}

	PageWidget::fill();
}

void MiscWidget::editName(const QString &name)
{
	data()->getMiscFile()->setMapName(name);
	emit modified();
}

void MiscWidget::editPmp(const QString &pmp)
{
	data()->getMiscFile()->setPmpData(
				QByteArray::fromHex(pmp.toLatin1())
				.leftJustified(data()->getMiscFile()->getPmpData().size(), '\x00', true));
	emit modified();
	pmpEdit->setText(data()->getMiscFile()->getPmpData().toHex());
}

void MiscWidget::editPmd(const QString &pmd)
{
	data()->getMiscFile()->setPmdData(
				QByteArray::fromHex(pmd.toLatin1())
				.leftJustified(data()->getMiscFile()->getPmdData().size(), '\x00', true));
	emit modified();
	pmdEdit->setText(data()->getMiscFile()->getPmdData().toHex());
}

void MiscWidget::editPvp(const QString &pvp)
{
	data()->getMiscFile()->setPvpData(
				QByteArray::fromHex(pvp.toLatin1())
				.leftJustified(data()->getMiscFile()->getPvpData().size(), '\x00', true));
	emit modified();
	pvpEdit->setText(data()->getMiscFile()->getPvpData().toHex());
}

void MiscWidget::editCaVector(int value)
{
	QObject *s = sender();

	if(s == caVectorX1Edit)			editCaVector(0, 0, value);
	else if(s == caVectorX2Edit)	editCaVector(0, 1, value);
	else if(s == caVectorX3Edit)	editCaVector(0, 2, value);
	else if(s == caVectorY1Edit)	editCaVector(1, 0, value);
	else if(s == caVectorY2Edit)	editCaVector(1, 1, value);
	else if(s == caVectorY3Edit)	editCaVector(1, 2, value);
	else if(s == caVectorZ1Edit)	editCaVector(2, 0, value);
	else if(s == caVectorZ2Edit)	editCaVector(2, 1, value);
	else if(s == caVectorZ3Edit)	editCaVector(2, 2, value);
}

void MiscWidget::editCaVector(int id, int id2, int value)
{
	if(data()->hasWalkmeshFile()) {
		Vertex_s v = data()->getWalkmeshFile()->camAxis(id), oldV;
		oldV = v;
		switch(id2) {
		case 0:
			v.x = value;
			break;
		case 1:
			v.y = value;
			break;
		case 2:
			v.z = value;
			break;
		}
		if(oldV.x != v.x || oldV.y != v.y || oldV.z != v.z) {
			data()->getWalkmeshFile()->setCamAxis(id, v);
			emit modified();
		}
	}
}

void MiscWidget::editCaPos(double value)
{
	QObject *s = sender();

	if(s == caSpaceXEdit)			editCaPos(0, value);
	else if(s == caSpaceYEdit)		editCaPos(1, value);
	else if(s == caSpaceZEdit)		editCaPos(2, value);
}

void MiscWidget::editCaPos(int id, int value)
{
	if(data()->hasWalkmeshFile()) {
		if(data()->getWalkmeshFile()->camPos(id) != value) {
			data()->getWalkmeshFile()->setCamPos(id, value);
			emit modified();
		}
	}
}
