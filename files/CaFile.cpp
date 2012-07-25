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
#include "files/CaFile.h"

CaFile::CaFile()
	: modified(false)
{
}

bool CaFile::isModified() const
{
	return modified;
}

bool CaFile::open(const QByteArray &ca)
{
	const char *ca_data = ca.constData();
	int ca_data_size = ca.size();
	quint32 i;

	cameras.clear();

	if(ca_data_size != 38 && ca_data_size % 40 != 0) {
		qWarning() << "size ca error" << ca_data_size << sizeof(CaStruct);
		return false;
	}

	if(sizeof(CaStruct) != 40) {
		qWarning() << "sizeof ca struct error" << sizeof(CaStruct);
		return false;
	}

	CaStruct caStruct;

	quint32 caCount = (ca_data_size / 40) + int(ca_data_size % 40 >= 38);

	for(i=0 ; i<caCount ; ++i) {
		memcpy(&caStruct, &ca_data[i*40], 38);

		cameras.append(caStruct);
	}

//	qDebug() << camera_axis[0].x << camera_axis[0].y << camera_axis[0].z
//			<< camera_axis[1].x << camera_axis[1].y << camera_axis[1].z
//			<< camera_axis[2].x << camera_axis[2].y << camera_axis[2].z;
//	qDebug() << camera_position[0] << camera_position[1] << camera_position[2];

	modified = false;

	return true;
}

bool CaFile::save(QByteArray &ca)
{
	if(!modified)	return false;

	foreach(CaStruct caStruct, cameras) {
		caStruct.camera_axis2z = caStruct.camera_axis[2].z;
		caStruct.camera_zoom2 = caStruct.camera_zoom;
		ca.append((char *)&caStruct, sizeof(CaStruct));
	}

	modified = false;

	return true;
}

int CaFile::cameraCount() const
{
	return cameras.size();
}

const CaStruct &CaFile::camera(int camID) const
{
	return cameras.at(camID);
}

void CaFile::setCamera(int camID, const CaStruct &cam)
{
	cameras[camID] = cam;
	modified = true;
}

void CaFile::insertCamera(int camID, const CaStruct &cam)
{
	cameras.insert(camID, cam);
	modified = true;
}

void CaFile::removeCamera(int camID)
{
	cameras.removeAt(camID);
	modified = true;
}
