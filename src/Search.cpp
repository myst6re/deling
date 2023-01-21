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
#include "Search.h"
#include "SearchAll.h"

Search::Search(QTreeWidget *fieldList, SearchAll *searchAllDialog, QWidget *parent)
    : QDialog(parent, Qt::Tool), fieldArchive(nullptr), fieldList(fieldList),
      searchAllDialog(searchAllDialog)
{
	fieldID = -1;
	textID = from = groupID = methodID = opcodeID = 0;
	setWindowTitle(tr("Rechercher"));
	setWindowModality(Qt::NonModal);
	setSizeGripEnabled(true);

	tabWidget = new QTabWidget(this);
	tabWidget->addTab(textPageWidget(), tr("Textes"));
	tabWidget->addTab(scriptPageWidget(), tr("Scripts"));

	buttonNext = new QPushButton(tr("Chercher le suivant"), this);
	buttonPrev = new QPushButton(tr("Chercher le précédent"), this);
	buttonSearchAll = new QPushButton(tr("Chercher tout"), this);
	buttonPrev->setAutoDefault(false);
	buttonNext->setAutoDefault(false);
	buttonNext->setEnabled(false);
	buttonPrev->setEnabled(false);
	buttonNext->setDefault(true);

	new QShortcut(QKeySequence::FindNext, this, SLOT(findNext()), nullptr, Qt::ApplicationShortcut);
	new QShortcut(QKeySequence::FindPrevious, this, SLOT(findPrev()), nullptr, Qt::ApplicationShortcut);

	int maxWidth = qMax(buttonPrev->sizeHint().width(), buttonNext->sizeHint().width());
	// buttonNext.width == buttonPrev.width
	buttonNext->setFixedWidth(maxWidth);
	buttonPrev->setFixedWidth(maxWidth);
	
	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(tabWidget, 0, 0, 1, 2);
	layout->addWidget(buttonPrev, 1, 0, Qt::AlignRight);
	layout->addWidget(buttonNext, 1, 1, Qt::AlignLeft);
	layout->addWidget(buttonSearchAll, 2, 0, 1, 2, Qt::AlignCenter);
	QMargins margins = layout->contentsMargins();
	margins.setTop(0);
	margins.setLeft(0);
	margins.setRight(0);
	layout->setContentsMargins(margins);

	connect(buttonNext, SIGNAL(released()), SLOT(findNext()));
	connect(buttonPrev, SIGNAL(released()), SLOT(findPrev()));
	connect(buttonSearchAll, SIGNAL(released()), SLOT(findAll()));
	connect(tabWidget, SIGNAL(currentChanged(int)), SLOT(setFocus()));
	connect(searchTextField, SIGNAL(textEdited(QString)), searchScriptTextField, SLOT(setText(QString)));
	connect(searchScriptTextField, SIGNAL(textEdited(QString)), searchTextField, SLOT(setText(QString)));
}

Qt::CaseSensitivity Search::sensitivity() const
{
	QCheckBox *checkBox = currentIndex() == Text
						  ? this->checkBox
						  : this->scriptCheckBox;
	if (checkBox->isChecked())
		return Qt::CaseSensitive;
	else
		return Qt::CaseInsensitive;
}

QString Search::text() const
{
	QLineEdit *lineEdit = currentIndex() == Text
						  ? this->searchTextField
						  : this->searchScriptTextField;
	return lineEdit->text();
}

FieldArchive::Sorting Search::sorting() const
{
	switch (fieldList->sortColumn()) {
	case 1:
		return FieldArchive::SortByDesc;
	case 2:
		return FieldArchive::SortByMapId;
	default:
		return FieldArchive::SortByName;
	}
}

QRegularExpression Search::regexp() const
{
	QCheckBox *checkBox = currentIndex() == Text
						  ? this->checkBox2
						  : this->scriptCheckBox2;

	QRegularExpression::PatternOptions options = sensitivity() == Qt::CaseInsensitive ? QRegularExpression::CaseInsensitiveOption : QRegularExpression::NoPatternOption;
	if (checkBox->isChecked()) {
		return QRegularExpression(text(), options);
	}
	return QRegularExpression(QRegularExpression::escape(text()), options);
}

