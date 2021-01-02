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
#include "ScriptExporter.h"

ScriptExporter::ScriptExporter(FieldArchive *archive) :
    _archive(archive)
{

}

bool ScriptExporter::toDir(const QDir &dir, ArchiveObserver *observer)
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

		if (f && f->isOpen() && f->hasJsmFile()) {
			JsmFile *jsm = (JsmFile *)f->getFile(Field::Jsm);
			const JsmScripts &scripts = jsm->getScripts();
			QString fieldName = f->name();

			if (fieldName.isEmpty()) {
				fieldName = QObject::tr("sans-nom");
			}

			QString path = fieldName;
			dir.mkpath(path);

			for (int groupID = 0; groupID < scripts.nbGroup(); ++groupID) {
				QString groupName = scripts.group(groupID).name();
				if (groupName.isEmpty()) {
					groupName = QObject::tr("sans-nom");
				}

				QString script;

				for (int methodID = 0; methodID < scripts.nbScript(groupID); ++methodID) {
					QString scriptName = scripts.script(groupID, methodID).name();
					if (scriptName.isEmpty()) {
						scriptName = QObject::tr("sans-nom");
					}

					script.append(QString("%1() begin\n").arg(scriptName));
					script.append(jsm->toString(groupID, methodID, true, f, 1, true));
					script.append("\nend\n\n");
				}

				QFile file(dir.filePath(QString("%1/%2-%3.txt").arg(path).arg(groupID).arg(groupName)));
				if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
					file.write(script.toUtf8());
				} else {
					_lastErrorString = file.errorString();
					return false;
				}
			}
		}
	}

	return true;
}
