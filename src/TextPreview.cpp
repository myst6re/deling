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
#include "TextPreview.h"

bool TextPreview::curFrame = true;
QImage TextPreview::iconImage;
TdwFile *TextPreview::font;
TdwFile *TextPreview::tdwFile = NULL;

TextPreview::TextPreview(QWidget *parent)
	: QWidget(parent), currentPage(0), currentWin(0), useTimer(false), acceptMove(false), fontColor(TdwFile::White)
{
	pagesPos.append(0);

	setFixedSize(320, 224);

	if(names.isEmpty()) {
		iconImage = QImage(":/images/icons.png");
		reloadFont();

		bool jp = tr("false", "Use Japanese Encoding") == "true";
		names << FF8Text::toFF8(Data::name(0), jp) << FF8Text::toFF8(Data::name(1), jp)
				<< FF8Text::toFF8(Data::name(2), jp) << FF8Text::toFF8(Data::name(3), jp)
				<< FF8Text::toFF8(Data::name(4), jp) << FF8Text::toFF8(Data::name(5), jp)
				<< FF8Text::toFF8(Data::name(6), jp) << FF8Text::toFF8(Data::name(7), jp)
				<< FF8Text::toFF8(Data::name(8), jp) << FF8Text::toFF8(Data::name(9), jp)
				<< FF8Text::toFF8(Data::name(10), jp) << FF8Text::toFF8(Data::name(15), jp)
				<< FF8Text::toFF8(Data::name(12), jp) << FF8Text::toFF8(Data::name(14), jp);

		namesWidth[0] = calcFF8TextWidth(names.at(0));
		namesWidth[1] = calcFF8TextWidth(names.at(1));
		namesWidth[2] = calcFF8TextWidth(names.at(2));
		namesWidth[3] = calcFF8TextWidth(names.at(3));
		namesWidth[4] = calcFF8TextWidth(names.at(4));
		namesWidth[5] = calcFF8TextWidth(names.at(5));
		namesWidth[6] = calcFF8TextWidth(names.at(6));
		namesWidth[7] = calcFF8TextWidth(names.at(7));
		namesWidth[8] = calcFF8TextWidth(names.at(8));
		namesWidth[9] = calcFF8TextWidth(names.at(9));
		namesWidth[10] = calcFF8TextWidth(names.at(10));
		namesWidth[11] = calcFF8TextWidth(names.at(11));
		namesWidth[12] = calcFF8TextWidth(names.at(12));
		namesWidth[13] = calcFF8TextWidth(names.at(13));

		locations << FF8Text::toFF8(tr("Galbadia"), jp) << FF8Text::toFF8(tr("Esthar"), jp)
				<< FF8Text::toFF8(tr("Balamb"), jp) << FF8Text::toFF8(tr("Dollet"), jp)
				<< FF8Text::toFF8(tr("Timber"), jp) << FF8Text::toFF8(tr("Trabia"), jp)
				<< FF8Text::toFF8(tr("Centra"), jp) << FF8Text::toFF8(tr("Horizon"), jp);

		locationsWidth[0] = calcFF8TextWidth(locations.at(0));
		locationsWidth[1] = calcFF8TextWidth(locations.at(1));
		locationsWidth[2] = calcFF8TextWidth(locations.at(2));
		locationsWidth[3] = calcFF8TextWidth(locations.at(3));
		locationsWidth[4] = calcFF8TextWidth(locations.at(4));
		locationsWidth[5] = calcFF8TextWidth(locations.at(5));
		locationsWidth[6] = calcFF8TextWidth(locations.at(6));
		locationsWidth[7] = calcFF8TextWidth(locations.at(7));
	}

	startTimer(500);
}

void TextPreview::setFontImageAdd(TdwFile *tdwFile)
{
	if(tdwFile == NULL || tdwFile->tableCount() < 1)	return;

	TextPreview::tdwFile = tdwFile;
}

QImage TextPreview::getIconImage(int charId)
{
	return iconImage.copy((charId%16)*31, (charId/16)*12, iconWidth[charId], 12);
}

int TextPreview::calcFF8TextWidth(const QByteArray &ff8Text)
{
	int width = 0;

    foreach(quint8 c, ff8Text) {
		if(c>=32 && c<227)
			width += font->charWidth(0, c-32);
	}

	return width;
}

void TextPreview::clear()
{
	ff8Text.clear();
	ff8Windows.clear();
	maxW=maxH=0;
}

