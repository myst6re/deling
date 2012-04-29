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
#include "FsPreviewWidget.h"

FsPreviewWidget::FsPreviewWidget(QWidget *parent) :
	QStackedWidget(parent)
{
	addWidget(new QWidget(this));
	addWidget(imageWidget());
	addWidget(textWidget());
	clearPreview();
}

QWidget *FsPreviewWidget::imageWidget()
{
	QScrollArea *scrollArea = new QScrollArea(this);
	scrollArea->setAlignment(Qt::AlignCenter);
	scrollArea->setFrameShape(QFrame::NoFrame);

	return scrollArea;
}

QWidget *FsPreviewWidget::textWidget()
{
	QPlainTextEdit *textEdit = new QPlainTextEdit(this);
	textEdit->setReadOnly(true);
	textEdit->setFrameShape(QFrame::NoFrame);
	return textEdit;
}

void FsPreviewWidget::clearPreview()
{
	setCurrentIndex(EmptyPage);
}

void FsPreviewWidget::imagePreview(const QPixmap &image, const QString &name)
{
	setCurrentIndex(ImagePage);
	BGPreview2 *lbl = new BGPreview2();
	lbl->setPixmap(image);
	lbl->setName(name);
	((QScrollArea *)currentWidget())->setWidget(lbl);
}

void FsPreviewWidget::textPreview(const QString &text)
{
	setCurrentIndex(TextPage);
	((QPlainTextEdit *)currentWidget())->setPlainText(text);
}
