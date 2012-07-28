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
#ifndef MSDWIDGET_H
#define MSDWIDGET_H

#include <QtGui>
#include "widgets/PageWidget.h"
#include "BGPreview.h"
#include "TextPreview.h"
#include "MsdHighlighter.h"
#include "Listwidget.h"

class MsdWidget : public PageWidget
{
	Q_OBJECT
public:
	MsdWidget(QWidget *parent=0);

	void build();
	void clear();
	void fill();
	QString selectedText() const;
	void setReadOnly(bool);
	void updateText();
	inline QString tabName() const { return tr("Textes"); }
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
//	FF8Windows *windows;
};

#endif // MSDWIDGET_H
