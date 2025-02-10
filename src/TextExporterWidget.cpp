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
#include "TextExporterWidget.h"
#include "Listwidget.h"
#include "Config.h"

TextExporterWidget::TextExporterWidget(const QStringList &langs, QWidget *parent) :
      QDialog(parent), _langs(nullptr), _column(nullptr)
{
	build(langs, false);
}

TextExporterWidget::TextExporterWidget(QWidget *parent) :
      QDialog(parent), _langs(nullptr), _column(nullptr)
{
	build(QStringList(), true);
}

void TextExporterWidget::build(const QStringList &langs, bool onlyOne)
{
	if (!onlyOne) {
		QString currentLang = Config::value("gameLang", "en").toString();
		_langs = new ListWidget(this);

		for (const QString &lang: langs) {
			QListWidgetItem *item = new QListWidgetItem(lang, _langs->listWidget());
			item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemNeverHasChildren);
			item->setCheckState(!currentLang.isEmpty() && currentLang == lang ? Qt::Checked : Qt::Unchecked);
		}
	}

	QGroupBox *group = new QGroupBox(tr("CSV format"), this);
	if (onlyOne) {
		_column = new QSpinBox(group);
		_column->setRange(0, 100);
		_column->setValue(0);
	}
	_encoding = new QComboBox(group);
	_encoding->addItem(QString("UTF-8"), CsvFile::Utf8);
	_fieldSeparator = new QLineEdit(group);
	_fieldSeparator->setText(",");
	_fieldSeparator->setCompleter(new QCompleter(QStringList() << "," << ";" << ":" << "{tab}"));
	_quoteCharater = new QLineEdit(group);
	_quoteCharater->setText("\"");
	_quoteCharater->setCompleter(new QCompleter(QStringList() << "\"" << "'"));
	QFormLayout *groupLayout = new QFormLayout(group);
	if (onlyOne) {
		groupLayout->addRow(tr("Column that contains the texts to import"), _column);
	}
	groupLayout->addRow(tr("Character encoding"), _encoding);
	groupLayout->addRow(tr("Field separator"), _fieldSeparator);
	groupLayout->addRow(tr("String separator"), _quoteCharater);
	
	QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok
	                                                 | QDialogButtonBox::Cancel, this);
	
	QGridLayout *layout = new QGridLayout(this);
	if (_langs != nullptr) {
		layout->addWidget(_langs, 0, 0);
		layout->addWidget(group, 0, 1);
	} else {
		layout->addWidget(group, 0, 0, 1, 2);
	}
	layout->addWidget(buttons, 1, 0, 1, 2);
	
	connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QStringList TextExporterWidget::langs() const
{
	QStringList ret;

	if (_langs != nullptr) {
		for (int row = 0; row < _langs->listWidget()->count(); ++row) {
			if (_langs->listWidget()->item(row)->checkState() == Qt::Checked) {
				ret.append(_langs->listWidget()->item(row)->text());
			}
		}
	}
	
	return ret;
}

QChar TextExporterWidget::lineEditChar(QLineEdit *lineEdit)
{
	QString text = lineEdit->text();
	
	if (text.isEmpty()) {
		return QChar();
	}
	
	if (text == "{tab}") {
		return QChar('\t');
	}
	
	return text.at(0);
}

QString TextExporterWidget::validate() const
{
	QChar fieldSep = fieldSeparator(), quoteChar = quoteCharater();

	if (!fieldSep.isNull() && fieldSep == quoteChar) {
		return tr("The field separator must be different than the string separator");
	}
	
	if (fieldSep == '\n' || fieldSep == '\r') {
		return tr("The field separator cannot be a line break");
	}
	
	if (quoteChar == '\n' || quoteChar == '\r') {
		return tr("The string separator cannot be a line break");
	}
	
	if (_column != nullptr && _column->value() == 0) {
		return tr("Select the column number that contains the texts to import, between 1 and 100");
	}
	
	return QString();
}

void TextExporterWidget::accept()
{
	QString validationText = validate();
	if (!validationText.isEmpty()) {
		QMessageBox::warning(this, tr("Error"), validationText);
		return;
	}
	
	QDialog::accept();
}
