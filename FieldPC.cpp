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
#include "FieldPC.h"

FieldPC::FieldPC(const QString &name, const QString &path, FsArchive *archive)
	: Field(name), _path(path), header(0)
{
	open(archive);
}

FieldPC::FieldPC(const QString &path)
	: Field(QString()), _path(QString()), header(0)
{
	open(path);
}

FieldPC::~FieldPC()
{
	if(header)		delete header;
}

bool FieldPC::isPc() const
{
	return true;
}

bool FieldPC::hasFiles2() const
{
	return header &&
			((header->fileExists(filePath(name() + ".mim")) && header->fileExists(filePath(name() + ".map")))
			|| header->fileExists(filePath(Tdw))
			|| header->fileExists(filePath("chara.one")));
}

//void FieldPC::setArchiveHeader(FsArchive *header)
//{
//	this->header = header;
//}

FsArchive *FieldPC::getArchiveHeader() const
{
	return header;
}

const QString &FieldPC::path() const
{
	return _path;
}

bool FieldPC::open(const QString &path)
{
	setOpen(false);

	QString archivePath = path;
	archivePath.chop(1);

	if(header)	delete header;
	header = new FsArchive(archivePath);
	if(!header->isOpen() || !header->fileExists("*.inf")) {
		qWarning() << "fieldData pas ouvert" << path;
		delete header;
		header = 0;
		return false;
	}

	QRegExp pathReg("^" + QRegExp::escape("C:\\ff8\\Data\\") + "(\\w+)" + QRegExp::escape("\\FIELD\\mapdata\\") + "(\\w+)" + QRegExp::escape("\\") + "(\\w+)" + QRegExp::escape("\\"), Qt::CaseInsensitive);
	FsHeader *inf_infos = header->getFile("*.inf");
	if(!inf_infos || pathReg.indexIn(inf_infos->path()) == -1) {
		qWarning() << "fieldData not opened wrong path" << inf_infos->path();
		return false;
	}
	_lang = pathReg.capturedTexts().at(1);
	_subDir = pathReg.capturedTexts().at(2);
	setName(pathReg.capturedTexts().at(3));

	/* INF */

	openFile(Inf, header->fileData(filePath(Inf)));

	/* MSD */

	if(header->fileExists(filePath(Msd)))
	{
		openFile(Msd, header->fileData(filePath(Msd)));
	}

	/* JSM & SYM */

	if(header->fileExists(filePath(Jsm)))
	{
		if(header->fileExists(filePath(Jsm)))
			openJsmFile(header->fileData(filePath(Jsm)), header->fileData(filePath(name() + ".sym")));
		else
			openJsmFile(header->fileData(filePath(Jsm)));
	}

	/* ID */

	if(header->fileExists(filePath(Id)))
	{
		openFile(Id, header->fileData(filePath(Id)));
	}

	/* CA */

	if(header->fileExists(filePath(Ca)))
	{
		openFile(Ca, header->fileData(filePath(Ca)));
	}

	/* MSK */

	if(header->fileExists(filePath(Msk)))
	{
		openFile(Msk, header->fileData(filePath(Msk)));
	}

	/* RAT */

	if(header->fileExists(filePath(Rat)))
	{
		openFile(Rat, header->fileData(filePath(Rat)));
	}

	/* MRT */

	if(header->fileExists(filePath(Mrt)))
	{
		openFile(Mrt, header->fileData(filePath(Mrt)));
	}

	/* SFX */

	if(header->fileExists(filePath(Sfx)))
	{
		openFile(Sfx, header->fileData(filePath(Sfx)));
	}

	/* PMP */

	if(header->fileExists(filePath(Pmp)))
	{
		openFile(Pmp, header->fileData(filePath(Pmp)));
	}

	/* PMD */

	if(header->fileExists(filePath(Pmd)))
	{
		openFile(Pmd, header->fileData(filePath(Pmd)));
	}

	/* PVP */

	if(header->fileExists(filePath(Pvp)))
	{
		openFile(Pvp, header->fileData(filePath(Pvp)));
	}

	setOpen(true);
	return true;
}

