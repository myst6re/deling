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
#include "widgets/TdwGrid.h"
#include "widgets/TdwLetter.h"
#include "widgets/TdwPalette.h"
#include "FF8Font.h"

class TdwWidget2 : public QWidget
{
	Q_OBJECT
public:
	TdwWidget2(bool isAdditionnalTable, QWidget *parent = nullptr);
	void clear();
	void setFF8Font(FF8Font *ff8Font);
	void setTdwFile(TdwFile *tdw);
	void setIsAdditionnalTable(bool isAdditionnalTable);
	void setReadOnly(bool ro);
signals:
	void letterEdited();
public slots:
	void setColor(int i);
	void setTable(int i);
	void setLetter(int i);
	void editLetter(const QString &letter);
	void editWidth(int w);
	void exportFont();
	void importFont();
	void reset();
	void resetLetter();
private slots:
	void setModified();
private:
	bool isAdditionnalTable;
	TdwGrid *tdwGrid;
	QPushButton *exportButton, *importButton;
	TdwLetter *tdwLetter;
	TdwPalette *tdwPalette;
	QComboBox *selectPal, *selectTable;
	QPushButton *fromImage1, *fromImage2;
	QLineEdit *textLetter;
	QSpinBox *widthLetter;
	QPushButton *resetButton2;
	FF8Font *ff8Font;
protected:
	void focusInEvent(QFocusEvent *);
};
