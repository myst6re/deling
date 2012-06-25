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
	enum Color {
		DarkGrey, Grey, Yellow, Red, Green, Blue, Purple, White
	};

	explicit TdwFile();
	virtual ~TdwFile();
	bool open(const QByteArray &tdw);
	void close();
	bool save(QByteArray &tdw);
	bool isModified() const;
	QImage image(Color color=White);
	static QImage image(const QByteArray &data, Color color=White);
	// tableId + charId
	QImage letter(quint8 tableId, int charId, Color color, bool curFrame);
	// Absolute charId
	QImage letter(int charId, Color color, bool curFrame);
	void setLetter(quint8 tableId, int charId, const QImage &image);
	void setLetter(int charId, const QImage &image);
	void setImage(const QImage &image, int hCount, int vCount);
	uint letterPixelIndex(quint8 tableId, int charId, const QPoint &pos) const;
	uint letterPixelIndex(int charId, const QPoint &pos) const;
	bool setLetterPixelIndex(quint8 tableId, int charId, const QPoint &pos, uint pixelIndex);
	bool setLetterPixelIndex(int charId, const QPoint &pos, uint pixelIndex);
	const quint8 *charWidth(quint8 tableId=0) const;
	void setCharWidth(quint8 tableId, int charId, quint8 width);
	int tableCount() const;
	int charCount(quint8 tableId=0) const;
private:
	bool isOptimizedVersion() const;
	int palette(Color color, int charId) const;
	QPoint letterPos(int charId) const;
	static QSize letterSize();
	QRect letterRect(int charId) const;
	TimFile _tim;
	bool modified;
	QList<int> _charCount;
	QList<quint8 *> _charWidth;
};

#endif // TDWFILE_H
