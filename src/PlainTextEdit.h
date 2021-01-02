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
#ifndef PLAINTEXTEDIT_H
#define PLAINTEXTEDIT_H

#include <QtWidgets>
#include "PreviewWidget.h"

class PlainTextEditPriv : public QPlainTextEdit
{
	Q_OBJECT
public:
	explicit PlainTextEditPriv(QWidget *parent = nullptr);
	inline PreviewWidget *previewWidget() const {
		return _overlay;
	}
signals:
	void lineHovered(const QString &line, QPoint mousePos);
private:
	PreviewWidget *_overlay;
	int _previousLineHovered;
protected:
	virtual void mouseMoveEvent(QMouseEvent *event);
};

class PlainTextEdit : public QWidget
{
	Q_OBJECT
public:
	explicit PlainTextEdit(QWidget *parent = nullptr);
	PlainTextEditPriv *textEdit() const;
	inline PreviewWidget *previewWidget() const {
		return _textEdit->previewWidget();
	}
	QSize sizeHint() const;
	inline QSize minimumSizeHint() const {
		return sizeHint();
	}
signals:
	
public slots:
private:
	PlainTextEditPriv *_textEdit;
protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual bool eventFilter(QObject *obj, QEvent *event);
};

#endif // PLAINTEXTEDIT_H
