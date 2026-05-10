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
#include "JsmMethodList.h"
#include "widgets/HelpWidget.h"
#include "Field.h"

JsmMethodList::JsmMethodList(QWidget *parent) :
    QTreeWidget(parent), _field(nullptr), _groupID(0)
{
	setHeaderLabels(QStringList() << tr("Id") << tr("Script") << tr("Script label"));
	setMaximumWidth(
	    fontMetrics().boundingRect(QString(16, 'M')).width());
	setMinimumWidth(
	    fontMetrics().boundingRect(QString(8, 'M')).width());
	setAutoScroll(false);
	setIndentation(0);
	setUniformRowHeights(true);
	setAlternatingRowColors(true);
	setContextMenuPolicy(Qt::ActionsContextMenu);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setSortingEnabled(false);

	_renameMethodAction = new QAction(QIcon::fromTheme(QStringLiteral("document-edit")), tr("Rename method"), this);
	_renameMethodAction->setShortcut(QKeySequence("F2"));
	_renameMethodAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	_addMethodAction = new QAction(QIcon::fromTheme(QStringLiteral("list-add")), tr("Add method"), this);
	_addMethodAction->setShortcut(QKeySequence("Ctrl++"));
	_addMethodAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	_delMethodAction = new QAction(QIcon::fromTheme(QStringLiteral("list-remove")), tr("Delete method"), this);
	_delMethodAction->setShortcut(QKeySequence(Qt::Key_Delete));
	_delMethodAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	_delMethodAction->setEnabled(false);
	_cutMethodAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-cut")), tr("Cut method"), this);
	_cutMethodAction->setShortcut(QKeySequence("Ctrl+X"));
	_cutMethodAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	_cutMethodAction->setEnabled(false);
	_copyMethodAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-copy")), tr("Copy method"), this);
	_copyMethodAction->setShortcut(QKeySequence("Ctrl+C"));
	_copyMethodAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	_copyMethodAction->setEnabled(false);
	_pasteMethodAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-paste")), tr("Paste method"), this);
	_pasteMethodAction->setShortcut(QKeySequence("Ctrl+V"));
	_pasteMethodAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	_pasteMethodAction->setEnabled(false);

	connect(this, &JsmMethodList::itemDoubleClicked, this, qOverload<QTreeWidgetItem *, int>(&JsmMethodList::rename));
	connect(this, &JsmMethodList::itemSelectionChanged, this, &JsmMethodList::upDownEnabled);

	connect(_renameMethodAction, &QAction::triggered, this, qOverload<>(&JsmMethodList::rename));
	connect(_addMethodAction, &QAction::triggered, this, &JsmMethodList::add);
	connect(_delMethodAction, &QAction::triggered, this, &JsmMethodList::del);
	connect(_cutMethodAction, &QAction::triggered, this, &JsmMethodList::cut);
	connect(_copyMethodAction, &QAction::triggered, this, &JsmMethodList::copy);
	connect(_pasteMethodAction, &QAction::triggered, this, &JsmMethodList::paste);

	this->addAction(_renameMethodAction);
	this->addAction(_addMethodAction);
	this->addAction(_delMethodAction);
	QAction *separator = new QAction(this);
	separator->setSeparator(true);
	this->addAction(separator);
	this->addAction(_cutMethodAction);
	this->addAction(_copyMethodAction);
	this->addAction(_pasteMethodAction);

	_toolBar = new QToolBar(tr("&Method Editor"));
	_toolBar->setIconSize(QSize(14, 14));
	_toolBar->addAction(_addMethodAction);
	_addMethodAction->setStatusTip(tr("Add a method"));
	_toolBar->addAction(_delMethodAction);
	_delMethodAction->setStatusTip(tr("Remove a method"));

	setMinimumWidth(_toolBar->sizeHint().width());
}

void JsmMethodList::setEnabled(bool enabled)
{
	QTreeWidget::setEnabled(enabled);
	enableActions(enabled);
}

void JsmMethodList::enableActions(bool enabled)
{
	_toolBar->setEnabled(enabled);
	setContextMenuPolicy(enabled ? Qt::ActionsContextMenu : Qt::NoContextMenu);
}

void JsmMethodList::upDownEnabled()
{
	QList<int> ids = selectedIDs();

	if (ids.isEmpty() || (ids.size() == 1 && ids.first() == 0)) {
		_delMethodAction->setEnabled(false);
		_cutMethodAction->setEnabled(false);
		_copyMethodAction->setEnabled(!ids.isEmpty());
	} else {
		_delMethodAction->setEnabled(topLevelItemCount() > 0);
		_cutMethodAction->setEnabled(true);
		_copyMethodAction->setEnabled(true);
	}
}

