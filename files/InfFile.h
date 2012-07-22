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

#include <QtCore>
#include "WalkmeshFile.h"

typedef struct {
	quint8 unknown[8];
} UnknownStruct1;

typedef struct {
    Vertex_s exitLine[2];
    Vertex_s destinationPoint;
    quint16 fieldId;
    quint8 unknown[12];
} Gateway;

typedef struct {
    quint8 unknown[16];
} UnknownStruct2;

typedef struct {
	char name[9];
	quint8 control;
	quint8 unknown0[10];
	UnknownStruct1 unknown1[8];
	quint8 unknown2[16];
    Gateway gateways[12];
	UnknownStruct2 unknown3[12];
} InfStruct;

class InfFile
{
public:
    InfFile();
    bool open(const QByteArray &inf);
    bool save(QByteArray &inf);
	bool isModified() const;
    QString getMapName() const;
	void setMapName(const QString &mapName);
    QList<Gateway> getGateways() const;
	const Gateway &getGateway(int id) const;
	void setGateway(int id, const Gateway &gateway);
	const UnknownStruct2 &getUnknown(int id) const;
	void setUnknown(int id, const UnknownStruct2 &unknown);
private:
    InfStruct infStruct;
	bool modified;
};

#endif // INFFILE_H
