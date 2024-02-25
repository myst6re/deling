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
#include "files/PvpFile.h"

PvpFile::PvpFile()
	: File(), _value(0x0C)
{
}

bool PvpFile::open(const QByteArray &pvp)
{
	if (pvp.size() != 4) {
		qWarning() << "error pvp size" << pvp.size();
		return false;
	}

	memcpy(&_value, pvp.constData(), 4);// values = 9, 10, 11, 12
	modified = false;

	return true;
}

bool PvpFile::save(QByteArray &pvp)
{
	pvp.append((char *)&_value, 4);

	return true;
}

quint32 PvpFile::value() const
{
	return _value;
}

void PvpFile::setValue(quint32 value)
{
	_value = value;
	modified = true;
}
