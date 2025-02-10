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

class MrtFile : public File
{
public:
	MrtFile();
	bool open(const QByteArray &mrt);
	bool save(QByteArray &mrt);
	inline QString filterText() const {
		return QObject::tr("Field battle formation PC file (*.mrt)");
	}
	quint16 formation(int index) const;
	void setFormation(int index, quint16 formation);
	QList<int> searchAllBattles() const;
private:
	quint16 formations[4];
};
