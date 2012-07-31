#include "SpecialCharactersDialog.h"

SpecialCharactersDialog::SpecialCharactersDialog(QWidget *parent) :
	QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint)
{
	setWindowTitle(tr("Caractères spéciaux"));

	bool hasTables = FF8Font::getCurrentConfigFont()->tdw()->tableCount() > 1;

	if(hasTables) {
		tableSelect = new QComboBox(this);
		int tableCount = FF8Font::getCurrentConfigFont()->tdw()->tableCount();
		for(int i=0 ; i<tableCount ; ++i) {
			tableSelect->addItem(tr("Table %1").arg(i+1));
		}
	}
	grid = new TdwGrid(this);
	grid->setTdwFile(FF8Font::getCurrentConfigFont()->tdw());

	QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
	buttonBox->addButton(tr("Insérer"), QDialogButtonBox::AcceptRole);
	buttonBox->addButton(QDialogButtonBox::Cancel);

	connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));
	if(hasTables)	connect(tableSelect, SIGNAL(currentIndexChanged(int)), grid, SLOT(setCurrentTable(int)));

	QGridLayout *layout = new QGridLayout(this);
	if(hasTables)	layout->addWidget(new QLabel(tr("Table")), 0, 0);
	if(hasTables)	layout->addWidget(tableSelect, 0, 1);
	layout->addWidget(grid, 1, 0, 1, 4);
	layout->addWidget(buttonBox, 2, 0, 1, 4);
}

QString SpecialCharactersDialog::selectedCharacter() const
{
	int tableID = grid->currentTable();
	int charID = grid->currentLetter();
	QByteArray ff8TextData;
	if(tableID > 0 && tableID < 4) {
		ff8TextData.append(char(0x18 + tableID));
	}
	ff8TextData.append(char(0x20 + charID));

	return FF8Text(ff8TextData);
}
