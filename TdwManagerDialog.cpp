#include "TdwManagerDialog.h"

TdwManagerDialog::TdwManagerDialog(QWidget *parent) :
	QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint)
{
	setWindowTitle(tr("Gestionnaire de police"));
	setSizeGripEnabled(true);

	list1 = new QListWidget(this);

	tdwWidget = new TdwWidget2(false, this);

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(list1, 0, 0);
	layout->addWidget(tdwWidget, 0, 1);
	layout->setColumnStretch(1, 1);

	fillList1();
	setTdw(list1->currentRow());

	connect(list1, SIGNAL(currentRowChanged(int)), SLOT(setTdw(int)));
}

void TdwManagerDialog::fillList1()
{
	QString currentEncoding = Config::value("encoding", "00").toString();

	foreach(const QString &fontName, Data::fontList()) {
		QListWidgetItem *item;

		if(fontName == "00" || fontName == "01") {
			item = new QListWidgetItem(fontName == "00" ? tr("Latin") : tr("Japonais"));
			item->setData(Qt::UserRole, fontName);
			list1->addItem(item);
		} else {
			FF8Font *font = Data::font(fontName);
			if(font) {
				item = new QListWidgetItem(font->name());
				item->setData(Qt::UserRole, fontName);
				list1->addItem(item);
			} else {
				continue;
			}
		}

		if(currentEncoding == fontName) {
			list1->setCurrentItem(item);
		}
	}
}

void TdwManagerDialog::setTdw(int id)
{
	QListWidgetItem *item = list1->item(id);

	if(!item)	return;

	QString fontName = item->data(Qt::UserRole).toString();

	FF8Font *font = Data::font(fontName);
	if(font) {
		tdwWidget->setFF8Font(font);
	}
}
