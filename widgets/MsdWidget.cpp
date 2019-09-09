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
#include "widgets/MsdWidget.h"

MsdWidget::MsdWidget(QWidget *parent)
	: PageWidget(parent)
{
}

void MsdWidget::build()
{
	if(isBuilded())	return;

	textPreview = new TextPreview();

	QAction *action;

	ListWidget *listWidget = new ListWidget(this);
	listWidget->addAction(ListWidget::Invisible, tr("Insérer un texte au-dessus"), this, SLOT(insertTextAbove()));
	listWidget->addAction(ListWidget::Add, tr("Insérer un texte en dessous"), this, SLOT(insertText()));
	actionRemoveText = listWidget->addAction(ListWidget::Rem, tr("Supprimer un texte"), this, SLOT(removeText()));

	toolBar0 = listWidget->toolBar();
	textList = listWidget->listWidget();

	toolBar = new QToolBar(this);
	toolBar->setIconSize(QSize(16, 16));
	action = toolBar->addAction(QIcon(":/images/icon-squall.png"), Data::name(0));
	action->setStatusTip(action->text());
	action->setData("{Squall}");
	action = toolBar->addAction(QIcon(":/images/icon-rinoa.png"), Data::name(4));
	action->setStatusTip(action->text());
	action->setData("{Rinoa}");
	action = toolBar->addAction(QIcon(":/images/icon-quistis.png"), Data::name(3));
	action->setStatusTip(action->text());
	action->setData("{Quistis}");
	action = toolBar->addAction(QIcon(":/images/icon-zell.png"), Data::name(1));
	action->setStatusTip(action->text());
	action->setData("{Zell}");
	action = toolBar->addAction(QIcon(":/images/icon-selphie.png"), Data::name(5));
	action->setStatusTip(action->text());
	action->setData("{Selphie}");
	action = toolBar->addAction(QIcon(":/images/icon-irvine.png"), Data::name(2));
	action->setStatusTip(action->text());
	action->setData("{Irvine}");
	action = toolBar->addAction(QIcon(":/images/icon-seifer.png"), Data::name(6));
	action->setStatusTip(action->text());
	action->setData("{Seifer}");
	action = toolBar->addAction(QIcon(":/images/icon-edea.png"), Data::name(7));
	action->setStatusTip(action->text());
	action->setData("{Edea}");
	action = toolBar->addAction(QIcon(":/images/icon-laguna.png"), Data::name(8));
	action->setStatusTip(action->text());
	action->setData("{Laguna}");
	action = toolBar->addAction(QIcon(":/images/icon-kiros.png"), Data::name(9));
	action->setStatusTip(action->text());
	action->setData("{Kiros}");
	action = toolBar->addAction(QIcon(":/images/icon-ward.png"), Data::name(10));
	action->setStatusTip(action->text());
	action->setData("{Ward}");
	action = toolBar->addAction(QIcon(":/images/icon-angelo.png"), Data::name(15));
	action->setStatusTip(action->text());
	action->setData("{Angelo}");
	action = toolBar->addAction(QIcon(":/images/icon-griever.png"), Data::name(12));
	action->setStatusTip(action->text());
	action->setData("{Griever}");
	action = toolBar->addAction(QIcon(":/images/icon-boko.png"), Data::name(14));
	action->setStatusTip(action->text());
	action->setData("{Boko}");
	toolBar->addSeparator();
	action = toolBar->addAction(QIcon(":/images/icon-newpage.png"), tr("Nouvelle page"));
	action->setStatusTip(action->text());
	action->setData("\n{NewPage}\n");
	action = toolBar->addAction(QIcon(":/images/icon-wait.png"), tr("Pause"));
	action->setStatusTip(action->text());
	action->setData("{Wait000}");
	toolBar->addSeparator();
	action = toolBar->addAction(QIcon(":/images/icon-darkgrey.png"), tr("Gris foncé"));
	action->setStatusTip(action->text());
	action->setData("{Darkgrey}");
	action = toolBar->addAction(QIcon(":/images/icon-grey.png"), tr("Gris"));
	action->setStatusTip(action->text());
	action->setData("{Grey}");
	action = toolBar->addAction(QIcon(":/images/icon-yellow.png"), tr("Jaune"));
	action->setStatusTip(action->text());
	action->setData("{Yellow}");
	action = toolBar->addAction(QIcon(":/images/icon-red.png"), tr("Rouge"));
	action->setStatusTip(action->text());
	action->setData("{Red}");
	action = toolBar->addAction(QIcon(":/images/icon-green.png"), tr("Vert"));
	action->setStatusTip(action->text());
	action->setData("{Green}");
	action = toolBar->addAction(QIcon(":/images/icon-blue.png"), tr("Bleu"));
	action->setStatusTip(action->text());
	action->setData("{Blue}");
	action = toolBar->addAction(QIcon(":/images/icon-purple.png"), tr("Violet"));
	action->setStatusTip(action->text());
	action->setData("{Purple}");
	action = toolBar->addAction(QIcon(":/images/icon-white.png"), tr("Blanc"));
	action->setStatusTip(action->text());
	action->setData("{White}");
	toolBar->addSeparator();
	action = toolBar->addAction(QIcon(":/images/icon-darkgreyblink.png"), tr("Gris foncé clignotant"));
	action->setStatusTip(action->text());
	action->setData("{DarkgreyBlink}");
	action = toolBar->addAction(QIcon(":/images/icon-greyblink.png"), tr("Gris clignotant"));
	action->setStatusTip(action->text());
	action->setData("{GreyBlink}");
	action = toolBar->addAction(QIcon(":/images/icon-yellowblink.png"), tr("Jaune clignotant"));
	action->setStatusTip(action->text());
	action->setData("{YellowBlink}");
	action = toolBar->addAction(QIcon(":/images/icon-redblink.png"), tr("Rouge clignotant"));
	action->setStatusTip(action->text());
	action->setData("{RedBlink}");
	action = toolBar->addAction(QIcon(":/images/icon-greenblink.png"), tr("Vert clignotant"));
	action->setStatusTip(action->text());
	action->setData("{GreenBlink}");
	action = toolBar->addAction(QIcon(":/images/icon-blueblink.png"), tr("Bleu clignotant"));
	action->setStatusTip(action->text());
	action->setData("{BlueBlink}");
	action = toolBar->addAction(QIcon(":/images/icon-purpleblink.png"), tr("Violet clignotant"));
	action->setStatusTip(action->text());
	action->setData("{PurpleBlink}");
	action = toolBar->addAction(QIcon(":/images/icon-whiteblink.png"), tr("Blanc clignotant"));
	action->setStatusTip(action->text());
	action->setData("{WhiteBlink}");
	toolBar->addSeparator();

	QMenu *menu1 = new QMenu(this);
	action = menu1->addAction(tr("Galbadia"));
	action->setData("{Galbadia}");
	action = menu1->addAction(tr("Esthar"));
	action->setData("{Esthar}");
	action = menu1->addAction(tr("Balamb"));
	action->setData("{Balamb}");
	action = menu1->addAction(tr("Dollet"));
	action->setData("{Dollet}");
	action = menu1->addAction(tr("Timber"));
	action->setData("{Timber}");
	action = menu1->addAction(tr("Trabia"));
	action->setData("{Trabia}");
	action = menu1->addAction(tr("Centra"));
	action->setData("{Centra}");
	action = menu1->addAction(tr("Horizon"));
	action->setData("{Horizon}");
	action = toolBar->addAction(tr("Lieux"));
	action->setMenu(menu1);

	QMenu *menu2 = new QMenu(this);
	action = menu2->addAction(tr("Var0"));
	action->setData("{Var0}");
	action = menu2->addAction(tr("Var00"));
	action->setData("{Var00}");
	action = menu2->addAction(tr("Varb0"));
	action->setData("{Varb0}");
	action = toolBar->addAction(tr("Var"));
	action->setMenu(menu2);

	QMenu *menu3 = new QMenu(this);
	action = menu3->addAction(QPixmap::fromImage(TextPreview::getIconImage(0x00)), tr("L2"));
	action->setData("{x0520}");
	action = menu3->addAction(QPixmap::fromImage(TextPreview::getIconImage(0x01)), tr("R2"));
	action->setData("{x0521}");
	action = menu3->addAction(QPixmap::fromImage(TextPreview::getIconImage(0x02)), tr("L1"));
	action->setData("{x0522}");
	action = menu3->addAction(QPixmap::fromImage(TextPreview::getIconImage(0x03)), tr("R1"));
	action->setData("{x0523}");
	action = menu3->addAction(QPixmap::fromImage(TextPreview::getIconImage(0x04)), tr("Triangle"));
	action->setData("{x0524}");
	action = menu3->addAction(QPixmap::fromImage(TextPreview::getIconImage(0x05)), tr("Rond"));
	action->setData("{x0525}");
	action = menu3->addAction(QPixmap::fromImage(TextPreview::getIconImage(0x06)), tr("Croix"));
	action->setData("{x0526}");
	action = menu3->addAction(QPixmap::fromImage(TextPreview::getIconImage(0x07)), tr("Carré"));
	action->setData("{x0527}");
	action = menu3->addAction(QPixmap::fromImage(TextPreview::getIconImage(0x08)), tr("Select"));
	action->setData("{x0528}");
	action = menu3->addAction(QPixmap::fromImage(TextPreview::getIconImage(0x0b)), tr("Start"));
	action->setData("{x052b}");
	action = menu3->addAction(QPixmap::fromImage(TextPreview::getIconImage(0x0c)), tr("Haut"));
	action->setData("{x052c}");
	action = menu3->addAction(QPixmap::fromImage(TextPreview::getIconImage(0x0d)), tr("Droite"));
	action->setData("{x052d}");
	action = menu3->addAction(QPixmap::fromImage(TextPreview::getIconImage(0x0e)), tr("Bas"));
	action->setData("{x052e}");
	action = menu3->addAction(QPixmap::fromImage(TextPreview::getIconImage(0x0f)), tr("Gauche"));
	action->setData("{x052f}");
	action = toolBar->addAction(tr("Touches"));
	action->setMenu(menu3);

	toolBar2 = new QToolBar(this);
	toolBar2->setIconSize(QSize(16, 16));
	toolBar2->addAction(tr("Caractères spéciaux"), this, SLOT(specialCharactersDialog()));

	QVBoxLayout *toolBars = new QVBoxLayout();
	toolBars->addWidget(toolBar);
	toolBars->addWidget(toolBar2);
	toolBars->setContentsMargins(QMargins());
	toolBars->setSpacing(0);

	textEdit = new QPlainTextEdit(this);
	new MsdHighlighter(textEdit->document());

	groupTextPreview = new QWidget(this);

	prevPage = new QToolButton(groupTextPreview);
	prevPage->setArrowType(Qt::UpArrow);
	prevPage->setToolButtonStyle(Qt::ToolButtonIconOnly);
	nextPage = new QToolButton(groupTextPreview);
	nextPage->setArrowType(Qt::DownArrow);
	nextPage->setToolButtonStyle(Qt::ToolButtonIconOnly);
	textPage = new QLabel(groupTextPreview);
	textPage->setTextFormat(Qt::PlainText);

	prevWin = new QToolButton(groupTextPreview);
	prevWin->setArrowType(Qt::UpArrow);
	prevWin->setToolButtonStyle(Qt::ToolButtonIconOnly);
	nextWin = new QToolButton(groupTextPreview);
	nextWin->setArrowType(Qt::DownArrow);
	nextWin->setToolButtonStyle(Qt::ToolButtonIconOnly);
	textWin = new QLabel(groupTextPreview);
	textWin->setTextFormat(Qt::PlainText);

	xCoord = new QSpinBox(groupTextPreview);
	yCoord = new QSpinBox(groupTextPreview);
	xCoord->setRange(0, 65535);
	yCoord->setRange(0, 65535);

	QGridLayout *layoutTextPreview = new QGridLayout(groupTextPreview);
	layoutTextPreview->addWidget(textPreview, 0, 0, 8, 1, Qt::AlignLeft | Qt::AlignTop);
	layoutTextPreview->addWidget(prevPage, 0, 1, 1, 2, Qt::AlignLeft | Qt::AlignTop);
	layoutTextPreview->addWidget(textPage, 1, 1, 1, 2, Qt::AlignLeft | Qt::AlignTop);
	layoutTextPreview->addWidget(nextPage, 2, 1, 1, 2, Qt::AlignLeft | Qt::AlignTop);
	layoutTextPreview->addWidget(new QLabel(tr("X")), 3, 1, Qt::AlignLeft | Qt::AlignHCenter);
	layoutTextPreview->addWidget(xCoord, 3, 2, Qt::AlignLeft | Qt::AlignHCenter);
	layoutTextPreview->addWidget(new QLabel(tr("Y")), 4, 1, Qt::AlignLeft | Qt::AlignHCenter);
	layoutTextPreview->addWidget(yCoord, 4, 2, Qt::AlignLeft | Qt::AlignHCenter);
	layoutTextPreview->addWidget(prevWin, 5, 1, 1, 2, Qt::AlignLeft | Qt::AlignBottom);
	layoutTextPreview->addWidget(textWin, 6, 1, 1, 2, Qt::AlignLeft | Qt::AlignBottom);
	layoutTextPreview->addWidget(nextWin, 7, 1, 1, 2, Qt::AlignLeft | Qt::AlignBottom);
	layoutTextPreview->setColumnStretch(2, 1);
	layoutTextPreview->setRowStretch(2, 1);
	layoutTextPreview->setRowStretch(5, 1);
	layoutTextPreview->setContentsMargins(QMargins());

	groupTextPreview->setFixedHeight(224);

	QGridLayout *mainLayout = new QGridLayout(this);
	mainLayout->addWidget(listWidget, 0, 0, 2, 1);
	mainLayout->addLayout(toolBars, 0, 1);
	mainLayout->addWidget(textEdit, 1, 1);
	mainLayout->addWidget(groupTextPreview, 2, 0, 1, 2);
	mainLayout->setContentsMargins(QMargins());

	connect(textList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), SLOT(fillTextEdit(QListWidgetItem*)));
	connect(textEdit, SIGNAL(textChanged()), SLOT(updateCurrentText()));
	connect(textEdit, SIGNAL(cursorPositionChanged()), SLOT(emitFromChanged()));
	connect(prevPage, SIGNAL(released()), SLOT(prevTextPreviewPage()));
	connect(nextPage, SIGNAL(released()), SLOT(nextTextPreviewPage()));
	connect(prevWin, SIGNAL(released()), SLOT(prevTextPreviewWin()));
	connect(nextWin, SIGNAL(released()), SLOT(nextTextPreviewWin()));
	connect(xCoord, SIGNAL(valueChanged(int)), SLOT(changeXCoord(int)));
	connect(yCoord, SIGNAL(valueChanged(int)), SLOT(changeYCoord(int)));
	connect(textPreview, SIGNAL(positionChanged(QPoint)), SLOT(changeCoord(QPoint)));
	connect(toolBar, SIGNAL(actionTriggered(QAction*)), SLOT(insertTag(QAction*)));
	connect(menu1, SIGNAL(triggered(QAction*)), SLOT(insertTag(QAction*)));
	connect(menu2, SIGNAL(triggered(QAction*)), SLOT(insertTag(QAction*)));
	connect(menu3, SIGNAL(triggered(QAction*)), SLOT(insertTag(QAction*)));

	PageWidget::build();
}