bool FieldPC::open(FsArchive *archive)
{
	setOpen(false);

	if(header)	delete header;

	QRegExp pathReg("^" + QRegExp::escape("C:\\ff8\\Data\\") + "(\\w+)" + QRegExp::escape("\\FIELD\\mapdata\\") + "(\\w+)" + QRegExp::escape("\\"), Qt::CaseInsensitive);
	FsHeader *fl_infos = archive->getFile("*"%name()%".fl");
	if(!fl_infos || pathReg.indexIn(fl_infos->path()) == -1) {
		qWarning() << "fieldData not opened" << name() << "wrong path" << fl_infos->path();
		return false;
	}
	_lang = pathReg.capturedTexts().at(1);
	_subDir = pathReg.capturedTexts().at(2);

	header = new FsArchive(archive->fileData("*"%name()%".fl"), archive->fileData("*"%name()%".fi"));
	if(!header->isOpen()) {
		qWarning() << "fieldData not opened" << name();
		delete header;
		header = 0;
		return false;
	}

	FsHeader *msd_infos, *jsm_infos, *sym_infos, *id_infos,
			*ca_infos, *msk_infos, *inf_infos, *rat_infos,
			*mrt_infos, *pmp_infos, *pmd_infos, *pvp_infos,
			*sfx_infos;

	msd_infos = header->getFile(filePath(Msd));
	jsm_infos = header->getFile(filePath(Jsm));
	sym_infos = header->getFile(filePath(name() + ".sym"));
	id_infos = header->getFile(filePath(Id));
	ca_infos = header->getFile(filePath(Ca));
	msk_infos = header->getFile(filePath(Msk));
	inf_infos = header->getFile(filePath(Inf));
	rat_infos = header->getFile(filePath(Rat));
	mrt_infos = header->getFile(filePath(Mrt));
	pmp_infos = header->getFile(filePath(Pmp));
	pmd_infos = header->getFile(filePath(Pmd));
	pvp_infos = header->getFile(filePath(Pvp));
	sfx_infos = header->getFile(filePath(Sfx));

	quint32 maxSize = 0;
	if(msd_infos!=NULL)
		maxSize = msd_infos->position()+msd_infos->uncompressed_size();
	if(jsm_infos!=NULL)
		maxSize = qMax(maxSize, jsm_infos->position()+jsm_infos->uncompressed_size());
	if(sym_infos!=NULL)
		maxSize = qMax(maxSize, sym_infos->position()+sym_infos->uncompressed_size());
	if(id_infos!=NULL)
		maxSize = qMax(maxSize, id_infos->position()+id_infos->uncompressed_size());
	if(ca_infos!=NULL)
		maxSize = qMax(maxSize, ca_infos->position()+ca_infos->uncompressed_size());
	if(msk_infos!=NULL)
		maxSize = qMax(maxSize, msk_infos->position()+msk_infos->uncompressed_size());
	if(inf_infos!=NULL)
		maxSize = qMax(maxSize, inf_infos->position()+inf_infos->uncompressed_size());
	if(rat_infos!=NULL)
		maxSize = qMax(maxSize, rat_infos->position()+rat_infos->uncompressed_size());
	if(mrt_infos!=NULL)
		maxSize = qMax(maxSize, mrt_infos->position()+mrt_infos->uncompressed_size());
	if(pmp_infos!=NULL)
		maxSize = qMax(maxSize, pmp_infos->position()+pmp_infos->uncompressed_size());
	if(pmd_infos!=NULL)
		maxSize = qMax(maxSize, pmd_infos->position()+pmd_infos->uncompressed_size());
	if(pvp_infos!=NULL)
		maxSize = qMax(maxSize, pvp_infos->position()+pvp_infos->uncompressed_size());
	if(sfx_infos!=NULL)
		maxSize = qMax(maxSize, sfx_infos->position()+sfx_infos->uncompressed_size());

	if(maxSize==0) {
		qWarning() << "No files !" << name();
		delete header;
		header = 0;
		return false;
	}

	QByteArray fs_data = archive->fileData("*"%name()%".fs", true, maxSize);

	if(fs_data.isEmpty()) {
		qWarning() << "No data !" << name() << maxSize;
		delete header;
		header = 0;
		return false;
	}

	/* MSD */

	if(msd_infos!=NULL)
	{
		openFile(Msd, msd_infos->data(fs_data));
	}

	/* JSM & SYM */

	if(jsm_infos!=NULL)
	{
		if(sym_infos!=NULL)
			openJsmFile(jsm_infos->data(fs_data), sym_infos->data(fs_data));
		else
			openJsmFile(jsm_infos->data(fs_data));
	}

	/* ID */

	if(id_infos!=NULL)
	{
		openFile(Id, id_infos->data(fs_data));
	}

	/* CA */

	if(ca_infos!=NULL)
	{
		openFile(Ca, ca_infos->data(fs_data));
	}

	/* MSK */

	if(msk_infos!=NULL)
	{
		openFile(Msk, msk_infos->data(fs_data));
	}

	/* RAT */

	if(rat_infos!=NULL)
	{
		openFile(Rat, rat_infos!=NULL ? rat_infos->data(fs_data) : QByteArray());
	}

	/* MRT */

	if(mrt_infos!=NULL)
	{
		openFile(Mrt, mrt_infos!=NULL ? mrt_infos->data(fs_data) : QByteArray());
	}

	/* INF */

	if(inf_infos!=NULL)
	{
		openFile(Inf, inf_infos!=NULL ? inf_infos->data(fs_data) : QByteArray());
	}

	/* SFX */

	if(sfx_infos!=NULL)
	{
		openFile(Sfx, sfx_infos!=NULL ? sfx_infos->data(fs_data) : QByteArray());
	}

	/* PMP */

	if(pmp_infos!=NULL)
	{
		openFile(Pmp, pmp_infos!=NULL ? pmp_infos->data(fs_data) : QByteArray());
	}

	/* PMD */

	if(pmd_infos!=NULL)
	{
		openFile(Pmd, pmd_infos!=NULL ? pmd_infos->data(fs_data) : QByteArray());
	}

	/* PVP */

	if(pvp_infos!=NULL)
	{
		openFile(Pvp, pvp_infos!=NULL ? pvp_infos->data(fs_data) : QByteArray());
	}

	setOpen(true);
	return true;
}

