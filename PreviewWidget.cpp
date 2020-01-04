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
	foreach (const QColor &color, colors) {
		p.fillRect(x, 0, 32, 32, color);
		x += 32;
	}

	p.end();

	colorPreview->setPixmap(pix);
	colorPreview->setFixedSize(pix.size());
	setFixedSize(pix.size());
	layout->setCurrentWidget(colorPreview);
}