void MsdWidget::clear()
{
	if(!isFilled())		return;

	textPreview->clear();
	dontUpdateCurrentText = true;
	textList->clear();
	textEdit->setPlainText(QString());
	dontUpdateCurrentText = false;

	PageWidget::clear();
}

void MsdWidget::setReadOnly(bool readOnly)
{
	if(isBuilded()) {
		textEdit->setReadOnly(readOnly);
		xCoord->setReadOnly(readOnly);
		yCoord->setReadOnly(readOnly);
		textPreview->setReadOnly(readOnly);
		toolBar0->setDisabled(readOnly);
		toolBar->setDisabled(readOnly);
		toolBar2->setDisabled(readOnly);
	}

	PageWidget::setReadOnly(readOnly);
}

void MsdWidget::setEditTextEnabled(bool enabled)
{
	if(!isBuilded())	return;

	textEdit->setEnabled(enabled);
	toolBar->setEnabled(enabled);
	toolBar2->setEnabled(enabled);
	actionRemoveText->setEnabled(enabled);
	groupTextPreview->setEnabled(enabled);
}

QString MsdWidget::selectedText() const
{
	if(!isBuilded())	return QString();

	return textEdit->textCursor().selectedText();
}

void MsdWidget::fill()
{
	if(!isBuilded())	build();
	if(isFilled())		clear();

	if(!hasData())		return;

	if(data()->hasTdwFile())
		textPreview->setFontImageAdd(data()->getTdwFile());

	bool hasTexts = false;

	if(data()->hasMsdFile()) {

		int nbTexts = data()->getMsdFile()->nbText();

		if(nbTexts!=0) {

			QIcon icon(":/images/text_icon.png"), iconDisabled(":/images/text_icon_disabled.png");

			for(int i=0 ; i<nbTexts ; ++i) {
				QListWidgetItem *item = new QListWidgetItem(tr("Texte %1").arg(i));
				if(data()->hasJsmFile() && data()->getJsmFile()->nbWindows(i)>0)
					item->setIcon(icon);
				else
					item->setIcon(iconDisabled);
				item->setData(Qt::UserRole, i);
				textList->addItem(item);
			}

			textList->setCurrentRow(0);
			hasTexts = true;
		}
	}

	setEditTextEnabled(hasTexts);
	if(!hasTexts) {
		textPreview->clearWin();
		textPreview->setText(QByteArray());
		changeTextPreviewPage();
		changeTextPreviewWin();
	}

	PageWidget::fill();
}

