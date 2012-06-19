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
#ifndef TDWFILE_H
#define TDWFILE_H

#include <QtCore>
#include "TimFile.h"

class TdwFile
{
public:
	explicit TdwFile();
	virtual ~TdwFile();
	bool open(const QByteArray &tdw);
	void close();
	bool save(QByteArray &tdw);
	bool isModified() const;
	QPixmap image(int palID=0);
	static QPixmap image(const QByteArray &data, int palID=0);
	QImage letter(int charId, int fontColor, bool curFrame);
	void setLetter(int charId, const QImage &image);
	uint letterPixelIndex(int charId, const QPoint &pos) const;
	bool setLetterPixelIndex(int charId, const QPoint &pos, uint pixelIndex);
	const quint8 *charWidth(quint8 tableID=0) const;
	int tableCount() const;
	int charCount(quint8 tableID=0) const;
private:
	static QPoint letterPos(int charId);
	static QSize letterSize();
	static QRect letterRect(int charId);
	TimFile _tim;
	bool modified;
	QList<int> _charCount;
	QList<quint8 *> _charWidth;
};

#endif // TDWFILE_H
