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
#pragma once

#include <QtWidgets>
#include "CsvFile.h"

class ListWidget;

class TextExporterWidget : public QDialog
{
	Q_OBJECT
public:
	TextExporterWidget(const QStringList &langs, QWidget *parent = nullptr);
	TextExporterWidget(QWidget *parent = nullptr);
	QStringList langs() const;
	CsvFile::CsvEncoding encoding() const {
		return CsvFile::CsvEncoding(_encoding->currentData().toInt());
	}
	QChar fieldSeparator() const {
		return TextExporterWidget::lineEditChar(_fieldSeparator);
	}
	QChar quoteCharater() const {
		return TextExporterWidget::lineEditChar(_quoteCharater);
	}
	quint8 column() const {
		return quint8(_column->value() - 1);
	}
protected slots:
	void accept() override;
private:
	void build(const QStringList &langs, bool onlyOne);
	QString validate() const;
	static QChar lineEditChar(QLineEdit *lineEdit);
	ListWidget *_langs;
	QSpinBox *_column;
	QLineEdit *_fieldSeparator, *_quoteCharater;
	QComboBox *_encoding;
};