void MsdWidget::fillTextEdit(QListWidgetItem *item)
{
	if(item==NULL || !data()->hasMsdFile())	return;

	textList->scrollToItem(item);
	if(!hasData() || !data()->hasMsdFile())	return;

	int textID = item->data(Qt::UserRole).toInt();
	QString text = data()->getMsdFile()->text(textID);
	dontUpdateCurrentText = true;
	textEdit->setPlainText(text);
	dontUpdateCurrentText = false;
	setEditTextEnabled(true);

	if(hasData() && data()->hasJsmFile()) {
		textPreview->resetCurrentWin();
		textPreview->setWins(data()->getJsmFile()->windows(textID), false);
	}
	else {
		textPreview->clearWin();
	}
	textPreview->setText(data()->getMsdFile()->data(textID));

	changeTextPreviewPage();
	changeTextPreviewWin();

	emit textIdChanged(textID);
}

void MsdWidget::emitFromChanged()
{
	emit fromChanged(textEdit->textCursor().position());
}

void MsdWidget::prevTextPreviewPage()
{
	textPreview->prevPage();
	changeTextPreviewPage();
}

void MsdWidget::nextTextPreviewPage()
{
	textPreview->nextPage();
	changeTextPreviewPage();
}

void MsdWidget::changeTextPreviewPage()
{
	int currentPage = textPreview->getCurrentPage(), nbPage = textPreview->getNbPages();

	textPage->setText(tr("Page %1/%2").arg(currentPage).arg(nbPage));
	prevPage->setEnabled(currentPage > 1);
	nextPage->setEnabled(currentPage < nbPage);
	textPage->setEnabled(nbPage > 0);
}

