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
	: File()
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

	modified = false;

	return true;
}

bool InfFile::save(QByteArray &inf)
{
	inf.append((char *)&infStruct, sizeof(InfStruct));

	return true;
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

quint8 InfFile::controlDirection() const
{
	return infStruct.control;
}

void InfFile::setControlDirection(quint8 controlDirection)
{
	infStruct.control = controlDirection;
	modified = true;
}

QByteArray InfFile::unknown() const
{
	return QByteArray((char *)infStruct.unknown, 6);
}

void InfFile::setUnknown(const QByteArray &unknown)
{
	memcpy(infStruct.unknown, unknown.leftJustified(6, '\0', true).constData(), 6);
	modified = true;
}

quint16 InfFile::pvp() const
{
	return infStruct.pvp;
}

void InfFile::setPvp(quint16 pvp)
{
	infStruct.pvp = pvp;
	modified = true;
}

quint16 InfFile::cameraFocusHeight() const
{
	return infStruct.cameraFocusHeight;
}

void InfFile::setCameraFocusHeight(quint16 cameraFocusHeight)
{
	infStruct.cameraFocusHeight = cameraFocusHeight;
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

QList<Trigger> InfFile::getTriggers(bool filter) const
{
	QList<Trigger> triggers;

	for(int i=0 ; i<12 ; ++i) {
		if(!filter || infStruct.triggers[i].doorID != 0xff) {
			triggers.append(infStruct.triggers[i]);
		}
	}

	return triggers;
}

const Trigger &InfFile::getTrigger(int id) const
{
	return infStruct.triggers[id];
}

void InfFile::setTrigger(int id, const Trigger &trigger)
{
	infStruct.triggers[id] = trigger;

	modified = true;
}

const Range &InfFile::cameraRange(int id) const
{
	return infStruct.cameraRange[id];
}

void InfFile::setCameraRange(int id, const Range &range)
{
	infStruct.cameraRange[id] = range;
	modified = true;
}

const Range &InfFile::screenRange(int id) const
{
	return infStruct.screenRange[id];
}

void InfFile::setScreenRange(int id, const Range &range)
{
	infStruct.screenRange[id] = range;
	modified = true;
}
