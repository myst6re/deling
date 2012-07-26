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
#include "Field.h"

Field::Field(const QString &name)
	: _isOpen(false), _name(name),
	  msdFile(0), jsmFile(0), idFile(0), caFile(0),
	  encounterFile(0), infFile(0), miscFile(0), backgroundFile(0),
	  tdwFile(0), mskFile(0), sfxFile(0), akaoListFile(0)
{
}

Field::~Field()
{
	deleteMsdFile();
	deleteJsmFile();
	deleteIdFile();
	deleteCaFile();
	deleteEncounterFile();
    deleteInfFile();
	deleteMiscFile();
	deleteBackgroundFile();
	deleteTdwFile();
	deleteCharaFile();
	deleteMskFile();
	deleteSfxFile();
	deleteAkaoListFile();
}

bool Field::isOpen() const
{
	return _isOpen;
}

void Field::setOpen(bool open)
{
	_isOpen = open;
}

bool Field::isPs() const
{
	return !isPc();
}

void Field::openMsdFile(const QByteArray &msd)
{
	deleteMsdFile();
	msdFile = new MsdFile();

	if(!msdFile->open(msd)) {
		qWarning() << "Field::openMsdFile error" << _name;
		deleteMsdFile();
	}
}

void Field::openJsmFile(const QByteArray &jsm, const QByteArray &sym)
{
	deleteJsmFile();
	jsmFile = new JsmFile();

	if(!jsmFile->open(jsm, sym)) {
		qWarning() << "Field::openJsmFile error" << _name;
		deleteJsmFile();
	}
}

void Field::openIdFile(const QByteArray &id)
{
	deleteIdFile();
	idFile = new IdFile();

	if(!idFile->open(id)) {
		qWarning() << "Field::openIdFile error" << _name;
		deleteIdFile();
	}
}

void Field::openCaFile(const QByteArray &ca)
{
	deleteCaFile();
	caFile = new CaFile();

	if(!caFile->open(ca)) {
		qWarning() << "Field::openCaFile error" << _name;
		deleteCaFile();
	}
}

void Field::openEncounterFile(const QByteArray &rat, const QByteArray &mrt)
{
	deleteEncounterFile();
	encounterFile = new EncounterFile();

	if(!encounterFile->open(rat, mrt)) {
		qWarning() << "Field::openEncounterFile error" << _name;
		deleteEncounterFile();
	}
}

void Field::openInfFile(const QByteArray &inf)
{
    deleteInfFile();
    infFile = new InfFile();

    if(!infFile->open(inf)) {
        qWarning() << "Field::openInfFile error" << _name;
        deleteInfFile();
    }
}

void Field::openMiscFile(const QByteArray &pmp,
						 const QByteArray &pmd, const QByteArray &pvp)
{
	deleteMiscFile();
	miscFile = new MiscFile();

    if(!miscFile->open(pmp, pmd, pvp)) {
		qWarning() << "Field::openMiscFile error" << _name;
		deleteMiscFile();
	}
}

void Field::openBackgroundFile(const QByteArray &map, const QByteArray &mim)
{
	if(backgroundFile==0)
		backgroundFile = new BackgroundFile();

//	QMultiMap<quint8, quint8> params;
//	if(hasJsmFile()) {
//		getJsmFile()->searchDefaultBGStates(params);
//	}

	if(!backgroundFile->open(map, mim)) {
		qWarning() << "Field::openBackgroundFile error" << _name;
		deleteBackgroundFile();
	}
}

void Field::openTdwFile(const QByteArray &tdw)
{
	if(tdwFile != 0)
		return;
	tdwFile = new TdwFile();

	if(!tdwFile->open(tdw)) {
		qWarning() << "Field::openTdwFile error" << _name;
		deleteTdwFile();
	}
}

CharaFile *Field::charaFile = 0;

void Field::openCharaFile(const QByteArray &one)
{
	deleteCharaFile();
	charaFile = new CharaFile();

	if(!charaFile->open(one, isPs())) {
		qWarning() << "Field::openCharaFile error" << _name;
		deleteCharaFile();
	}
}

void Field::openMskFile(const QByteArray &msk)
{
	deleteMskFile();
	mskFile = new MskFile();

	if(!mskFile->open(msk)) {
		qWarning() << "Field::openMskFile error" << _name;
		deleteMskFile();
	}
}

void Field::openSfxFile(const QByteArray &sfx)
{
	deleteSfxFile();
	sfxFile = new SfxFile();

	if(!sfxFile->open(sfx)) {
		qWarning() << "Field::openSfxFile error" << _name;
		deleteSfxFile();
	}
}

void Field::openAkaoListFile(const QByteArray &akao)
{
	deleteAkaoListFile();
	akaoListFile = new AkaoListFile();

	if(!akaoListFile->open(akao)) {
		qWarning() << "Field::openAkaoListFile error" << _name;
		deleteAkaoListFile();
	}
}