QWidget *Search::textPageWidget()
{
	QWidget *ret = new QWidget(this);

	searchTextField = new QLineEdit(ret);
	searchTextField->setFocus();
	checkBox = new QCheckBox(tr("Sensible à la casse"), ret);
	checkBox2 = new QCheckBox(tr("Expression régulière"), ret);

	QVBoxLayout *layout = new QVBoxLayout(ret);
	layout->addWidget(searchTextField);
	layout->addWidget(checkBox);
	layout->addWidget(checkBox2);
	layout->addStretch(1);

	return ret;
}

QWidget *Search::scriptPageWidget()
{
	QWidget *ret = new QWidget(this);

	typeScriptChoice = new QComboBox(ret);
	typeScriptChoice->addItem(tr("Texte"));
	typeScriptChoice->addItem(tr("Opcode"));
	typeScriptChoice->addItem(tr("Variable"));
	typeScriptChoice->addItem(tr("Exécution"));
	typeScriptChoice->addItem(tr("Saut d'écran"));

	scriptStacked = new QStackedWidget(ret);
	scriptStacked->addWidget(scriptPageWidget1());
	scriptStacked->addWidget(scriptPageWidget2());
	scriptStacked->addWidget(scriptPageWidget3());
	scriptStacked->addWidget(scriptPageWidget4());
	scriptStacked->addWidget(scriptPageWidget5());

	QVBoxLayout *layout = new QVBoxLayout(ret);
	layout->addWidget(typeScriptChoice);
	layout->addWidget(scriptStacked);
	layout->addStretch(1);

	connect(typeScriptChoice, SIGNAL(currentIndexChanged(int)), scriptStacked, SLOT(setCurrentIndex(int)));
	connect(typeScriptChoice, SIGNAL(currentIndexChanged(int)), SLOT(setFocus()));

	return ret;
}

QWidget *Search::scriptPageWidget1()
{
	QWidget *ret = new QWidget(this);

	searchScriptTextField = new QLineEdit(ret);
	searchScriptTextField->setFocus();
	scriptCheckBox = new QCheckBox(tr("Sensible à la casse"), ret);
	scriptCheckBox2 = new QCheckBox(tr("Expression régulière"), ret);

	QVBoxLayout *layout = new QVBoxLayout(ret);
	layout->addWidget(searchScriptTextField);
	layout->addWidget(scriptCheckBox);
	layout->addWidget(scriptCheckBox2);
	layout->setContentsMargins(QMargins());
	layout->addStretch(1);

	return ret;
}

QWidget *Search::scriptPageWidget2()
{
	QWidget *ret = new QWidget(this);

	searchOpcode = new QComboBox(ret);
	for (int i = 0; i < JSM_OPCODE_COUNT; ++i) {
		searchOpcode->addItem(QString("%1 - %2").arg(i, 3, 16, QChar('0')).arg(JsmOpcode::opcodes[i]));
	}
	searchOpcode->setEditable(true);
	searchOpcode->setInsertPolicy(QComboBox::NoInsert);
	searchOpcode->completer()->setCompletionMode(QCompleter::PopupCompletion);
	searchOpcode->completer()->setFilterMode(Qt::MatchContains);

	searchOpcodeValue = new QSpinBox(ret);
	searchOpcodeValue->setRange(-1, 162777216);// 0 -> 2^24
	searchOpcodeValue->setValue(-1);

	QGridLayout *layout = new QGridLayout(ret);
	layout->addWidget(searchOpcode, 0, 0, 1, 2);
	layout->addWidget(new QLabel(tr("Paramètre :"), ret), 1, 0);
	layout->addWidget(searchOpcodeValue, 1, 1);
	layout->setRowStretch(2, 1);
	layout->setColumnStretch(1, 1);
	layout->setContentsMargins(QMargins());

	return ret;
}

