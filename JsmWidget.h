#ifndef JSMWIDGET_H
#define JSMWIDGET_H

#include <QtGui>
#include "PageWidget.h"
#include "JsmFile.h"
#include "BGPreview.h"
#include "JsmHighlighter.h"
#include "PlainTextEdit.h"

class JsmWidget : public PageWidget
{
    Q_OBJECT
public:
	explicit JsmWidget(QWidget *parent = 0);
	void build();
//	bool filled();
	void clear();
	void setReadOnly(bool readOnly);
	void fillList1(JsmFile *);
	void gotoScript(int, int, int);
	int selectedOpcode();
private:
	void saveSession();
	QList<QTreeWidgetItem *> nameList() const;
	QList<QTreeWidgetItem *> methodList(int groupID) const;
	QTreeWidget *list1;
	QTreeWidget *list2;
	QPlainTextEdit *textEdit;
	QToolBar *toolBar;
	QLabel *errorLabel;
	JsmFile *jsmFile;
	static int currentItem(QTreeWidget *);
//	void gotoScriptLabel(int groupID, int labelID);
	int groupID, methodID;
private slots:
	void compile();
	void fillList2();
	void fillTextEdit();
//	void jump(QTreeWidgetItem *);
};

#endif // JSMWIDGET_H
