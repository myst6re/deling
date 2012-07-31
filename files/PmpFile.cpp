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

//	if(pmp.size() != 4) {
//		int height = (pmp.size() - 516) / 128;
//		const char *constPmp = pmp.constData();

//		QImage pal(16, 16, QImage::Format_RGB32);
//		QRgb *pxPal = (QRgb *)pal.bits();
//		qDebug() << pal.width()*pal.height() << pal.byteCount();
//		for(int i=0 ; i<pal.width()*pal.height() ; ++i) {
//			quint16 color;
//			memcpy(&color, &constPmp[4+i*2], 2);
//			pxPal[i] = FF8Image::fromPsColor(color);
//		}
//		pal.save(QString("pmp/%1-palette.png").arg(currentFieldName));

////		for(int palID=0 ; palID<16 ; ++palID) {
//			QImage img(64, height, QImage::Format_RGB32);
//			QRgb *px = (QRgb *)img.bits();

//			for(int j=0 ; j<img.width()*img.height() ; ++j) {
//				quint16 color;
//				memcpy(&color, &constPmp[516 + j*2], 2);

//				px[j] = FF8Image::fromPsColor(color);

////				++j;
////				memcpy(&color, &constPmp[4 + palID * 32 + ((quint8)pmp.at(516 + j/2) >> 4)*2], 2);

////				px[j] = FF8Image::fromPsColor(color);
//			}


//			img.save(QString("pmp/%1-non.png").arg(currentFieldName));
////		}
//	}

	return true;
}

bool PmpFile::save(QByteArray &pmp)
{
	pmp = this->pmp;

	return true;
}

const QByteArray &PmpFile::getPmpData() const
{
	return pmp;
}

void PmpFile::setPmpData(const QByteArray &pmp)
{
	this->pmp = pmp;
	modified = true;
}
