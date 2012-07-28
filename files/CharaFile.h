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
#ifndef CHARAFILE_H
#define CHARAFILE_H

#include "files/File.h"
#include "CharaModel.h"

class CharaFile : public File
{
public:
	CharaFile();
	virtual ~CharaFile();
	bool open(const QByteArray &one, bool ps);
	inline QString filterText() const {
		return QString();
	}
	const CharaModel &model(int id) const;
	void setModel(int id, const CharaModel &model);
	int modelCount() const;
private:
	QList<CharaModel> models;
};

#endif // CHARAFILE_H
