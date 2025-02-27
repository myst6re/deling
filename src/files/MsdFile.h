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
#pragma once

#include "files/File.h"
#include "FF8Text.h"

class MsdFile : public File
{
public:
	MsdFile(bool paddedFormat = false);
	bool open(const QByteArray &msd) override;
	bool save(QByteArray &msd) const override;
	bool saveForExport(QByteArray &msd) override;
	inline QString filterText() const override {
		return QObject::tr("Field dialogs PC file (*.msd)");
	}
	QByteArray data(int id) const;
	FF8Text text(int) const;
	void setText(int, const FF8Text &);
	void setTexts(const QList<QByteArray> &texts);
	inline const QList<QByteArray> &getTexts() const {
		return texts;
	}
	void insertText(int);
	void removeText(int);
	inline int nbText() const {
		return texts.size();
	}
	bool searchText(const QRegularExpression &txt, int &textID, int &from, int &size) const;
	bool searchTextReverse(const QRegularExpression &txt, int &textID, int &from, int &size) const;
	bool isJp() const;
private:
	QList<QByteArray> texts;
	QList<bool> needEndOfString;
	bool _paddedFormat;
};
