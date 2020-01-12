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
#ifndef BACKGROUNDEXPORTER_H
#define BACKGROUNDEXPORTER_H

#include "FieldArchive.h"

class BackgroundExporter
{
public:
	explicit BackgroundExporter(FieldArchive *archive);
	bool toDir(const QDir &dir, ArchiveObserver *observer = nullptr);
	inline const QString &errorString() const {
		return _lastErrorString;
	}
private:
	FieldArchive *_archive;
	QString _lastErrorString;
};

#endif // BACKGROUNDEXPORTER_H
