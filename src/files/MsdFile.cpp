/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2024 Arzel Jérôme <myst6re@gmail.com>
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
#include "Data.h"

MsdFile::MsdFile(bool paddedFormat) :
	File(), _paddedFormat(paddedFormat)
{
}

bool MsdFile::open(const QByteArray &msd)
{
	const char *msd_data = msd.constData();
	int textPos, nextTextPos, nbText, dataSize=msd.size();
	texts.clear();
	needEndOfString.clear();

	modified = false;

	if (dataSize == 0) {
		return true;
	}

	memcpy(&textPos, msd_data, 4);

	if (textPos % 4 != 0) {
		lastError = QObject::tr("Invalid file format");
		return false;
	}

	nbText = textPos/4;
	needEndOfString.reserve(nbText);
	texts.reserve(nbText);
	for (int i = 1; i < nbText; ++i) {
		memcpy(&nextTextPos, &msd_data[i * 4], 4);
		if (_paddedFormat && nextTextPos == 0) {
			continue;
		}
		if (nextTextPos >= dataSize || nextTextPos < textPos) {
			lastError = QObject::tr("Invalid file format");
			return false;
		}
		bool needEOS = nextTextPos - textPos > 0 && msd_data[nextTextPos - 1] == '\x00';
		texts.append(QByteArray(&msd_data[textPos], (nextTextPos - textPos) - (needEOS ? 1 : 0)));
		needEndOfString.append(needEOS);
		textPos = nextTextPos;
	}
	bool needEOS = dataSize-textPos > 0 && msd_data[dataSize - 1] == '\x00';
	texts.append(QByteArray(&msd_data[textPos], (dataSize - textPos) - (needEOS ? 1 : 0)));
	needEndOfString.append(needEOS);

	return true;
}

bool MsdFile::save(QByteArray &msd)
{
	QByteArray msd_data;
	int headerSize = nbText()*4, pos, i=0;
	
	if (_paddedFormat) {
		headerSize += 4;
	}

	for (const QByteArray &text: texts) {
		pos = headerSize + msd_data.size();
		msd.append((char *)&pos, 4);
		if (_paddedFormat) {
			QByteArray timmedText = FF8Text(text).toFF8();
			msd_data.append(timmedText);
			if (timmedText.size() % 4 != 0) {
				// Padding
				msd_data.append(4 - timmedText.size() % 4, '\0');
			}
		} else {
			msd_data.append(text);
		}
		
		if (!_paddedFormat && (!text.isEmpty() || needEndOfString.at(i++))) {
			msd_data.append('\0');
		}
	}

	if (_paddedFormat) {
		msd.append(4, '\0');
		msd_data.append(4, '\0');
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
	if (id >= 0 && id < nbText()) {
		texts.replace(id, text.toFF8());
		modified = true;
	}
}

void MsdFile::setTexts(const QList<QByteArray> &texts)
{
	this->texts = texts;
	needEndOfString = QList<bool>(texts.size(), true);
	modified = true;
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
	if (id>=0 && id<nbText()) {
//		qDebug() << "MsdFile::removeText(int id)" << id;
		texts.removeAt(id);
		needEndOfString.removeAt(id);
		modified = true;
	}
}

bool MsdFile::searchText(const QRegularExpression &txt, int &textID, int &from, int &size) const
{
	if (textID < 0) {
		textID = 0;
	}

	for (; textID < nbText(); ++textID) {
		QRegularExpressionMatch match = txt.match(text(textID), from);
		if (match.hasMatch()) {
			from = int(match.capturedStart());
			size = int(match.capturedLength());
			return true;
		}
		from = 0;
	}

	return false;
}

bool MsdFile::searchTextReverse(const QRegularExpression &txt, int &textID, int &from, int &size) const
{
	if (textID >= nbText()) {
		textID = nbText() - 1;
		from = 2147483647;
	}

	for (; textID >= 0; --textID) {
		const FF8Text t = text(textID);
		QRegularExpressionMatch previousMatch;
		QRegularExpressionMatchIterator it = txt.globalMatch(t);
		while (it.hasNext()) {
			QRegularExpressionMatch match = it.next();
			
			if (match.capturedStart() > from) {
				break;
			}
			
			previousMatch = match;
		}

		if (previousMatch.hasMatch()) {
			from = int(previousMatch.capturedStart());
			size = int(previousMatch.capturedLength());
			return true;
		}

		from = 2147483647;
	}

	return false;
}

bool MsdFile::isJp() const
{
	for (const QByteArray &text: texts) {
		if (text.contains('\x19') || text.contains('\x1a') || text.contains('\x1b') || text.contains('\x1c')) return true;
	}
	return false;
}
