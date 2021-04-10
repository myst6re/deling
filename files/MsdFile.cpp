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
#include "files/MsdFile.h"

MsdFile::MsdFile() :
	File()
{
}

bool MsdFile::open(const QByteArray &msd)
{
	const char *msd_data = msd.constData();
	int textPos, nextTextPos, nbText, dataSize=msd.size();
	texts.clear();
	needEndOfString.clear();

	modified = false;

	if(dataSize==0)	return true;

	memcpy(&textPos, msd_data, 4);

	if(textPos%4 != 0) {
		lastError = QObject::tr("Format de fichier invalide");
		return false;
	}

	nbText = textPos/4;
	for(int i=1 ; i<nbText ; ++i) {
		memcpy(&nextTextPos, &msd_data[i*4], 4);
		if(nextTextPos>=dataSize || nextTextPos<textPos) {
			lastError = QObject::tr("Format de fichier invalide");
			return false;
		}
		texts.append(QByteArray(&msd_data[textPos], nextTextPos-textPos));
		needEndOfString.append(nextTextPos-textPos > 0 && msd_data[nextTextPos-1] == '\x00');
		textPos = nextTextPos;
	}
	texts.append(QByteArray(&msd_data[textPos], dataSize-textPos));
	needEndOfString.append(dataSize-textPos > 0 && msd_data[dataSize-1] == '\x00');

	return true;
}

bool MsdFile::save(QByteArray &msd)
{
	QByteArray msd_data;
	int headerSize = nbText()*4, pos, i=0;

	foreach(const QByteArray &text, texts) {
		pos = headerSize + msd_data.size();
		msd.append((char *)&pos, 4);
		msd_data.append(text);
		if(!text.isEmpty() || needEndOfString.at(i++)) {
			msd_data.append('\x00');
		}
	}

	msd.append(msd_data);

	return true;
}

QByteArray MsdFile::data(int id) const
{
	return texts.value(id, QByteArray());
}

FF8Text MsdFile::text(int id) const
{
	return FF8Text(data(id));
}

void MsdFile::setText(int id, const FF8Text &text)
{
	if(id>=0 && id<nbText()) {
		texts.replace(id, text.toFF8());
		modified = true;
	}
}

void MsdFile::insertText(int id)
{
//	qDebug() << "MsdFile::insertText(int id)" << id;
	texts.insert(id, QByteArray());
	needEndOfString.insert(id, true);
	modified = true;
}

void MsdFile::removeText(int id)
{
	if(id>=0 && id<nbText()) {
//		qDebug() << "MsdFile::removeText(int id)" << id;
		texts.removeAt(id);
		needEndOfString.removeAt(id);
		modified = true;
	}
}

bool MsdFile::searchText(const QRegExp &txt, int &textID, int &from, int &size) const
{
	if (textID < 0) {
		textID = 0;
	}

	for (; textID < nbText(); ++textID) {
		from = txt.indexIn(text(textID), from);
		if (from != -1) {
			size = txt.matchedLength();
			return true;
		}
		from = 0;
	}

	return false;
}

bool MsdFile::searchTextReverse(const QRegExp &txt, int &textID, int &from, int &size) const
{
	if (textID >= nbText()) {
		textID = nbText() - 1;
		from = 2147483647;
	}

	for (; textID >= 0; --textID) {
		const FF8Text t = text(textID);
		int offset = from - t.size();
		if (offset >= 0) {
			offset = -1;
		}
		from = txt.lastIndexIn(t, offset);
		if (from != -1) {
			size = txt.matchedLength();
			return true;
		}
		from = 2147483647;
	}

	return false;
}

bool MsdFile::isJp() const
{
	foreach(const QByteArray &text, texts) {
		if(text.contains('\x19') || text.contains('\x1a') || text.contains('\x1b') || text.contains('\x1c')) return true;
	}
	return false;
}
