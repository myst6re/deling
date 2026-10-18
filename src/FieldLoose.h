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
 *
 * Only where each file is is remembered: like FieldPC with its archive, a file is read and
 * built the first time it is asked for (getFile), and again after it was unloaded or replaced.
 * So a folder of hundreds of fields (FieldArchiveLoose) costs little until a field is used.
 */
class FieldLoose : public Field
{
public:
	explicit FieldLoose(const QString &name);

	/**
	 * Remember a file of this field. Adding a file whose type is already there replaces it
	 * at the next buildFiles(). Returns false if the extension is unknown or the file cannot
	 * be read, and fills errorString().
	 */
	bool addFile(const QString &path);
	/**
	 * Build now every File that is not built yet, or whose file was replaced since, instead of
	 * when a page asks for it. Heavy files (background, models, font, particles) are left out
	 * when withHeavyFiles is false. A file already built, and maybe edited, is kept.
	 */
	void buildFiles(bool withHeavyFiles = true);
	/**
	 * Write every modified file back to the path it was added from, leaving untouched
	 * files alone. Returns false if any write failed.
	 */
	bool saveFiles();
	bool hasFile(FileType fileType) const override;
	File *getFile(FileType fileType) override;

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
	/**
	 * Open another field whose files sit near knownPaths, in the folders the game uses
	 * (mapdata/bc/bcgate1a/bcgate1a.id: a folder per field, grouped by its first two letters).
	 * Returns nullptr when no walkmesh of that field is found. The caller owns the result.
	 */
	static FieldLoose *openNeighbour(const QStringList &knownPaths, const QString &name);

private:
	QByteArray readFile(const QString &ext) const;
	bool canBuild(FileType fileType) const;
	bool mustBuild(FileType fileType) const;
	void buildFile(FileType fileType);

	QMap<QString, QString> _filePaths; // extension -> where it is
	QSet<QString> _replaced;           // extensions added since they were last built
	QStringList _paths;                // in the order they were added
	QString _errorString;
};
