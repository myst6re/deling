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
	this->rat = rat;
	this->mrt = mrt;

	return true;
}

bool EncounterFile::save(QByteArray &rat, QByteArray &mrt)
{
	rat = this->rat;
	mrt = this->mrt;
	modified = false;

	return true;
}

bool EncounterFile::isModified()
{
	return modified;
}

const QByteArray &EncounterFile::getRatData()
{
	return rat;
}

void EncounterFile::setRatData(const QByteArray &rat)
{
	this->rat = rat;
	modified = true;
}

const QByteArray &EncounterFile::getMrtData()
{
	return mrt;
}

void EncounterFile::setMrtData(const QByteArray &mrt)
{
	this->mrt = mrt;
	modified = true;
}

QList<int> EncounterFile::searchAllBattles()
{
	QList<int> battles;
	int id;

	if(mrt.size() == 8) {
		if(mrt != QByteArray("\x00\x00\x00\x00\x00\x00\x00\x00", 8)) {
			const char *constData = mrt.constData();
			for(int i=0 ; i<4 ; ++i) {
				id = 0;
				memcpy(&id, constData, 2);
				battles.append(id);
				constData += 2;
			}
		}
	} else {
		qDebug() << mrt.size();
	}

	return battles;
}
