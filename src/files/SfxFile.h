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

class SfxFile : public File
{
public:
	SfxFile();
	bool open(const QByteArray &sfx);
	bool save(QByteArray &sfx);
	inline QString filterText() const {
		return QObject::tr("Field sounds PC file (*.sfx)");
	}
	int valueCount() const;
	quint32 value(int id) const;
	void setValue(int id, quint32 v);
	void insertValue(int id, quint32 v);
	void removeValue(int id);
private:
	QList<quint32> _values;
};
