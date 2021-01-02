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
#include "files/MskFile.h"

MskFile::MskFile()
	: File()
{
}

MskFile::~MskFile()
{
	foreach(Vertex_s *vertex, vertices) {
		delete[] vertex;
	}
}

bool MskFile::open(const QByteArray &msk)
{
	const char *mskData = msk.constData();
	quint32 count;

	foreach(Vertex_s *vertex, vertices) {
		delete[] vertex;
	}
	vertices.clear();

	modified = false;

	if(msk.isEmpty())	return true;

	if(msk.size() < 4) {
		qWarning() << "msk size too short" << msk.size();
		return false;
	}

	memcpy(&count, mskData, 4);

	if(quint32(msk.size()) != 4 + count * 24) {
		qWarning() << "msk invalid size" << count << msk.size();
		return false;
	}

	for(quint32 i=0 ; i<count ; ++i) {
		Vertex_s *vertex = new Vertex_s[4];

		memcpy(vertex, &mskData[4 + i*24], 24);

		vertices.append(vertex);
	}

	return true;
}

bool MskFile::save(QByteArray &msk)
{
	qint32 count = vertices.size();
	msk.append((char *)&count, 4);

	foreach(Vertex_s *vertex, vertices) {
		msk.append((char *)&vertex, 24);
	}

	return true;
}

int MskFile::cameraPositionCount() const
{
	return vertices.size();
}

Vertex_s *MskFile::cameraPosition(int frame) const
{
	return vertices.value(frame, nullptr);
}

void MskFile::setCameraPosition(int frame, Vertex_s camPos[4])
{
	if(frame < vertices.size()) {
		delete[] vertices.at(frame);
		vertices.replace(frame, camPos);
	}
}

void MskFile::insertCameraPosition(int frame, Vertex_s camPos[4])
{
	vertices.insert(frame, camPos);
}

void MskFile::removeCameraPosition(int frame)
{
	if(frame < vertices.size()) {
		delete[] vertices.takeAt(frame);
	}
}

