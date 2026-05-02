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
#include "files/SfxFile.h"

SfxFile::SfxFile()
	: File()
{
}

bool SfxFile::open(const QByteArray &sfx)
{
	int sfx_data_size = sfx.size();

	if (sfx_data_size % 4 != 0) {
		qWarning() << "invalid sfx size" << sfx_data_size;
		return false;
	}

	const quint32 *sfx_data = (const quint32 *)sfx.constData();

	int count = sfx_data_size / 4;

	for (int i = 0; i < count; ++i) {
		_values.append(sfx_data[i]);
	}

	modified = false;

	return true;
}

bool SfxFile::save(QByteArray &sfx) const
{
	for (const quint32 &v: _values) {
		sfx.append((char *)&v, 4);
	}

	return true;
}

quint32 SfxFile::sfxGameId(int id) const
{
	return toSfxGameId(_values.at(id));
}

quint32 SfxFile::toSfxGameId(quint32 value)
{
	int add = 0;

	switch (value / 10000) {
	case 1:
		add = 2150;
		break;
	case 21:
		add = 150;
		break;
	case 22:
		add = 180;
		break;
	case 24:
		add = 370;
		break;
	case 30:
		add = 710;
		break;
	case 35:
		add = 980;
		break;
	case 40:
		add = 1230;
		break;
	case 41:
		add = 1510;
		break;
	case 42:
		add = 1920;
		break;
	case 50:
		add = 2784;
		break;
	case 69:
		if (value != 690000) {
			add = 1960;
		} else {
			add = 2060;
		}
		break;
	}
	
	return value + add - 10000 * (value / 10000);
}

quint32 SfxFile::fromSfxGameId(quint32 sfxGameId)
{
	if (sfxGameId == 2060) {
		return 410000 + (sfxGameId - 2060);
	}
	if (sfxGameId >= 2784) {
		return 500000 + (sfxGameId - 2784);
	}
	if (sfxGameId >= 2150) {
		return  10000 + (sfxGameId - 2150);
	}
	if (sfxGameId >= 1960) {
		return 690000 + (sfxGameId - 1960);
	}
	if (sfxGameId >= 1920) {
		return 420000 + (sfxGameId - 1920);
	}
	if (sfxGameId >= 1510) {
		return 410000 + (sfxGameId - 1510);
	}
	if (sfxGameId >= 1230) {
		return 400000 + (sfxGameId - 1230);
	}
	if (sfxGameId >= 980) {
		return 350000 + (sfxGameId - 980);
	}
	if (sfxGameId >= 710) {
		return 300000 + (sfxGameId - 710);
	}
	if (sfxGameId >= 370) {
		return 240000 + (sfxGameId - 370);
	}
	if (sfxGameId >= 180) {
		return 220000 + (sfxGameId - 180);
	}
	if (sfxGameId >= 150) {
		return 210000 + (sfxGameId - 150);
	}

	return sfxGameId;
}

void SfxFile::setSfxGameId(int id, quint32 sfxGameId)
{
	_values[id] = fromSfxGameId(sfxGameId);
	modified = true;
}

void SfxFile::insertSfxGameId(int id, quint32 sfxGameId)
{
	_values.insert(id, fromSfxGameId(sfxGameId));
	modified = true;
}

void SfxFile::removeSfxGameId(int id)
{
	_values.removeAt(id);
	modified = true;
}
