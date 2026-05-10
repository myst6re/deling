/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2026 Arzel Jérôme <myst6re@gmail.com>
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
#include "JsmScripts.h"

class Field;

class JsmMethodList : public QTreeWidget
{
    Q_OBJECT
public:
	explicit JsmMethodList(QWidget *parent = nullptr);

	int selectedID() const;

	inline QToolBar *toolBar() const {
		return _toolBar;
	}
	void clearCopiedMethods();
	void setEnabled(bool enabled);

	void clear();
	void setField(Field *field);
	void fill(int groupID);
	//void localeRefresh();
	void scroll(int, bool focus = true);
	void enableActions(bool enabled);

private slots:
	inline void rename() {
		rename(currentItem(), 1);
	}
	void rename(QTreeWidgetItem *item, int column);
	void renameOK(QTreeWidgetItem *item, int column);
	void add();
	void del(bool totalDel = true);
	void cut();
	void copy();
	void paste();
	void upDownEnabled();

signals:
	void modified();

private:
	inline void fill() {
		fill(_groupID);
	}
	QList<QTreeWidgetItem *> nameList(int groupID) const;
	QTreeWidgetItem *findItem(int id) const;
	QList<int> selectedIDs() const;

	QToolBar *_toolBar;
	QAction *_renameMethodAction, *_addMethodAction, *_delMethodAction, *_cutMethodAction, *_copyMethodAction, *_pasteMethodAction;

	Field *_field;
	int _groupID;

	QList<JsmMethod> _methodCopied;
};
