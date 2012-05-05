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
#ifndef MSDFILE_H
#define MSDFILE_H

#include <QtCore>
#include "FF8Text.h"
#include "Data.h"

class MsdFile
{
public:
	MsdFile();
	bool open(const QString &path);
	bool open(const QByteArray &msd);
	bool save(const QString &path);
	QByteArray save();
	static QString lastError;
	QByteArray data(int id);
	FF8Text text(int);
	void setText(int, const FF8Text &);
	void insertText(int);
	void removeText(int);
	int nbText();
	int searchText(const QString &text, int &textID, int from=0, Qt::CaseSensitivity cs=Qt::CaseInsensitive, bool regExp=false);
	int searchTextReverse(const QString &text, int &textID, int from=0, Qt::CaseSensitivity cs=Qt::CaseInsensitive, bool regExp=false);
	bool isModified();
	bool isJp();
private:
//	QString path;
	QList<QByteArray> texts;
	QList<bool> needEndOfString;
	bool modified;
};

#endif // MSDFILE_H
