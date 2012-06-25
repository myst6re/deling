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
#include "files/TdwFile.h"

TdwFile::TdwFile() :
	modified(false)
{
}

TdwFile::~TdwFile()
{
	foreach(quint8 *table, _charWidth) {
		delete table;
	}
}

bool TdwFile::isModified() const
{
	return modified;
}

bool TdwFile::open(const QByteArray &tdw)
{
	quint32 posHeader, sizeHeader, posData, i;
	const char *constData = tdw.constData();
	quint8 space, tableID, tableCount, *charWidth;

	if(tdw.size() <= 8) {
		return false;
	}
	memcpy(&posHeader, constData, 4);
	memcpy(&posData, &constData[4], 4);

	if(posHeader != 8 || posData > (quint32)tdw.size()) {
		qWarning() << "format tdw incorrect ou inconnu : posHeader" << posHeader;
		return false;
	}

	sizeHeader = posData - posHeader;

	if(sizeHeader != 0) {
		tableCount = sizeHeader / 112 + int(sizeHeader % 112 != 0);

		for(tableID=0 ; tableID<tableCount ; ++tableID) {

			charWidth = new quint8[224];

			for(i=tableID*112 ; i<sizeHeader && i<quint32(tableID*112+112) ; ++i) {
				space = tdw.at(posHeader+i);
				charWidth[i*2] = space & 0xF;
				charWidth[i*2 + 1] = space >> 4;
			}
			for( ; i<quint32(tableID*112+112) ; ++i) {
				charWidth[i*2] = 0;
				charWidth[i*2 + 1] = 0;
			}

			_charWidth.append(charWidth);
			_charCount.append(qMin((sizeHeader - tableID*112) * 2, quint32(224)));
		}
	}

	if(!_tim.open(tdw.mid(posData))) {
		return false;
	}

	return true;
}

void TdwFile::close()
{
	if(!isModified()) {
		_tim.clear();
		_charCount.clear();
		foreach(quint8 *table, _charWidth) {
			delete table;
		}
		_charWidth.clear();
	}
}

bool TdwFile::save(QByteArray &tdw)
{
	quint32 posHeader=8, posData=0;
	quint8 space, i;

	tdw.append((char *)&posHeader, 4);
	tdw.append((char *)&posData, 4);

	foreach(quint8 *table, _charWidth) {
		for(i=0 ; i<112 ; ++i) {
			space = ((table[i*2 + 1] & 0xF) << 4) | (table[i*2] & 0xF);
			if(space == 0)	break;
			tdw.append((char)space);
		}
		if(i % 4 != 0) {
			tdw.append(QByteArray(i % 4, '\0'));
		}

	}

	posData = tdw.size();
	tdw.replace(4, 4, (char *)&posData, 4);

	QByteArray data;
	if(_tim.save(data)) {
		tdw.append(data);
	} else {
		return false;
	}

	modified = false;

	return true;
}

QImage TdwFile::image(Color color)
{
	int palID = (color % 8)*2;
	_tim.setCurrentColorTable(palID);
	return _tim.image();
}

QImage TdwFile::image(const QByteArray &data, Color color)
{
	TdwFile tdw;
	if(tdw.open(data)) {
		return tdw.image(color);
	}

	return QImage();
}

QPoint TdwFile::letterPos(int charId)
{
	QSize size = letterSize();
	charId /= 2;
	return QPoint((charId%21)*size.width(), (charId/21)*size.height());
}

QSize TdwFile::letterSize()
{
	return QSize(12, 12);
}

QRect TdwFile::letterRect(int charId)
{
	return QRect(letterPos(charId), letterSize());
}

QImage TdwFile::letter(quint8 tableId, int charId, Color color, bool curFrame)
{
	return letter(tableId * 224 + charId, color, curFrame);
}

