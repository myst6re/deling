#include "PreviewWidget.h"

PreviewWidget::PreviewWidget(QWidget *parent)
    : QWidget(parent), layout(nullptr), textPreview(nullptr), bgPreview(nullptr)
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