bool FieldPC::open2()
{
	if(!header)
		return false;

	if(header->fileExists(filePath(name() + ".map")) && header->fileExists(filePath(name() + ".mim"))) {
		openBackgroundFile(header->fileData(filePath(name() + ".map")), header->fileData(filePath(name() + ".mim")));
	}
	if(!hasTdwFile()) {
		openFile(Tdw, header->fileData(filePath(Tdw)));
	}
	openCharaFile(header->fileData(filePath("chara.one")));

	return true;
}

bool FieldPC::open2(FsArchive *archive)
{
	if(!header)
		return false;

	FsHeader *fi_infos_mim = header->getFile(filePath(name() + ".mim"));
	FsHeader *fi_infos_map = header->getFile(filePath(name() + ".map"));
	FsHeader *fi_infos_tdw = !hasTdwFile() ? header->getFile(filePath(Tdw)) : NULL;
	FsHeader *fi_infos_one = header->getFile(filePath("chara.one"));

	if(fi_infos_mim==NULL || fi_infos_map==NULL)
		return false;

	quint32 maxPos = qMax(fi_infos_mim->position()+fi_infos_mim->uncompressed_size(),
						  fi_infos_map->position()+fi_infos_map->uncompressed_size());
	if(fi_infos_tdw!=NULL) {
		maxPos = qMax(maxPos, fi_infos_tdw->position()+fi_infos_tdw->uncompressed_size());
	}
	if(fi_infos_one!=NULL) {
		maxPos = qMax(maxPos, fi_infos_one->position()+fi_infos_one->uncompressed_size());
	}

	QByteArray fs_data = archive->fileData(path(), true, maxPos);

	if(fs_data.isEmpty())	 return false;

	if(fi_infos_mim!=NULL && fi_infos_map!=NULL) {
		openBackgroundFile(fi_infos_map->data(fs_data), fi_infos_mim->data(fs_data));
	}
	if(fi_infos_tdw!=NULL && !hasTdwFile()) {
		openFile(Tdw, fi_infos_tdw->data(fs_data));
	}
	if(fi_infos_one!=NULL) {
		openCharaFile(fi_infos_one->data(fs_data));
	}

	return true;
}

bool FieldPC::save(const QString &path)
{
	QString archivePath = path;
	archivePath.chop(1);

	QFile fs(FsArchive::fsPath(archivePath));
	if(fs.open(QIODevice::ReadWrite)) {
		QFile fl(FsArchive::flPath(archivePath));
		if(fl.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
			QFile fi(FsArchive::fiPath(archivePath));
			if(fi.open(QIODevice::WriteOnly | QIODevice::Truncate)) {

				QByteArray fs_data=fs.readAll(), fl_data, fi_data;

				/* fs_data must be filled by the file contents
				 * fl_data and fi_data are rebuilt
				 */
				save(fs_data, fl_data, fi_data);

				fs.resize(0);
				fs.reset();
				fs.write(fs_data);
				fl.write(fl_data);
				fi.write(fi_data);

				fi.close();
			} else {
				return false;
			}
			fl.close();
		} else {
			return false;
		}
		fs.close();
	} else {
		return false;
	}

	return true;
}

