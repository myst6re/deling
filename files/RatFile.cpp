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
#include "files/RatFile.h"

RatFile::RatFile()
	: File(), _rate(0)
{
}

bool RatFile::open(const QByteArray &rat)
{
	if(rat.size() != 4) {
		qWarning() << "RatFile::open Bad sizes" << rat.size();
		return false;
	}

	_rate = rat.at(0);
	modified = false;

	return true;
}

bool RatFile::save(QByteArray &rat)
{
	rat.append((char)_rate);
	rat.append((char)_rate);
	rat.append((char)_rate);
	rat.append((char)_rate);

	return true;
}

quint8 RatFile::rate() const
{
	return _rate;
}

void RatFile::setRate(quint8 rate)
{
	_rate = rate;
	modified = true;
}
