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
#pragma once

#include <QtCore>

#define OBJFILE_SECTION_COUNT 48

class ObjFile : public QFile
{
public:
	ObjFile(const QString &fileName);
	bool extract(quint32 id, const QString &fileName);
	bool extract(const QString &dirName);
	static bool build(const QString &dirName, const QString &fileName);
private:
	bool extract(quint32 offset, quint32 size, const QString &fileName);
	QList<quint32> openToc();
	bool sectionInfos(quint32 id, quint32 &offset, quint32 &size);
};
