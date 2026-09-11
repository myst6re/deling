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

#include "Field.h"

/**
 * A field made of loose files picked from disk, rather than read out of an archive.
 *
 * Files are added one at a time and may come from different directories, so a walkmesh
 * taken from one place can be looked at with the camera and background of another. Only
 * what was actually added is built: a lone .id gives a field whose Walkmesh page works and
 * whose other pages stay greyed out, because the pages drive themselves from
 * Field::hasFile(). Files that only mean something together are held until their partner
 * is added - map + mim make the background, jsm + sym the script, one + pcb the models -
 * so the order they are picked in does not matter.
 */
class FieldLoose : public Field
{
public:
	explicit FieldLoose(const QString &name);

	/**
	 * Read one file and remember where it came from. Adding a file whose type is already
	 * loaded replaces it. Returns false if the extension is unknown or the read failed,
	 * and fills errorString().
	 */
	bool addFile(const QString &path);
	/**
	 * Build (or rebuild) every File the added bytes are enough for. Call once after a
	 * batch of addFile().
	 */
	void buildFiles();
	/**
	 * Write every modified file back to the path it was added from, leaving untouched
	 * files alone. Returns false if any write failed.
	 */
	bool saveFiles();

	inline const QStringList &paths() const {
		return _paths;
	}
	inline const QString &errorString() const {
		return _errorString;
	}

	/**
	 * True when path has an extension Deling can open outside an archive, and sets ext to
	 * that extension, lowercased and without the dot.
	 */
	static bool looseExtension(const QString &path, QString &ext);
	/** Every extension looseExtension() accepts, for the file dialog filter. */
	static const QStringList &looseExtensions();

private:
	QMap<QString, QByteArray> _data;   // extension -> file content
	QMap<QString, QString> _filePaths; // extension -> where it came from
	QStringList _paths;                // in the order they were added
	QString _errorString;
};
