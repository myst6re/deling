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
#include "widgets/JsmWidget.h"
#include "WalkmeshGLWidget.h"
#include "Config.h"

JsmWidget::JsmWidget(QWidget *parent)
    : PageWidget(parent), mainModels(nullptr), fieldArchive(nullptr),
      _regText(QRegExp("\\btext_(\\d+)\\b")), _regMap(QRegExp("\\bmap_(\\d+)\\b")),
      _regSetLine(QRegExp("setline\\(\\s*(-?\\d+)\\s*,\\s*(-?\\d+)\\s*,\\s*(-?\\d+)\\s*,\\s*(-?\\d+)\\s*,\\s*(-?\\d+)\\s*,\\s*(-?\\d+)\\s*\\)")),
      groupID(-1), methodID(-1)
{
}

void JsmWidget::build()
{
	if(isBuilded())	return;

	QFont font;
	font.setPointSize(8);

	warningWidget = new QLabel(tr("Attention : Les scripts de cet écran sont dans un ancien format mal reconnu par Deling. Ce que vous pourrez lire ici n'aura peut-être aucun sens."), this);
	warningWidget->hide();
	warningWidget->setWordWrap(true);
	warningWidget->setTextFormat(Qt::PlainText);

	list1 = new QTreeWidget(this);
	list1->setHeaderLabels(QStringList() << tr("Id") << tr("Groupe") << tr("Exec"));
	list1->setFixedWidth(180);
	list1->setAutoScroll(false);
	list1->setIndentation(0);
	list1->setFont(font);
	list1->setUniformRowHeights(true);

	modelPreview = new CharaPreview(this);
	modelPreview->setFixedSize(list1->width(), list1->width());
	modelPreview->setMainModels(mainModels);

	QVBoxLayout *list1Layout = new QVBoxLayout();
	list1Layout->addWidget(list1, 1);
	list1Layout->addWidget(modelPreview, 0, Qt::AlignBottom);
	list1Layout->setContentsMargins(QMargins());

	list2 = new QTreeWidget(this);
	list2->setHeaderLabels(QStringList() << tr("Id") << tr("Script") << tr("Script label"));
	list2->setFixedWidth(180);
	list2->setAutoScroll(false);
	list2->setIndentation(0);
	list2->setFont(font);
	list2->setUniformRowHeights(true);

	tabBar = new QTabBar(this);
	tabBar->setDrawBase(false);
	tabBar->addTab(tr("Instructions"));
	tabBar->addTab(tr("Pseudo-code"));

	PlainTextEdit *te = new PlainTextEdit(this);
	textEdit = te->textEdit();
	QFont font2 = textEdit->document()->defaultFont();
	font2.setStyleHint(QFont::TypeWriter);
	font2.setFamily("Courrier");
	textEdit->document()->setDefaultFont(font2);
	highlighter = new JsmHighlighter(textEdit->document());
	// continue highlight when window is inactive
	QPalette pal = textEdit->palette();
	pal.setColor(QPalette::Inactive, QPalette::Highlight, pal.color(QPalette::Active, QPalette::Highlight));
	pal.setColor(QPalette::Inactive, QPalette::HighlightedText, pal.color(QPalette::Active, QPalette::HighlightedText));
	textEdit->setPalette(pal);

	errorLabel = new QLabel(this);
	errorLabel->setWordWrap(true);
	errorLabel->setTextFormat(Qt::PlainText);

	QWidget *errorWidget = new QWidget(this);

	QHBoxLayout *errorLayout = new QHBoxLayout(errorWidget);
	errorLayout->addStretch();
	errorLayout->addWidget(errorLabel);

	toolBar = new QToolBar(this);
	QAction *compileAction = toolBar->addAction(tr("Compiler"), this, SLOT(compile()));
	compileAction->setToolTip(tr("Compiler (Ctrl+B)"));
	compileAction->setStatusTip(tr("Compiler (Ctrl+B)"));
	compileAction->setShortcutContext(Qt::ApplicationShortcut);
	compileAction->setShortcut(QKeySequence("Ctrl+B"));
	toolBar->addWidget(errorWidget);
	toolBar->setEnabled(false);

	QGridLayout *mainLayout = new QGridLayout(this);
	mainLayout->addWidget(warningWidget, 0, 0, 1, 4);
	mainLayout->addLayout(list1Layout, 1, 0, 3, 1);
	mainLayout->addWidget(list2, 1, 1, 3, 1);
	mainLayout->addWidget(tabBar, 1, 2, 1, 2);
	mainLayout->addWidget(te, 2, 2);
	mainLayout->addWidget(textEdit, 2, 3);
	mainLayout->addWidget(toolBar, 3, 2, 1, 2);
	mainLayout->setContentsMargins(QMargins());

	tabBar->setCurrentIndex(Config::value("scriptType").toInt());

	connect(list1, SIGNAL(itemSelectionChanged()), SLOT(fillList2()));
	connect(list2, SIGNAL(itemSelectionChanged()), SLOT(fillTextEdit()));
	connect(tabBar, SIGNAL(currentChanged(int)), SLOT(fillTextEdit()));
	connect(textEdit, SIGNAL(lineHovered(QString,QPoint)), SLOT(showPreview(QString,QPoint)));

	PageWidget::build();
}

