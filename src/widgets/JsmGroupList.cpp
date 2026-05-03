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
#include "JsmGroupList.h"
#include "widgets/HelpWidget.h"
#include "Field.h"

JsmGroupList::JsmGroupList(QWidget *parent) :
    QTreeWidget(parent), _field(nullptr)
{
	setHeaderLabels(QStringList() << tr("Id") << tr("Entity"));
	setMaximumWidth(
	    fontMetrics().boundingRect(QString(20, 'M')).width());
	setMinimumWidth(
	    fontMetrics().boundingRect(QString(8, 'M')).width());
	setAutoScroll(false);
	setIndentation(0);
	setUniformRowHeights(true);
	setAlternatingRowColors(true);
	setContextMenuPolicy(Qt::ActionsContextMenu);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	sortByColumn(0, Qt::AscendingOrder);

	_renameGroupAction = new QAction(QIcon::fromTheme(QStringLiteral("document-edit")), tr("Rename group"), this);
	_renameGroupAction->setShortcut(QKeySequence("F2"));
	_renameGroupAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	_addGroupAction = new QAction(QIcon::fromTheme(QStringLiteral("list-add")), tr("Add group"), this);
	_addGroupAction->setShortcut(QKeySequence("Ctrl++"));
	_addGroupAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	_delGroupAction = new QAction(QIcon::fromTheme(QStringLiteral("list-remove")), tr("Delete group"), this);
	_delGroupAction->setShortcut(QKeySequence(Qt::Key_Delete));
	_delGroupAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	_delGroupAction->setEnabled(false);
	/* _cutGroupAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-cut")), tr("Cut group"), this);
	_cutGroupAction->setShortcut(QKeySequence("Ctrl+X"));
	_cutGroupAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	_cutGroupAction->setEnabled(false);
	_copyGroupAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-copy")), tr("Copy group"), this);
	_copyGroupAction->setShortcut(QKeySequence("Ctrl+C"));
	_copyGroupAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	_copyGroupAction->setEnabled(false);
	_pasteGroupAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-paste")), tr("Paste group"), this);
	_pasteGroupAction->setShortcut(QKeySequence("Ctrl+V"));
	_pasteGroupAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	_pasteGroupAction->setEnabled(false);
	_upGroupAction = new QAction(QIcon::fromTheme(QStringLiteral("go-up")), tr("Move up"), this);
	_upGroupAction->setShortcut(QKeySequence("Shift+Up"));
	_upGroupAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	_upGroupAction->setEnabled(false);
	_downGroupAction = new QAction(QIcon::fromTheme(QStringLiteral("go-down")), tr("Move down"), this);
	_downGroupAction->setShortcut(QKeySequence("Shift+Down"));
	_downGroupAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	_downGroupAction->setEnabled(false); */

	connect(this, &JsmGroupList::itemDoubleClicked, this, qOverload<QTreeWidgetItem *, int>(&JsmGroupList::rename));
	connect(this, &JsmGroupList::itemSelectionChanged, this, &JsmGroupList::upDownEnabled);

	connect(_renameGroupAction, &QAction::triggered, this, qOverload<>(&JsmGroupList::rename));
	connect(_addGroupAction, &QAction::triggered, this, &JsmGroupList::add);
	connect(_delGroupAction, &QAction::triggered, this, &JsmGroupList::del);
	/* connect(_cutGroupAction, &QAction::triggered, this, &JsmGroupList::cut);
	connect(_copyGroupAction, &QAction::triggered, this, &JsmGroupList::copy);
	connect(_pasteGroupAction, &QAction::triggered, this, &JsmGroupList::paste);
	connect(_upGroupAction, &QAction::triggered, this, &JsmGroupList::up);
	connect(_downGroupAction, &QAction::triggered, this, &JsmGroupList::down); */

	this->addAction(_renameGroupAction);
	this->addAction(_addGroupAction);
	this->addAction(_delGroupAction);
	/* QAction *separator = new QAction(this);
	separator->setSeparator(true);
	this->addAction(separator);
	this->addAction(_cutGroupAction);
	this->addAction(_copyGroupAction);
	this->addAction(_pasteGroupAction);
	QAction *separator2 = new QAction(this);
	separator2->setSeparator(true);
	this->addAction(separator2);
	this->addAction(_upGroupAction);
	this->addAction(_downGroupAction); */

	_toolBar = new QToolBar(tr("&Group Editor"));
	_toolBar->setIconSize(QSize(14, 14));
	_toolBar->addAction(_addGroupAction);
	_addGroupAction->setStatusTip(tr("Add a group"));
	_toolBar->addAction(_delGroupAction);
	_delGroupAction->setStatusTip(tr("Remove a group"));
	/* _toolBar->addSeparator();
	_toolBar->addAction(_upGroupAction);
	_upGroupAction->setStatusTip(tr("Up"));
	_toolBar->addAction(_downGroupAction);
	_downGroupAction->setStatusTip(tr("Down")); */

	_helpWidget = new HelpWidget(32, HelpWidget::IconWarning, this);
	_helpWidget->hide();
	_helpWidget->setMaximumWidth(maximumWidth());

	setMinimumWidth(_toolBar->sizeHint().width());
}

