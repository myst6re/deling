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
#include "files/PmpFile.h"

QString PmpFile::currentFieldName;

PmpFile::PmpFile()
	: File()
{
}

bool PmpFile::open(const QByteArray &pmp)
{
	this->pmp = pmp;
	modified = false;

//	quint32 lzsSize;

//	memcpy(&lzsSize, pmp.constData(), 4);

//	qDebug() << lzsSize << pmp.left(24).toHex() << pmp.size();
//	int dec = 0;
//	this->pmp = LZS::decompress(&(pmp.constData()[dec]), pmp.size()-dec);
//	if(pmp.size() != 4) {
//		palette().save(QString("pmp/%1-palette.png").arg(currentFieldName));
//		for(int palID=0 ; palID<16 ; ++palID) {
//			image(4, palID).save(QString("pmp/%1-%2.png").arg(currentFieldName).arg(palID));
//		}
//	}

	return true;
}

bool PmpFile::save(QByteArray &pmp)
{
	pmp = this->pmp;

	return true;
}

QByteArray PmpFile::getPmpData() const
{
	return pmp.left(4);
}

void PmpFile::setPmpData(const QByteArray &pmp)
{
	this->pmp.replace(0, 4, pmp.leftJustified(4, '\0', true));
	modified = true;
}

QImage PmpFile::palette() const
{
	if(pmp.size() < 516) return QImage();

	const char *constPmp = pmp.constData();

	QImage pal(16, 16, QImage::Format_RGB32);
	QRgb *pxPal = (QRgb *)pal.bits();

	for(int i=0 ; i<pal.width()*pal.height() ; ++i) {
		quint16 color;
		memcpy(&color, &constPmp[4+i*2], 2);
		pxPal[i] = FF8Image::fromPsColor(color);
	}

	return pal;
}

QImage PmpFile::image(quint8 deph, quint8 palID) const
{
	if(pmp.size() <= 516) return QImage();

	if(deph == 4) {
		int width = 256;
		int height = (pmp.size() - 516) * 2 / width;
		const char *constPmp = pmp.constData();

		if(pmp.size() < 516 + width * height / 2) return QImage();

		QImage img(width, height, QImage::Format_RGB32);
		QRgb *px = (QRgb *)img.bits();

		for(int j=0 ; j<width*height ; ++j) {
			quint16 color;
			memcpy(&color, &constPmp[4 + palID * 32 + ((quint8)pmp.at(516 + j/2) & 0xF)*2], 2);

			px[j] = FF8Image::fromPsColor(color);

			++j;
			memcpy(&color, &constPmp[4 + palID * 32 + ((quint8)pmp.at(516 + j/2) >> 4)*2], 2);

			px[j] = FF8Image::fromPsColor(color);
		}

		return img;
	} else if(deph == 8) {
		int width = 128;
		int height = (pmp.size() - 516) / width;
		const char *constPmp = pmp.constData();

		if(pmp.size() < 516 + width * height) return QImage();

		QImage img(width, height, QImage::Format_RGB32);
		QRgb *px = (QRgb *)img.bits();

		for(int j=0 ; j<width*height ; ++j) {
			quint16 color;
			memcpy(&color, &constPmp[4 + (quint8)pmp.at(516 + j)*2], 2);

			px[j] = FF8Image::fromPsColor(color);
		}

		return img;
	} else if(deph == 16) {
		int width = 64;
		int height = (pmp.size() - 516) / 2 / width;
		const char *constPmp = pmp.constData();

		if(pmp.size() < 516 + width * height * 2) return QImage();

		QImage img(width, height, QImage::Format_RGB32);
		QRgb *px = (QRgb *)img.bits();

		for(int j=0 ; j<width*height ; ++j) {
			quint16 color;
			memcpy(&color, &constPmp[516 + j*2], 2);

			px[j] = FF8Image::fromPsColor(color);
		}

		return img;
	}

	return QImage();
}
