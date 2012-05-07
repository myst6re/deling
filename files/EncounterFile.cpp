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
#include "files/EncounterFile.h"

EncounterFile::EncounterFile()
	: modified(false)
{
}

bool EncounterFile::open(const QByteArray &rat, const QByteArray &mrt)
{
	if(rat.size() != 4 || mrt.size() != 8) {
		qWarning() << "EncounterFile::open Bad sizes" << rat.size() << mrt.size();
		return false;
	}

	memcpy(rates, rat.constData(), 4);
	memcpy(formations, mrt.constData(), 2*4);

	return true;
}

bool EncounterFile::save(QByteArray &rat, QByteArray &mrt)
{
	rat = QByteArray((char *)rates, 4);
	mrt = QByteArray((char *)formations, 2*4);
	modified = false;

	return true;
}

bool EncounterFile::isModified() const
{
	return modified;
}

quint16 EncounterFile::formation(int index) const
{
	return formations[index];
}

quint8 EncounterFile::rate(int index) const
{
	return rates[index];
}

void EncounterFile::setFormation(int index, quint16 formation)
{
	formations[index] = formation;
	modified = true;
}

void EncounterFile::setRate(int index, quint8 rate)
{
	rates[index] = rate;
	modified = true;
}

QList<int> EncounterFile::searchAllBattles() const
{
	QList<int> battles;

	for(int i=0 ; i<4 ; ++i) {
		if(formations[i] != 0)
			battles.append(formations[i]);
	}

	return battles;
}
