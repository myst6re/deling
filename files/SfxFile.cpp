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
#include "files/SfxFile.h"

SfxFile::SfxFile()
	: File()
{
}

bool SfxFile::open(const QByteArray &sfx)
{
	int sfx_data_size = sfx.size();

	if(sfx_data_size % 4 != 0) {
		qWarning() << "invalid sfx size" << sfx_data_size;
		return false;
	}

	const quint32 *sfx_data = (const quint32 *)sfx.constData();

	int count = sfx_data_size/4;

	for(int i=0 ; i<count ; ++i) {
		_values.append(sfx_data[i]);
	}

	modified = false;

	return true;
}

bool SfxFile::save(QByteArray &sfx)
{
	foreach(const quint32 &v, _values) {
		sfx.append((char *)&v, 4);
	}

	modified = false;

	return true;
}

int SfxFile::valueCount() const
{
	return _values.size();
}

quint32 SfxFile::value(int id) const
{
	return _values.at(id);
}

void SfxFile::setValue(int id, quint32 v)
{
	_values[id] = v;
	modified = true;
}