void JsmWidget::compile()
{
	groupID = currentItem(list1);
	methodID = currentItem(list2);
	if(groupID==-1 || methodID==-1)	return;

	QString errorStr;
	QPalette pal = errorLabel->palette();
	int l = data()->getJsmFile()->fromString(groupID, methodID, textEdit->toPlainText(), errorStr);
	if(l != 0) {
		pal.setColor(QPalette::Active, QPalette::ButtonText, Qt::darkRed);
		pal.setColor(QPalette::Inactive, QPalette::ButtonText, Qt::darkRed);
		errorLabel->setPalette(pal);
		errorLabel->setText(l != -1 ? tr("Ligne %1 -> %2").arg(l).arg(errorStr) : errorStr);
	} else {
		pal.setColor(QPalette::Active, QPalette::ButtonText, Qt::darkGreen);
		pal.setColor(QPalette::Inactive, QPalette::ButtonText, Qt::darkGreen);
		errorLabel->setPalette(pal);
		errorLabel->setText(tr("Compilé avec succès"));
		emit modified();
	}
}

void JsmWidget::clear()
{
	if(!isFilled())		return;

	list1->blockSignals(true);
	list2->blockSignals(true);

	if(hasData() && data()->hasJsmFile())	saveSession();

	list1->clear();
	modelPreview->clear();
	list2->clear();
	textEdit->clear();
	errorLabel->clear();
	warningWidget->hide();

	list1->blockSignals(false);
	list2->blockSignals(false);

	PageWidget::clear();
}

void JsmWidget::saveSession()
{
	if(!hasData() || !data()->hasJsmFile() || !isBuilded())	return;

	data()->getJsmFile()->setCurrentOpcodeScroll(this->groupID, this->methodID, textEdit->verticalScrollBar()->value(), textEdit->textCursor());
	if(textEdit->document()->isModified()) {
		data()->getJsmFile()->setDecompiledScript(this->groupID,
		                                          this->methodID,
		                                          textEdit->toPlainText(),
		                                          false);
	}
}

void JsmWidget::setReadOnly(bool readOnly)
{
	if(isBuilded()) {
		textEdit->setReadOnly(readOnly);
		toolBar->setEnabled(!readOnly);
	}

	PageWidget::setReadOnly(readOnly);
}

void JsmWidget::setData(Field *field)
{
	if(hasData() && data()->hasJsmFile()) {
		saveSession();
		this->methodID = this->groupID = -1;
	}

	PageWidget::setData(field);
}

void JsmWidget::setMainModels(QHash<int, CharaModel *> *mainModels)
{
	this->mainModels = mainModels;
	if(isBuilded())
		modelPreview->setMainModels(mainModels);
}

void JsmWidget::setFieldArchive(FieldArchive *fieldArchive)
{
	this->fieldArchive = fieldArchive;
}

