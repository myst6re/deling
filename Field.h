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
#ifndef FIELD_H
#define FIELD_H

#include <QtCore>
#include "files/MsdFile.h"
#include "files/JsmFile.h"
#include "files/IdFile.h"
#include "files/CaFile.h"
#include "files/InfFile.h"
#include "files/PmpFile.h"
#include "files/PmdFile.h"
#include "files/PvpFile.h"
#include "files/RatFile.h"
#include "files/MrtFile.h"
#include "files/BackgroundFile.h"
#include "files/TdwFile.h"
#include "files/CharaFile.h"
#include "files/MskFile.h"
#include "files/SfxFile.h"
#include "files/AkaoListFile.h"

#define FILE_COUNT		15

class Field
{
public:
	enum FileType {
		Msd, Jsm, Id, Ca, Rat, Mrt, Inf, Pmp, Pmd, Pvp, Background, Tdw, Msk, Sfx, AkaoList
	};

	Field(const QString &name);
	virtual ~Field();

	bool isOpen() const;

	virtual bool isPc() const=0;
	bool isPs() const;

	bool isModified() const;
	void setModified(bool modified);
	const QString &name() const;

	bool hasFile(FileType fileType) const;
	bool hasMsdFile() const;
	bool hasJsmFile() const;
	bool hasIdFile() const;
	bool hasCaFile() const;
	bool hasRatFile() const;
	bool hasMrtFile() const;
    bool hasInfFile() const;
	bool hasPmpFile() const;
	bool hasPmdFile() const;
	bool hasPvpFile() const;
	bool hasBackgroundFile() const;
	bool hasTdwFile() const;
	bool hasCharaFile() const;
	bool hasMskFile() const;
	bool hasSfxFile() const;
	bool hasAkaoListFile() const;

	virtual bool hasFiles2() const=0;
	bool hasFiles() const;

	File *getFile(FileType fileType) const;
	MsdFile *getMsdFile() const;
	JsmFile *getJsmFile() const;
	IdFile *getIdFile() const;
	CaFile *getCaFile() const;
	RatFile *getRatFile() const;
	MrtFile *getMrtFile() const;
	InfFile *getInfFile() const;
	PmpFile *getPmpFile() const;
	PmdFile *getPmdFile() const;
	PvpFile *getPvpFile() const;
	BackgroundFile *getBackgroundFile() const;
	TdwFile *getTdwFile() const;
	CharaFile *getCharaFile() const;
	MskFile *getMskFile() const;
	SfxFile *getSfxFile() const;
	AkaoListFile *getAkaoListFile() const;

	void addMsdFile();
	void addRatFile();
	void addMrtFile();
	void addPvpFile();
	void addMskFile();
	void addSfxFile();
	void addAkaoListFile();
protected:
	void setOpen(bool open);
	void setName(const QString &name);

	void openFile(FileType fileType, const QByteArray &data);
	void openJsmFile(const QByteArray &jsm, const QByteArray &sym=QByteArray(), bool oldFormat = false);
	void openBackgroundFile(const QByteArray &map, const QByteArray &mim);
	void openCharaFile(const QByteArray &one);
	virtual void setFile(FileType fileType);
private:
	File *newFile(FileType fileType);
	void deleteFile(FileType fileType);
	void deleteCharaFile();

	bool _isOpen;
	QString _name;
	static CharaFile *charaFile;

	QList<File *> files;
};

#endif // FIELD_H