void JsmGroupList::setEnabled(bool enabled)
{
	QTreeWidget::setEnabled(enabled);
	enableActions(enabled);
}

void JsmGroupList::enableActions(bool enabled)
{
	_toolBar->setEnabled(enabled);
	setContextMenuPolicy(enabled ? Qt::ActionsContextMenu : Qt::NoContextMenu);
}

void JsmGroupList::upDownEnabled()
{
	if (selectedItems().isEmpty()) {
		_delGroupAction->setEnabled(false);
		_cutGroupAction->setEnabled(false);
		_copyGroupAction->setEnabled(false);
		/* _upGroupAction->setEnabled(false);
		_downGroupAction->setEnabled(false); */
	} else {
		_delGroupAction->setEnabled(topLevelItemCount() > 0);
		_cutGroupAction->setEnabled(true);
		_copyGroupAction->setEnabled(true);
		/* _upGroupAction->setEnabled(topLevelItemCount() > 1 && currentItem() != topLevelItem(0));
		_downGroupAction->setEnabled(topLevelItemCount() > 1 && currentItem() != topLevelItem(topLevelItemCount()-1)); */
	}
}

void JsmGroupList::clear()
{
	QTreeWidget::clear();
	_field = nullptr;
}

void JsmGroupList::setField(Field *field)
{
	_field = field;
}

QList<QTreeWidgetItem *> JsmGroupList::nameList() const
{
	QList<QTreeWidgetItem *> items;
	QTreeWidgetItem *item;
	const JsmScripts &scripts = _field->getJsmFile()->getScripts();
	int nbGroup = scripts.nbGroup();

	for (int groupID = 0; groupID < nbGroup; ++groupID) {
		const JsmGroup &grp = scripts.group(groupID);
		item = new QTreeWidgetItem(QStringList() << QString("%1").arg(groupID, 3) << grp.name());
		item->setData(0, Qt::UserRole, groupID);
		switch (grp.type()) {
		case JsmGroup::Main:
			item->setIcon(0, QIcon(":/images/main.png"));
			break;
		case JsmGroup::Model:
			switch (grp.character()) {
			case 0:
				item->setIcon(0, QIcon(":/images/icon-squall.png"));
				break;
			case 1:
				item->setIcon(0, QIcon(":/images/icon-zell.png"));
				break;
			case 2:
				item->setIcon(0, QIcon(":/images/icon-irvine.png"));
				break;
			case 3:
				item->setIcon(0, QIcon(":/images/icon-quistis.png"));
				break;
			case 4:
				item->setIcon(0, QIcon(":/images/icon-rinoa.png"));
				break;
			case 5:
				item->setIcon(0, QIcon(":/images/icon-selphie.png"));
				break;
			case 6:
				item->setIcon(0, QIcon(":/images/icon-seifer.png"));
				break;
			case 7:
				item->setIcon(0, QIcon(":/images/icon-edea.png"));
				break;
			case 8:
				item->setIcon(0, QIcon(":/images/icon-laguna.png"));
				break;
			case 9:
				item->setIcon(0, QIcon(":/images/icon-kiros.png"));
				break;
			case 10:
				item->setIcon(0, QIcon(":/images/icon-ward.png"));
				break;
			case -1:
				item->setIcon(0, QIcon(":/images/3d_model.png"));
				break;
			case DRAWPOINT_CHARACTER:
				item->setIcon(0, QIcon(":/images/icon-drawpoint.png"));
				break;
			default:
				item->setIcon(0, QIcon(":/images/icon-unknown.png"));
				break;
			}
			break;
		case JsmGroup::Location:
			item->setIcon(0, QIcon(":/images/location.png"));
			break;
		case JsmGroup::Door:
			item->setIcon(0, QIcon(":/images/door.png"));
			break;
		case JsmGroup::Background:
			item->setIcon(0, QIcon(":/images/background.png"));
			break;
		default:
			QPixmap pixnull(32, 32);
			pixnull.fill(Qt::transparent);
			item->setIcon(0, QIcon(pixnull));
			break;
		}
		items.append(item);
	}

	return items;
}