void MsdWidget::prevTextPreviewWin()
{
	textPreview->prevWin();
	changeTextPreviewWin();
}

void MsdWidget::nextTextPreviewWin()
{
	textPreview->nextWin();
	changeTextPreviewWin();
}

void MsdWidget::changeTextPreviewWin()
{
	int currentWin = textPreview->getCurrentWin(), nbWin = textPreview->getNbWin();

	textWin->setText(tr("Fenêtre %1/%2").arg(currentWin).arg(nbWin));
	prevWin->setEnabled(currentWin > 1);
	nextWin->setEnabled(currentWin < nbWin);
	textWin->setEnabled(nbWin > 0);
	xCoord->setEnabled(nbWin > 0);
	yCoord->setEnabled(nbWin > 0);

	updateWindowCoord();
}

void MsdWidget::changeCoord(const QPoint &point)
{
	xCoord->setValue(point.x());
	yCoord->setValue(point.y());
}

void MsdWidget::changeXCoord(int x)
{
	if(textPreview->getNbWin()<=0)	return;

	int textID = textList->currentItem()->data(Qt::UserRole).toInt();
	int winID = textPreview->getCurrentWin()-1;
	FF8Window ff8Window = textPreview->getWindow();
	if(ff8Window.x >= 0 && ff8Window.x != x) {
		ff8Window.x = x;

		//		qDebug() << "changeXCoord()" << x << textID << winID;

		data()->getJsmFile()->setWindow(textID, winID, ff8Window);
		textPreview->setWins(data()->getJsmFile()->windows(textID));
		emit modified();
	}
}

