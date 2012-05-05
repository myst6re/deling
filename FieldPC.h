/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2012 Arzel Jérôme <myst6re@gmail.com>
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
#ifndef FIELDPC_H
#define FIELDPC_H

#include "Field.h"
#include "FsArchive.h"

class FieldPC : public Field
{
public:
	FieldPC(const QString &name, const QString &path, FsArchive *archive);
	FieldPC(const QString &path);
	~FieldPC();

	bool hasMapMimFiles() const;
//	void setArchiveHeader(FsArchive *header);
	FsArchive *getArchiveHeader() const;
	const QString &path() const;
	bool open(const QString &path);
	bool open(FsArchive *archive);
	bool open2();
	bool open2(FsArchive *archive);
	void save(QByteArray &fs_data, QByteArray &fl_data, QByteArray &fi_data);
	void optimize(QByteArray &fs_data, QByteArray &fl_data, QByteArray &fi_data);

private:
	QString _path;
	FsArchive *header;
};

#endif // FIELDPC_H
