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
#pragma once

#include "files/File.h"
#include "Vertex.h"

struct Camera {
	Vertex camera_axis[3];
	qint16 camera_axis2z;// copy (padding)
	qint32 camera_position[3];
	qint32 blank;
	quint16 camera_zoom;
	quint16 camera_zoom2;// copy (padding)
};

class CaFile : public File
{
public:
	CaFile();
	bool open(const QByteArray &ca);
	bool save(QByteArray &ca) const;
	inline QString filterText() const {
		return QObject::tr("Field camera PC File (*.ca)");
	}
	bool hasCamera() const;
	int cameraCount() const;
	const Camera &camera(int camID) const;
	void setCamera(int camID, const Camera &cam);
	void insertCamera(int camID, const Camera &cam);
	void removeCamera(int camID);
private:
	QList<Camera> cameras;
};