QWidget *Search::scriptPageWidget3()
{
	QWidget *ret = new QWidget(this);

	selectScriptVar = new QComboBox(ret);
	for (int i=0 ; i<1536 ; ++i)
		selectScriptVar->addItem(QString("%1 - %2").arg(i, 4, 10, QChar('0')).arg(Config::value(QString("var%1").arg(i)).toString()));
	selectScriptVar->setEditable(true);
	selectScriptVar->setInsertPolicy(QComboBox::NoInsert);
	selectScriptVar->completer()->setCompletionMode(QCompleter::PopupCompletion);
	selectScriptVar->completer()->setFilterMode(Qt::MatchContains);
	QGroupBox *group = new QGroupBox(ret);
	QRadioButton *allScriptVar = new QRadioButton(tr("Tout"), ret);
	popScriptVar = new QRadioButton(tr("Pop uniquement"), ret);
	pushScriptVar = new QRadioButton(tr("Push uniquement"), ret);
	pushScriptVar->hide();

	allScriptVar->setChecked(true);

	QHBoxLayout *groupLayout = new QHBoxLayout(group);
	groupLayout->addWidget(allScriptVar);
	groupLayout->addWidget(popScriptVar);
	//groupLayout->addWidget(pushScriptVar);
	groupLayout->addStretch(1);

	QVBoxLayout *layout = new QVBoxLayout(ret);
	layout->addWidget(selectScriptVar, 0);
	layout->addWidget(group, 0);
	layout->addStretch(1);
	layout->setContentsMargins(QMargins());

	return ret;
}

QWidget *Search::scriptPageWidget4()
{
	QWidget *ret = new QWidget(this);

	selectScriptGroup = new QSpinBox(ret);
	selectScriptGroup->setRange(0, 256);
	selectScriptLabel = new QSpinBox(ret);
	selectScriptLabel->setRange(0, 65535);

	QGridLayout *layout = new QGridLayout(ret);
	layout->addWidget(new QLabel(tr("Groupe id"),ret), 0, 0);
	layout->addWidget(new QLabel(tr("Label"),ret), 0, 1);
	layout->addWidget(selectScriptGroup, 1, 0);
	layout->addWidget(selectScriptLabel, 1, 1);
	layout->setRowStretch(2, 1);
	layout->setContentsMargins(QMargins());

	return ret;
}

QWidget *Search::scriptPageWidget5()
{
	QWidget *ret = new QWidget(this);

	selectMap = new QSpinBox(ret);
	selectMap->setRange(0, 65535);

	QGridLayout *layout = new QGridLayout(ret);
	layout->addWidget(new QLabel(tr("Écran id"),ret), 0, 0);
	layout->addWidget(selectMap, 0, 1);
	layout->setRowStretch(1, 1);
	layout->setContentsMargins(QMargins());

	return ret;
}

Search::Tabs Search::currentIndex() const
{
	return Tabs(tabWidget->currentIndex());
}

void Search::setCurrentIndex(int index)
{
	if (index < 2) {
		tabWidget->setCurrentIndex(index);
	}
}

void Search::focusInEvent(QFocusEvent *)
{
	if (currentIndex() == Text) {
		searchTextField->setFocus();
		searchTextField->selectAll();
	} else if (currentIndex() == Script) {
		searchScriptTextField->setFocus();
		searchScriptTextField->selectAll();
	}
}

void Search::setFieldArchive(FieldArchive *fieldArchive)
{
	this->fieldArchive = fieldArchive;
	buttonNext->setEnabled(fieldArchive != nullptr);
	buttonPrev->setEnabled(fieldArchive != nullptr);
	buttonSearchAll->setEnabled(fieldArchive != nullptr);
}

void Search::setFieldId(int fieldID)
{
	this->fieldID = fieldID;
	this->textID = -1;
	this->from = -1;
}

void Search::setTextId(int textID)
{
	this->textID = textID;
	this->from = -1;
}

void Search::setFrom(int from)
{
	this->from = from;
}

