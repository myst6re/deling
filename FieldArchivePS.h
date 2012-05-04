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
#ifndef FSARCHIVEPS_H
#define FSARCHIVEPS_H

#include "FieldArchive.h"
#include "FieldPS.h"
#include "FF8DiscArchive.h"

class FieldArchivePS : public FieldArchive
{
public:
	FieldArchivePS();
	~FieldArchivePS();
	QString archivePath() const;
	FF8DiscArchive *getFF8DiscArchive() const;
	int open(const QString &, QProgressDialog *progress);
	bool openBG(Field *field, QByteArray &chara_data) const;
private:
	FF8DiscArchive *iso;
};

#endif // FSARCHIVEPS_H
