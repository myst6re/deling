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
	File()
{
}

TdwFile::~TdwFile()
{
	foreach(quint8 *charWidth, _charWidth) {
		delete[] charWidth;
	}
}

bool TdwFile::isNull() const
{
	return !_tim.isValid();
}

bool TdwFile::open(const QByteArray &tdw)
{
	quint32 posHeader, sizeHeader, posData, i;
	const char *constData = tdw.constData();
	quint8 space, tableID, tableCount;

	if(tdw.size() <= 8) {
		qWarning() << "format tdw incorrect ou inconnu : too short";
		return false;
	}
	memcpy(&posHeader, constData, 4);
	memcpy(&posData, &constData[4], 4);

	if(posHeader != 8 || posData > (quint32)tdw.size()) {
		qWarning() << "format tdw incorrect ou inconnu : posHeader" << posHeader;
		return false;
	}

	sizeHeader = posData - posHeader;

	foreach(quint8 *table, _charWidth) {
		delete table;
	}

	_charWidth.clear();
	_charCount.clear();

	if(sizeHeader != 0) {
		tableCount = sizeHeader / 112 + int(sizeHeader % 112 != 0);

		for(tableID=0 ; tableID<tableCount ; ++tableID) {

			quint8 *charWidth = new quint8[224];

			for(i=0 ; i<sizeHeader && i<quint32(112) ; ++i) {
				space = tdw.at(posHeader + tableID*112 + i);
				charWidth[i*2] = space & 0xF;
				charWidth[i*2 + 1] = space >> 4;
			}
			for( ; i<quint32(112) ; ++i) {
				charWidth[i*2] = 0;
				charWidth[i*2 + 1] = 0;
			}

			_charWidth.append(charWidth);
			_charCount.append(qMin((sizeHeader - tableID*112) * 2, quint32(224)));
		}
	} // else empty tdw, the tim contains only palettes

	if(!_tim.open(tdw.mid(posData))
			|| (_tim.colorTableCount() != 8 && _tim.colorTableCount() != 16 && _tim.colorTableCount() != 32)) {
		qWarning() << "format tdw incorrect ou inconnu : tim" << _tim.colorTableCount();
		return false;
	}

	modified = false;

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

	return true;
}