void MsdWidget::changeYCoord(int y)
{
	if(textPreview->getNbWin()<=0)	return;

	int textID = textList->currentItem()->data(Qt::UserRole).toInt();
	int winID = textPreview->getCurrentWin()-1;
	FF8Window ff8Window = textPreview->getWindow();
	if(ff8Window.y >= 0 && ff8Window.y != y) {
		ff8Window.y = y;

		//		qDebug() << "changeYCoord()" << y << textID << winID;

		data()->getJsmFile()->setWindow(textID, winID, ff8Window);
		textPreview->setWins(data()->getJsmFile()->windows(textID));
		emit modified();
	}
}

void MsdWidget::insertTag(QAction *action)
{
	if(sender() != action->parentWidget())	return;// toolBar/Menu signals hack
	textEdit->insertPlainText(action->data().toString());
	textEdit->setFocus();
}

void MsdWidget::updateCurrentText()
{
	//	qDebug() << "MsdWidget::updateCurrentText";
	if(dontUpdateCurrentText || !hasData())	return;

	if(!data()->hasMsdFile()) {
		data()->addMsdFile();
	}

	emit modified();
	data()->getMsdFile()->setText(textList->currentRow(), textEdit->toPlainText());
	textPreview->setText(data()->getMsdFile()->data(textList->currentRow()), false);
	changeTextPreviewPage();
	changeTextPreviewWin();
}