void TextPreview::reloadFont()
{
	FF8Font *ff8Font = FF8Font::font(Config::value("encoding", "00").toString());
	if(!ff8Font) {
		font = FF8Font::font("00")->tdw();
	} else {
		font = ff8Font->tdw();
	}
}

void TextPreview::setWins(const QList<FF8Window> &windows, bool update)
{
	ff8Windows = windows;
	if(update)
		calcSize();
	//qDebug() << "type" << window.type << "x" << window.x << "y" << window.y << "u1" << window.u1;
}

void TextPreview::resetCurrentWin()
{
	currentWin = 0;
}

int TextPreview::getCurrentWin()
{
	return ff8Windows.isEmpty() ? 0 : currentWin+1;
}

FF8Window TextPreview::getWindow()
{
	if(!ff8Windows.isEmpty())
		return ff8Windows.at(currentWin);
	else {
		FF8Window ff8Window;
		ff8Window.type = NOWIN;
		ff8Window.script_pos = ff8Window.ask_first =
		        ff8Window.ask_first2 = ff8Window.ask_last =
		        ff8Window.u1 = ff8Window.x = ff8Window.y = 0;
		return ff8Window;
	}
}

void TextPreview::setReadOnly(bool readOnly)
{
	this->readOnly = readOnly;
}

int TextPreview::getNbWin()
{
	return ff8Windows.size();
}

void TextPreview::nextWin()
{
	if(currentWin+1 < ff8Windows.size()) {
//		qDebug() << "TextPreview::nextWin()";
		++currentWin;
		update();
	}
}

void TextPreview::prevWin()
{
	if(currentWin > 0) {
//		qDebug() << "TextPreview::prevWin()";
		--currentWin;
		update();
	}
}

void TextPreview::clearWin()
{
	ff8Windows.clear();
}

void TextPreview::setText(const QByteArray &textData, bool reset)
{
	ff8Text = textData;
	if(reset)
		currentPage = 0;
	calcSize();
}

int TextPreview::getCurrentPage()
{
	return pagesPos.isEmpty() ? 0 : currentPage+1;
}

int TextPreview::getNbPages()
{
	return pagesPos.size();
}

void TextPreview::nextPage()
{
	if(currentPage+1 < pagesPos.size()) {
		++currentPage;
		update();
	}
}

void TextPreview::prevPage()
{
	if(currentPage > 0) {
		--currentPage;
		update();
	}
}

void TextPreview::calcSize()
{
	int line=0, ask_first=-1, ask_last=-1, width, height=28, size=this->ff8Text.size();
	maxW=maxH=0;
	pagesPos.clear();
	pagesPos.append(0);
	quint8 caract;
	bool jp = font->tableCount() == 4;
	if(!ff8Windows.isEmpty()) {
		const FF8Window &ff8Window = ff8Windows.at(currentWin);

		if(ff8Window.type == JsmOpcode::AASK) {
			ask_first = ff8Window.ask_first;
			ask_last = ff8Window.ask_last;
		}
	}

	width = ask_first==0 && ask_last>=0 ? 79 : 15;

	for(int i=0 ; i<size ; ++i) {
		caract = (quint8)ff8Text.at(i);
		if(caract==0) break;
		switch(caract) {
		case 0x1: // New Page
			if(height>maxH)	maxH = height;
			if(width>maxW)	maxW = width;
			width = 15;
			height = 28;
			pagesPos.append(i+1);
			break;
		case 0x2: // \n
			if(width>maxW)	maxW = width;
			++line;
			width = (ask_first<=line && ask_last>=line ? 79 : 15);//32+15+32 (padding for arrow) or 15
			height += 16;
			break;
		case 0x3: // Character names
			caract = (quint8)ff8Text.at(++i);
			if(caract>=0x30 && caract<=0x3a)
				width += namesWidth[caract-0x30];
			else if(caract==0x40)
				width += namesWidth[11];
			else if(caract==0x50)
				width += namesWidth[12];
			else if(caract==0x60)
				width += namesWidth[13];
			break;
		case 0x4: // Vars
			caract = (quint8)ff8Text.at(++i);
			if((caract>=0x20 && caract<=0x27) || (caract>=0x30 && caract<=0x37))
				width += font->charWidth(0, 1);// 0
			else if(caract>=0x40 && caract<=0x47)
				width += font->charWidth(0, 1)*8;// 00000000
			break;
		case 0x5: // Icons
			caract = (quint8)ff8Text.at(++i)-0x20;
            if(caract<96)
				width += iconWidth[caract]+iconPadding[caract];
			break;
		case 0xe: // Locations
			caract = (quint8)ff8Text.at(++i);
			if(caract>=0x20 && caract<=0x27)
				width += locationsWidth[caract-0x20];
			break;
		case 0x19: // Jap 1
			if(jp) {
				caract = (quint8)ff8Text.at(++i);
				if(caract>=0x20)
					width += font->charWidth(1, caract-0x20);
			}
			break;
		case 0x1a: // Jap 2
			if(jp) {
				caract = (quint8)ff8Text.at(++i);
				if(caract>=0x20)
					width += font->charWidth(2, caract-0x20);
			}
			break;
		case 0x1b: // Jap 3
			if(jp) {
				caract = (quint8)ff8Text.at(++i);
				if(caract>=0x20)
					width += font->charWidth(3, caract-0x20);
			}
			break;
		case 0x1c: // Jap 4
			if(tdwFile) {
				caract = (quint8)ff8Text.at(++i);
				if(caract>=0x20)
					width += tdwFile->charWidth(0, caract-0x20);
			}
			break;
		default:
			if(caract<0x20)
				++i;
			else if(jp) {
				width += font->charWidth(0, caract-0x20);
			} else {
				if(caract<232)
					width += font->charWidth(0, caract-0x20);
				else if(caract>=232)
					width += font->charWidth(0, (quint8)optimisedDuo[caract-232][0]) + font->charWidth(0, (quint8)optimisedDuo[caract-232][1]);
			}
			break;
		}
	}

	if(height>maxH)	maxH = height;
	if(width>maxW)	maxW = width;
	if(maxW>322)	maxW = 322;
	if(maxH>226)	maxH = 226;

	update();
}

