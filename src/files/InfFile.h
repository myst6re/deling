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

struct Range {
	qint16 top;
	qint16 bottom;
	qint16 right;
	qint16 left;
};

struct Gateway {
	Vertex exitLine[2];
	Vertex destinationPoint;
	quint16 fieldId;
	quint16 unknown1[4];
	quint32 unknown2;
};

struct Trigger {
	Vertex trigger_line[2];
	quint8 doorID;
	quint8 _blank[3];
};

struct InfStruct {
	char name[9];
	quint8 control;
	quint8 unknown[6];
	quint16 pvp;
	qint16 cameraFocusHeight;
	Range cameraRange[8];
	Range screenRange[2];
	Gateway gateways[12];
	Trigger triggers[12];
};

class InfFile : public File
{
public:
	InfFile();
	bool open(const QByteArray &inf);
	bool save(QByteArray &inf) const;
	inline QString filterText() const {
		return QObject::tr("Field gate and doors PC File (*.inf)");
	}
    QString getMapName() const;
	void setMapName(const QString &mapName);
	quint8 controlDirection() const;
	void setControlDirection(quint8 controlDirection);
	QByteArray unknown() const;
	void setUnknown(const QByteArray &unknown);
	quint16 pvp() const;
	void setPvp(quint16 pvp);
	qint16 cameraFocusHeight() const;
	void setCameraFocusHeight(qint16 cameraFocusHeight);
    QList<Gateway> getGateways() const;
	const Gateway &getGateway(int id) const;
	void setGateway(int id, const Gateway &gateway);
	QList<Trigger> getTriggers(bool filter=true) const;
	const Trigger &getTrigger(int id) const;
	void setTrigger(int id, const Trigger &trigger);
	const Range &cameraRange(int id) const;
	void setCameraRange(int id, const Range &range);
	const Range &screenRange(int id) const;
	void setScreenRange(int id, const Range &range);
private:
	InfStruct infStruct;
};
