#include "PlainTextEdit.h"

PlainTextEdit::PlainTextEdit(QWidget *parent) :
	QWidget(parent)
{
	_textEdit = new QPlainTextEdit(parent);
	_textEdit->viewport()->installEventFilter(this);
}

QPlainTextEdit *PlainTextEdit::textEdit()
{
	return _textEdit;
}

bool PlainTextEdit::eventFilter(QObject *obj, QEvent *event)
{
	setFixedHeight(_textEdit->height());
	setFixedWidth(_textEdit->fontMetrics().width(QString::number(_textEdit->document()->blockCount())) + 4);
	update();
	return QObject::eventFilter(obj, event);
}

void PlainTextEdit::paintEvent(QPaintEvent */*event*/)
{
	QPainter painter(this);

	QFontMetrics fontMetrics = _textEdit->fontMetrics();

	/* Draw background */

//	int minWidth = fontMetrics.width(QString::number(_textEdit->document()->blockCount())) + 4;

//	painter.fillRect(QRect(0, event->rect().top(), minWidth, height()), QColor(0xf0,0xf0,0xf0));
	painter.setPen(QColor(0xa0,0xa0,0xa0));

	/* Draw line numbers */

	int line=_textEdit->verticalScrollBar()->value() + 1;
	int y= _textEdit->frameWidth() + fontMetrics.ascent();

	if(line == 1) {
		y += _textEdit->document()->documentMargin();
	}

	QTextBlock currentBlock = _textEdit->textCursor().block();
	QTextBlock block = _textEdit->document()->findBlockByNumber(line);

	while(block.isValid() && y <= height()) {

		if(block.previous() == currentBlock) {
			QFont font = painter.font();
			font.setBold(true);
			painter.setFont(font);
		}

		painter.drawText(width() - fontMetrics.width(QString::number(line)) - 3, y, QString::number(line));
		y += _textEdit->document()->documentLayout()->blockBoundingRect(block).height();

		if(block.previous() == currentBlock) {
			QFont font = painter.font();
			font.setBold(false);
			painter.setFont(font);
		}

		block = block.next();
		line++;
	}
}
