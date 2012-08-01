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
#include "TdwWidget2.h"

TdwWidget2::TdwWidget2(bool isAdditionnalTable, QWidget *parent) :
	QWidget(parent), isAdditionnalTable(isAdditionnalTable), ff8Font(0)
{
	tdwGrid = new TdwGrid(this);
	tdwLetter = new TdwLetter(this);
	tdwPalette = new TdwPalette(this);
	selectPal = new QComboBox(this);
	QStringList colors;
	colors << tr("Gris foncé") << tr("Gris") << tr("Jaune") << tr("Rouge") << tr("Vert") << tr("Bleu") << tr("Violet") << tr("Blanc");
	selectPal->addItems(colors);
	selectPal->setCurrentIndex(7);

	selectTable = new QComboBox(this);

	fromImage1 = new QPushButton(tr("À partir d'une image..."), this);
	fromImage1->setVisible(false);//TODO
	fromImage2 = new QPushButton(tr("À partir d'une image..."), this);
	fromImage2->setVisible(false);//TODO
//	QPushButton *resetButton1 = new QPushButton(tr("Annuler les modifications"), this);//TODO
	textLetter = new QLineEdit(this);
	textLetter->setReadOnly(isAdditionnalTable);
	exportButton = new QPushButton(tr("Exporter..."), this);
	resetButton2 = new QPushButton(tr("Annuler les modifications"), this);
	resetButton2->setEnabled(false);

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(tdwGrid, 0, 0, 1, 2, Qt::AlignRight);
	layout->addWidget(tdwLetter, 0, 2, 2, 2, Qt::AlignLeft);
	layout->addWidget(fromImage1, 1, 0, 1, 2, Qt::AlignLeft);
	layout->addWidget(selectPal, 2, 0, Qt::AlignRight);
	layout->addWidget(selectTable, 2, 1, Qt::AlignLeft);
	layout->addWidget(tdwPalette, 2, 2, 1, 2, Qt::AlignLeft);
	layout->addWidget(textLetter, 3, 2);
	layout->addWidget(fromImage2, 3, 3, Qt::AlignRight);
//	layout->addWidget(resetButton1, 4, 0, 1, 2, Qt::AlignLeft);
	layout->addWidget(exportButton, 4, 0, 1, 2, Qt::AlignLeft);
	layout->addWidget(resetButton2, 4, 2, 1, 2, Qt::AlignRight);
	layout->setRowStretch(5, 1);
	layout->setColumnStretch(3, 1);
	layout->setContentsMargins(QMargins());

	connect(selectPal, SIGNAL(currentIndexChanged(int)), SLOT(setColor(int)));
	connect(selectTable, SIGNAL(currentIndexChanged(int)), SLOT(setTable(int)));
	connect(tdwGrid, SIGNAL(letterClicked(int)), SLOT(setLetter(int)));
	connect(tdwLetter, SIGNAL(imageChanged(QRect)), tdwGrid, SLOT(updateLetter(QRect)));
	connect(tdwLetter, SIGNAL(imageChanged(QRect)), SLOT(setModified()));
//	connect(resetButton1, SIGNAL(clicked()), SLOT(reset()));
	connect(exportButton, SIGNAL(clicked()), SLOT(exportFont()));
	connect(resetButton2, SIGNAL(clicked()), SLOT(resetLetter()));
	connect(tdwPalette, SIGNAL(colorChanged(int)), tdwLetter, SLOT(setPixelIndex(int)));
	connect(textLetter, SIGNAL(textEdited(QString)), SLOT(editLetter(QString)));
}

void TdwWidget2::clear()
{
	tdwGrid->clear();
	tdwLetter->clear();
	tdwPalette->clear();
}

void TdwWidget2::setFF8Font(FF8Font *ff8Font)
{
	this->ff8Font = ff8Font;
	setTdwFile(ff8Font->tdw());
	setReadOnly(ff8Font->isReadOnly());
}

void TdwWidget2::setTdwFile(TdwFile *tdw)
{
	tdwGrid->setTdwFile(tdw);
	tdwLetter->setTdwFile(tdw);
	tdwPalette->setTdwFile(tdw);
	exportButton->setVisible(!tdw->isOptimizedVersion());
	setLetter(0);

	if(selectTable->count() != tdw->tableCount()) {
		selectTable->clear();
		for(int i=1 ; i<=tdw->tableCount() ; ++i) {
			selectTable->addItem(tr("Table %1").arg(i));
		}
		selectTable->setEnabled(selectTable->count() > 1);
	}
}

