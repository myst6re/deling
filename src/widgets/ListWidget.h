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

class AbstractListWidget : public QWidget
{
	Q_OBJECT
public:
	enum ActionType {
		Invisible, Add, Rem, Up, Down, Copy, Cut, Paste
	};
	
	explicit AbstractListWidget(QWidget *widget, QWidget *parent = nullptr);
	QAction *addAction(ActionType type, const QString &text,
	                   const QObject *receiver, const char *member);
	
	QToolBar *toolBar() const;
protected:
	inline QWidget *widget() const {
		return _widget;
	}
private:
	QToolBar *_toolBar;
	QWidget *_widget;
};

class ListWidget : public AbstractListWidget
{
	Q_OBJECT
public:
	explicit ListWidget(QWidget *parent = nullptr);
	
	inline QListWidget *listWidget() const {
		return (QListWidget *)widget();
	}
	inline int selectedID() const {
		return listWidget()->currentRow();
	}
};

class TreeWidget : public AbstractListWidget
{
	Q_OBJECT
public:
	explicit TreeWidget(QWidget *parent = nullptr);
	
	inline QTreeWidget *treeWidget() const {
		return (QTreeWidget *)widget();
	}
	inline int selectedID() const {
		QTreeWidgetItem *item = treeWidget()->currentItem();
		return item == nullptr ? -1 : item->data(0, Qt::UserRole).toInt();
	}
	QTreeWidgetItem *findItem(int id) const;
};