void JsmWidget::fill()
{
	if(!isBuilded())	build();

	if(isFilled())		clear();

//	qDebug() << "JsmWidget::fill()";

	if(!hasData() || !data()->hasJsmFile())		return;

	int groupID = data()->getJsmFile()->currentGroupItem();

	modelPreview->setMainModels(mainModels);

	list1->addTopLevelItems(nameList());
	list1->scrollToTop();
	list1->resizeColumnToContents(0);
	list1->resizeColumnToContents(1);
	list1->resizeColumnToContents(2);

	if(data()->getJsmFile()->oldFormat()) {
		warningWidget->show();
	}

	QTreeWidgetItem *item = list1->topLevelItem(groupID);
	if(item) 	list1->setCurrentItem(item);

	PageWidget::fill();
}

void JsmWidget::fillList2()
{
//	qDebug() << QString("JsmWidget::fillList2(%1)").arg(currentItem(list1));

	list2->clear();
	int groupID = currentItem(list1);
	if(groupID==-1)	return;
	int methodID = data()->getJsmFile()->currentMethodItem(groupID);

	list1->scrollToItem(list1->currentItem());

	list2->addTopLevelItems(methodList(groupID));
	list2->scrollToTop();
	list2->resizeColumnToContents(0);
	list2->resizeColumnToContents(1);

	const JsmGroup &group = data()->getJsmFile()->getScripts().group(groupID);
	int modelID = group.modelId(), bgParamID = group.backgroundParamId();

	if(modelID != -1 && data()->hasCharaFile()
			&& modelID < data()->getCharaFile()->modelCount()) {
		modelPreview->setEnabled(true);
		modelPreview->setModel(data()->getCharaFile()->model(modelID));
	} else if(bgParamID != -1 && data()->hasBackgroundFile()
			  && data()->getBackgroundFile()->allparams.contains(bgParamID)) {
		modelPreview->setEnabled(true);
		modelPreview->fill(QPixmap::fromImage(data()->getBackgroundFile()->background(QList<quint8>() << bgParamID, true)));
	} else {
		modelPreview->setEnabled(false);
		modelPreview->clear();
	}

	QTreeWidgetItem *item = list2->topLevelItem(methodID);
	if(item) 	list2->setCurrentItem(item);
}

void JsmWidget::fillTextEdit()
{
//	qDebug() << QString("JsmWidget::fillTextEdit(%1, %2)").arg(currentItem(list1)).arg(currentItem(list2));

	saveSession();
	textEdit->previewWidget()->hide();
	Config::setValue("scriptType", tabBar->currentIndex());
	groupID = currentItem(list1);
	methodID = currentItem(list2);
	if(groupID==-1 || methodID==-1) {
		textEdit->clear();
		toolBar->setEnabled(false);
		return;
	}

	list2->scrollToItem(list2->currentItem());

	if(tabBar->currentIndex() > 0) {
		toolBar->setEnabled(false);
		textEdit->setReadOnly(true);
		highlighter->setPseudoCode(true);
		textEdit->setPlainText(data()->getJsmFile()->toString(groupID, methodID, true, data()));
	} else {
		toolBar->setEnabled(!isReadOnly());
		textEdit->setReadOnly(isReadOnly());
		highlighter->setPseudoCode(false);
		textEdit->setPlainText(data()->getJsmFile()->toString(groupID, methodID, false, data()));

		int scroll = data()->getJsmFile()->currentOpcodeScroll(groupID, methodID);
		int anchor;
		int position = data()->getJsmFile()->currentTextCursor(groupID, methodID, anchor);

		if(position >= 0) {
			QTextCursor newCursor = textEdit->textCursor();
			newCursor.setPosition(anchor);
			newCursor.setPosition(position, QTextCursor::KeepAnchor);
			textEdit->setTextCursor(newCursor);
		}

		textEdit->verticalScrollBar()->setValue(scroll);
	}
}

