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
		_errorString = QObject::tr("Not a worldmap archive.");
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
		_errorString = QObject::tr("Unable to read the worldmap (readSegments).");
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
		_errorString = QObject::tr("Unable to read the worldmap (readEncounterRegions).");
		return 3;
	}

	if (!wmset.readEncounters(map)) {
		_errorString = QObject::tr("Unable to read the worldmap (readEncounters).");
		return 4;
	}

	if (!wmset.readSpecialTextures(map)) {
		_errorString = QObject::tr("Unable to read the worldmap (readSpecialTextures).");
		return 5;
	}

	if (!wmset.readRoadTextures(map)) {
		_errorString = QObject::tr("Unable to read the worldmap (readRoadTextures).");
		return 6;
	}

	if (!wmset.readDrawPoints(map)) {
		_errorString = QObject::tr("Unable to read the worldmap (readDrawPoints).");
		return 7;
	}
	
	if (!wmset.readTexts(map)) {
		_errorString = QObject::tr("Unable to read the worldmap (readTexts).");
		return 8;
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
		_errorString = QObject::tr("Unable to read the worldmap (readTextures).");
		return 9;
	}

	if (progress) {
		progress->setObserverValue(3);
	}

	_isOpen = true;

	return 0;
}

bool WmArchive::save(FsArchive *fsArchive, const Map &map, QByteArray &outputWmsetData)
{
	WmsetFile wmset;

	QByteArray wmsetData = fsArchive->fileData("*world\\dat\\wmset??.obj");
	QBuffer wmsetBuffer(&wmsetData);
	wmsetBuffer.open(QIODevice::ReadOnly);
	wmset.setDevice(&wmsetBuffer);
	
	QBuffer wmsetOutputBuffer(&outputWmsetData);
	wmsetOutputBuffer.open(QIODevice::WriteOnly);
	
	QByteArray toc;
	wmsetOutputBuffer.write(wmset.tocData());
	if (wmsetOutputBuffer.pos() == 0) {
		return false;
	}

	for (int i = 0; i < OBJFILE_SECTION_COUNT; ++i) {
		quint32 pos = quint32(wmsetOutputBuffer.pos());
		toc.append((const char *)&pos, 4);

		if (i == 13 && map.texts().isModified()) {
			QByteArray data;
			if (!map.texts().save(data)) {
				return false;
			}
			wmsetOutputBuffer.write(data);
		} else {
			wmsetOutputBuffer.write(wmset.readSection(i));
		}
	}
	
	wmsetOutputBuffer.reset();
	wmsetOutputBuffer.write(toc);
	
	return true;
}