void FieldPC::save(QByteArray &fs_data, QByteArray &fl_data, QByteArray &fi_data)
{
	if(!header)	return;

	if(hasMsdFile() && getMsdFile()->isModified()) {
		QByteArray msd;
		if(getMsdFile()->save(msd)) {
			header->setFileData(filePath(Msd), fs_data, msd);
		}
	}
	if(hasJsmFile() && getJsmFile()->isModified()) {
		QByteArray jsm, sym;
		if(getJsmFile()->save(jsm, sym)) {
			header->setFileData(filePath(Jsm), fs_data, jsm);
			if(!sym.isEmpty()) {
				header->setFileData(filePath(name() + ".sym"), fs_data, sym);
			}
		}
	}
	if(hasRatFile() && getRatFile()->isModified()) {
		QByteArray rat;
		if(getRatFile()->save(rat)) {
			header->setFileData(filePath(Rat), fs_data, rat);
		}
	}
	if(hasMrtFile() && getMrtFile()->isModified()) {
		QByteArray mrt;
		if(getMrtFile()->save(mrt)) {
			header->setFileData(filePath(Mrt), fs_data, mrt);
		}
	}
	if(hasInfFile() && getInfFile()->isModified()) {
		QByteArray inf;
		if(getInfFile()->save(inf)) {
			header->setFileData(filePath(Inf), fs_data, inf);
		}
	}
	if(hasPmpFile() && getPmpFile()->isModified()) {
		QByteArray pmp;
		if(getPmpFile()->save(pmp)) {
			header->setFileData(filePath(Pmp), fs_data, pmp);
		}
	}
	if(hasPmdFile() && getPmdFile()->isModified()) {
		QByteArray pmd;
		if(getPmdFile()->save(pmd)) {
			header->setFileData(filePath(Pmd), fs_data, pmd);
		}
	}
	if(hasPvpFile() && getPvpFile()->isModified()) {
		QByteArray pvp;
		if(getPvpFile()->save(pvp)) {
			header->setFileData(filePath(Pvp), fs_data, pvp);
		}
	}
	if(hasIdFile() && getIdFile()->isModified()) {
		QByteArray id;
		if(getIdFile()->save(id)) {
			header->setFileData(filePath(Id), fs_data, id);
		}
	}
	if(hasCaFile() && getCaFile()->isModified()) {
		QByteArray ca;
		if(getCaFile()->save(ca)) {
			header->setFileData(filePath(Ca), fs_data, ca);
		}
	}
	if(hasMskFile() && getMskFile()->isModified()) {
		QByteArray msk;
		if(getMskFile()->save(msk)) {
			header->setFileData(filePath(Msk), fs_data, msk);
		}
	}
	if(hasTdwFile() && getTdwFile()->isModified()) {
		QByteArray tdw;
		if(getTdwFile()->save(tdw)) {
			header->setFileData(filePath(Tdw), fs_data, tdw);
		}
	}
	if(hasSfxFile() && getSfxFile()->isModified()) {
		QByteArray sfx;
		if(getSfxFile()->save(sfx)) {
			header->setFileData(filePath(Sfx), fs_data, sfx);
		}
	}
	header->save(fl_data, fi_data);
}

void FieldPC::optimize(QByteArray &fs_data, QByteArray &fl_data, QByteArray &fi_data)
{
	if(!header)	return;

	header->fileToTheEnd(filePath(Tdw), fs_data);
	header->fileToTheEnd(filePath(name() + ".map"), fs_data);
	header->fileToTheEnd(filePath(name() + ".mim"), fs_data);
	header->fileToTheEnd(filePath("chara.one"), fs_data);

	header->save(fl_data, fi_data);
}

void FieldPC::setFile(FileType fileType)
{
	QString path = filePath(fileType);
	if(!path.isEmpty()) {
		if(!header->fileExists(path)) {
			header->addFile(path, false);
			Field::setFile(fileType);
		}
	}
}

QString FieldPC::filePath(FileType fileType) const
{
	switch(fileType) {
	case Msd:			return filePath(name() + ".msd");
	case Jsm:			return filePath(name() + ".jsm");
	case Id:			return filePath(name() + ".id");
	case Ca:			return filePath(name() + ".ca");
	case Rat:			return filePath(name() + ".rat");
	case Mrt:			return filePath(name() + ".mrt");
	case Inf:			return filePath(name() + ".inf");
	case Pmp:			return filePath(name() + ".pmp");
	case Pmd:			return filePath(name() + ".pmd");
	case Pvp:			return filePath(name() + ".pvp");
	case Background:	return QString();
	case Tdw:			return filePath(name() + ".tdw");
	case Msk:			return filePath(name() + ".msk");
	case Sfx:			return filePath(name() + ".sfx");
	case AkaoList:		return QString();
	}
	return QString();
}

QString FieldPC::filePath(const QString &fileName) const
{
	return "C:\\ff8\\Data\\" + _lang + "\\FIELD\\mapdata\\" + _subDir + "\\" + name() + "\\" + fileName;
}
