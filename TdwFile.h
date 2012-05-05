#ifndef TDWFILE_H
#define TDWFILE_H

#include <QtCore>
#include "FF8Image.h"

class TdwFile
{
public:
	TdwFile();
	bool open(const QByteArray &tdw);
	bool isModified();
	QPixmap image(int palID=0) const;
	static QPixmap image(const QByteArray &data, int palID=0);
	QImage letter(int charId, int fontColor, bool curFrame) const;
	quint8 *charWidth();
private:
	static QByteArray tim;
	bool modified;
	quint8 _charWidth[224];
};

#endif // TDWFILE_H