void Field::deleteMsdFile()
{
	if(msdFile!=0) {
		delete msdFile;
		msdFile = 0;
	}
}

void Field::deleteJsmFile()
{
	if(jsmFile!=0) {
		delete jsmFile;
		jsmFile = 0;
	}
}

void Field::deleteIdFile()
{
	if(idFile!=0) {
		delete idFile;
		idFile = 0;
	}
}

void Field::deleteCaFile()
{
	if(caFile!=0) {
		delete caFile;
		caFile = 0;
	}
}

void Field::deleteEncounterFile()
{
	if(encounterFile!=0) {
		delete encounterFile;
		encounterFile = 0;
	}
}

void Field::deleteInfFile()
{
	if(infFile!=0) {
        delete infFile;
		infFile = 0;
    }
}

void Field::deleteMiscFile()
{
	if(miscFile!=0) {
		delete miscFile;
		miscFile = 0;
	}
}

void Field::deleteBackgroundFile()
{
	if(backgroundFile!=0) {
		delete backgroundFile;
		backgroundFile = 0;
	}
}

void Field::deleteTdwFile()
{
	if(tdwFile!=0) {
		delete tdwFile;
		tdwFile = 0;
	}
}

void Field::deleteCharaFile()
{
	if(charaFile!=0) {
		delete charaFile;
		charaFile = 0;
	}
}

void Field::deleteMskFile()
{
	if(mskFile!=0) {
		delete mskFile;
		mskFile = 0;
	}
}

void Field::deleteSfxFile()
{
	if(sfxFile!=0) {
		delete sfxFile;
		sfxFile = 0;
	}
}

void Field::deleteAkaoListFile()
{
	if(akaoListFile!=0) {
		delete akaoListFile;
		akaoListFile = 0;
	}
}

bool Field::hasMsdFile() const
{
	return msdFile != 0;
}

bool Field::hasJsmFile() const
{
	return jsmFile != 0;
}

bool Field::hasCaFile() const
{
	return caFile != 0;
}

bool Field::hasIdFile() const
{
	return idFile != 0;
}

bool Field::hasEncounterFile() const
{
	return encounterFile != 0;
}

bool Field::hasInfFile() const
{
	return infFile != 0;
}

bool Field::hasMiscFile() const
{
	return miscFile != 0;
}

bool Field::hasBackgroundFile() const
{
	return backgroundFile != 0;
}

bool Field::hasTdwFile() const
{
	return tdwFile != 0;
}

bool Field::hasCharaFile() const
{
	return charaFile != 0;
}

bool Field::hasMskFile() const
{
	return mskFile != 0;
}

bool Field::hasSfxFile() const
{
	return sfxFile != 0;
}

bool Field::hasAkaoListFile() const
{
	return akaoListFile != 0;
}

bool Field::hasFiles() const
{
	return hasMsdFile() || hasJsmFile() || hasMapMimFiles()
			|| hasCaFile() || hasIdFile() || hasEncounterFile()
			|| hasInfFile() || hasMiscFile() || hasTdwFile()
			|| hasCharaFile() || hasMskFile()
			|| hasSfxFile() || hasAkaoListFile();
}

MsdFile *Field::getMsdFile() const
{
	return msdFile;
}

JsmFile *Field::getJsmFile() const
{
	return jsmFile;
}

IdFile *Field::getIdFile() const
{
	return idFile;
}

CaFile *Field::getCaFile() const
{
	return caFile;
}

EncounterFile *Field::getEncounterFile() const
{
	return encounterFile;
}

InfFile *Field::getInfFile() const
{
    return infFile;
}

MiscFile *Field::getMiscFile() const
{
	return miscFile;
}

BackgroundFile *Field::getBackgroundFile() const
{
	return backgroundFile;
}

TdwFile *Field::getTdwFile() const
{
	return tdwFile;
}

CharaFile *Field::getCharaFile() const
{
	return charaFile;
}

MskFile *Field::getMskFile() const
{
	return mskFile;
}

SfxFile *Field::getSfxFile() const
{
	return sfxFile;
}

AkaoListFile *Field::getAkaoListFile() const
{
	return akaoListFile;
}

bool Field::isModified() const
{
	return (hasMsdFile() && msdFile->isModified())
			|| (hasJsmFile() && jsmFile->isModified())
			|| (hasEncounterFile() && encounterFile->isModified())
			|| (hasInfFile() && infFile->isModified())
			|| (hasMiscFile() && miscFile->isModified())
			|| (hasIdFile() && idFile->isModified())
			|| (hasCaFile() && caFile->isModified())
			|| (hasBackgroundFile() && backgroundFile->isModified())
			|| (hasTdwFile() && tdwFile->isModified())
			|| (hasCharaFile() && charaFile->isModified())
			|| (hasMskFile() && mskFile->isModified())
			|| (hasSfxFile() && sfxFile->isModified())
			|| (hasAkaoListFile() && akaoListFile->isModified());
}

const QString &Field::name() const
{
	return _name;
}
