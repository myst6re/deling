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
	enum FileExt {
		Msd, Jsm, Id, Ca, Rat, Mrt, Inf, Pmp, Pmd, Pvp, Map, Tdw, Msk, Sfx, CharaOne, Mim, Sym
	};

	FieldPC(const QString &name, const QString &path, FsArchive *archive, const QString &gameLang);
	explicit FieldPC(const QString &path, const QString &gameLang);
	virtual ~FieldPC();

	bool isPc() const;
	bool hasFiles2() const;
//	void setArchiveHeader(FsArchive *header);
	FsArchive *getArchiveHeader() const;
	const QString &path() const;
	bool open(const QString &path);
	bool open(FsArchive *archive);
	bool open2(FsArchive *archive = nullptr);
	bool save(const QString &path);
	void save(QByteArray &fs_data, QByteArray &fl_data, QByteArray &fi_data);
	void optimize(QByteArray &fs_data, QByteArray &fl_data, QByteArray &fi_data);
	bool isMultiLanguage() const;
	QStringList languages() const;
protected:
	virtual void setFile(FileType fileType);
private:
	bool openOptimized(const QList<FileExt> &selectedExts);
	bool openOptimized(const QList<FileExt> &selectedExts, FsArchive *archive);
	QString fileName(FileExt fileExt, bool useGameLang) const;
	QString filePath(FileExt fileExt) const;
	QString filePath(FileExt fileType, bool useGameLang) const;
	QString filePath(const QString &fileName) const;
	inline static QList<FileExt> openExts() {
		return QList<FileExt>() << Inf << Msd << Jsm << Mrt << Sym;
	}
	inline static QList<FileExt> open2Exts() {
		return QList<FileExt>() << Msk << Rat << Pmp << Pmd << Pvp << Sfx
		                        << Id << Ca << Tdw << Map << Mim << CharaOne;
	}
	static FileType extToType(FileExt fileExt, bool *ok);
	static FileExt typeToExt(FileType fileType, bool *ok);
	QString _path;
	QString _lang, _subDir, _gameLang;
	FsArchive *header;
};

#endif // FIELDPC_H
