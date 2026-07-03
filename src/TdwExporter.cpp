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
#include "TdwExporter.h"
#include "FieldPC.h"
#include "FieldArchivePC.h"
#include "ArchiveObserver.h"

TdwExporter::TdwExporter(FieldArchive *archive) : _archive(archive)
{
}

bool TdwExporter::toDir(const QDir &dir, ArchiveObserver *observer)
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

		if (f && f->isOpen() && f->hasTdwFile()) {
			TdwFile *tdw = f->getTdwFile();
			char *format = (char *)"PNG";
			if (tdw->isOpen()) {
				QString fieldName = f->name();

				if (fieldName.isEmpty()) {
					fieldName = QObject::tr("Unamed");
				}
				QByteArray data;
				if (tdw) {
					QString path = dir.filePath(fieldName + ".png");
					QFile f(path);
					tdw->image(TdwFile::Color::White).save(path, format);
				} else {
					_lastErrorString =
					    QObject::tr("Unable to export '%1' to image")
					        .arg(fieldName);
				}
			}
		}
	}

	return true;
}
