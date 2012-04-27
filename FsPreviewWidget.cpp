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

void FsPreviewWidget::imagePreview(const QPixmap &image)
{
	setCurrentIndex(ImagePage);
	QLabel *lbl = new QLabel();
	lbl->setPixmap(image);
	((QScrollArea *)currentWidget())->setWidget(lbl);
}

void FsPreviewWidget::textPreview(const QString &text)
{
	setCurrentIndex(TextPage);
	((QPlainTextEdit *)currentWidget())->setPlainText(text);
}
