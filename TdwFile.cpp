#include "TdwFile.h"

QByteArray TdwFile::tim;

TdwFile::TdwFile() :
	modified(false)
{
}

bool TdwFile::isModified()
{
	return modified;
}

bool TdwFile::open(const QByteArray &tdw)
{
	quint32 pos, i;
	const char *constData = tdw.constData();
	quint8 space;

	if(tdw.size() <= 8) {
		return false;
	}
	memcpy(&pos, &constData[4], 4);

	for(i=0 ; i<pos-8 && i<112 ; ++i) {
		space = tdw.at(8+i);
		_charWidth[i*2] = space & 0xF;
		_charWidth[i*2 + 1] = space >> 4;
	}
	for( ; i<112 ; ++i) {
		_charWidth[i*2] = 0;
		_charWidth[i*2 + 1] = 0;
	}

	tim = tdw.mid(pos);
	if(FF8Image::tim(tim).isNull()) {
		tim = QByteArray();
	}

	return true;
}

QPixmap TdwFile::image(int palID) const
{
	return FF8Image::tim(tim);
}

QPixmap TdwFile::image(const QByteArray &data, int palID)
{
	quint32 pos;
	const char *constData = data.constData();

	if(data.size() <= 8) {
		return QPixmap();
	}
	memcpy(&pos, &constData[4], 4);

	return FF8Image::tim(data.mid(pos), palID);
}

QImage TdwFile::letter(int charId, int fontColor, bool curFrame) const
{
	quint32 palSize=0, color=0, x=0, y=0;
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
	while(y < 12)
	{
		image.setPixel(x, y, (quint8)tim.at(20+palSize+i) & 0xF);
		++x;

		image.setPixel(x, y, (quint8)tim.at(20+palSize+i) >> 4);
		++x;

		if(x==12) {
			x = 0;
			++y;
			i += w - 5;
		} else {
			++i;
		}
	}
	return image;
}

quint8 *TdwFile::charWidth()
{
	return _charWidth;
}
