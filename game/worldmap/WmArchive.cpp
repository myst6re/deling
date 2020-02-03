#include "WmArchive.h"
#include "game/worldmap/WmsetFile.h"
#include "game/worldmap/WmxFile.h"
#include "game/worldmap/TexlFile.h"

WmArchive::WmArchive() :
    _isOpen(false)
{
}

bool WmArchive::open(const QString &filename, Map &map, ArchiveObserver *progress)
{
	if (progress) {
		progress->setObserverMaximum(3);
	}

	QString archivePath = filename;
	archivePath.chop(1);

	if (!_fsArchive.open(archivePath)) {
		_errorString = QObject::tr("Impossible d'ouvrir l'archive.");
		return false;
	}

	if (!_fsArchive.fileExists("*world\\dat\\wmx.obj")) {
		_errorString = QObject::tr("Pas une archive mappemonde.");
		return false;
	}

	_isOpen = false;

	WmxFile wmx;
	WmsetFile wmset;
	TexlFile texl;

	QByteArray wmxData = _fsArchive.fileData("*world\\dat\\wmx.obj");
	QBuffer wmxBuffer(&wmxData);
	wmxBuffer.open(QIODevice::ReadOnly);
	wmx.setDevice(&wmxBuffer);

	if(!wmx.readSegments(map, 768)) {
		_errorString = QObject::tr("Impossible de lire la mappemonde (1).");
		return false;
	}

	wmxData.clear();

	if (progress) {
		progress->setObserverValue(1);
	}

	QByteArray wmsetData = _fsArchive.fileData("*world\\dat\\wmsetfr.obj");
	QBuffer wmsetBuffer(&wmsetData);
	wmsetBuffer.open(QIODevice::ReadOnly);
	wmset.setDevice(&wmsetBuffer);

	if (!wmset.readEncounterRegions(map)) {
		_errorString = QObject::tr("Impossible de lire la mappemonde (2).");
		return false;
	}

	if (!wmset.readEncounters(map)) {
		_errorString = QObject::tr("Impossible de lire la mappemonde (3).");
		return false;
	}

	wmsetData.clear();

	if (progress) {
		progress->setObserverValue(2);
	}

	QByteArray texlData = _fsArchive.fileData("*world\\dat\\texl.obj");
	QBuffer texlBuffer(&texlData);
	texlBuffer.open(QIODevice::ReadOnly);
	texl.setDevice(&texlBuffer);

	if (!texl.readTextures(map)) {
		_errorString = QObject::tr("Impossible de lire la mappemonde (4).");
		return false;
	}

	if (progress) {
		progress->setObserverValue(3);
	}

	_isOpen = true;

	return true;
}
