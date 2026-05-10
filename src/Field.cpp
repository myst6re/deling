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
#include "Field.h"
#include "game/worldmap/Map.h"
#include "JsmPseudoCompiler.h"

Map *Field::worldmapFile = nullptr;

Field::Field(const QString &name)
	: _isOpen(false), _name(name), files(FILE_COUNT, nullptr)
{
	worldmapFile = nullptr;
}

Field::~Field()
{
	for (int i = 0; i < files.size(); ++i) {
		deleteFile(FileType(i));
	}
}

bool Field::isOpen() const
{
	return _isOpen;
}

void Field::setOpen(bool open)
{
	_isOpen = open;
}

bool Field::isPc() const
{
	return false;
}

bool Field::isPs() const
{
	return false;
}

void Field::openFile(FileType fileType, const QByteArray &data)
{
	File *f = _getFile(fileType);

	if (f == nullptr) {
		f = newFile(fileType);
	}

	if (f->isOpen()) {
		return;
	}

	if (!f->open(data)) {
		qWarning() << "Field::openFile error" << _name << fileType;
		f->setOpen(false);
	} else {
		f->setOpen(true);
	}
}

void Field::openJsmFile(const QByteArray &jsm, const QByteArray &sym, bool oldFormat)
{
	JsmFile *f = (JsmFile *)_getFile(Jsm);

	if (f == nullptr) {
		f = (JsmFile *)newFile(Jsm);
	}

	if (f->isOpen()) {
		return;
	}

	qDebug() << "Field::openJsmFile" << name();

	if (!f->open(jsm, sym, oldFormat)) {
		qWarning() << "Field::openJsmFile error" << _name;
		f->setOpen(false);
	} else {
		f->setOpen(true);

		/* QFile f0(QString("E:/Desktop/jsm-original/%1.jsm").arg(name()));
		f0.open(QIODevice::WriteOnly | QIODevice::Truncate);
		f0.write(jsm);
		f0.close();
		QFile f0sym(QString("E:/Desktop/jsm-original/%1.sym").arg(name()));
		f0sym.open(QIODevice::WriteOnly | QIODevice::Truncate);
		f0sym.write(sym);
		f0sym.close();
		QFile f0Pseudo(QString("E:/Desktop/jsm-original/%1.rb").arg(name()));
		f0Pseudo.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
		f0Pseudo.write(f->toString(true, true).toLatin1());
		f0Pseudo.close(); */

		QFile f1(QString("E:/Desktop/jsm/%1.jsm").arg(name()));
		f1.open(QIODevice::WriteOnly | QIODevice::Truncate);
		QByteArray jsm2, sym2;
		f->save(jsm2, sym2);
		f1.write(jsm2);
		f1.close();
		QFile f1sym(QString("E:/Desktop/jsm/%1.sym").arg(name()));
		f1sym.open(QIODevice::WriteOnly | QIODevice::Truncate);
		f1sym.write(sym2);
		f1sym.close();
		/* QFile f1Pseudo(QString("E:/Desktop/jsm/%1.rb").arg(name()));
		f1Pseudo.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
		f1Pseudo.write(f->toString(true, true).toLatin1());
		f1Pseudo.close(); */
	}
}

void Field::openBackgroundFile(const QByteArray &map, const QByteArray &mim)
{
	BackgroundFile *f = (BackgroundFile *)_getFile(Background);

	if (f == nullptr) {
		f = (BackgroundFile *)newFile(Background);
	}

	if (f->isOpen()) {
		return;
	}

	if (!f->open(map, mim/*, &params */)) {
		qWarning() << "Field::openBackgroundFile error" << _name;
		f->setOpen(false);
	} else {
		f->setOpen(true);
	}
}

