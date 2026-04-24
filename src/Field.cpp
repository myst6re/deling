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
	PmpFile::currentFieldName = _name;
	deleteFile(fileType);
	File *f = newFile(fileType);

	if (!f->open(data)) {
		qWarning() << "Field::openFile error" << _name;
		deleteFile(fileType);
	}
}

void Field::openJsmFile(const QByteArray &jsm, const QByteArray &sym, bool oldFormat)
{
	deleteFile(Jsm);
	JsmFile *f = (JsmFile *)newFile(Jsm);

	//qDebug() << "openJsmFile" << name();

	if (!f->open(jsm, sym, oldFormat)) {
		qWarning() << "Field::openJsmFile error" << _name;
		deleteFile(Jsm);
	} else {
		/* QFile f0(QString("E:/Desktop/jsm-original/%1.jsm").arg(name()));
		f0.open(QIODevice::WriteOnly | QIODevice::Truncate);
		f0.write(jsm);
		f0.close();
		QFile f0sym(QString("E:/Desktop/jsm-original/%1.sym").arg(name()));
		f0sym.open(QIODevice::WriteOnly | QIODevice::Truncate);
		f0sym.write(sym);
		f0sym.close();

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

		for (int groupID = 0; groupID < f->getScripts().groups().size(); ++groupID) {
			for (int methodID = 0; methodID < f->getScripts().group(groupID).scriptCount(); ++methodID) {
				JsmPseudoCompiler compiler;
				JsmData result;
				QString errorStr;
				int errorLine;
				if (!compiler.compile(f->toString(groupID, methodID, true, this), f->getScripts(), result, errorStr, errorLine)) {
					qWarning() << "compile error" << name() << groupID << methodID << errorStr;
					return;
				}
			}
		} */
	}
}

void Field::openBackgroundFile(const QByteArray &map, const QByteArray &mim)
{
	BackgroundFile *f;

	if (!hasBackgroundFile()) {
		f = (BackgroundFile *)newFile(Background);
	} else {
		f = getBackgroundFile();
	}

	/* QMultiMap<quint8, quint8> params;
	if (hasJsmFile()) {
		getJsmFile()->searchDefaultBGStates(params);
	} */

	if (!f->open(map, mim/*, &params */)) {
		qWarning() << "Field::openBackgroundFile error" << _name;
		deleteFile(Background);
	}
}

void Field::openCharaFile(const QByteArray &one, const QByteArray &pcb)
{
	qDebug() << "openCharaFile" << name();
	CharaOneFile *f;

	if (!hasCharaFile()) {
		f = (CharaOneFile *)newFile(CharaOne);
	} else {
		f = getCharaFile();
	}

	if (!f->open(one, pcb, isPs())) {
		qWarning() << "Field::openCharaFile error" << _name;
		deleteFile(CharaOne);
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
	File *f = getFile(fileType);

	if (f != nullptr && worldmapFile == nullptr) {
		delete f;
		files[fileType] = nullptr;
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

	return hasFiles2();
}

bool Field::hasFiles2() const
{
	return false;
}

File *Field::getFile(FileType fileType) const
{
	if (fileType == Msd && hasWorldmapFile()) {
		return worldmapFile->textsPtr();
	}

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

CharaOneFile *Field::getCharaFile() const
{
	return (CharaOneFile *)getFile(CharaOne);
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
	for (File *f: files) {
		if (f && f->isModified()) {
			return true;
		}
	}
	return hasMsdFile() && getMsdFile()->isModified();
}

void Field::setModified(bool modified)
{
	for (File *f: files) {
		if (f) {
			f->setModified(modified);
		}
	}

	if (hasMsdFile()) {
		getMsdFile()->setModified(modified);
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
