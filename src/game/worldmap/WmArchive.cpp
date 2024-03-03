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
#include "WmArchive.h"
#include "game/worldmap/WmsetFile.h"
#include "game/worldmap/WmxFile.h"
#include "game/worldmap/TexlFile.h"

WmArchive::WmArchive() :
    _isOpen(false)
{
}

int WmArchive::open(FsArchive *fsArchive, Map &map, ArchiveObserver *progress)
{
	if (progress) {
		progress->setObserverMaximum(3);
	}
	
	if (!fsArchive->fileExists("*world\\dat\\wmx.obj")) {
		_errorString = QObject::tr("Pas une archive mappemonde.");
		return 1;
	}

	_isOpen = false;

	WmxFile wmx;
	WmsetFile wmset;
	TexlFile texl;
	
	QByteArray wmxData = fsArchive->fileData("*world\\dat\\wmx.obj");
	QBuffer wmxBuffer(&wmxData);
	wmxBuffer.open(QIODevice::ReadOnly);
	wmx.setDevice(&wmxBuffer);

	if(!wmx.readSegments(map, 768)) {
		_errorString = QObject::tr("Impossible de lire la mappemonde (1).");
		return 2;
	}

	wmxData.clear();

	if (progress) {
		progress->setObserverValue(1);
	}

	QByteArray wmsetData = fsArchive->fileData("*world\\dat\\wmset??.obj");
	QBuffer wmsetBuffer(&wmsetData);
	wmsetBuffer.open(QIODevice::ReadOnly);
	wmset.setDevice(&wmsetBuffer);

	if (!wmset.readEncounterRegions(map)) {
		_errorString = QObject::tr("Impossible de lire la mappemonde (2).");
		return 3;
	}

	if (!wmset.readEncounters(map)) {
		_errorString = QObject::tr("Impossible de lire la mappemonde (3).");
		return 4;
	}

	if (!wmset.readSpecialTextures(map)) {
		_errorString = QObject::tr("Impossible de lire la mappemonde (4).");
		return 5;
	}

	if (!wmset.readRoadTextures(map)) {
		_errorString = QObject::tr("Impossible de lire la mappemonde (4).");
		return 6;
	}

	if (!wmset.readDrawPoints(map)) {
		_errorString = QObject::tr("Impossible de lire la mappemonde (5).");
		return 7;
	}

	wmsetData.clear();

	if (progress) {
		progress->setObserverValue(2);
	}

	QByteArray texlData = fsArchive->fileData("*world\\dat\\texl.obj");
	QBuffer texlBuffer(&texlData);
	texlBuffer.open(QIODevice::ReadOnly);
	texl.setDevice(&texlBuffer);

	if (!texl.readTextures(map)) {
		_errorString = QObject::tr("Impossible de lire la mappemonde (6).");
		return 8;
	}

	if (progress) {
		progress->setObserverValue(3);
	}

	_isOpen = true;

	return 0;
}