void Search::setSearchText(const QString &text)
{
	if (text.isEmpty())	return;
	searchTextField->setText(text);
	searchTextField->selectAll();
	searchScriptTextField->setText(text);
	searchScriptTextField->selectAll();
}

void Search::setSearchOpcode(int opcode)
{
	searchOpcode->setCurrentIndex(opcode);
}

void Search::findNext()
{
	bool found = false;
	if (fieldArchive==NULL)	return;

	buttonNext->setEnabled(false);
	buttonNext->setDefault(true);

	if (currentIndex() == Text)
	{
		found = findNextText();
	}
	else if (currentIndex() == Script)
	{
		found = findNextScript();
	}

	if (!found)
	{
		QMessageBox::information(this, windowTitle(), tr("Dernier fichier,\npoursuite de la recherche dans le premier fichier."));
	}

	buttonNext->setEnabled(true);
}

void Search::findPrev()
{
	bool found = false;
	if (fieldArchive==NULL)	return;

	buttonPrev->setEnabled(false);
	buttonPrev->setDefault(true);

	if (currentIndex() == Text)
	{
		found = findPrevText();
	}
	else if (currentIndex() == Script)
	{
		found = findPrevScript();
	}

	if (!found)
	{
		QMessageBox::information(this, windowTitle(), tr("Premier fichier,\npoursuite de la recherche dans le dernier fichier."));
	}

	buttonPrev->setEnabled(true);
}

void Search::findAll()
{
	if (fieldArchive == nullptr) {
		return;
	}

	searchAllDialog->show();
	searchAllDialog->activateWindow();
	searchAllDialog->raise();

	parentWidget()->setEnabled(false);
	setEnabled(false);

	if (currentIndex() == Text) {
		findAllText();
	}
	else if (currentIndex() == Script) {
		findAllScript();
	}

	parentWidget()->setEnabled(true);
	setEnabled(true);
}

bool Search::findNextText()
{
	int size;
	FieldArchive::Sorting sort = sorting();

	++from;

	if (fieldArchive->searchText(regexp(), fieldID, textID, from, size, sort)) {
		emit foundText(fieldID, textID, from, size);
		return true;
	}

	fieldID = textID = from = -1;

	return false;
}

bool Search::findPrevText()
{
	int size;
	FieldArchive::Sorting sort = sorting();

	--from;

	if (fieldArchive->searchTextReverse(regexp(), fieldID, textID, from, size, sort)) {
		emit foundText(fieldID, textID, from, size);
		return true;
	}

	fieldID = textID = from = 2147483647;

	return false;

}

bool Search::findNextScript()
{
	int sav;

	++opcodeID;

	if (findNextScript(fieldID, groupID, methodID, opcodeID)) {
		sav = opcodeID;
		emit foundOpcode(fieldID, groupID, methodID, opcodeID);
		// "found" signal may change 'opcodeID' value
		opcodeID = sav;
		return true;
	}

	return false;
}

bool Search::findNextScript(int &fieldID, int &groupID, int &methodID, int &opcodeID)
{
	bool found = false;
	FieldArchive::Sorting sort = sorting();

	if (typeScriptChoice->currentIndex() == 0) {
		found = fieldArchive->searchScriptText(regexp(), fieldID, groupID, methodID, opcodeID, sort);
	}
	else if (typeScriptChoice->currentIndex() == 1) {
		found = fieldArchive->searchScript(JsmFile::SearchOpcode, searchOpcode->currentIndex() | (searchOpcodeValue->value() << 16), fieldID, groupID, methodID, opcodeID, sort);
	}
	else if (typeScriptChoice->currentIndex() == 2) {
		found = fieldArchive->searchScript(JsmFile::SearchVar, (popScriptVar->isChecked() << 31) | (pushScriptVar->isChecked() << 30) | (selectScriptVar->currentIndex() & 0x3FFFFFFF), fieldID, groupID, methodID, opcodeID, sort);
	}
	else if (typeScriptChoice->currentIndex() == 3) {
		Field *field = fieldArchive->getField(fieldID);
		if (field != nullptr && field->hasJsmFile()) {
			found = field->getJsmFile()->search(JsmFile::SearchExec, (selectScriptGroup->value() & 0xFFFF) | ((selectScriptLabel->value() & 0xFFFF) << 16), groupID, methodID, opcodeID);
		}
		if (!found) {
			groupID = methodID = opcodeID = 2147483647;
			return false;
		}
	}
	else if (typeScriptChoice->currentIndex() == 4) {
		Field *field = fieldArchive->getField(fieldID);
		if (field != nullptr && field->hasJsmFile()) {
			found = field->getJsmFile()->search(JsmFile::SearchMapJump, (selectScriptGroup->value() & 0xFFFF) | ((selectScriptLabel->value() & 0xFFFF) << 16), groupID, methodID, opcodeID);
		}
		if (!found) {
			groupID = methodID = opcodeID = 2147483647;
			return false;
		}
	}

	if (found)
	{
		return true;
	}

	fieldID = groupID = methodID = opcodeID = 2147483647;

	return false;
}