QPoint TextPreview::realPos(const FF8Window &ff8Window)
{
	int windowX=ff8Window.x, windowY=ff8Window.y;
	if(windowX < 0) {
		windowX = 0;
	}
	if(windowY < 0) {
		windowY = 0;
	}

	if(windowX+maxW>312) {
		windowX = 312-maxW;
	}
	if(windowY+maxH>223) {
		windowY = 223-maxH;
	}

	if(windowX<8)	windowX = 8;
	if(windowY<8)	windowY = 8;

	return QPoint(windowX, windowY);
}

void TextPreview::drawTextArea(QPainter *painter)
{
	useTimer = false;

	if(ff8Text.isEmpty())	return;

	bool jp = font->tableCount() == 4;
	FF8Window ff8Window = getWindow();

	/* Window Background */

	if(ff8Window.type != NOWIN) {
		painter->translate(realPos(ff8Window));
	}

	painter->setBrush(QPixmap(":/images/menu-fond.png"));

	/* Frame */
	QPen pen(QColor(41,41,41));
	pen.setWidth(1);
	pen.setJoinStyle(Qt::MiterJoin);
	painter->setPen(pen);
	painter->drawRect(0, 0, maxW, maxH);

	painter->setPen(QColor(132,132,132));
	painter->drawLine(2, 1, maxW-2, 1);
	painter->drawLine(1, 2, 1, maxH-2);

	painter->setPen(QColor(58,58,58));
	painter->drawLine(2, maxH-1, maxW-1, maxH-1);
	painter->drawLine(maxW-1, 2, maxW-1, maxH-2);

	painter->setPen(QColor(99,99,99));
	painter->drawPoint(1, 1);

	painter->setPen(QColor(82,82,82));
	painter->drawPoint(maxW-1, 1);
	painter->drawPoint(1, maxH-1);

	int ask_first=-1, ask_last=-1;

	if(ff8Window.type == JsmOpcode::AASK) {
		ask_first = ff8Window.ask_first;
		ask_last = ff8Window.ask_last;
		if(ask_last >= ask_first)
			painter->drawPixmap(10, 11+16*ask_first, QPixmap(":/images/cursor.png"));
	}

	/* Text */

	fontColor = TdwFile::White;

	int charId, line=0, x = (ask_first==0 && ask_last>=0 ? 40 : 8), y = 8;
	int start = pagesPos.value(currentPage, 0), size = ff8Text.size();

	for(int i=start ; i<size ; ++i)
	{
		charId = (quint8)ff8Text.at(i);

		if(charId>=32)
		{
			if(jp) {
				letter(&x, &y, charId-32, painter);// 210-32
			} else {
				if(charId<227) {
					letter(&x, &y, charId-32, painter);
				} else if(charId>=232) {
					letter(&x, &y, optimisedDuo[charId-232][0], painter);
					letter(&x, &y, optimisedDuo[charId-232][1], painter);
				}
			}
		}
		else if(charId<=1)//NewPage
			break;
		else if(charId==2)//\n
		{
			++line;
			x = (ask_first<=line && ask_last>=line ? 40 : 8);
			y += 16;
		}
		else if(charId<32)
		{
			++i;
			switch(charId)
			{
			case 0x03: // Character names
				charId = (quint8)ff8Text.at(i);

				if(charId>=0x30 && charId<=0x3a)
					word(&x, &y, names.at(charId-0x30), painter, 5);
				else if(charId==0x40)
					word(&x, &y, names.at(11), painter, 5);
				else if(charId==0x50)
					word(&x, &y, names.at(12), painter, 5);
				else if(charId==0x60)
					word(&x, &y, names.at(13), painter, 5);
				break;
			case 0x04:// Var
				charId = (quint8)ff8Text.at(i);

				if((charId>=0x20 && charId<=0x27) || (charId>=0x30 && charId<=0x37))
					word(&x, &y, FF8Text::toFF8("0", false), painter);
				else if(charId>=0x40 && charId<=0x47)
					word(&x, &y, FF8Text::toFF8("00000000", false), painter);
				break;
			case 0x05: // Icons
				charId = (quint8)ff8Text.at(i)-0x20;

				if(charId>=0 && charId<96)
				{
					painter->drawImage(x, y, getIconImage(charId));
					x += iconWidth[charId] + iconPadding[charId];
				}
				break;
			case 0x06: // Colors
				charId = (quint8)ff8Text.at(i);

				if(charId>=0x20 && charId<=0x27)// Colors
					fontColor = (TdwFile::Color)(charId-0x20);
				else if(charId>=0x28 && charId<=0x2f)// BlinkColors
				{
					useTimer = true;
					fontColor = (TdwFile::Color)(charId-0x20);
				}
				break;
			case 0x0e: // Locations
				charId = (quint8)ff8Text.at(i);
				if(charId>=0x20 && charId<=0x27)
					word(&x, &y, locations.at(charId-0x20), painter, 5);
				break;
			case 0x19: // Jap 1
				if(jp) {
					letter(&x, &y, (quint8)ff8Text.at(i)-0x20, painter, 1);
				}
				break;
			case 0x1a: // Jap 2
				if(jp) {
					letter(&x, &y, (quint8)ff8Text.at(i)-0x20, painter, 2);
				}
				break;
			case 0x1b: // Jap 3
				if(jp) {
					letter(&x, &y, (quint8)ff8Text.at(i)-0x20, painter, 3);
				}
				break;
			case 0x1c: // Jap 4
				if(jp) {
					letter(&x, &y, (quint8)ff8Text.at(i)-0x20, painter, 4);
				}
				break;
			}
		}
	}
}

