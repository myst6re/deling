/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2024 Arzel Jérôme <myst6re@gmail.com>
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
#pragma once

#include <QWidget>
#include <QStackedLayout>
#include "files/TdwFile.h"
#include "BGPreview.h"
#include "TextPreview.h"

class PreviewWidget : public QWidget
{
	Q_OBJECT
public:
	explicit PreviewWidget(QWidget *parent = nullptr);

	void showText(const QByteArray &textData, FF8Window ff8Window, TdwFile *tdwFile = nullptr);
	void showBackground(const QPixmap &background);
	void showColors(const QList<QColor> &colors);

private:
	void buildLayout();

	QStackedLayout *layout;
	TextPreview *textPreview;
	BGPreview *bgPreview;
	QLabel *colorPreview;
};