QImage TdwFile::image(Color color)
{
	_tim.setCurrentColorTable(palette(color, 0));
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

bool TdwFile::isOptimizedVersion() const
{
	return _tim.colorTableCount() >= 16;
}

int TdwFile::palette(Color color, int charId) const
{
	if(isOptimizedVersion()) {
		return (color % 8)*2 + (charId % 2);
	} else {
		return color % 8;
	}
}

QPoint TdwFile::letterPos(int charId) const
{
	QSize size = letterSize();
	if(isOptimizedVersion()) {
		charId /= 2;
	}
	return QPoint((charId%21)*size.width(), (charId/21)*size.height());
}

QSize TdwFile::letterSize()
{
	return QSize(12, 12);
}

QRect TdwFile::letterRect(int charId) const
{
	return QRect(letterPos(charId), letterSize());
}

QImage TdwFile::letter(quint8 tableId, quint8 charId, Color color, bool curFrame)
{
	return letter(tableId * 224 + charId, color, curFrame);
}

QImage TdwFile::letter(int charId, Color color, bool curFrame)
{
	_tim.setCurrentColorTable(palette(color, charId));
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

void TdwFile::setLetter(quint8 tableId, quint8 charId, const QImage &image)
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

	modified = true;
}

uint TdwFile::letterPixelIndex(quint8 tableId, quint8 charId, const QPoint &pos) const
{
	return letterPixelIndex(tableId * 224 + charId, pos);
}

uint TdwFile::letterPixelIndex(int charId, const QPoint &pos) const
{
	uint index = _tim.image().pixelIndex(letterPos(charId) + pos);

	if(isOptimizedVersion()) {
		// returns a number between 0 and 3
		const QRgb &color = _tim.colorTable(charId % 2).at(index);

		return _tim.colorTable(0).indexOf(color);
	} else {
		// returns a number between 0 and 15
		return index % 16;
	}
}

bool TdwFile::setLetterPixelIndex(quint8 tableId, quint8 charId, const QPoint &pos, uint pixelIndex)
{
	return setLetterPixelIndex(tableId * 224 + charId, pos, pixelIndex);
}

bool TdwFile::setLetterPixelIndex(int charId, const QPoint &pos, uint pixelIndex)
{
	if(isOptimizedVersion()) {
		// pixelIndex must be a number between 0 and 3
		QVector<QRgb> modifColorTable, notModifColorTable;
		modifColorTable = _tim.colorTable(charId % 2);
		notModifColorTable = _tim.colorTable((charId + 1) % 2);
		uint realPixelIndex = _tim.imagePtr()->pixelIndex(letterPos(charId) + pos);

		const QRgb &newColor = _tim.colorTable(0).at(pixelIndex % 4);
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
	} else {
		// pixelIndex must be a number between 0 and 15
		_tim.imagePtr()->setPixel(letterPos(charId) + pos, pixelIndex % 16);
		modified = true;
		return true;
	}
}

quint8 TdwFile::charWidth(quint8 tableId, quint8 charId) const
{
	return tableId < _charWidth.size() && charId < 224 ? _charWidth[tableId][charId] : 0;
}

void TdwFile::setCharWidth(quint8 tableId, quint8 charId, quint8 width)
{
	_charWidth[tableId][charId] = width;
	modified = true;
}

int TdwFile::tableCount() const
{
	return _charWidth.size();
}

int TdwFile::charCount(quint8 tableId) const
{
	return _charCount.value(tableId, 0);

	/*if(tableId >= tableCount())		return 0;

	quint8 *charWidth = _charWidth.at(tableId);

	for(int i=0 ; i<224 ; ++i) {
		if(charWidth[i]==0)		return i;
	}

	return 224;*/
}

TexFile TdwFile::toTexFile() const
{
	if(_tim.colorTableCount() != 8 || _tim.image().width() != 256 || _tim.image().height() != 120) {
		qWarning() << "tim format nbPal=" << _tim.colorTableCount() << _tim.image().width() << "x" << _tim.image().height();
		return TexFile();
	}

	QImage img(256, 128, QImage::Format_Indexed8);
	QVector<QRgb> colorTable = _tim.image().colorTable();
	for(int i=0 ; i<colorTable.size() ; ++i) {
		QRgb &color = colorTable[i];
		if(color != 0) {
			color = qRgba(qRed(color), qGreen(color), qBlue(color), 254);
		} else {
			color = qRgba(0, 0, 0, 0);
		}
	}
	img.setColorTable(colorTable);
	img.fill(0);

	for(int y=0 ; y<120 ; ++y) {
		for(int x=0 ; x<256 ; ++x) {
			img.setPixel(x, y, _tim.image().pixelIndex(x, y));
		}
	}

	TexStruct header = TexStruct();

	header.version = 2;
//	header.unknown1= 0;
	header.hasColorKey= 1;
	header.unknown2= 1;
	header.unknown3= 31;
	header.minBitsPerColor= 4;
	header.maxBitsPerColor= 8;
	header.minAlphaBits= 4;
	header.maxAlphaBits= 8;
	header.minBitsPerPixel= 8;
	header.maxBitsPerPixel= 32;
//	header.unknown4= 0;
	header.nbPalettes= 16;
	header.nbColorsPerPalette1= 16;
	header.bitDepth= 4;
	header.imageWidth= 256;
	header.imageHeight= 128;
//	header.pitch= 0;
//	header.unknown5= 0;
	header.hasPal= 1;
	header.bitsPerIndex= 8;
//	header.indexedTo8bit= 0;
	header.paletteSize= 256;
	header.nbColorsPerPalette2= 16;
	header.runtimeData1= 94790092;
	header.bitsPerPixel= 8;
	header.bytesPerPixel= 1;
//	header.nbRedBits1= 0;
//	header.nbGreenBits1= 0;
//	header.nbBlueBits1= 0;
//	header.nbAlphaBits1= 0;
//	header.redBitmask= 0;
//	header.greenBitmask= 0;
//	header.blueBitmask= 0;
//	header.alphaBitmask= 0;
//	header.redShift= 0;
//	header.greenShift= 0;
//	header.blueShift= 0;
//	header.alphaShift= 0;
//	header.nbRedBits2= 0;
//	header.nbGreenBits2= 0;
//	header.nbBlueBits2= 0;
//	header.nbAlphaBits2= 0;
//	header.redMax= 0;
//	header.greenMax= 0;
//	header.blueMax= 0;
//	header.alphaMax= 0;
//	header.hasColorKeyArray= 0;
//	header.runtimeData2= 0;
	header.referenceAlpha= 255;
	header.runtimeData3= 4;
//	header.unknown6= 0;
//	header.paletteIndex= 0;
//	header.runtimeData4= 0;
	header.runtimeData5= 94822908;
//	header.unknown7= 0;
	header.unknown8= 288;
	header.unknown9= 224;
	header.unknown10= 960;
	header.unknown11= 256;

	return TexFile(TextureFile(img, _tim.colorTables()), header);
}
