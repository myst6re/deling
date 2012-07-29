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
			((header->fileExists("*"%name()%".mim") && header->fileExists("*"%name()%".map"))
			|| header->fileExists("*"%name()%".tdw")
			|| header->fileExists("*chara.one"));
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
	if(!header->isOpen()) {
		qWarning() << "fieldData pas ouvert" << name();
		delete header;
		header = 0;
		return false;
	}

	/* MSD */

	if(header->fileExists("*.msd"))
	{
		openFile(Msd, header->fileData("*.msd"));
	}

	/* JSM & SYM */

	if(header->fileExists("*.jsm"))
	{
		if(header->fileExists("*.sym"))
			openJsmFile(header->fileData("*.jsm"), header->fileData("*.sym"));
		else
			openJsmFile(header->fileData("*.jsm"));
	}

	/* ID */

	if(header->fileExists("*.id"))
	{
		openFile(Id, header->fileData("*.id"));
	}

	/* CA */

	if(header->fileExists("*.ca"))
	{
		openFile(Ca, header->fileData("*.ca"));
	}

	/* MSK */

	if(header->fileExists("*.msk"))
	{
		openFile(Msk, header->fileData("*.msk"));
	}

	/* RAT */

	if(header->fileExists("*.rat"))
	{
		openFile(Rat, header->fileData("*.rat"));
	}

	/* MRT */

	if(header->fileExists("*.mrt"))
	{
		openFile(Mrt, header->fileData("*.mrt"));
	}

	/* INF */

	if(header->fileExists("*.inf"))
	{
		openFile(Inf, header->fileData("*.inf"));
	}

	/* SFX */

	if(header->fileExists("*.sfx"))
	{
		openFile(Sfx, header->fileData("*.sfx"));
	}

	/* PMP */

	if(header->fileExists("*.pmp"))
	{
		openFile(Pmp, header->fileData("*.pmp"));
	}

	/* PMD */

	if(header->fileExists("*.pmd"))
	{
		openFile(Pmd, header->fileData("*.pmd"));
	}

	/* PVP */

	if(header->fileExists("*.pvp"))
	{
		openFile(Pvp, header->fileData("*.pvp"));
	}

	setOpen(true);
	return true;
}

bool FieldPC::open(FsArchive *archive)
{
	setOpen(false);

	if(header)	delete header;
	header = new FsArchive(archive->fileData("*"%name()%".fl"), archive->fileData("*"%name()%".fi"));
	if(!header->isOpen()) {
		qWarning() << "fieldData pas ouvert" << name();
		delete header;
		header = 0;
		return false;
	}

	FsHeader *msd_infos, *jsm_infos, *sym_infos, *id_infos,
			*ca_infos, *msk_infos, *inf_infos, *rat_infos,
			*mrt_infos, *pmp_infos, *pmd_infos, *pvp_infos,
			*sfx_infos;

	msd_infos = header->getFile("*"%name()%".msd");
	jsm_infos = header->getFile("*"%name()%".jsm");
	sym_infos = header->getFile("*"%name()%".sym");
	id_infos = header->getFile("*"%name()%".id");
	ca_infos = header->getFile("*"%name()%".ca");
	msk_infos = header->getFile("*"%name()%".msk");
	inf_infos = header->getFile("*"%name()%".inf");
	rat_infos = header->getFile("*"%name()%".rat");
	mrt_infos = header->getFile("*"%name()%".mrt");
	pmp_infos = header->getFile("*"%name()%".pmp");
	pmd_infos = header->getFile("*"%name()%".pmd");
	pvp_infos = header->getFile("*"%name()%".pvp");
	sfx_infos = header->getFile("*"%name()%".sfx");

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

	if(header->fileExists("*"%name()%".map") && header->fileExists("*"%name()%".mim")) {
		openBackgroundFile(header->fileData("*"%name()%".map"), header->fileData("*"%name()%".mim"));
	}
	if(!hasTdwFile()) {
		openFile(Tdw, header->fileData("*"%name()%".tdw"));
	}
	openCharaFile(header->fileData("*chara.one"));

	return true;
}