void JsmWidget::showPreview(const QString &line, QPoint cursorPos)
{
	int posText = _regText.indexIn(line);
	PreviewWidget *preview = textEdit->previewWidget();

	if(posText >= 0) {
		int textId = _regText.capturedTexts().last().toInt();
		MsdFile *file = data()->getMsdFile();

		if(file && textId < file->nbText()) {

			FF8Window win = FF8Window();
			win.type = NOWIN;

			preview->show();
			preview->showText(file->data(textId), win);
			preview->move(cursorPos);

			return;
		}
	}

	int posMap = _regMap.indexIn(line);

	if(posMap >= 0 && fieldArchive) {
		int mapId = _regMap.capturedTexts().last().toInt();
		Field *field = fieldArchive->getFieldFromMapId(mapId);

		if(field != nullptr) {
			if(fieldArchive->openBG(field)) {
				BackgroundFile *file = field->getBackgroundFile();

				if(file) {
					preview->show();
					preview->showBackground(QPixmap::fromImage(file->background()));
					preview->move(cursorPos);

					return;
				}
			}
		}
	}

	int posLine = _regSetLine.indexIn(line);

	if(posLine >= 0) {
		QStringList texts = _regSetLine.capturedTexts();
		Vertex_s vertex[2];
		vertex[0].x = qint16(texts.at(1).toInt());
		vertex[0].y = qint16(texts.at(2).toInt());
		vertex[0].z = qint16(texts.at(3).toInt());
		vertex[1].x = qint16(texts.at(4).toInt());
		vertex[1].y = qint16(texts.at(5).toInt());
		vertex[1].z = qint16(texts.at(6).toInt());

		WalkmeshGLWidget walkmeshWidget;
		walkmeshWidget.hide();
		walkmeshWidget.fill(data());
		walkmeshWidget.setLineToDraw(vertex);
		QPixmap pixmap = walkmeshWidget.renderPixmap(320, 224);

		preview->show();
		preview->showBackground(pixmap);
		preview->move(cursorPos);

		return;
	}

	textEdit->previewWidget()->hide();
}

