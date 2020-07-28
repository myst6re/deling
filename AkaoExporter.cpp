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
#include "AkaoExporter.h"
#include "FieldPC.h"
#include "FieldArchivePC.h"
#include "files/PsfFile.h"

AkaoExporter::AkaoExporter(FieldArchive *archive) :
    _archive(archive)
{

}

bool AkaoExporter::toDir(const QDir &dir, ArchiveObserver *observer)
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

		if (f && f->isOpen() && f->hasAkaoListFile()) {
			AkaoListFile *akaoList = f->getAkaoListFile();
			QString fieldName = f->name();

			if (fieldName.isEmpty()) {
				fieldName = QObject::tr("sans-nom");
			}

			for (int i = 0; i < akaoList->akaoCount(); ++i) {
				const QByteArray &akao = akaoList->akao(i);
				QString filePath = dir.filePath(QString::number(akaoList->akaoID(i)) + ".minipsf");
				PsfFile psf = PsfFile::fromAkao(akao, PsfTags("ff8.psflib"));
				QByteArray d1 = psf.save();

				if (QFile::exists(filePath)) {
					QFile f1(filePath);
					f1.open(QIODevice::ReadOnly);
					QByteArray d2 = f1.readAll();

					if (d1 == d2) {
						continue; // Already exported
					}

					filePath = dir.filePath(QString::number(akaoList->akaoID(i)) + "-" + fieldName + ".minipsf");
				}

				QFile f(filePath);
				if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
					f.write(d1);
					f.close();
				} else {
					_lastErrorString = QObject::tr("Impossible d'exporter '%1' en minipsf. Message : %2").arg(fieldName);
				}
			}
		}
	}

	return true;
}
