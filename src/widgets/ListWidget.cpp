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
#include "widgets/ListWidget.h"

AbstractListWidget::AbstractListWidget(QWidget *widget, QWidget *parent) :
	QWidget(parent), _widget(widget)
{
	setContextMenuPolicy(Qt::ActionsContextMenu);
	setFixedWidth(120);

	_toolBar = new QToolBar(this);
	_toolBar->setIconSize(QSize(16, 16));

	_widget->setParent(this);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(_toolBar);
	layout->addWidget(_widget);
	layout->setContentsMargins(QMargins());
}

QAction *AbstractListWidget::addAction(ActionType type, const QString &text,
                                       const QObject *receiver, const char *member)
{
	QIcon icon;
	QKeySequence shortcut;
	QAction *action;
	bool inToolbar = false;

	switch (type) {
	case Add:
		icon = QIcon::fromTheme(QStringLiteral("list-add"));
		shortcut = QKeySequence("Ctrl++");
		inToolbar = true;
		break;
	case Invisible:
		action = new QAction(text, this);
		action->setStatusTip(text);
		QWidget::addAction(action);
		connect(action, SIGNAL(triggered()), receiver, member);
		return action;
	case Rem:
		icon = QIcon::fromTheme(QStringLiteral("list-remove"));
		shortcut = QKeySequence::Delete;
		inToolbar = true;
		break;
	case Up:
		icon = QIcon::fromTheme(QStringLiteral("go-up"));
		shortcut = QKeySequence("Shift+Up");
		inToolbar = true;
		break;
	case Down:
		icon = QIcon::fromTheme(QStringLiteral("go-down"));
		shortcut = QKeySequence("Shift+Down");
		inToolbar = true;
		break;
	case Copy:
		icon = QIcon::fromTheme(QStringLiteral("edit-copy"));
		shortcut = QKeySequence::Copy;
		break;
	case Cut:
		icon = QIcon::fromTheme(QStringLiteral("edit-cut"));
		shortcut = QKeySequence::Cut;
		break;
	case Paste:
		icon = QIcon::fromTheme(QStringLiteral("edit-paste"));
		shortcut = QKeySequence::Paste;
		break;
	}

	action = QWidget::addAction(icon, text, receiver, member);
	action->setShortcut(shortcut);
	action->setStatusTip(text);

	if (inToolbar) {
		_toolBar->addAction(action);
	}

	QWidget::addAction(action);
	_widget->addAction(action);

	return action;
}

QToolBar *AbstractListWidget::toolBar() const
{
	return _toolBar;
}

ListWidget::ListWidget(QWidget *parent) :
	AbstractListWidget(new QListWidget, parent)
{
	listWidget()->setUniformItemSizes(true);
}

TreeWidget::TreeWidget(QWidget *parent) :
	AbstractListWidget(new QTreeWidget, parent)
{
	treeWidget()->setAutoScroll(false);
	treeWidget()->setIndentation(0);
	treeWidget()->setUniformRowHeights(true);
	treeWidget()->setAlternatingRowColors(true);
	treeWidget()->setContextMenuPolicy(Qt::ActionsContextMenu);
	treeWidget()->sortByColumn(0, Qt::AscendingOrder);
}

QTreeWidgetItem *TreeWidget::findItem(int id) const
{
	int count = treeWidget()->topLevelItemCount();
	for (int i = 0; i < count; ++i) {
		QTreeWidgetItem *item = treeWidget()->topLevelItem(i);
		if (item->data(0, Qt::UserRole).toInt() == id) {
			return item;
		}
	}
	return nullptr;
}
