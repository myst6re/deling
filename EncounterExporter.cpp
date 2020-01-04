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
#include "EncounterExporter.h"
#include "FieldPC.h"
#include "FieldArchivePC.h"

EncounterExporter::EncounterExporter(FieldArchive *archive) :
    _archive(archive)
{

}

bool EncounterExporter::toDir(const QDir &dir, ArchiveObserver *observer)
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

		if (f && f->isOpen() && (!f->isPc() || ((FieldPC *)f)->open2(((FieldArchivePC *)_archive)->getFsArchive())) && f->hasMrtFile()) {
			MrtFile *mrt = f->getMrtFile();
			QString fieldName = f->name();

			if (fieldName.isEmpty()) {
				fieldName = QObject::tr("sans-nom");
			}

			QFile file(dir.filePath(fieldName + ".txt"));
			if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
				for (int i = 0; i < 4; ++i) {
					file.write(QString("Stage %1\n").arg(mrt->formation(i)).toUtf8());
				}
				if (f->hasRatFile()) {
					RatFile *rat = f->getRatFile();
					file.write(QString("Rate %1\n").arg(rat->rate()).toUtf8());
				} else {
					file.write(QString("Rate -\n").toUtf8());
				}
			} else {
				_lastErrorString = file.errorString();
				return false;
			}
		}
	}

	return true;
}