void JsmMethodList::clear()
{
	QTreeWidget::clear();
	_field = nullptr;
	_groupID = 0;
}

void JsmMethodList::setField(Field *field)
{
	_field = field;
}

QList<QTreeWidgetItem *> JsmMethodList::nameList(int groupID) const
{
	QList<QTreeWidgetItem *> items;
	QTreeWidgetItem *item;
	int count;
	QString name;
	const JsmScripts &scripts = _field->getJsmFile()->getScripts();

	if (scripts.groups().size() <= groupID) {
		qWarning() << "JsmFile::methodList error 1" << groupID << scripts.groups().size();
		return items;
	}

	const JsmGroup &jsmGroup = scripts.group(groupID);
	count = jsmGroup.methodCount();

	for (int methodID = 0; methodID < count; ++methodID) {
		const JsmMethod &jsmMethod = jsmGroup.method(methodID);
		item = new QTreeWidgetItem(QStringList() << QString("%1").arg(methodID, 3) << jsmMethod.name() << QString("%1").arg(jsmGroup.absMethodId() + methodID, 3));
		item->setData(0, Qt::UserRole, methodID);
		items.append(item);
	}

	return items;
}

void JsmMethodList::fill(int groupID)
{
	QTreeWidget::clear();

	if (_field == nullptr) {
		return;
	}

	_groupID = groupID;

	addTopLevelItems(nameList(groupID));
	scrollToTop();
	resizeColumnToContents(0);
	resizeColumnToContents(1);
	resizeColumnToContents(2);
	upDownEnabled();

	_renameMethodAction->setEnabled(true);
	_addMethodAction->setEnabled(topLevelItemCount() < 256);
}

/* void JsmMethodList::localeRefresh()
{
	if (_field == nullptr) {
		return;
	}

	int methodID = selectedID();
	QTreeWidgetItem *currentItem = this->currentItem();
	if (methodID >= 0 && methodID < _scripts->grpScriptCount() && currentItem != nullptr) {
		const GrpScript &currentGrpScript = _scripts->grpScript(methodID);
		currentItem->setText(2, currentGrpScript.typeString());
		currentItem->setForeground(2, currentGrpScript.typeColor());

		updateHelpWidget();
	}
} */

void JsmMethodList::rename(QTreeWidgetItem *item, int column)
{
	if (item == nullptr || column != 1 || item->data(0, Qt::UserRole).toInt() == 0) {
		return;
	}

	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
	editItem(item, column);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	connect(this, &JsmMethodList::itemChanged, this, &JsmMethodList::renameOK);
}

void JsmMethodList::renameOK(QTreeWidgetItem *item, int column)
{
	if (column != 1 || _field == nullptr) {
		return;
	}
	disconnect(this, &JsmMethodList::itemChanged, this, &JsmMethodList::renameOK);
	QString newName = item->text(column).left(15);
	if (newName != item->text(column)) {
		item->setText(column, newName);
	}

	JsmScripts &scripts = _field->getJsmFile()->getScripts();
	if (_groupID >= scripts.groups().size()) {
		return;
	}

	JsmGroup &jsmGroup = scripts.group(_groupID);
	int methodID = selectedID();
	if (methodID >= 1 && scripts.setMethodName(_groupID, methodID, newName)) {
		emit modified();
	} else {
		QMessageBox::warning(this, tr("Name already exist"), tr("This name already exist, please choose another"));
	}
}

void JsmMethodList::add()
{
	if (_field == nullptr || topLevelItemCount() > 255) {
		return;
	}

	int methodID = std::max(1, selectedID() + 1);

	QDialog dialog(this);
	QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
	connect(buttons, SIGNAL(accepted()), &dialog, SLOT(accept()));
	connect(buttons, SIGNAL(rejected()), &dialog, SLOT(reject()));
	QLineEdit *nameLineEdit = new QLineEdit(QString("Method%1").arg(methodID), &dialog);
	nameLineEdit->setMaxLength(15);
	QFormLayout *formLayout = new QFormLayout();
	formLayout->addRow(tr("&Name:"), nameLineEdit);
	QVBoxLayout *layout = new QVBoxLayout(&dialog);
	layout->addLayout(formLayout);
	layout->addWidget(buttons);

	if (dialog.exec() != QDialog::Accepted) {
		return;
	}

	JsmScripts &scripts = _field->getJsmFile()->getScripts();

	if (scripts.insertMethod(_groupID, methodID, nameLineEdit->text())) {
		fill();
		scroll(methodID);
		emit modified();
	} else {
		QMessageBox::warning(this, tr("Name already exist"), tr("This name already exist, please choose another"));
	}
}