bool Search::findPrevScript()
{
	int sav;
	bool found = false;
	FieldArchive::Sorting sort = sorting();

	--opcodeID;

	if (typeScriptChoice->currentIndex() == 0) {
		found = fieldArchive->searchScriptTextReverse(regexp(), fieldID, groupID, methodID, opcodeID, sort);
	}
	else if (typeScriptChoice->currentIndex() == 1) {
		found = fieldArchive->searchScriptReverse(JsmFile::SearchOpcode, searchOpcode->currentIndex(), fieldID, groupID, methodID, opcodeID, sort);
	}
	else if (typeScriptChoice->currentIndex() == 2) {
		found = fieldArchive->searchScriptReverse(JsmFile::SearchVar, (popScriptVar->isChecked() << 31) | (pushScriptVar->isChecked() << 30) | (selectScriptVar->currentIndex() & 0x3FFFFFFF), fieldID, groupID, methodID, opcodeID, sort);
	}
	else if (typeScriptChoice->currentIndex() == 3) {
		found = fieldArchive->searchScriptReverse(JsmFile::SearchExec, selectScriptGroup->value() | (selectScriptLabel->value() << 16), fieldID, groupID, methodID, opcodeID, sort);
	}
	else if (typeScriptChoice->currentIndex() == 4) {
		found = fieldArchive->searchScriptReverse(JsmFile::SearchMapJump, selectScriptGroup->value() | (selectScriptLabel->value() << 16), fieldID, groupID, methodID, opcodeID, sort);
	}

	if (found)
	{
		sav = opcodeID;
		emit foundOpcode(fieldID, groupID, methodID, opcodeID);
		// "found" signal may change 'opcodeID' value
		opcodeID = sav;
		return true;
	}

	fieldID = groupID = methodID = opcodeID = 2147483647;

	return false;
}

void Search::findAllScript()
{
	int fieldID = -1, groupID = 0, methodID = 0, opcodeID = 0;

	QDeadlineTimer t(50);

	searchAllDialog->setScriptSearch();
	while (findNextScript(fieldID, groupID, methodID, ++opcodeID)) {
		if (t.hasExpired()) {
			QCoreApplication::processEvents();
			t.setRemainingTime(50);
		}
		// Cancelled
		if (searchAllDialog->isHidden()) {
			break;
		}
		searchAllDialog->addResultOpcode(fieldID, groupID, methodID, opcodeID);
	}
}

void Search::findAllText()
{
	FieldArchive::Sorting sort = sorting();
	int fieldID = -1, textID = -1, from = -1;

	QDeadlineTimer t(50);

	searchAllDialog->setTextSearch();
	int size;
	while (fieldArchive->searchText(regexp(), fieldID, textID, ++from, size, sort)) {
		if (t.hasExpired()) {
			QCoreApplication::processEvents();
			t.setRemainingTime(50);
		}
		// Cancelled
		if (searchAllDialog->isHidden()) {
			break;
		}
		searchAllDialog->addResultText(fieldID, textID, from, size);
	}
}
