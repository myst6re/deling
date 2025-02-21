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
#include "TdwManagerDialog.h"
#include "Listwidget.h"
#include "Config.h"

TdwManagerDialog::TdwManagerDialog(QWidget *parent) :
	QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint)
{
	setWindowTitle(tr("Font manager"));
	setSizeGripEnabled(true);

    ListWidget *listWidget = new ListWidget(this);
    plusAction = listWidget->addAction(ListWidget::Add, tr("Copy"), this, SLOT(addFont()));
    minusAction = listWidget->addAction(ListWidget::Rem, tr("Remove"), this, SLOT(removeFont()));
    toolbar1 = listWidget->toolBar();
    list1 = listWidget->listWidget();

	tdwWidget = new TdwWidget2(false, this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(listWidget);
    layout->addWidget(tdwWidget, 1);

	fillList1();
	setTdw(list1->currentRow());

	connect(list1, SIGNAL(currentRowChanged(int)), SLOT(setTdw(int)));
}

void TdwManagerDialog::fillList1()
{
	QString currentEncoding = Config::value("encoding", "00").toString();

	for (const QString &fontName: FF8Font::fontList()) {
		QListWidgetItem *item;

		if (fontName == "00" || fontName == "01") {
			item = new QListWidgetItem(fontName == "00" ? tr("Latin") : tr("Japanese"));
			item->setData(Qt::UserRole, fontName);
			list1->addItem(item);
		} else {
			FF8Font *font = FF8Font::font(fontName);
			if (font) {
				item = new QListWidgetItem(font->name());
				item->setData(Qt::UserRole, fontName);
				list1->addItem(item);
			} else {
				continue;
			}
		}

		if (currentEncoding == fontName) {
			list1->setCurrentItem(item);
		}
	}
}

void TdwManagerDialog::setTdw(int id)
{
	QListWidgetItem *item = list1->item(id);

	if (!item)	return;

	QString fontName = item->data(Qt::UserRole).toString();

	FF8Font *font = FF8Font::font(fontName);
	if (font) {
		tdwWidget->setFF8Font(font);
	}

	minusAction->setEnabled(font && !font->isReadOnly());
}

void TdwManagerDialog::addFont()
{
	QListWidgetItem *item = list1->currentItem();

	if (!item)	return;

	QString name, nameId;

	if (newNameDialog(name, nameId)) {
		if (FF8Font::copyFont(nameId, item->data(Qt::UserRole).toString(), name)) {
			item = new QListWidgetItem(name);
			item->setData(Qt::UserRole, nameId);
			list1->addItem(item);
		}
	}
}

bool TdwManagerDialog::newNameDialog(QString &name, QString &nameId)
{
	QDialog dialog(this, Qt::Dialog | Qt::WindowCloseButtonHint);

	QLineEdit *nameEdit = new QLineEdit(&dialog);
	QLineEdit *fileNameEdit = new QLineEdit(&dialog);

	QPushButton *ok = new QPushButton(tr("OK"));
	ok->setDefault(true);

	QFormLayout *formLayout = new QFormLayout;
	formLayout->addRow(tr("N&ame displayed:"), nameEdit);
	formLayout->addRow(tr("&File name:"), fileNameEdit);

	QVBoxLayout *layout = new QVBoxLayout(&dialog);
	layout->addLayout(formLayout, 1);
	layout->addWidget(ok, 0, Qt::AlignCenter);

	connect(ok, SIGNAL(clicked()), &dialog, SLOT(accept()));

	if (dialog.exec() == QDialog::Accepted) {
		QString name1 = nameEdit->text(), name2 = QDir::cleanPath(fileNameEdit->text());
		QStringList fontList = FF8Font::fontList();
		if (name1.isEmpty() || name2.isEmpty()
				|| fontList.contains(name1)
				|| QFile::exists(FF8Font::fontDirPath()+"/"+name2)) {
			QMessageBox::warning(this, tr("Choose another name"), tr("That name already exists or is invalid, please choose another."));
			return false;
		}

		name = std::move(name1);
		nameId = std::move(name2);
		return true;
	}
	return false;
}

void TdwManagerDialog::removeFont()
{
	QList<QListWidgetItem *> items = list1->selectedItems();
	if (items.isEmpty())		return;

	if (QMessageBox::Yes != QMessageBox::question(this, tr("Remove a font"), tr("Do you really want to delete the selected font?"), QMessageBox::Yes | QMessageBox::Cancel)) {
		return;
	}

	QListWidgetItem *item = items.first();

	if (FF8Font::removeFont(item->data(Qt::UserRole).toString())) {
		delete item;
	}
}
