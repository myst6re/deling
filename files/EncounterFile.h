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
#ifndef ENCOUNTERFILE_H
#define ENCOUNTERFILE_H

#include <QtCore>

class EncounterFile
{
public:
	EncounterFile();
	bool open(const QByteArray &rat, const QByteArray &mrt);
	bool save(QByteArray &rat, QByteArray &mrt);
	bool isModified() const;
	quint16 formation(int index) const;
	quint8 rate(int index) const;
	void setFormation(int index, quint16 formation);
	void setRate(int index, quint8 rate);
	QList<int> searchAllBattles() const;
private:
	quint16 formations[4];
	quint8 rates[4];
	bool modified;
};

#endif // ENCOUNTERFILE_H