void JsmMethodList::del(bool totalDel)
{
	if (_field == nullptr || topLevelItemCount() == 0) {
		return;
	}
	QList<int> selectedIDs = this->selectedIDs();
	if (selectedIDs.isEmpty()) {
		return;
	}

	if (totalDel && QMessageBox::warning(this, tr("Delete"), tr("Are you sure you want to remove %1?\n"
	                                                           "Some scripts can refer to it!")
	                                    .arg(selectedIDs.size() == 1 ?
	                                         tr("the method selected") :
	                                         tr("the selected methods")),
	                                    QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Cancel) {
		return;
	}

	JsmScripts &scripts = _field->getJsmFile()->getScripts();
	std::sort(selectedIDs.begin(), selectedIDs.end());
	for (int i = selectedIDs.size() - 1; i >= 0; --i) {
		if (selectedIDs.at(i) != 0) {
			scripts.removeMethod(_groupID, selectedIDs.at(i));
		}
	}

	blockSignals(true);
	fill();
	blockSignals(false);

	emit modified();

	if (topLevelItemCount() != 0) {
		if (selectedIDs.at(0) >= topLevelItemCount() && selectedIDs.at(0) > 0) {
			scroll(selectedIDs.at(0) - 1);
		} else if (selectedIDs.at(0) < topLevelItemCount()) {
			scroll(selectedIDs.at(0));
		}
	}
}

void JsmMethodList::cut()
{
	copy();
	del(false);
}

void JsmMethodList::copy()
{
	if (_field == nullptr) {
		return;
	}
	QList<int> selectedIDs = this->selectedIDs();
	if (selectedIDs.isEmpty()) {
		return;
	}

	clearCopiedMethods();
	const JsmScripts &scripts = _field->getJsmFile()->getScripts();

	if (_groupID >= scripts.groups().size()) {
		return;
	}

	const JsmGroup &jsmGroup = scripts.group(_groupID);
	for (const int id: selectedIDs) {
		if (id >= jsmGroup.methodCount()) {
			continue;
		}
		_methodCopied.append(jsmGroup.method(id));
	}

	_pasteMethodAction->setEnabled(true);
}

void JsmMethodList::paste()
{
	if (_field == nullptr || _methodCopied.isEmpty()) {
		return;
	}
	int methodID = selectedID() + 1;
	if (methodID == 0) {
		methodID = topLevelItemCount(); // Last position
	}
	int i = std::max(1, methodID), num = 1;
	JsmScripts &scripts = _field->getJsmFile()->getScripts();

	if (_groupID >= scripts.groups().size()) {
		return;
	}

	for (JsmMethod methodCopied: std::as_const(_methodCopied)) {
		QString methodName = methodCopied.name();
		while (!scripts.canSetMethodName(_groupID, methodName)) {
			methodName = QString("%1%2").arg(methodCopied.name()).arg(num++);
		}
		methodCopied.setNameUnsafe(methodName);
		scripts.insertMethod(_groupID, i++, methodCopied);
	}

	fill();
	scroll(methodID);
	emit modified();
}

void JsmMethodList::clearCopiedMethods()
{
	_pasteMethodAction->setEnabled(false);
	_methodCopied.clear();
}

void JsmMethodList::scroll(int id, bool focus)
{
	if (selectedID() != id) {
		QTreeWidgetItem *item = findItem(id);
		if (!item) {
			return;
		}
		setCurrentItem(item);
		scrollToItem(item, QAbstractItemView::PositionAtTop);
	}

	if (focus) {
		setFocus();
	}
}

QTreeWidgetItem *JsmMethodList::findItem(int id) const
{
	QList<QTreeWidgetItem *> items = findItems(QString("%1").arg(id, 3), Qt::MatchExactly);
	if (items.isEmpty()) {
		return nullptr;
	}
	return items.first();
}

int JsmMethodList::selectedID() const
{
	QList<QTreeWidgetItem *> items = selectedItems();
	if (items.isEmpty())	return -1;

	return items.first()->data(0, Qt::UserRole).toInt();
}

QList<int> JsmMethodList::selectedIDs() const
{
	QList<int> list;
	QList<QTreeWidgetItem *> selItems = selectedItems();

	for (const QTreeWidgetItem *item: selItems) {
		list.append(item->data(0, Qt::UserRole).toInt());
	}

	return list;
}
