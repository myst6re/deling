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
#include "files/AkaoListFile.h"

AkaoListFile::AkaoListFile()
	: File()
{
}

bool AkaoListFile::open(const QByteArray &akao)
{
	if(akao.isEmpty())	return true;

	const char *akao_data = akao.constData();
	int akao_data_size = akao.size();

	if(akao_data_size < 4) {
		qWarning() << "akaoList file too short" << akao_data_size;
		return false;
	}

	quint32 position, count;

	memcpy(&position, akao_data, 4);

	count = position / 4;

	if((quint32)akao_data_size < position) {
		qWarning() << "akaoList file too short (1)" << akao_data_size << position;
		return false;
	}

	QList<quint32> positions;
	positions.append(position);

	for(quint32 i=1 ; i<count ; ++i) {
		memcpy(&position, &akao_data[i * 4], 4);
		if((quint32)akao_data_size < position) {
			qWarning() << "akaoList file too short (2)" << akao_data_size << position;
			return false;
		}
		if(position < positions.last()) {
			qWarning() << "akaoList invalid offset list" << akao_data_size << position << positions.last();
			return false;
		}
		positions.append(position);
	}

	positions.append(akao_data_size);

	for(quint32 i=0 ; i<count ; ++i) {
		const quint32 &pos = positions.at(i);
		_akaos.append(akao.mid(pos, positions.at(i+1)-pos));
	}

	modified = false;

	return true;
}

bool AkaoListFile::save(QByteArray &akao)
{
	quint32 pos = _akaos.size() * 4;

	for (const QByteArray &data: _akaos) {
		akao.append((char *)&pos, 4);
		pos += data.size();
	}

	for (const QByteArray &data: _akaos) {
		akao.append(data);
	}

	return true;
}

int AkaoListFile::akaoCount() const
{
	return _akaos.size();
}

const QByteArray &AkaoListFile::akao(int id) const
{
	return _akaos.at(id);
}

bool AkaoListFile::setAkao(int id, const QByteArray &akao)
{
	if(akao.startsWith("AKAO")) {
		_akaos[id] = akao;
		modified = true;
		return true;
	}
	return false;
}

bool AkaoListFile::insertAkao(int id, const QByteArray &akao)
{
	if(akao.startsWith("AKAO")) {
		_akaos.insert(id, akao);
		modified = true;
		return true;
	}
	return false;
}

void AkaoListFile::removeAkao(int id)
{
	_akaos.removeAt(id);
	modified = true;
}
