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
#include "files/MrtFile.h"

MrtFile::MrtFile()
	: File()
{
	memset(formations, 0, 2*4);
}

bool MrtFile::open(const QByteArray &mrt)
{
	if (mrt.size() != 8) {
		qWarning() << "MrtFile::open Bad sizes" << mrt.size();
		return false;
	}

	memcpy(formations, mrt.constData(), 2*4);

	modified = false;

	return true;
}

bool MrtFile::save(QByteArray &mrt)
{
	mrt = QByteArray((char *)formations, 2*4);

	return true;
}

quint16 MrtFile::formation(int index) const
{
	return formations[index];
}

void MrtFile::setFormation(int index, quint16 formation)
{
	formations[index] = formation;
	modified = true;
}

QList<int> MrtFile::searchAllBattles() const
{
	QList<int> battles;

	for (int i = 0; i < 4; ++i) {
		if (formations[i] != 0)
			battles.append(formations[i]);
	}

	return battles;
}
