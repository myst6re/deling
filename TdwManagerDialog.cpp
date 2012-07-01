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
}
