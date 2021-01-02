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
	: _isOpen(false), _name(name)
{
	for(int i=0 ; i<FILE_COUNT ; ++i) {
		files.append(nullptr);
	}
}

Field::~Field()
{
	for(int i=0 ; i<files.size() ; ++i) {
		deleteFile(FileType(i));
	}

	deleteCharaFile();
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

void Field::openFile(FileType fileType, const QByteArray &data)
{
	PmpFile::currentFieldName = _name;
	deleteFile(fileType);
	File *f = newFile(fileType);

	if(!f->open(data)) {
		qWarning() << "Field::openFile error" << _name;
		deleteFile(fileType);
	}
}

void Field::openJsmFile(const QByteArray &jsm, const QByteArray &sym, bool oldFormat)
{
	deleteFile(Jsm);
	JsmFile *f = (JsmFile *)newFile(Jsm);

	if(!f->open(jsm, sym, oldFormat)) {
		qWarning() << "Field::openJsmFile error" << _name;
		deleteFile(Jsm);
	}
}

void Field::openBackgroundFile(const QByteArray &map, const QByteArray &mim)
{
	BackgroundFile *f;

	if(!hasBackgroundFile()) {
		f = (BackgroundFile *)newFile(Background);
	} else {
		f = getBackgroundFile();
	}

	/* QMultiMap<quint8, quint8> params;
	if(hasJsmFile()) {
		getJsmFile()->searchDefaultBGStates(params);
	} */

	if(!f->open(map, mim/*, &params */)) {
		qWarning() << "Field::openBackgroundFile error" << _name;
		deleteFile(Background);
	}
}

CharaFile *Field::charaFile = nullptr;

void Field::openCharaFile(const QByteArray &one)
{
	deleteCharaFile();
	charaFile = new CharaFile();

	if(!charaFile->open(one, isPs())) {
		qWarning() << "Field::openCharaFile error" << _name;
		deleteCharaFile();
	}
}

File *Field::newFile(FileType fileType)
{
	switch(fileType) {
	case Msd:
		return files[fileType] = new MsdFile();
	case Jsm:
		return files[fileType] = new JsmFile();
	case Id:
		return files[fileType] = new IdFile();
	case Ca:
		return files[fileType] = new CaFile();
	case Rat:
		return files[fileType] = new RatFile();
	case Mrt:
		return files[fileType] = new MrtFile();
	case Inf:
		return files[fileType] = new InfFile();
	case Pmp:
		return files[fileType] = new PmpFile();
	case Pmd:
		return files[fileType] = new PmdFile();
	case Pvp:
		return files[fileType] = new PvpFile();
	case Background:
		return files[fileType] = new BackgroundFile();
	case Tdw:
		return files[fileType] = new TdwFile();
	case Msk:
		return files[fileType] = new MskFile();
	case Sfx:
		return files[fileType] = new SfxFile();
	case AkaoList:
		return files[fileType] = new AkaoListFile();
	}
	return nullptr;
}

void Field::deleteFile(FileType fileType)
{
	File *f = getFile(fileType);

	if(f != nullptr) {
		delete f;
		files[fileType] = nullptr;
	}
}

void Field::deleteCharaFile()
{
	if(charaFile != nullptr) {
		delete charaFile;
		charaFile = nullptr;
	}
}

bool Field::hasFile(FileType fileType) const
{
	return getFile(fileType) != nullptr;
}

bool Field::hasMsdFile() const
{
	return hasFile(Msd);
}

bool Field::hasJsmFile() const
{
	return hasFile(Jsm);
}

bool Field::hasCaFile() const
{
	return hasFile(Ca);
}

bool Field::hasIdFile() const
{
	return hasFile(Id);
}

bool Field::hasRatFile() const
{
	return hasFile(Rat);
}

bool Field::hasMrtFile() const
{
	return hasFile(Mrt);
}

bool Field::hasInfFile() const
{
	return hasFile(Inf);
}

bool Field::hasPmpFile() const
{
	return hasFile(Pmp);
}

bool Field::hasPmdFile() const
{
	return hasFile(Pmd);
}

bool Field::hasPvpFile() const
{
	return hasFile(Pvp);
}

bool Field::hasBackgroundFile() const
{
	return hasFile(Background);
}

bool Field::hasTdwFile() const
{
	return hasFile(Tdw);
}

bool Field::hasCharaFile() const
{
	return charaFile != nullptr;
}

bool Field::hasMskFile() const
{
	return hasFile(Msk);
}

bool Field::hasSfxFile() const
{
	return hasFile(Sfx);
}

bool Field::hasAkaoListFile() const
{
	return hasFile(AkaoList);
}

bool Field::hasFiles() const
{
	for(int i=0 ; i<files.size() ; ++i) {
		if(hasFile(FileType(i))) {
			return true;
		}
	}

	return hasFiles2() || hasCharaFile();
}

File *Field::getFile(FileType fileType) const
{
	return files.at(fileType);
}

MsdFile *Field::getMsdFile() const
{
	return (MsdFile *)getFile(Msd);
}

JsmFile *Field::getJsmFile() const
{
	return (JsmFile *)getFile(Jsm);
}

IdFile *Field::getIdFile() const
{
	return (IdFile *)getFile(Id);
}

CaFile *Field::getCaFile() const
{
	return (CaFile *)getFile(Ca);
}

RatFile *Field::getRatFile() const
{
	return (RatFile *)getFile(Rat);
}

MrtFile *Field::getMrtFile() const
{
	return (MrtFile *)getFile(Mrt);
}

InfFile *Field::getInfFile() const
{
	return (InfFile *)getFile(Inf);
}

PmpFile *Field::getPmpFile() const
{
	return (PmpFile *)getFile(Pmp);
}

PmdFile *Field::getPmdFile() const
{
	return (PmdFile *)getFile(Pmd);
}

PvpFile *Field::getPvpFile() const
{
	return (PvpFile *)getFile(Pvp);
}

BackgroundFile *Field::getBackgroundFile() const
{
	return (BackgroundFile *)getFile(Background);
}

TdwFile *Field::getTdwFile() const
{
	return (TdwFile *)getFile(Tdw);
}

CharaFile *Field::getCharaFile() const
{
	return charaFile;
}

MskFile *Field::getMskFile() const
{
	return (MskFile *)getFile(Msk);
}

SfxFile *Field::getSfxFile() const
{
	return (SfxFile *)getFile(Sfx);
}

AkaoListFile *Field::getAkaoListFile() const
{
	return (AkaoListFile *)getFile(AkaoList);
}

void Field::addMsdFile()
{
	setFile(Msd);
}

void Field::addRatFile()
{
	setFile(Rat);
}

void Field::addMrtFile()
{
	setFile(Mrt);
}

void Field::addPvpFile()
{
	setFile(Pvp);
}

void Field::addMskFile()
{
	setFile(Msk);
}

void Field::addSfxFile()
{
	setFile(Sfx);
}

void Field::addAkaoListFile()
{
	setFile(AkaoList);
}

void Field::setFile(FileType fileType)
{
	deleteFile(fileType);
	newFile(fileType);
}

bool Field::isModified() const
{
	foreach(File *f, files) {
		if(f && f->isModified()) {
			return true;
		}
	}
	return false;
//	return hasCharaFile() && charaFile->isModified();
}

void Field::setModified(bool modified)
{
	foreach(File *f, files) {
		if(f) {
			f->setModified(modified);
		}
	}
}

const QString &Field::name() const
{
	return _name;
}

void Field::setName(const QString &name)
{
	_name = name;
}