void JsmGroupList::fill()
{
	QTreeWidget::clear();

	if (_field == nullptr) {
		return;
	}

	addTopLevelItems(nameList());
	scrollToTop();
	resizeColumnToContents(0);
	resizeColumnToContents(1);
	resizeColumnToContents(2);

	updateHelpWidget();

	_renameGroupAction->setEnabled(true);
	_addGroupAction->setEnabled(topLevelItemCount() < 256);
	upDownEnabled();
}

/* void JsmGroupList::localeRefresh()
{
	if (_field == nullptr) {
		return;
	}

	int grpScriptID = selectedID();
	QTreeWidgetItem *currentItem = this->currentItem();
	if (grpScriptID >= 0 && grpScriptID < _scripts->grpScriptCount() && currentItem != nullptr) {
		const GrpScript &currentGrpScript = _scripts->grpScript(grpScriptID);
		currentItem->setText(2, currentGrpScript.typeString());
		currentItem->setForeground(2, currentGrpScript.typeColor());

		updateHelpWidget();
	}
} */

void JsmGroupList::updateHelpWidget()
{
	if (_field == nullptr) {
		return;
	}

	_helpWidget->hide();
	QStringList texts;
	const JsmScripts &scripts = _field->getJsmFile()->getScripts();

	/* if (scripts.countModels() > 16) {
		_helpWidget->show();
		texts.append(tr("You have more than 16 models in this field, "
		                "the game may crash if all models are visible at the same time."));
	} */

	if (scripts.nbGroup() > 48) {
		_helpWidget->show();
		texts.append(tr("You have more than 48 groups in this field, "
		                                  "the game may crash."));
	}

	_helpWidget->setText(texts.join("\n"));
}

void JsmGroupList::rename(QTreeWidgetItem *item, int column)
{
	if (item==nullptr || column != 1) {
		return;
	}

	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
	editItem(item, column);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	connect(this, &JsmGroupList::itemChanged, this, &JsmGroupList::renameOK);
}

void JsmGroupList::renameOK(QTreeWidgetItem *item, int column)
{
	if (column != 1 || _field == nullptr) {
		return;
	}
	disconnect(this, &JsmGroupList::itemChanged, this, &JsmGroupList::renameOK);
	QString newName = item->text(column).left(8);
	if (newName != item->text(column)) {
		item->setText(column, newName);
	}

	JsmScripts &scripts = _field->getJsmFile()->getScripts();
	int groupID = item->text(0).toInt();
	if (groupID >= 0 && scripts.group(groupID).name() != newName) {
		scripts.setGroupName(groupID, newName);
		emit modified();
	}
}

