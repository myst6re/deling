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
#ifndef INFFILE_H
#define INFFILE_H

#include "files/File.h"
#include "files/CaFile.h"

typedef struct {
	qint16 top;
	qint16 bottom;
	qint16 right;
	qint16 left;
} Range;

typedef struct {
    Vertex_s exitLine[2];
    Vertex_s destinationPoint;
    quint16 fieldId;
	quint16 unknown1[4];
	quint32 unknown2;
} Gateway;

typedef struct {
	Vertex_s trigger_line[2];
	quint8 doorID;
	quint8 _blank[3];
} Trigger;

typedef struct {
	char name[9];
	quint8 control;
	quint8 unknown[10];
	Range cameraRange[8];
	Range screenRange[2];
    Gateway gateways[12];
	Trigger triggers[12];
} InfStruct;

class InfFile : public File
{
public:
    InfFile();
    bool open(const QByteArray &inf);
	bool save(QByteArray &inf);
	inline QString filterText() const {
		return QObject::tr("Fichier passages et portes écran PC (*.inf)");
	}
    QString getMapName() const;
	void setMapName(const QString &mapName);
	quint8 controlDirection() const;
	void setControlDirection(quint8 controlDirection);
	QByteArray unknown() const;
	void setUnknown(const QByteArray &unknown);
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

#endif // INFFILE_H
