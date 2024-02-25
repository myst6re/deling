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
#include "PreviewWidget.h"

PreviewWidget::PreviewWidget(QWidget *parent)
    : QWidget(parent), layout(nullptr), textPreview(nullptr), bgPreview(nullptr),
      colorPreview(nullptr)
{
	setFocusPolicy(Qt::NoFocus);
}

void PreviewWidget::buildLayout()
{
	if (layout == nullptr) {
		layout = new QStackedLayout(this);
	}
}

void PreviewWidget::showText(const QByteArray &textData, FF8Window ff8Window, TdwFile *tdwFile)
{
	if (textPreview == nullptr) {
		buildLayout();
		textPreview = new TextPreview(this);
		textPreview->setReadOnly(true);
		layout->addWidget(textPreview);
	}

	textPreview->setFontImageAdd(tdwFile);
	textPreview->setWins(QList<FF8Window>() << ff8Window, false);
	textPreview->setText(textData);
	if (ff8Window.type == NOWIN) {
		setFixedSize(textPreview->windowSize() + QSize(1, 1));
	} else {
		setFixedSize(textPreview->size());
	}
	layout->setCurrentWidget(textPreview);
}

void PreviewWidget::showBackground(const QPixmap &background)
{
	if (bgPreview == nullptr) {
		buildLayout();
		bgPreview = new BGPreview(this);
		bgPreview->setFixedSize(320, 224);
		bgPreview->setWithClick(false);
		layout->addWidget(bgPreview);
	}

	bgPreview->fill(background);
	setFixedSize(bgPreview->size());
	layout->setCurrentWidget(bgPreview);
}

void PreviewWidget::showColors(const QList<QColor> &colors)
{
	if (colors.isEmpty()) {
		return;
	}

	if (colorPreview == nullptr) {
		buildLayout();
		colorPreview = new QLabel(this);
		colorPreview->setFrameShape(QFrame::Box);
		layout->addWidget(colorPreview);
	}

	QPixmap pix(32 * colors.size(), 32);
	QPainter p(&pix);

	int x = 0;
	for (const QColor &color: colors) {
		p.fillRect(x, 0, 32, 32, color);
		x += 32;
	}

	p.end();

	colorPreview->setPixmap(pix);
	colorPreview->setFixedSize(pix.size());
	setFixedSize(pix.size());
	layout->setCurrentWidget(colorPreview);
}
