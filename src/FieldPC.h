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
#include "Field.h"

class FsArchive;

class FieldPC : public Field
{
public:
	enum FileExt {
		ExtMsd, ExtJsm, ExtId, ExtCa, ExtRat, ExtMrt, ExtInf, ExtPcb, ExtPmp,
		ExtPmd, ExtPvp, ExtMap, ExtTdw, ExtMsk, ExtSfx, ExtCharaOne, ExtMim, ExtSym
	};

	FieldPC(const QString &name, const QString &path, FsArchive *archive, const QString &gameLang);
	explicit FieldPC(const QString &path, const QString &gameLang);
	virtual ~FieldPC();

	inline bool isPc() const {
		return true;
	}
	inline bool isPs() const {
		return false;
	}
	bool hasFile(FileType fileType) const override;
	File *getFile(FileType fileType) override;
//	void setArchiveHeader(FsArchive *header);
	inline FsArchive *getArchiveHeader() const {
		return header;
	}
	inline const QString &path() const {
		return _path;
	}
	bool open(const QString &path);
	bool open(FsArchive *archive);
	bool openFull();
	bool save(const QString &path);
	void save(QByteArray &fs_data, QByteArray &fl_data, QByteArray &fi_data);
	void optimize(QByteArray &fs_data, QByteArray &fl_data, QByteArray &fi_data);
	bool isMultiLanguage() const;
	QStringList languages() const;
	bool changeGameLang(const QString &gameLang);
protected:
	virtual void setFile(FileType fileType);
private:
	bool openOptimizedWithoutArchive(const QList<FileType> &selectedFileTypes);
	bool openOptimized(const QList<FileType> &selectedFileTypes);
	QString fileName(FileExt fileExt, bool useGameLang) const;
	QString filePath(FileExt fileExt) const;
	QString filePath(FileExt fileType, bool useGameLang) const;
	QString filePath(const QString &fileName) const;
	inline static QList<FileExt> openExts() {
		return QList<FileExt>() << ExtInf << ExtMsd << ExtJsm << ExtMrt << ExtSym << ExtMsk
		                        << ExtRat << ExtPcb << ExtPmp << ExtPmd << ExtPvp << ExtSfx
		                        << ExtId << ExtCa << ExtTdw << ExtMap << ExtMim << ExtCharaOne;
	}
	static FileType extToType(FileExt fileExt);
	static QList<FileExt> typeToExt(FileType fileType);
	QString _path, _lang, _subDir, _gameLang;
	FsArchive *header, *_archive;
	static QRegularExpression _pathReg, _pathRegWithoutArchive;
};