void TextPreview::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event)
	QPixmap pix(320, 224);
	QPainter painter(&pix);

	/* Screen background */

	painter.setBrush(Qt::black);
	painter.drawRect(0, 0, 320, 224);

	drawTextArea(&painter);

	painter.end();

	QPainter painter2(this);

	if(!isEnabled()) {
		QStyleOption opt;
		opt.initFrom(this);
		painter2.drawPixmap(0, 0, QWidget::style()->generatedIconPixmap(QIcon::Disabled, pix, &opt));
	}
	else {
		painter2.drawPixmap(0, 0, pix);
	}

	painter2.end();
}

void TextPreview::timerEvent(QTimerEvent *)
{
	if(useTimer) {
		curFrame = !curFrame;
		repaint();
	}
}

void TextPreview::mousePressEvent(QMouseEvent *event)
{
	if(!readOnly && event->button() == Qt::LeftButton) {
		FF8Window ff8Window = getWindow();
		if(ff8Window.type != NOWIN) {
			QPoint real = realPos(ff8Window);

			acceptMove = event->x() >= real.x() && event->x() < real.x()+maxW
						 && event->y() >= real.y() && event->y() < real.y()+maxH;

			if(acceptMove) {
				moveStartPosition = event->pos();
				setCursor(QCursor(Qt::ClosedHandCursor));
			}
		}
		else
			acceptMove = false;
	}
}

