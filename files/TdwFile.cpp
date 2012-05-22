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

TimFile TdwFile::tim;

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
		tableCount = sizeHeader > 112 ? sizeHeader / 112 : 1;

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

	if(!tim.open(tdw.mid(posData))) {
		return false;
	}

	return true;
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
	if(tim.save(data)) {
		tdw.append(data);
	} else {
		return false;
	}

	modified = false;

	return true;
}

QPixmap TdwFile::image(int palID) const
{
	tim.setCurrentColorTable(palID);
	return QPixmap::fromImage(tim.image());
}

QPixmap TdwFile::image(const QByteArray &data, int palID)
{
	TdwFile tdw;
	if(tdw.open(data)) {
		return tdw.image(palID);
	}

	return QPixmap();
}

QImage TdwFile::letter(int charId, int fontColor, bool curFrame) const
{
	int palID = (fontColor % 8)*2 + (charId % 2 != 0);
	tim.setCurrentColorTable(palID);
	const QImage &img = tim.image();
	charId /= 2;
	QImage ret = img.copy((charId%21)*12, (charId/21)*12, 12, 12);
	if(fontColor > 7 && !curFrame) {
		QVector<QRgb> colorTable;
		foreach(QRgb color, ret.colorTable()) {
			colorTable.append(qRgba(qRed(color) * 0.75, qGreen(color) * 0.75, qBlue(color) * 0.75, qAlpha(color)));
		}
		ret.setColorTable(colorTable);
	}
	return ret;

	/*quint32 palSize=0, color=0, x=0, y=0;
	quint16 w, h;
	const char *constData = tim.constData();
	int i, posPal;
	bool blink = fontColor > 7;

	if(tim.isNull()) {
		return QImage();
	}

	posPal = (fontColor % 8)*64 + (charId % 2 == 0 ? 20 : 52);

	memcpy(&palSize, &constData[8], 4);
	memcpy(&w, &constData[16+palSize], 2);
	memcpy(&h, &constData[18+palSize], 2);
	w*=2;

	QImage image(12, 12, QImage::Format_Indexed8);
	uchar *pixels = image.bits();

	for(i=0 ; i<16 ; ++i) {
		memcpy(&color, &constData[posPal + i*2], 2);
//		qDebug() << QString::number(color, 16);
		if(color == 0) {
			image.setColor(i, qRgba(0,0,0,0));
		} else if(blink && !curFrame) {
			image.setColor(i, qRgb((color & 31)*COEFF_COLOR*0.75, (color>>5 & 31)*COEFF_COLOR*0.75, (color>>10 & 31)*COEFF_COLOR*0.75));
		} else {
			image.setColor(i, qRgb((color & 31)*COEFF_COLOR, (color>>5 & 31)*COEFF_COLOR, (color>>10 & 31)*COEFF_COLOR));
		}
	}

	i = charId/2 * 6;
	while(y < 12 && 20+palSize+i < (quint32)tim.size())
	{
		pixels[x + y*12] = (quint8)tim.at(20+palSize+i) & 0xF;
		++x;

		pixels[x + y*12] = (quint8)tim.at(20+palSize+i) >> 4;
		++x;

		if(x==12) {
			x = 0;
			++y;
			i += w - 5;
		} else {
			++i;
		}
	}
	return image;*/
}

const quint8 *TdwFile::charWidth(quint8 tableID) const
{
	return _charWidth.at(tableID);
}

int TdwFile::tableCount() const
{
	return _charWidth.size();
}

int TdwFile::charCount(quint8 tableID) const
{
	return _charCount.value(tableID, 0);

	if(tableID >= tableCount())		return 0;

	quint8 *charWidth = _charWidth.at(tableID);

	for(int i=0 ; i<224 ; ++i) {
		if(charWidth[i]==0)		return i;
	}

	return 224;
}
