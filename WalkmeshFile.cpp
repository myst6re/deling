#include "WalkmeshFile.h"

WalkmeshFile::WalkmeshFile()
	: modified(false)
{
}

bool WalkmeshFile::isModified()
{
	return modified;
}

bool WalkmeshFile::open(const QByteArray &id, const QByteArray &ca)
{
	const char *id_data = id.constData(), *ca_data = ca.constData();
	int id_data_size = id.size(), ca_data_size = ca.size();
	quint32 i, nbSector, accessStart;

	if(id_data_size < 4)	return false;

	memcpy(&nbSector, id_data, 4);

	accessStart = 4+nbSector*24;

	if((quint32)id_data_size < accessStart+nbSector*6)	return false;

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

	if(!ca.isEmpty() && ca_data_size==38)
	{
		memcpy(&camera_axis, ca_data, 18);
		memcpy(&camera_unknown1, &ca_data[18], 2);
		memcpy(&camera_position, &ca_data[20], 12);
		memcpy(&camera_unknown2, &ca_data[32], 4);
		memcpy(&camera_zoom, &ca_data[36], 2);
		if(ca.size() != 38) {
			qWarning() << "Mauvaise taille open ca";
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

	ca.append((char *)&camera_axis, 18);
	ca.append((char *)&camera_unknown1, 2);
	ca.append((char *)&camera_position, 12);
	ca.append((char *)&camera_unknown2, 4);
	ca.append((char *)&camera_zoom, 2);

	if(ca.size() != 38) {
		qWarning() << "Mauvaise taille save ca";
	}

	return true;
}

QList<Triangle> WalkmeshFile::getTriangles()
{
	return triangles;
}

Vertex_s WalkmeshFile::camAxis(quint8 id)
{
	return camera_axis[id];
}

qint32 WalkmeshFile::camPos(quint8 id)
{
	return camera_position[id];
}

void WalkmeshFile::setCamAxis(quint8 id, const Vertex_s &cam_axis)
{
	camera_axis[id] = cam_axis;
	modified = true;
}

void WalkmeshFile::setCamPos(quint8 id, qint32 camPos)
{
	camera_position[id] = camPos;
	modified = true;
}