void TextPreview::mouseMoveEvent(QMouseEvent *event)
{
	if(readOnly || (!(event->buttons() & Qt::LeftButton) && !acceptMove))
		return;

	FF8Window ff8Window = getWindow();

	if(ff8Window.type == NOWIN)	return;

	FF8Window originalWindow = ff8Window;

	int x = qMax(0, ff8Window.x) + event->x() - moveStartPosition.x();
	int y = qMax(0, ff8Window.y) + event->y() - moveStartPosition.y();

	if(x<0)	x = 0;
	if(y<0)	y = 0;

	ff8Window.x = x;
	ff8Window.y = y;
	QPoint real = realPos(ff8Window);

	if(originalWindow.x < 0) {
		real.setX(originalWindow.x);
	}
	if(originalWindow.y < 0) {
		real.setY(originalWindow.y);
	}

	if(originalWindow.x != real.x() || originalWindow.y != real.y()) {
		ff8Window.x = qMax(0, real.x());
		ff8Window.y = qMax(0, real.y());

		emit positionChanged(real);

		moveStartPosition = event->pos();

		ff8Windows.replace(currentWin, ff8Window);

		update();
	}
}

void TextPreview::mouseReleaseEvent(QMouseEvent *)
{
	if(acceptMove) {
		unsetCursor();
	}
}

void TextPreview::letter(int *x, int *y, int charId, QPainter *painter, quint8 tableId)
{
	QImage image;

	if(tableId >= 5) {
		bool tagInJp = tr("false", "Use Japanese Encoding") == "true";
		image = FF8Font::font(tagInJp ? "01" : "00")->tdw()->letter(charId, fontColor, curFrame);
	} else if(tableId == 4) {
		if(tdwFile) {
			image = tdwFile->letter(0, charId, fontColor, curFrame);
		}
	} else {
		image = font->letter(tableId, charId, fontColor, curFrame);
	}

	if(!image.isNull()) {
		painter->drawImage(*x, *y, image);
	}

	if(tableId >= 5) {
		bool tagInJp = tr("false", "Use Japanese Encoding") == "true";
		*x += FF8Font::font(tagInJp ? "01" : "00")->tdw()->charWidth(0, charId);
	} else if(tableId == 4) {
		if(tdwFile) {
			*x += tdwFile->charWidth(0, charId);
		}
	} else {
		*x += font->charWidth(tableId, charId);
	}
}

void TextPreview::word(int *x, int *y, const QByteArray &charIds, QPainter *painter, quint8 tableId)
{
	foreach(char charId, charIds) {
		if(charId<0x20)	return;
		letter(x, y, charId-0x20, painter, tableId);
	}
}

const quint8 TextPreview::iconWidth[96] =
{
	15, 15, 15, 15, 9, 9, 9, 9, 31, 0, 0, 31, 12, 12, 12, 12,
	15, 15, 15, 15, 9, 9, 9, 9, 31, 0, 0, 31, 12, 12, 12, 12,
	0, 8, 6, 11, 8, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	12, 12, 12, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 24
};

const quint8 TextPreview::iconPadding[96] =
{
	2, 2, 2, 2, 8, 8, 8, 8, 2, 17, 17, 2, 3, 3, 3, 3,
	2, 2, 2, 2, 8, 8, 8, 8, 2, 17, 17, 2, 3, 3, 3, 3,
	9, 1, 3, 6, 9, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

const char *TextPreview::optimisedDuo[24] =
{
	"\x47\x4c",//in
	"\x43\x00",//e
	"\x4c\x43",//ne
	"\x52\x4d",//to
	"\x50\x43",//re
	"\x2c\x34",//HP
	"\x4a\x00",//l
	"\x4a\x4a",//ll
	"\x2b\x2a",//GF
	"\x4c\x52",//nt
	"\x47\x4a",//il
	"\x4d\x00",//o
	"\x43\x44",//ef
	"\x4d\x4c",//on
	"\x00\x55",// w
	"\x00\x50",// r
	"\x55\x47",//wi
	"\x44\x47",//fi
	"\x29\x27",//EC
	"\x51\x00",//s
	"\x3f\x50",//ar
	"\x2a\x29",//FE
	"\x00\x37",// S
	"\x3f\x45" //ag
};

QList<QByteArray> TextPreview::names;

int TextPreview::namesWidth[14];

QList<QByteArray> TextPreview::locations;

int TextPreview::locationsWidth[8];
