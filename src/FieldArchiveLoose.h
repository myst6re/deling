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
#include "FieldArchive.h"

class FieldLoose;

/**
 * Every field found in a folder and its subfolders, as if they came from field.fs: the files of
 * a field are the ones sharing its name in one folder (mapdata/bc/bcgate1a/bcgate1a.inf, .id...).
 *
 * Opening only looks for the files: each one is read when it is first used (see FieldLoose).
 * Saving writes each modified file back where it was found.
 */
class FieldArchiveLoose : public FieldArchive
{
public:
	FieldArchiveLoose();
	QString archivePath() const override;
	int open(const QString &path, ArchiveObserver *progress) override;
	bool openModels() override;
	bool openFull(Field *field) const override;
	bool save(ArchiveObserver *progress);
private:
	// A folder only counts as a field when it has one of the files that make a field a field
	static bool isField(const QStringList &paths);

	QString _path;
	QStringList _mainModelPaths; // field/model/main_chr/dNNN.mch, read when first needed
};
