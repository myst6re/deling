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
#include "MiscFile.h"

MiscFile::MiscFile()
	: modified(false)
{
}

bool MiscFile::open(const QByteArray &inf, const QByteArray &rat, const QByteArray &mrt, const QByteArray &pmp, const QByteArray &pmd, const QByteArray &pvp)
{
	quint16 mapId;
	const char *constInf = inf.constData();

	mapName = inf.left(8);

	gateways.clear();
	for(int i=0 ; i<12 ; ++i) {
		memcpy(&mapId, &constInf[118 + i*32], 2);
		gateways.append(mapId);
	}

	this->inf = inf;
	this->rat = rat;
	this->mrt = mrt;
	this->pmp = pmp;
	this->pmd = pmd;
	this->pvp = pvp;

	return true;
}

bool MiscFile::save(QByteArray &inf, QByteArray &rat, QByteArray &mrt, QByteArray &pmp, QByteArray &pmd, QByteArray &pvp)
{
	quint16 mapId;

	this->inf.replace(0, 8, mapName.toLatin1().leftJustified(8, '\x00', true));
	for(int i=0 ; i<12 ; ++i) {
		mapId = gateways.at(i);
		this->inf.replace(118 + i*32, 2, (char *)&mapId, 2);
	}

	inf = this->inf;
	rat = this->rat;
	mrt = this->mrt;
	pmp = this->pmp;
	pmd = this->pmd;
	pvp = this->pvp;
	modified = false;

	return true;
}

bool MiscFile::isModified()
{
	return modified;
}

const QString &MiscFile::getMapName()
{
	return mapName;
}

void MiscFile::setMapName(const QString &mapName)
{
	this->mapName = mapName;
//	qDebug() << "newMapName" << mapName;
	modified = true;
}

const QList<quint16> &MiscFile::getGateways()
{
	return gateways;
}

void MiscFile::setGateways(const QList<quint16> &gateways)
{
	this->gateways = gateways;
	modified = true;
}

const QByteArray &MiscFile::getRatData()
{
	return rat;
}

void MiscFile::setRatData(const QByteArray &rat)
{
	this->rat = rat;
	modified = true;
}

const QByteArray &MiscFile::getMrtData()
{
	return mrt;
}

void MiscFile::setMrtData(const QByteArray &mrt)
{
	this->mrt = mrt;
	modified = true;
}

const QByteArray &MiscFile::getPvpData()
{
	return pvp;
}

void MiscFile::setPvpData(const QByteArray &pvp)
{
	this->pvp = pvp;
	modified = true;
}

const QByteArray &MiscFile::getPmpData()
{
	return pmp;
}

void MiscFile::setPmpData(const QByteArray &pmp)
{
	this->pmp = pmp;
	modified = true;
}

const QByteArray &MiscFile::getPmdData()
{
	return pmd;
}

void MiscFile::setPmdData(const QByteArray &pmd)
{
	this->pmd = pmd;
	modified = true;
}

QList<int> MiscFile::searchAllBattles()
{
	QList<int> battles;
	int id;

	if(mrt.size() == 8) {
		if(mrt != QByteArray("\x00\x00\x00\x00\x00\x00\x00\x00", 8)) {
			const char *constData = mrt.constData();
			for(int i=0 ; i<4 ; ++i) {
				id = 0;
				memcpy(&id, constData, 2);
				battles.append(id);
				constData += 2;
			}
		}
	} else {
		qDebug() << mrt.size();
	}

	return battles;
}