void TdwWidget2::setIsAdditionnalTable(bool isAdditionnalTable)
{
	this->isAdditionnalTable = isAdditionnalTable;
	textLetter->setReadOnly(isAdditionnalTable);
}

void TdwWidget2::setReadOnly(bool ro)
{
	textLetter->setReadOnly(isAdditionnalTable || ro);
	tdwLetter->setReadOnly(ro);
	tdwPalette->setReadOnly(ro);
	fromImage1->setDisabled(ro);
	fromImage2->setDisabled(ro);
}

void TdwWidget2::setColor(int i)
{
	tdwGrid->setColor((TdwFile::Color)i);
	tdwLetter->setColor((TdwFile::Color)i);
	tdwPalette->setCurrentPalette((TdwFile::Color)i);
}

void TdwWidget2::setTable(int i)
{
	tdwGrid->setCurrentTable(i);
	tdwLetter->setCurrentTable(i);
	setLetter(0);
}

void TdwWidget2::setLetter(int i)
{
	tdwLetter->setLetter(i);
	tdwGrid->setLetter(i);

	QByteArray ba;

	if(isAdditionnalTable) {
		ba.append('\x1c');
	} else if(tdwGrid->currentTable() >= 1 && tdwGrid->currentTable() <= 3) {
		ba.append(char(0x18 + tdwGrid->currentTable()));
	}

	if(tdwGrid->currentTable() <= 3) {
		if(ff8Font) {
			textLetter->setText(FF8Text(ba.append((char)(0x20 + i)), ff8Font->tables()));
		} else {
			textLetter->setText(FF8Text(ba.append((char)(0x20 + i))));
		}
	}
	resetButton2->setEnabled(false);
}

void TdwWidget2::editLetter(const QString &letter)
{
	if(ff8Font) {
		ff8Font->setChar(tdwGrid->currentTable(), tdwGrid->currentLetter(), letter);
	}
}

void TdwWidget2::exportFont()
{
	QStringList filter;
	filter.append(tr("Fichier texture FF8 (*.tex)"));
	filter.append(tr("Fichier police FF8 (*.tdw)"));
	QString selectedFilter;

	QString path = QFileDialog::getSaveFileName(this, tr("Exporter police de caractère"), "sysfnt", filter.join(";;"), &selectedFilter);
	if(path.isNull())		return;

	int index = filter.indexOf(selectedFilter);
	if(index == 0) {
		QByteArray data;
		TexFile tex = ff8Font->tdw()->toTexFile();
		if(tex.isValid()) {
			if(tex.save(data)) {
				QFile f(path);
				if(f.open(QIODevice::WriteOnly)) {
					f.write(data);
					f.close();
				} else {
					QMessageBox::warning(this, tr("Erreur"), tr("Erreur d'ouverture du fichier. (%1)").arg(f.errorString()));
				}
			} else {
				QMessageBox::warning(this, tr("Erreur"), tr("Erreur lors de l'enregistrement."));
			}
		} else {
			QMessageBox::information(this, tr("Information"), tr("Format de police de caractère inexportable."));
		}
	} else if(index == 1) {
		QByteArray data;
		TdwFile tdw = *(ff8Font->tdw());
		if(tdw.save(data)) {
			QFile f(path);
			if(f.open(QIODevice::WriteOnly)) {
				f.write(data);
				f.close();
			} else {
				QMessageBox::warning(this, tr("Erreur"), tr("Erreur d'ouverture du fichier. (%1)").arg(f.errorString()));
			}
		} else {
			QMessageBox::warning(this, tr("Erreur"), tr("Erreur lors de l'enregistrement."));
		}
	}
}

void TdwWidget2::reset()
{
//	tdwGrid->reset();//TODO
	tdwLetter->update();
}

void TdwWidget2::resetLetter()
{
	tdwLetter->reset();
	tdwGrid->update();
	resetButton2->setEnabled(false);
}

void TdwWidget2::setModified()
{
	emit letterEdited();
	resetButton2->setEnabled(true);
}

void TdwWidget2::focusInEvent(QFocusEvent *)
{
	tdwGrid->setFocus();
}
