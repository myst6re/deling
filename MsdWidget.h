#ifndef MSDWIDGET_H
#define MSDWIDGET_H

#include <QtGui>
#include "PageWidget.h"
#include "BGPreview.h"
#include "TextPreview.h"
#include "MsdFile.h"
#include "MsdHighlighter.h"

class MsdWidget : public PageWidget
{
	Q_OBJECT
public:
	MsdWidget(QWidget *parent=0);

	void build();
//	bool filled();
	void clear();
	void fillTextList(MsdFile *msdFile=0, JsmFile *jsmFile=0);
	QString selectedText();
	void setReadOnly(bool);
	void updateText();
public slots:
	void gotoText(const QString &text, int textID, Qt::CaseSensitivity cs, bool reverse, bool regexp);
private slots:
	void fillTextEdit(QListWidgetItem *);
	void updateCurrentText();
	void prevTextPreviewPage();
	void nextTextPreviewPage();
	void changeTextPreviewPage();
	void prevTextPreviewWin();
	void nextTextPreviewWin();
	void changeTextPreviewWin();
	void changeCoord(const QPoint &point);
	void changeXCoord(int);
	void changeYCoord(int);
	void insertTag(QAction *);
	void insertText();
	void removeText();

signals:
	void textIdChanged(int);
private:
	void updateWindowCoord();

	QListWidget *textList;
	QToolBar *toolBar0, *toolBar, *toolBar2;
	QPlainTextEdit *textEdit;
	TextPreview *textPreview;
	QToolButton *prevPage, *prevWin;
	QToolButton *nextPage, *nextWin;
	QLabel *textPage, *textWin;
	QSpinBox *xCoord, *yCoord;

	bool dontUpdateCurrentText;
	MsdFile *msdFile;
	JsmFile *jsmFile;
//	FF8Windows *windows;
};

#endif // MSDWIDGET_H
