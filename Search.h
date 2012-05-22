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
#ifndef DEF_SEARCH
#define DEF_SEARCH

#include <QtGui>
#include "FieldArchive.h"

class Search : public QDialog
{
    Q_OBJECT
public:
	explicit Search(QTreeWidget *fieldList, QWidget *parent=0);
	void setFieldArchive(FieldArchive *);
	void setSearchText(const QString &text);
	void setSearchOpcode(int opcode);
	void setCurrentIndex(int index);
public slots:
	void setFieldId(int);
	void setTextId(int);
	void setFocus2();
signals:
	void foundText(QString text, int fieldID, int textID, Qt::CaseSensitivity cs, bool reverse, bool regexp);
	void foundOpcode(int, int, int, int);
private slots:
	void findNext();
	void findPrev();
private:
	Qt::CaseSensitivity sensitivity();
	FieldArchive::Sorting sorting();
	bool findNextText();
	bool findPrevText();
	bool findNextScript();
	bool findPrevScript();
	QWidget *textPageWidget();
	QWidget *scriptPageWidget();
	QWidget *scriptPageWidget1();
	QWidget *scriptPageWidget2();
	QWidget *scriptPageWidget3();
	QWidget *scriptPageWidget4();
	QTabWidget *tabWidget;
	QLineEdit *searchTextField, *searchScriptTextField;
	QCheckBox *checkBox, *checkBox2, *scriptCheckBox;
	QComboBox *typeScriptChoice, *searchOpcode;
	QSpinBox *searchOpcodeValue;
	QComboBox *selectScriptVar;
	QCheckBox *popScriptVar;
	QSpinBox *selectScriptGroup, *selectScriptLabel;
	QStackedWidget *scriptStacked;
	QPushButton *buttonNext, *buttonPrev;
	FieldArchive *fieldArchive;
	QTreeWidget *fieldList;
	int fieldID;
	int textID, from;
	int groupID, methodID, opcodeID;
};

#endif
