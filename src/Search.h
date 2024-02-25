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
#include "FieldArchive.h"

class SearchAll;

class Search : public QDialog
{
    Q_OBJECT
public:
	enum Tabs {
		Text=0, Script
	};

	explicit Search(QTreeWidget *fieldList, SearchAll *searchAllDialog, QWidget *parent = nullptr);
	void setFieldArchive(FieldArchive *);
	void setSearchText(const QString &text);
	void setSearchOpcode(int opcode);
	void setCurrentIndex(int index);
	Tabs currentIndex() const;
public slots:
	void setFieldId(int);
	void setTextId(int);
	void setFrom(int);
signals:
	void foundText(int fieldID, int textID, int from, int size);
	void foundOpcode(int, int, int, int);
private slots:
	void findNext();
	void findPrev();
	void findAll();
protected:
	void focusInEvent(QFocusEvent *);
private:
	QString text() const;
	Qt::CaseSensitivity sensitivity() const;
	FieldArchive::Sorting sorting() const;
	QRegularExpression regexp() const;
	bool findNextText();
	bool findPrevText();
	bool findNextScript();
	bool findNextScript(int &fieldID, int &groupID, int &methodID, int &opcodeID);
	bool findPrevScript();
	void findAllScript();
	void findAllText();
	QWidget *textPageWidget();
	QWidget *scriptPageWidget();
	QWidget *scriptPageWidget1();
	QWidget *scriptPageWidget2();
	QWidget *scriptPageWidget3();
	QWidget *scriptPageWidget4();
	QWidget *scriptPageWidget5();
	QTabWidget *tabWidget;
	QLineEdit *searchTextField, *searchScriptTextField;
	QCheckBox *checkBox, *checkBox2, *scriptCheckBox, *scriptCheckBox2;
	QSpinBox *selectMap;
	QComboBox *typeScriptChoice, *searchOpcode;
	QSpinBox *searchOpcodeValue;
	QComboBox *selectScriptVar;
	QRadioButton *popScriptVar, *pushScriptVar;
	QSpinBox *selectScriptGroup, *selectScriptLabel;
	QStackedWidget *scriptStacked;
	QPushButton *buttonNext, *buttonPrev, *buttonSearchAll;
	FieldArchive *fieldArchive;
	QTreeWidget *fieldList;
	SearchAll *searchAllDialog;
	int fieldID;
	int textID, from;
	int groupID, methodID, opcodeID;
};
