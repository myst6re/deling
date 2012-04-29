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
#include "VarManager.h"

VarManager::VarManager(FieldArchive *fieldArchive, QWidget *parent)
	: QWidget(parent, Qt::Tool)
{
	setWindowTitle(tr("Gestionnaire de variables"));
	QFont font;
	font.setPointSize(8);
	
	var = new QSpinBox(this);
	var->setRange(0, 1536);// Valid vars : 4 -> 1536

	name = new QLineEdit(this);
	list = new QTreeWidget(this);
	list->setFont(font);
	list->setHeaderLabels(QStringList() << tr("Var") << tr("Type") << tr("Nom") << tr("Écran") << tr("MEM fr") << tr("MEM us"));
	list->setAutoScroll(false);
	list->setIndentation(0);
	list->setUniformRowHeights(true);

	countLabel = new QLabel(this);

	searchButton = new QPushButton(tr("Adresses utilisées"), this);
	QPushButton *saveButton = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton), tr("Sauver"), this);
	saveButton->setShortcut(QKeySequence::Save);
	
	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(new QLabel(tr("Var"),this), 0, 0);
	layout->addWidget(var, 0, 1);
	layout->addWidget(new QLabel(tr("Nom"),this), 1, 0);
	layout->addWidget(name, 1, 1);
	layout->addWidget(list, 2, 0, 1, 2);
	layout->addWidget(countLabel, 3, 0, 1, 2);
	layout->addWidget(searchButton, 4, 0, Qt::AlignRight);
	layout->addWidget(saveButton, 4, 1, Qt::AlignLeft);

	setFieldArchive(fieldArchive);
	fillList();
	
	connect(var, SIGNAL(valueChanged(int)), SLOT(scrollToList(int)));
	connect(list, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), SLOT(updateName(QTreeWidgetItem*)));
	connect(name, SIGNAL(editingFinished()), SLOT(editName()));
	connect(searchButton, SIGNAL(released()), SLOT(search()));
	connect(saveButton, SIGNAL(released()), SLOT(save()));
}

void VarManager::setFieldArchive(FieldArchive *fieldArchive)
{
	this->fieldArchive = fieldArchive;
	searchButton->setEnabled(fieldArchive!=NULL);
}

void VarManager::updateName(QTreeWidgetItem *item)
{
	var->setValue(list->indexOfTopLevelItem(item));
	name->setText(item->text(2));
}

void VarManager::editName()
{
	if(list->currentItem()==NULL)	return;
	list->currentItem()->setText(2, name->text());
}

void VarManager::fillList()
{
	list->clear();
	for(quint16 i=0 ; i<1536 ; ++i)
	{
		list->addTopLevelItem(new QTreeWidgetItem(QStringList() << QString("%1").arg(i, 4, 10, QChar('0')) << "" << Config::value(QString("var%1").arg(i)).toString() << "" << QString::number(0x78570+i,16).toUpper() << QString::number(0x780D8+i,16).toUpper()));
	}
	scrollToList(0);
	list->resizeColumnToContents(0);
	list->resizeColumnToContents(1);
	list->resizeColumnToContents(2);
}

void VarManager::scrollToList(int index)
{
	QTreeWidgetItem *item = list->topLevelItem(index);
	list->setCurrentItem(item);
	list->scrollToItem(item);
}

void VarManager::search()
{
	setEnabled(false);

	QMultiMap<int, QString> vars = fieldArchive->searchAllVars();

	quint32 key;
	qint32 param;
	QTreeWidgetItem *item;
	QMap<int, bool> count;
	int lastVar=-1;

	QMapIterator<int, QString> i(vars);
	while(i.hasNext()) {
		i.next();
		if(lastVar == i.key())	continue;

		lastVar = i.key();
		JsmOpcode op(lastVar);
		key = op.key();
		param = op.param();
		count.insert(param, true);

		item = list->topLevelItem(param);
		item->setBackground(0, QColor(0xff,0xe5,0x99));

		if(key == 10 || key == 11 || key == 16) {
			item->setText(1, QString("%1Byte").arg(key == 16 ? "Signed " : ""));
		}
		else if(key == 12 || key == 13 || key == 17) {
			item->setText(1, QString("%1Word").arg(key == 17 ? "Signed " : ""));
			count.insert(param+1, true);
			list->topLevelItem(param+1)->setBackground(0, QColor(0xff,0xe5,0x99));
		}
		else if(key == 14 || key == 15 || key == 18) {
			item->setText(1, QString("%1Long").arg(key == 18 ? "Signed " : ""));
			count.insert(param+1, true);
			list->topLevelItem(param+1)->setBackground(0, QColor(0xff,0xe5,0x99));
			count.insert(param+2, true);
			list->topLevelItem(param+2)->setBackground(0, QColor(0xff,0xe5,0x99));
			count.insert(param+3, true);
			list->topLevelItem(param+3)->setBackground(0, QColor(0xff,0xe5,0x99));
		}
		QStringList fields(vars.values(lastVar));
		fields.append(item->text(3).split(", ", QString::SkipEmptyParts));
		fields.removeDuplicates();
		item->setText(3, fields.join(", "));
	}

	list->resizeColumnToContents(0);
	list->resizeColumnToContents(1);

	setEnabled(true);
	countLabel->setText(QString("Vars utilisés : %1/1536").arg(count.size()));
}

void VarManager::save()
{
	QString text;
	for(int i=0 ; i<1536 ; ++i)
	{
		text = list->topLevelItem(i)->text(2);
		if(!text.isEmpty()) {
			Config::setValue(QString("var%1").arg(i), text);
		}
	}
}