void Field::openCharaFile(const QByteArray &one, const QByteArray &pcb)
{
	CharaOneFile *f = (CharaOneFile *)_getFile(CharaOne);

	if (f == nullptr) {
		f = (CharaOneFile *)newFile(CharaOne);
	}

	if (f->isOpen()) {
		return;
	}

	if (!f->open(one, pcb, isPs())) {
		qWarning() << "Field::openCharaFile error" << _name;
		f->setOpen(false);
	} else {
		f->setOpen(true);
	}
}

File *Field::newFile(FileType fileType)
{
	switch (fileType) {
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
	case CharaOne:
		return files[fileType] = new CharaOneFile();
	}
	return nullptr;
}

void Field::deleteFile(FileType fileType)
{
	File *f = _getFile(fileType);

	if (f != nullptr && worldmapFile == nullptr) {
		delete f;
		files[fileType] = nullptr;
	}
}

bool Field::hasFile(FileType fileType) const
{
	return _getFile(fileType) != nullptr;
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
	return hasFile(CharaOne);
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

bool Field::hasWorldmapFile() const
{
	return worldmapFile != nullptr;
}

bool Field::hasFiles() const
{
	for (int i = 0; i < files.size(); ++i) {
		if (hasFile(FileType(i))) {
			return true;
		}
	}

	return false;
}

File *Field::getFile(FileType fileType)
{
	return _getFile(fileType);
}

File *Field::_getFile(FileType fileType) const
{
	if (fileType == Msd && hasWorldmapFile()) {
		return worldmapFile->textsPtr();
	}

	return files.at(fileType);
}

MsdFile *Field::getMsdFile()
{
	return (MsdFile *)getFile(Msd);
}

JsmFile *Field::getJsmFile()
{
	return (JsmFile *)getFile(Jsm);
}

IdFile *Field::getIdFile()
{
	return (IdFile *)getFile(Id);
}

CaFile *Field::getCaFile()
{
	return (CaFile *)getFile(Ca);
}

RatFile *Field::getRatFile()
{
	return (RatFile *)getFile(Rat);
}

MrtFile *Field::getMrtFile()
{
	return (MrtFile *)getFile(Mrt);
}

InfFile *Field::getInfFile()
{
	return (InfFile *)getFile(Inf);
}

PmpFile *Field::getPmpFile()
{
	return (PmpFile *)getFile(Pmp);
}

PmdFile *Field::getPmdFile()
{
	return (PmdFile *)getFile(Pmd);
}

PvpFile *Field::getPvpFile()
{
	return (PvpFile *)getFile(Pvp);
}

BackgroundFile *Field::getBackgroundFile()
{
	return (BackgroundFile *)getFile(Background);
}

TdwFile *Field::getTdwFile()
{
	return (TdwFile *)getFile(Tdw);
}

CharaOneFile *Field::getCharaFile()
{
	return (CharaOneFile *)getFile(CharaOne);
}

MskFile *Field::getMskFile()
{
	return (MskFile *)getFile(Msk);
}

SfxFile *Field::getSfxFile()
{
	return (SfxFile *)getFile(Sfx);
}

AkaoListFile *Field::getAkaoListFile()
{
	return (AkaoListFile *)getFile(AkaoList);
}

Map *Field::getWorldmapFile() const
{
	return worldmapFile;
}

void Field::setWorldmapFile(Map *mapFile)
{
	worldmapFile = mapFile;
}

void Field::addMsdFile()
{
	setFile(Msd);
}

void Field::addJsmFile()
{
	setFile(Jsm);
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

void Field::addCharaFile()
{
	setFile(CharaOne);
}

void Field::setFile(FileType fileType)
{
	deleteFile(fileType);
	newFile(fileType);
}

bool Field::isModified() const
{
	for (int i = 0; i < files.size(); ++i) {
		File *f = _getFile(FileType(i));
		if (f && f->isModified()) {
			return true;
		}
	}
	return false;
}

void Field::setModified(bool modified)
{
	for (int i = 0; i < files.size(); ++i) {
		File *f = _getFile(FileType(i));
		if (f) {
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
