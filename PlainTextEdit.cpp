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
#include "PlainTextEdit.h"

#if (QT_VERSION < QT_VERSION_CHECK(5, 10, 0))
#	define setTabStopDistance setTabStopWidth
#endif

#if (QT_VERSION < QT_VERSION_CHECK(5, 11, 0))
#	define horizontalAdvance width
#endif

PlainTextEditPriv::PlainTextEditPriv(QWidget *parent) :
    QPlainTextEdit(parent), _previousLineHovered(-1)
{
	setMouseTracking(true);

	_overlay = new PreviewWidget(this);
	_overlay->hide();
}

void PlainTextEditPriv::mouseMoveEvent(QMouseEvent *event)
{
	QTextBlock currentBlock = cursorForPosition(event->pos()).block();

	if(_previousLineHovered != currentBlock.firstLineNumber()) {
		_previousLineHovered = currentBlock.firstLineNumber();

		int maxH = int(document()->documentLayout()->blockBoundingRect(currentBlock).height()),
		    i = 0;

		for(; i < maxH; ++i) {
			if (cursorForPosition(event->pos() + QPoint(0, i)).block() != currentBlock) {
				break;
			}
		}

		int margin = maxH / 3;

		emit lineHovered(currentBlock.text(), QPoint(margin, event->pos().y() + i + margin));
	}

	QPlainTextEdit::mouseMoveEvent(event);
}

PlainTextEdit::PlainTextEdit(QWidget *parent) :
	QWidget(parent)
{
	_textEdit = new PlainTextEditPriv(parent);
	_textEdit->viewport()->installEventFilter(this);
	_textEdit->setTabStopDistance(30.0);
	_textEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
	_textEdit->setMouseTracking(true);
}

QSize PlainTextEdit::sizeHint() const
{
	return QSize(_textEdit->fontMetrics()
	             .horizontalAdvance(QString::number(_textEdit->document()->blockCount()))
	             + 4,
	             _textEdit->height());
}

PlainTextEditPriv *PlainTextEdit::textEdit() const
{
	return _textEdit;
}

bool PlainTextEdit::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::Paint) {
		setFixedHeight(_textEdit->height());
		setFixedWidth(_textEdit->fontMetrics().horizontalAdvance(QString::number(_textEdit->document()->blockCount())) + 4);
		update();
	}

	return QObject::eventFilter(obj, event);
}

void PlainTextEdit::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event)
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
	QTextBlock block = _textEdit->document()->findBlockByNumber(line - 1);

	while(block.isValid() && y <= height()) {

		if(block == currentBlock) {
			QFont font = painter.font();
			font.setBold(true);
			painter.setFont(font);
		}

		painter.drawText(width() - fontMetrics.horizontalAdvance(QString::number(line)) - 3, y, QString::number(line));
		y += _textEdit->document()->documentLayout()->blockBoundingRect(block).height();

		if(block == currentBlock) {
			QFont font = painter.font();
			font.setBold(false);
			painter.setFont(font);
		}

		block = block.next();
		line++;
	}
}
