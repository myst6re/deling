#include "TdwManagerDialog.h"

TdwManagerDialog::TdwManagerDialog(QWidget *parent) :
	QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint)
{
	setWindowTitle(tr("Gestionnaire de police"));
	setSizeGripEnabled(true);

	toolbar1 = new QToolBar(this);
	toolbar1->setIconSize(QSize(16, 16));
	plusAction = toolbar1->addAction(QIcon(":/images/plus.png"), tr("Copier"), this, SLOT(addFont()));
	plusAction->setShortcut(QKeySequence("Ctrl++"));
	minusAction = toolbar1->addAction(QIcon(":/images/minus.png"), tr("Effacer"), this, SLOT(removeFont()));
	minusAction->setShortcut(QKeySequence::Delete);

	list1 = new QListWidget(this);
	list1->setFixedWidth(125);

	tdwWidget = new TdwWidget2(false, this);

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(toolbar1, 0, 0);
	layout->addWidget(list1, 1, 0);
	layout->addWidget(tdwWidget, 0, 1, 2, 1);
	layout->setColumnStretch(1, 1);

	fillList1();
	setTdw(list1->currentRow());

	connect(list1, SIGNAL(currentRowChanged(int)), SLOT(setTdw(int)));
}

void TdwManagerDialog::fillList1()
{
	QString currentEncoding = Config::value("encoding", "00").toString();

	foreach(const QString &fontName, FF8Font::fontList()) {
		QListWidgetItem *item;

		if(fontName == "00" || fontName == "01") {
			item = new QListWidgetItem(fontName == "00" ? tr("Latin") : tr("Japonais"));
			item->setData(Qt::UserRole, fontName);
			list1->addItem(item);
		} else {
			FF8Font *font = FF8Font::font(fontName);
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

	FF8Font *font = FF8Font::font(fontName);
	if(font) {
		tdwWidget->setFF8Font(font);
	}

	minusAction->setEnabled(!font->isReadOnly());
}

void TdwManagerDialog::addFont()
{
	QListWidgetItem *item = list1->currentItem();

	if(!item)	return;

	QString name, nameId;

	if(newNameDialog(name, nameId)) {
		if(FF8Font::copyFont(nameId, item->data(Qt::UserRole).toString(), name)) {
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
	formLayout->addRow(tr("Nom &affiché :"), nameEdit);
	formLayout->addRow(tr("Nom du &fichier :"), fileNameEdit);

	QVBoxLayout *layout = new QVBoxLayout(&dialog);
	layout->addLayout(formLayout, 1);
	layout->addWidget(ok, 0, Qt::AlignCenter);

	connect(ok, SIGNAL(clicked()), &dialog, SLOT(accept()));

	if(dialog.exec() == QDialog::Accepted) {
		QString name1 = nameEdit->text();
		QString name2 = QDir::cleanPath(fileNameEdit->text());

		QStringList fontList = FF8Font::fontList();
		if(name1.isEmpty() || name2.isEmpty()
				|| fontList.contains(name1)
				|| QFile::exists(FF8Font::fontDirPath()+"/"+name2)) {
			QMessageBox::warning(this, tr("Choisissez un autre nom"), tr("Ce nom existe déjà ou est invalide, veuillez en choisir un autre."));
			return false;
		}

		name = name1;
		nameId = name2;
		return true;
	}
	return false;
}

void TdwManagerDialog::removeFont()
{
	QList<QListWidgetItem *> items = list1->selectedItems();
	if(items.isEmpty())		return;

	if(QMessageBox::Yes != QMessageBox::question(this, tr("Supprimer une police"), tr("Voulez-vous vraiment supprimer la police sélectionnée ?"), QMessageBox::Yes | QMessageBox::Cancel)) {
		return;
	}

	QListWidgetItem *item = items.first();

	if(FF8Font::removeFont(item->data(Qt::UserRole).toString())) {
		delete item;
	}
}
