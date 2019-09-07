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

#include <QtWidgets>
#include "FieldArchive.h"

class Search : public QDialog
{
    Q_OBJECT
public:
	enum Tabs {
		Text=0, Script
	};

	explicit Search(QTreeWidget *fieldList, QWidget *parent=0);
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
protected:
	void focusInEvent(QFocusEvent *);
private:
	QString text() const;
	Qt::CaseSensitivity sensitivity() const;
	FieldArchive::Sorting sorting() const;
	QRegExp regexp() const;
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
	QWidget *scriptPageWidget5();
	QTabWidget *tabWidget;
	QLineEdit *searchTextField, *searchScriptTextField;
	QCheckBox *checkBox, *checkBox2, *scriptCheckBox, *scriptCheckBox2;
	QSpinBox *selectMap;
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
