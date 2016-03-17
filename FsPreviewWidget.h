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
#ifndef FSPREVIEWWIDGET_H
#define FSPREVIEWWIDGET_H

#include <QtGui>
#include "BGPreview2.h"

class FsPreviewWidget : public QStackedWidget
{
	Q_OBJECT
public:
	enum Pages { EmptyPage, ImagePage, TextPage };
    explicit FsPreviewWidget(QWidget *parent = 0);
	void clearPreview();
	void imagePreview(const QPixmap &image, const QString &name=QString(), int palID=0, int palCount=0);
	void textPreview(const QString &text);
signals:
	void currentPaletteChanged(int);
private:
	QWidget *imageWidget();
	QWidget *textWidget();
	QScrollArea *scrollArea;
	QComboBox *palSelect;
};

#endif // FSPREVIEWWIDGET_H