QList<QTreeWidgetItem *> JsmWidget::nameList() const
{
	QList<QTreeWidgetItem *> items;
	QTreeWidgetItem *item;
	int nbGroup = data()->getJsmFile()->getScripts().nbGroup();
	int directorCount=1, squallCount=1, zellCount=1, irvineCount=1, quistisCount=1;
	int rinoaCount=1, selphieCount=1, seiferCount=1, edeaCount=1, lagunaCount=1, kirosCount=1;
	int wardCount=1, drawPointCount=1, eventLineCount=1, doorCount=1;

	for(int groupID=0 ; groupID<nbGroup ; ++groupID) {
		const JsmGroup &grp = data()->getJsmFile()->getScripts().group(groupID);
		QString name = grp.name();
		item = new QTreeWidgetItem(QStringList() << QString("%1").arg(groupID, 3) << QString() << QString("%1").arg(grp.execOrder(), 3));
		item->setData(0, Qt::UserRole, groupID);
		switch(grp.type()) {
		case JsmGroup::Main:
			if(name.isEmpty())	name = QString("Director%1").arg(directorCount);
			directorCount++;
			item->setIcon(0, QIcon(":/images/main.png"));
			break;
		case JsmGroup::Model:
			switch(grp.character()) {
			case 0:
				if(name.isEmpty())	name = QString("Squall%1").arg(squallCount);
				squallCount++;
				item->setIcon(0, QIcon(":/images/icon-squall.png"));
				break;
			case 1:
				if(name.isEmpty())	name = QString("Zell%1").arg(zellCount);
				zellCount++;
				item->setIcon(0, QIcon(":/images/icon-zell.png"));
				break;
			case 2:
				if(name.isEmpty())	name = QString("Irvine%1").arg(irvineCount);
				irvineCount++;
				item->setIcon(0, QIcon(":/images/icon-irvine.png"));
				break;
			case 3:
				if(name.isEmpty())	name = QString("Quistis%1").arg(quistisCount);
				quistisCount++;
				item->setIcon(0, QIcon(":/images/icon-quistis.png"));
				break;
			case 4:
				if(name.isEmpty())	name = QString("Rinoa%1").arg(rinoaCount);
				rinoaCount++;
				item->setIcon(0, QIcon(":/images/icon-rinoa.png"));
				break;
			case 5:
				if(name.isEmpty())	name = QString("Selphie%1").arg(selphieCount);
				selphieCount++;
				item->setIcon(0, QIcon(":/images/icon-selphie.png"));
				break;
			case 6:
				if(name.isEmpty())	name = QString("Seifer%1").arg(seiferCount);
				seiferCount++;
				item->setIcon(0, QIcon(":/images/icon-seifer.png"));
				break;
			case 7:
				if(name.isEmpty())	name = QString("Edea%1").arg(edeaCount);
				edeaCount++;
				item->setIcon(0, QIcon(":/images/icon-edea.png"));
				break;
			case 8:
				if(name.isEmpty())	name = QString("Laguna%1").arg(lagunaCount);
				lagunaCount++;
				item->setIcon(0, QIcon(":/images/icon-laguna.png"));
				break;
			case 9:
				if(name.isEmpty())	name = QString("Kiros%1").arg(kirosCount);
				kirosCount++;
				item->setIcon(0, QIcon(":/images/icon-kiros.png"));
				break;
			case 10:
				if(name.isEmpty())	name = QString("Ward%1").arg(wardCount);
				wardCount++;
				item->setIcon(0, QIcon(":/images/icon-ward.png"));
				break;
			case -1:
				item->setIcon(0, QIcon(":/images/3d_model.png"));
				break;
			case DRAWPOINT_CHARACTER:
				if(name.isEmpty())	name = QString("DrawPoint%1").arg(drawPointCount);
				drawPointCount++;
				item->setIcon(0, QIcon(":/images/icon-drawpoint.png"));
				break;
			default:
				item->setIcon(0, QIcon(":/images/icon-unknown.png"));
				break;
			}
			break;
		case JsmGroup::Location:
			if(name.isEmpty())	name = QString("EventLine%1").arg(eventLineCount);
			eventLineCount++;
			item->setIcon(0, QIcon(":/images/location.png"));
			break;
		case JsmGroup::Door:
			if(name.isEmpty())	name = QString("Door%1").arg(doorCount);
			doorCount++;
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
		if(name.isEmpty()) 	name = QString("Module%1").arg(groupID);
		item->setText(1, name);
		items.append(item);
	}

	// qDebug() << data()->getJsmFile()->printCount();

	return items;
}

QList<QTreeWidgetItem *> JsmWidget::methodList(int groupID) const
{
	QList<QTreeWidgetItem *> items;
	QTreeWidgetItem *item;
	int begin, count;
	QString name;

	if(data()->getJsmFile()->getScripts().nbGroup()<=groupID) {
		qWarning() << "JsmFile::methodList error 1" << groupID << data()->getJsmFile()->getScripts().nbGroup();
		return items;
	}

	JsmGroup::Type groupType = data()->getJsmFile()->getScripts().group(groupID).type();

	begin = data()->getJsmFile()->getScripts().firstMethodID(groupID);
	count = data()->getJsmFile()->getScripts().nbScript(groupID);

	if(data()->getJsmFile()->getScripts().nbScript()<begin+count) {
		qWarning() << "JsmFile::methodList error 2" << data()->getJsmFile()->getScripts().nbScript() << (begin+count);
		return items;
	}

	for(int methodID=0 ; methodID<count ; ++methodID) {
		const JsmScript &script = data()->getJsmFile()->getScripts().script(groupID, methodID);
		if(methodID==0) {
			name = QString();
		}
		else {
			name = script.name();
			if(name.isEmpty()) {
				if(methodID==1) {
					name = "default";
				} else {
					switch(groupType) {
					case JsmGroup::Model:
						switch(methodID) {
						case 2:		name = "talk";		break;
						case 3:		name = "push";		break;
						}
						break;
					case JsmGroup::Location:
						switch(methodID) {
						case 2:		name = "talk";		break;
						case 3:		name = "push";		break;
						case 4:		name = "across";	break;
						case 5:		name = "touch";		break;
						case 6:		name = "touchoff";	break;
						case 7:		name = "touchon";	break;
						}
						break;
					case JsmGroup::Door:
						switch(methodID) {
						case 2:		name = "open";		break;
						case 3:		name = "close";		break;
						case 4:		name = "on";		break;
						case 5:		name = "off";		break;
						}
						break;
					default:
						break;
					}
					if(name.isEmpty())		name = QString("Method%1").arg(methodID);
				}
			}
		}
		item = new QTreeWidgetItem(QStringList() << QString("%1").arg(methodID, 3) << name << QString("%1").arg(begin+methodID, 3));
		item->setData(0, Qt::UserRole, methodID);
		if(script.flag()) {
			item->setForeground(0, Qt::darkGreen);
		}
		items.append(item);
	}

	return items;
}

int JsmWidget::currentItem(QTreeWidget *list)
{
	QList<QTreeWidgetItem *> items = list->selectedItems();
	if(items.isEmpty())	return -1;

	return items.first()->data(0, Qt::UserRole).toInt();
}

//void JsmWidget::jump(QTreeWidgetItem *item)
//{
//	QList<QTreeWidgetItem *> items;

//	if(item==NULL)	return;
//	bool ok;
//	uint pos, raw, key;

//	raw = item->text(2).toUInt(&ok, 16);
//	if(!ok) return;
//	key = raw>>24;

//	if(key == 2 || key == 3) {
//		pos = item->text(3).toUInt(&ok);
//		if(!ok) return;

//		if(key == 2)
//			pos += (qint16)(raw & 0x0000FFFF);
//		else
//			pos += raw & 0x0000FFFF;

//		items = list3->findItems(QString().setNum(pos), Qt::MatchExactly, 3);
//		if(items.isEmpty())	return;

//		item = list3->itemAbove(items.first());
//		list3->setCurrentItem(item);
//		list3->scrollToItem(item);
//	}
//	else if((key == 20 || key == 21 || key == 22) && list3->topLevelItemCount() > 2) {
//		int group = item->text(1).toUInt(&ok);
//		if(!ok) return;

//		int label = list3->itemAbove(item)->text(1).toUInt(&ok);
//		if(!ok) return;

//		gotoScriptLabel(group, label);
//	}
//}

void JsmWidget::gotoScript(int groupID, int methodID, int opcodeID)
{
	if(!isBuilded())	build();

	// Force opcode list
	tabBar->setCurrentIndex(0);

	QList<QTreeWidgetItem *> items = list1->findItems(QString("%1").arg(groupID, 3), Qt::MatchExactly);
	QTreeWidgetItem *item;
	if(items.isEmpty())	return;

	item = items.first();
	list1->setCurrentItem(item);
	list1->scrollToItem(item);

	items = list2->findItems(QString("%1").arg(methodID, 3), Qt::MatchExactly);
	if(items.isEmpty())	return;

	item = items.first();
	list2->setCurrentItem(item);
	list2->scrollToItem(item);

	QTextCursor cursor = textEdit->textCursor();
	cursor.setPosition(textEdit->document()->findBlockByLineNumber(data()->getJsmFile()->opcodePositionInText(groupID, methodID, opcodeID)).position());
	cursor.movePosition(QTextCursor::StartOfLine);
	cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
	textEdit->setTextCursor(cursor);
	textEdit->ensureCursorVisible();
}

//void JsmWidget::gotoScriptLabel(int groupID, int labelID)
//{
//	QList<QTreeWidgetItem *> items = list1->findItems(QString("%1").arg(groupID, 3), Qt::MatchExactly);
//	QTreeWidgetItem *item;
//	if(items.isEmpty())	return;

//	item = items.first();
//	list1->setCurrentItem(item);
//	list1->scrollToItem(item);

//	items = list2->findItems(QString("%1").arg(labelID, 3), Qt::MatchExactly, 2);
//	if(items.isEmpty())	return;

//	item = items.first();
//	list2->setCurrentItem(item);
//	list2->scrollToItem(item);

//	if(list3->topLevelItemCount()<1)	return;
//	list3->scrollToItem(list3->topLevelItem(0));
//}

int JsmWidget::selectedOpcode()
{
	if(!isBuilded())	return 0;

	QTextCursor cursor = textEdit->textCursor();
	cursor.movePosition(QTextCursor::StartOfWord);
	cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
	int index;
	if((index = JsmFile::opcodeName.indexOf(cursor.selectedText().toUpper())) != -1) {
		return index;
	}
	return 0;
}
