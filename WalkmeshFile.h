#ifndef WALKMESHFILE_H
#define WALKMESHFILE_H

#include <QtCore>

typedef struct {
	qint16 x, y, z;
} Vertex_s;

typedef struct {
	qint16 x, y, z, res;
} Vertex_sr;

typedef struct {
	Vertex_sr vertices[3];
} Triangle;

typedef struct {
	qint16 a1, a2, a3;
} Access;

class WalkmeshFile
{
public:
	WalkmeshFile();
	bool open(const QByteArray &id, const QByteArray &ca=QByteArray());
	bool save(QByteArray &ca);
	QList<Triangle> getTriangles();
	Vertex_s camAxis(quint8);
	qint32 camPos(quint8);
	void setCamAxis(quint8 id, const Vertex_s &cam_axis);
	void setCamPos(quint8 id, qint32 camPos);
	bool isModified();
private:
	quint16 camera_unknown1;
	quint32 camera_unknown2;
	quint16 camera_zoom;
	Vertex_s camera_axis[3];
	qint32 camera_position[3];
	QList<Triangle> triangles;
	QList<Access> access;
	bool modified;
};

#endif // WALKMESHFILE_H
