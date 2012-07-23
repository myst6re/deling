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
#include "files/InfFile.h"

InfFile::InfFile()
	: modified(false)
{
}

bool InfFile::open(const QByteArray &inf)
{
    if(sizeof(InfStruct) != inf.size()) {
        qWarning() << "invalid inf size" << sizeof(InfStruct) << inf.size();
        return false;
    }

	const char *constInf = inf.constData();

    memcpy(&infStruct, constInf, sizeof(InfStruct));

	return true;
}

bool InfFile::save(QByteArray &inf)
{
    inf.append((char *)&infStruct, sizeof(InfStruct));

	modified = false;

	return true;
}

bool InfFile::isModified() const
{
	return modified;
}

QString InfFile::getMapName() const
{
    return QString(QByteArray(infStruct.name, 8));
}

void InfFile::setMapName(const QString &mapName)
{
    char *name = mapName.toLatin1().leftJustified(8, '\0', true).data();

    memcpy(&infStruct.name, name, 8);
	infStruct.name[8] = '\0';

	modified = true;
}

QList<Gateway> InfFile::getGateways() const
{
    QList<Gateway> gates;

    for(int i=0 ; i<12 ; ++i) {
        gates.append(infStruct.gateways[i]);
    }

    return gates;
}

const Gateway &InfFile::getGateway(int id) const
{
	return infStruct.gateways[id];
}

void InfFile::setGateway(int id, const Gateway &gateway)
{
	infStruct.gateways[id] = gateway;

	modified = true;
}

const Trigger &InfFile::getTrigger(int id) const
{
	return infStruct.triggers[id];
}

void InfFile::setTrigger(int id, const Trigger &trigger)
{
	infStruct.triggers[id] = trigger;
}
