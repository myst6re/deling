/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2020 Arzel Jérôme <myst6re@gmail.com>
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
#include "BackgroundExporter.h"
#include "FieldPC.h"
#include "FieldArchivePC.h"

BackgroundExporter::BackgroundExporter(FieldArchive *archive) :
    _archive(archive)
{

}

bool BackgroundExporter::toDir(const QDir &dir, ArchiveObserver *observer)
{
	if (!_archive) {
		return false;
	}

	FieldArchiveIterator it = _archive->iterator();

	if (observer) {
		observer->setObserverMaximum(quint32(_archive->nbFields()));
	}

	int i = 0;

	while (it.hasNext()) {
		Field *f = it.next();

		if (observer) {
			if (observer->observerWasCanceled()) {
				return false;
			}
			observer->setObserverValue(i++);
		}

		if (f && f->isOpen() && (!f->isPc() || ((FieldPC *)f)->open2(((FieldArchivePC *)_archive)->getFsArchive())) && f->hasBackgroundFile()) {
			BackgroundFile *background = f->getBackgroundFile();
			QString fieldName = f->name();

			if (fieldName.isEmpty()) {
				fieldName = QObject::tr("sans-nom");
			}

			if (!background->background().save(dir.filePath(fieldName + ".png"))) {
				_lastErrorString = QObject::tr("Impossible d'exporter '%1' en image").arg(fieldName);
			}
		}
	}

	return true;
}
