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
#include "FsPreviewWidget.h"
#include "BGPreview2.h"

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
	QWidget *ret = new QWidget(this);

	imageSelect = new QComboBox(ret);
	palSelect = new QComboBox(ret);
	exportAll = new QPushButton(tr("Exporter tout"), ret);

	scrollArea = new QScrollArea(ret);
	scrollArea->setAlignment(Qt::AlignCenter);
	scrollArea->setFrameShape(QFrame::NoFrame);

	QHBoxLayout *hlayout = new QHBoxLayout;
	hlayout->addWidget(imageSelect, 1);
	hlayout->addWidget(palSelect, 1);
	hlayout->addWidget(exportAll);

	QVBoxLayout *layout = new QVBoxLayout(ret);
	layout->addLayout(hlayout);
	layout->addWidget(scrollArea, 1);

	connect(imageSelect, SIGNAL(currentIndexChanged(int)), SIGNAL(currentImageChanged(int)));
	connect(palSelect, SIGNAL(currentIndexChanged(int)), SIGNAL(currentPaletteChanged(int)));
	connect(exportAll, SIGNAL(clicked(bool)), SIGNAL(exportAllClicked()));

	return ret;
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

void FsPreviewWidget::imagePreview(const QPixmap &image, const QString &name,
                                   int palID, int palCount, int imageID,
                                   int imageCount)
{
	QString imageName = name;
	setCurrentIndex(ImagePage);
	BGPreview2 *lbl = new BGPreview2();
	lbl->setPixmap(image);
	scrollArea->setWidget(lbl);

	imageSelect->blockSignals(true);
	imageSelect->clear();
	if (imageCount > 1) {
		imageName.append(QString("-%1").arg(imageID));
		imageSelect->setVisible(true);
		for (int i = 0; i < imageCount; ++i) {
			imageSelect->addItem(tr("Image %1").arg(i));
		}
		imageSelect->setCurrentIndex(imageID);
	} else {
		imageSelect->setVisible(false);
	}
	imageSelect->blockSignals(false);

	palSelect->blockSignals(true);
	palSelect->clear();
	if (palCount > 1) {
		imageName.append(QString("-%1").arg(palID));
		palSelect->setVisible(true);
		for (int i = 0; i < palCount; ++i) {
			palSelect->addItem(tr("Palette %1").arg(i));
		}
		palSelect->setCurrentIndex(palID);
	} else {
		palSelect->setVisible(false);
	}
	palSelect->blockSignals(false);
	lbl->setName(imageName);
	exportAll->setVisible(imageSelect->isVisible() || palSelect->isVisible());
}

void FsPreviewWidget::textPreview(const QString &text)
{
	setCurrentIndex(TextPage);
	((QPlainTextEdit *)currentWidget())->setPlainText(text);
}