void JsmGroupList::add()
{
	if (_field == nullptr || topLevelItemCount() > 255) {
		return;
	}

	int grpScriptID = selectedID() + 1;
	JsmScripts &scripts = _field->getJsmFile()->getScripts();

	scripts.insertGroup(grpScriptID, JsmGroup::No, QString("module%1").arg(grpScriptID));
	fill();
	scroll(grpScriptID);
	emit modified();
	rename();
}

void JsmGroupList::del(bool totalDel)
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
	                                    .arg(selectedIDs.size()==1 ?
	                                         tr("the group selected") :
	                                         tr("the selected groups")),
	                                    QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Cancel) {
		return;
	}

	JsmScripts &scripts = _field->getJsmFile()->getScripts();
	std::sort(selectedIDs.begin(), selectedIDs.end());
	for (int i = selectedIDs.size() - 1; i >= 0; --i) {
		scripts.removeGroup(i);
	}

	blockSignals(true);
	fill();
	blockSignals(false);

	emit modified();

	if (topLevelItemCount() != 0) {
		if (selectedIDs.at(0) >= topLevelItemCount() && selectedIDs.at(0) > 0) {
			scroll(selectedIDs.at(0)-1);
		} else if (selectedIDs.at(0) < topLevelItemCount()) {
			scroll(selectedIDs.at(0));
		}
	}
}

void JsmGroupList::cut()
{
	copy();
	del(false);
}

void JsmGroupList::copy()
{
	if (_field == nullptr) {
		return;
	}
	QList<int> selectedIDs = this->selectedIDs();
	if (selectedIDs.isEmpty()) {
		return;
	}

	clearCopiedGroups();
	/* const JsmScripts &scripts = _field->getJsmFile()->getScripts();
	for (const int id: selectedIDs) {
		_grpScriptCopied.append(scripts.group(id));
	} */

	_pasteGroupAction->setEnabled(true);
}

void JsmGroupList::paste()
{
	if (_field == nullptr /* || _grpScriptCopied.isEmpty() */) {
		return;
	}
	int grpScriptID = selectedID() + 1;
	if (grpScriptID == 0) {
		grpScriptID = topLevelItemCount(); // Last position
	}
	/* int i = grpScriptID;
	JsmScripts &scripts = _field->getJsmFile()->getScripts();
	for (const JsmGroup &GScopied : std::as_const(_grpScriptCopied)) {
		scripts.insertGroup(i++, GScopied);
	} */

	fill();
	scroll(grpScriptID);
	emit modified();
}

void JsmGroupList::clearCopiedGroups()
{
	_pasteGroupAction->setEnabled(false);
	//_grpScriptCopied.clear();
}

void JsmGroupList::move(bool direction)
{
	if (_field == nullptr) {
		return;
	}
	int grpScriptID = selectedID();
	if (grpScriptID == -1) {
		return;
	}
	/* if (_scripts->moveGrpScript(grpScriptID, direction)) {
		fill();
		scroll(direction ? grpScriptID + 1 : grpScriptID - 1);
		emit modified();
	} else {
		setFocus();
	} */
}

void JsmGroupList::scroll(int id, bool focus)
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

QTreeWidgetItem *JsmGroupList::findItem(int id) const
{
	QList<QTreeWidgetItem *> items = findItems(QString("%1").arg(id, 3), Qt::MatchExactly);
	if (items.isEmpty()) {
		return nullptr;
	}
	return items.first();
}

int JsmGroupList::selectedID() const
{
	QList<QTreeWidgetItem *> items = selectedItems();
	if (items.isEmpty())	return -1;

	return items.first()->data(0, Qt::UserRole).toInt();
}

QList<int> JsmGroupList::selectedIDs() const
{
	QList<int> list;
	QList<QTreeWidgetItem *> selItems = selectedItems();

	for (const QTreeWidgetItem *item: selItems) {
		list.append(item->data(0, Qt::UserRole).toInt());
	}

	return list;
}