void MsdWidget::updateWindowCoord()
{
	FF8Window ff8Win = textPreview->getWindow();
	if(ff8Win.x < 0) {
		xCoord->setEnabled(false);
		xCoord->setValue(0);
	} else {
		xCoord->setEnabled(true);
		xCoord->setValue(ff8Win.x);
	}

	if(ff8Win.y < 0) {
		yCoord->setEnabled(false);
		yCoord->setValue(0);
	} else {
		yCoord->setEnabled(true);
		yCoord->setValue(ff8Win.y);
	}
}

void MsdWidget::updateText()
{
	if(!isBuilded())	return;

	fillTextEdit(textList->currentItem());
	textPreview->reloadFont();
	textPreview->calcSize();
}

void MsdWidget::specialCharactersDialog()
{
	if(!isBuilded())	return;

	SpecialCharactersDialog dialog(this);
	if(dialog.exec() == QDialog::Accepted) {
		textEdit->insertPlainText(dialog.selectedCharacter());
	}
}

void MsdWidget::insertTextAbove()
{
	if(!data()->hasMsdFile()) {
		data()->addMsdFile();
	}

	int row = textList->currentRow();
	data()->getMsdFile()->insertText(row);
	fill();
	textList->setCurrentRow(row);
	emit modified();
}

void MsdWidget::insertText()
{
	if(!data()->hasMsdFile()) {
		data()->addMsdFile();
	}

	int row = textList->currentRow()+1;
	data()->getMsdFile()->insertText(row);
	fill();
	textList->setCurrentRow(row);
	emit modified();
}

void MsdWidget::removeText()
{
	int row = textList->currentRow();
	data()->getMsdFile()->removeText(row);
	fill();
	textList->setCurrentRow(qMax(row-1, 0));
	setEditTextEnabled(!textList->selectedItems().isEmpty());
	emit modified();
}

void MsdWidget::gotoText(int textID, int from, int size)
{
	if(!isBuilded())	build();

	for(int i=0 ; i<textList->count() ; ++i) {
		if(textID == textList->item(i)->data(Qt::UserRole).toInt()) {
			blockSignals(true);
			textEdit->blockSignals(true);
			textList->setCurrentItem(textList->item(i));
			textList->scrollToItem(textList->item(i));
			QTextCursor t = textEdit->textCursor();
			t.setPosition(from);
			t.setPosition(from + size, QTextCursor::KeepAnchor);
			textEdit->setTextCursor(t);
			blockSignals(false);
			textEdit->blockSignals(false);
		}
	}
}