bool FieldPC::open2(FsArchive *archive)
{
	if(!header)
		return false;

	FsHeader *fi_infos_mim = header->getFile("*"%name()%".mim");
	FsHeader *fi_infos_map = header->getFile("*"%name()%".map");
	FsHeader *fi_infos_tdw = !hasTdwFile() ? header->getFile("*"%name()%".tdw") : NULL;
	FsHeader *fi_infos_one = header->getFile("*chara.one");

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

void FieldPC::save(QByteArray &fs_data, QByteArray &fl_data, QByteArray &fi_data)
{
	if(!header)	return;

	if(hasMsdFile() && getMsdFile()->isModified()) {
		QByteArray msd;
		if(getMsdFile()->save(msd)) {
			header->setFileData("*"%name()%".msd", fs_data, msd);
		}
	}
	if(hasJsmFile() && getJsmFile()->isModified()) {
		QByteArray jsm, sym;
		if(getJsmFile()->save(jsm, sym)) {
			header->setFileData("*"%name()%".jsm", fs_data, jsm);
			if(!sym.isEmpty()) {
				header->setFileData("*"%name()%".sym", fs_data, sym);
			}
		}
	}
	if(hasRatFile() && getRatFile()->isModified()) {
		QByteArray rat;
		if(getRatFile()->save(rat)) {
			header->setFileData("*"%name()%".rat", fs_data, rat);
		}
	}
	if(hasMrtFile() && getMrtFile()->isModified()) {
		QByteArray mrt;
		if(getMrtFile()->save(mrt)) {
			header->setFileData("*"%name()%".mrt", fs_data, mrt);
		}
	}
	if(hasInfFile() && getInfFile()->isModified()) {
		QByteArray inf;
		if(getInfFile()->save(inf)) {
			header->setFileData("*"%name()%".inf", fs_data, inf);
		}
	}
	if(hasPmpFile() && getPmpFile()->isModified()) {
		QByteArray pmp;
		if(getPmpFile()->save(pmp)) {
			header->setFileData("*"%name()%".pmp", fs_data, pmp);
		}
	}
	if(hasPmdFile() && getPmdFile()->isModified()) {
		QByteArray pmd;
		if(getPmdFile()->save(pmd)) {
			header->setFileData("*"%name()%".pmd", fs_data, pmd);
		}
	}
	if(hasPvpFile() && getPvpFile()->isModified()) {
		QByteArray pvp;
		if(getPvpFile()->save(pvp)) {
			header->setFileData("*"%name()%".pvp", fs_data, pvp);
		}
	}
	if(hasIdFile() && getIdFile()->isModified()) {
		QByteArray id;
		if(getIdFile()->save(id)) {
			header->setFileData("*"%name()%".id", fs_data, id);
		}
	}
	if(hasCaFile() && getCaFile()->isModified()) {
		QByteArray ca;
		if(getCaFile()->save(ca)) {
			header->setFileData("*"%name()%".ca", fs_data, ca);
		}
	}
	if(hasMskFile() && getMskFile()->isModified()) {
		QByteArray msk;
		if(getMskFile()->save(msk)) {
			header->setFileData("*"%name()%".msk", fs_data, msk);
		}
	}
	if(hasTdwFile() && getTdwFile()->isModified()) {
		QByteArray tdw;
		if(getTdwFile()->save(tdw)) {
			header->setFileData("*"%name()%".tdw", fs_data, tdw);
		}
	}
	if(hasSfxFile() && getSfxFile()->isModified()) {
		QByteArray sfx;
		if(getSfxFile()->save(sfx)) {
			header->setFileData("*"%name()%".sfx", fs_data, sfx);
		}
	}
	header->save(fl_data, fi_data);
}

void FieldPC::optimize(QByteArray &fs_data, QByteArray &fl_data, QByteArray &fi_data)
{
	if(!header)	return;

	header->fileToTheEnd("*"%name()%".tdw", fs_data);
	header->fileToTheEnd("*"%name()%".map", fs_data);
	header->fileToTheEnd("*"%name()%".mim", fs_data);
	header->fileToTheEnd("*chara.one", fs_data);

	header->save(fl_data, fi_data);
}
