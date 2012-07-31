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
#include "TexFile.h"

TexFile::TexFile(const QByteArray &data) :
    TextureFile()
{
    open(data);
}

bool TexFile::open(const QByteArray &data)
{
    const char *constData = data.constData();
    int palID=0;
	quint32 w, h, headerSize;

	memcpy(&header, constData, sizeof(TexStruct));
	qDebug() << sizeof(TexStruct);
	debug();

	if(header.version == 1) {
		headerSize = sizeof(TexStruct) - 4;
	} else if(header.version == 2) {
		headerSize = sizeof(TexStruct);
	} else {
		qWarning() << "unknown tex version!";
		return false;
	}

	w = header.imageWidth;
	h = header.imageHeight;

	_image = QImage(w, h, QImage::Format_ARGB32);
    QRgb *pixels = (QRgb *)_image.bits();

	int size = w*h*header.bitsPerPixel, i=0;
    quint32 x=0, y=0;

	if(header.nbPalettes > 0)
	{
		quint32 index, imageStart = headerSize+header.nbColorsPerPalette1*4*header.nbPalettes,
				paletteStart = headerSize+header.nbColorsPerPalette1*4*palID;

        while(i<size && _image.valid(x, y))
        {
            index = paletteStart+((quint8)data.at(imageStart+i))*4;
			pixels[x + y*w] = qRgba(data.at(index+2), data.at(index+1), data.at(index), data.at(index+3));

            ++x;
			if(x==w)
            {
                x = 0;
                ++y;
            }
            ++i;
        }
    }
    else
    {
        quint16 color;

        while(i<size && _image.valid(x, y))
        {
			memcpy(&color, &constData[headerSize+i], 2);
			pixels[x + y*w] = FF8Image::fromPsColor(color);

            ++x;
			if(x==w)
            {
                x = 0;
                ++y;
            }
            i+=2;
        }
    }

    return true;
}

bool TexFile::save(QByteArray &data)
{
    // TODO: save TEX

    return false;
}

void TexFile::debug()
{
	TexStruct h = header;

	QFile f("debugTex.txt");
	f.open(QIODevice::WriteOnly);
	f.write(QString("version= %1 | unknown1= %2 | hasColorKey= %3 | unknown2= %4 | unknown3= %5\n")
			.arg(h.version).arg(h.unknown1).arg(h.hasColorKey).arg(h.unknown2).arg(h.unknown3).toLatin1());
	f.write(QString("minBitsPerColor= %1 | maxBitsPerColor= %2 | minAlphaBits= %3 | maxAlphaBits= %4 | minBitsPerPixel= %5\n")
			.arg(h.minBitsPerColor).arg(h.maxBitsPerColor).arg(h.minAlphaBits).arg(h.maxAlphaBits).arg(h.minBitsPerPixel).toLatin1());
	f.write(QString("maxBitsPerPixel= %1 | unknown4= %2 | nbPalettes= %3 | nbColorsPerPalette1= %4 | bitDepth= %5\n")
			.arg(h.maxBitsPerPixel).arg(h.unknown4).arg(h.nbPalettes).arg(h.nbColorsPerPalette1).arg(h.bitDepth).toLatin1());
	f.write(QString("imageWidth= %1 | imageHeight= %2 | pitch= %3 | unknown5= %4 | hasPal= %5\n")
			.arg(h.imageWidth).arg(h.imageHeight).arg(h.pitch).arg(h.unknown5).arg(h.hasPal).toLatin1());
	f.write(QString("bitsPerIndex= %1 | indexedTo8bit= %2 | paletteSize= %3 | nbColorsPerPalette2= %4 | runtimeData1= %5\n")
			.arg(h.bitsPerIndex).arg(h.indexedTo8bit).arg(h.paletteSize).arg(h.nbColorsPerPalette2).arg(h.runtimeData1).toLatin1());
	f.write(QString("bitsPerPixel= %1 | bytesPerPixel= %2 | nbRedBits1= %3 | nbGreenBits1= %4 | nbBlueBits1= %5\n")
			.arg(h.bitsPerPixel).arg(h.bytesPerPixel).arg(h.nbRedBits1).arg(h.nbGreenBits1).arg(h.nbBlueBits1).toLatin1());
	f.write(QString("nbAlphaBits1= %1 | redBitmask= %2 | greenBitmask= %3 | blueBitmask= %4 | alphaBitmask= %5\n")
			.arg(h.nbAlphaBits1).arg(h.redBitmask).arg(h.greenBitmask).arg(h.blueBitmask).arg(h.alphaBitmask).toLatin1());
	f.write(QString("redShift= %1 | greenShift= %2 | blueShift= %3 | alphaShift= %4 | nbRedBits2= %5\n")
			.arg(h.redShift).arg(h.greenShift).arg(h.blueShift).arg(h.alphaShift).arg(h.nbRedBits2).toLatin1());
	f.write(QString("nbGreenBits2= %1 | nbBlueBits2= %2 | nbAlphaBits2= %3 | redMax= %4 | greenMax= %5\n")
			.arg(h.nbGreenBits2).arg(h.nbBlueBits2).arg(h.nbAlphaBits2).arg(h.redMax).arg(h.greenMax).toLatin1());
	f.write(QString("blueMax= %1 | alphaMax= %2 | hasColorKeyArray= %3 | runtimeData2= %4 | referenceAlpha= %5\n")
			.arg(h.blueMax).arg(h.alphaMax).arg(h.hasColorKeyArray).arg(h.runtimeData2).arg(h.referenceAlpha).toLatin1());
	f.write(QString("runtimeData3= %1 | unknown6= %2 | paletteIndex= %3 | runtimeData4= %4 | runtimeData5= %5\n")
			.arg(h.runtimeData3).arg(h.unknown6).arg(h.paletteIndex).arg(h.runtimeData4).arg(h.runtimeData5).toLatin1());
	f.write(QString("unknown7= %1 | unknown8= %2 | unknown9= %3 | unknown10= %4 | unknown11= %5\n")
			.arg(h.unknown7).arg(h.unknown8).arg(h.unknown9).arg(h.unknown10).arg(h.unknown11).toLatin1());
	f.close();
}