QImage TdwFile::letter(int charId, Color color, bool curFrame)
{
	int palID = (color % 8)*2 + (charId % 2);
	_tim.setCurrentColorTable(palID);
	const QImage &img = _tim.image();
	QImage ret = img.copy(letterRect(charId));
	if(color > 7 && !curFrame) {
		QVector<QRgb> colorTable;
		foreach(QRgb color, ret.colorTable()) {
			colorTable.append(qRgba(qRed(color) * 0.75, qGreen(color) * 0.75, qBlue(color) * 0.75, qAlpha(color)));
		}
		ret.setColorTable(colorTable);
	}
	return ret;
}

void TdwFile::setLetter(quint8 tableId, int charId, const QImage &image)
{
	setLetter(tableId * 224 + charId, image);
}

void TdwFile::setLetter(int charId, const QImage &image)
{
	QRect rect = letterRect(charId);

	if(image.size() != rect.size())	return;

	int x2, y2 = rect.y();

	for(int y=0 ; y<image.height() ; ++y) {
		x2 = rect.x();

		for(int x=0 ; x<image.width() ; ++x) {
			_tim.imagePtr()->setPixel(x2, y2, image.pixelIndex(x, y));
			++x2;
		}

		++y2;
	}

	modified = true;
}

void TdwFile::setImage(const QImage &image, int hCount, int vCount)
{
	int letterWidth = image.width() / hCount, letterHeight = image.height() / vCount;
	int charCount = qMin(this->charCount(), hCount * vCount);
	QSize letterSize = this->letterSize();
	int x=0, y=0;

	for(int charId=0 ; charId<charCount ; ++charId) {
		setLetter(charId, image.copy(x * letterWidth, y * letterHeight, letterWidth, letterHeight).scaled(letterSize));
		if(++x == hCount) {
			x = 0;
			++y;
		}
	}
}

uint TdwFile::letterPixelIndex(quint8 tableId, int charId, const QPoint &pos) const
{
	return letterPixelIndex(tableId * 224 + charId, pos);
}

uint TdwFile::letterPixelIndex(int charId, const QPoint &pos) const
{
	// returns a number between 0 and 3
	uint index = _tim.image().pixelIndex(letterPos(charId) + pos);
	const QRgb &color = _tim.colorTable(charId % 2).at(index);

	return _tim.colorTable(0).indexOf(color);
}

bool TdwFile::setLetterPixelIndex(quint8 tableId, int charId, const QPoint &pos, uint pixelIndex)
{
	return setLetterPixelIndex(tableId * 224 + charId, pos, pixelIndex);
}

bool TdwFile::setLetterPixelIndex(int charId, const QPoint &pos, uint pixelIndex)
{
	// pixelIndex must be a number between 0 and 3

	QVector<QRgb> modifColorTable, notModifColorTable;
	modifColorTable = _tim.colorTable(charId % 2);
	notModifColorTable = _tim.colorTable((charId + 1) % 2);
	uint realPixelIndex = _tim.imagePtr()->pixelIndex(letterPos(charId) + pos);

	const QRgb &newColor = _tim.colorTable(0).at(pixelIndex);
	const QRgb &notModifColor = notModifColorTable.at(realPixelIndex);
	int index = -1;

	while((index = modifColorTable.indexOf(newColor, index + 1)) != -1) {
		if(notModifColorTable.at(index) == notModifColor) {
			_tim.imagePtr()->setPixel(letterPos(charId) + pos, index);
			modified = true;
			return true;
		}
	}

	return false;
}

const quint8 *TdwFile::charWidth(quint8 tableId) const
{
	return _charWidth.at(tableId);
}

void TdwFile::setCharWidth(quint8 tableId, int charId, quint8 width)
{
	_charWidth[tableId][charId] = width;
}

int TdwFile::tableCount() const
{
	return _charWidth.size();
}

int TdwFile::charCount(quint8 tableId) const
{
	return _charCount.value(tableId, 0);

	if(tableId >= tableCount())		return 0;

	quint8 *charWidth = _charWidth.at(tableId);

	for(int i=0 ; i<224 ; ++i) {
		if(charWidth[i]==0)		return i;
	}

	return 224;
}
