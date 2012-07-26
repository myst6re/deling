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
#include "files/MiscFile.h"
#include "files/EncounterFile.h"
#include "files/BackgroundFile.h"
#include "files/TdwFile.h"
#include "files/CharaFile.h"
#include "files/MskFile.h"
#include "files/SfxFile.h"
#include "files/AkaoListFile.h"

class Field
{
public:
	Field(const QString &name);
	virtual ~Field();

	bool isOpen() const;
	void setOpen(bool open);

	virtual bool isPc() const=0;
	bool isPs() const;

	void openMsdFile(const QByteArray &msd);
	void openJsmFile(const QByteArray &jsm, const QByteArray &sym=QByteArray());
	void openIdFile(const QByteArray &id);
	void openCaFile(const QByteArray &ca);
	void openEncounterFile(const QByteArray &rat, const QByteArray &mrt);
    void openInfFile(const QByteArray &inf);
    void openMiscFile(const QByteArray &pmp, const QByteArray &pmd, const QByteArray &pvp);
	void openBackgroundFile(const QByteArray &map, const QByteArray &mim);
	void openTdwFile(const QByteArray &tdw);
	void openCharaFile(const QByteArray &one);
	void openMskFile(const QByteArray &msk);
	void openSfxFile(const QByteArray &sfx);
	void openAkaoListFile(const QByteArray &akao);

	bool hasMsdFile() const;
	bool hasJsmFile() const;
	bool hasIdFile() const;
	bool hasCaFile() const;
	bool hasEncounterFile() const;
    bool hasInfFile() const;
	bool hasMiscFile() const;
	bool hasBackgroundFile() const;
	bool hasTdwFile() const;
	bool hasCharaFile() const;
	bool hasMskFile() const;
	bool hasSfxFile() const;
	bool hasAkaoListFile() const;

	virtual bool hasMapMimFiles() const=0;
	bool hasFiles() const;

	MsdFile *getMsdFile() const;
	JsmFile *getJsmFile() const;
	IdFile *getIdFile() const;
	CaFile *getCaFile() const;
	EncounterFile *getEncounterFile() const;
    InfFile *getInfFile() const;
	MiscFile *getMiscFile() const;
	BackgroundFile *getBackgroundFile() const;
	TdwFile *getTdwFile() const;
	CharaFile *getCharaFile() const;
	MskFile *getMskFile() const;
	SfxFile *getSfxFile() const;
	AkaoListFile *getAkaoListFile() const;

	bool isModified() const;
	const QString &name() const;

protected:
	void deleteMsdFile();
	void deleteJsmFile();
	void deleteIdFile();
	void deleteCaFile();
	void deleteEncounterFile();
    void deleteInfFile();
	void deleteMiscFile();
	void deleteBackgroundFile();
	void deleteTdwFile();
	void deleteCharaFile();
	void deleteMskFile();
	void deleteSfxFile();
	void deleteAkaoListFile();

	bool _isOpen;
	QString _name;
	MsdFile *msdFile;
	JsmFile *jsmFile;
	IdFile *idFile;
	CaFile *caFile;
	EncounterFile *encounterFile;
    InfFile *infFile;
	MiscFile *miscFile;
	BackgroundFile *backgroundFile;
	TdwFile *tdwFile;
	static CharaFile *charaFile;
	MskFile *mskFile;
	SfxFile *sfxFile;
	AkaoListFile *akaoListFile;
};

#endif // FIELD_H
