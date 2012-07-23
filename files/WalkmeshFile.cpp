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
#include "files/WalkmeshFile.h"

WalkmeshFile::WalkmeshFile()
	: modified(false)
{
}

bool WalkmeshFile::isModified() const
{
	return modified;
}

bool WalkmeshFile::open(const QByteArray &id, const QByteArray &ca)
{
	const char *id_data = id.constData(), *ca_data = ca.constData();
	int id_data_size = id.size(), ca_data_size = ca.size();
	quint32 i, nbSector, accessStart;

	if(id_data_size < 4) {
		qWarning() << "size id error" << id_data_size;
		return false;
	}

	memcpy(&nbSector, id_data, 4);

	accessStart = 4+nbSector*24;

	if((quint32)id_data_size != accessStart+nbSector*6) {
		qWarning() << "size id error" << id_data_size << (accessStart+nbSector*6);
		return false;
	}

	Triangle triangle;
	Access acc;
	triangles.clear();
	access.clear();
	for(i=0 ; i<nbSector ; ++i) {
		memcpy(&triangle, &id_data[4+i*24], 24);
		triangles.append(triangle);
//		qDebug() << triangle.vertices[0].x << triangle.vertices[0].y << triangle.vertices[0].z << triangle.vertices[0].res;
//		qDebug() << triangle.vertices[1].x << triangle.vertices[1].y << triangle.vertices[1].z << triangle.vertices[1].res;
//		qDebug() << triangle.vertices[2].x << triangle.vertices[2].y << triangle.vertices[2].z << triangle.vertices[2].res;
//		qDebug() << "=====";
		memcpy(&acc, &id_data[accessStart+i*6], 6);
		access.append(acc);
//		qDebug() << acc.a1 << acc.a2 << acc.a3;
//		qDebug() << "=====";
	}

	cameras.clear();

	if(!ca.isEmpty())
	{
		if(ca_data_size != 38 && ca_data_size != 40 && ca_data_size != 80) {
			qWarning() << "size ca error" << ca_data_size << sizeof(CaStruct);
			return false;
		}

		CaStruct caStruct;

		int caCount = ca_data_size == 80 ? 2 : 1;

		for(i=0 ; i<caCount ; ++i) {
			memcpy(&caStruct, &ca_data[i*40], 38);
			cameras.append(caStruct);
		}

//		qDebug() << camera_axis[0].x << camera_axis[0].y << camera_axis[0].z
//				<< camera_axis[1].x << camera_axis[1].y << camera_axis[1].z
//				<< camera_axis[2].x << camera_axis[2].y << camera_axis[2].z;
//		qDebug() << camera_position[0] << camera_position[1] << camera_position[2];
	}

	modified = false;

	return true;
}

bool WalkmeshFile::save(QByteArray &ca)
{
	if(!modified)	return false;

	foreach(const CaStruct &caStruct, cameras) {
		ca.append((char *)&caStruct, 38);
		ca.append((char *)&caStruct.camera_zoom, 2);//Padding
	}

	return true;
}

const QList<Triangle> &WalkmeshFile::getTriangles() const
{
	return triangles;
}

int WalkmeshFile::cameraCount() const
{
	return cameras.size();
}

const CaStruct &WalkmeshFile::camera(int camID) const
{
	return cameras.at(camID);
}

void WalkmeshFile::setCamera(int camID, const CaStruct &cam)
{
	cameras[camID] = cam;
}
