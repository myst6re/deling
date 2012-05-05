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
	: Field(name), _path(path)
{
	open(archive);
}

FieldPC::FieldPC(const QString &path)
	: Field(QString()), _path(QString())
{
	open(path);
}

FieldPC::~FieldPC()
{
	if(header!=NULL)		delete header;
}

bool FieldPC::hasMapMimFiles() const
{
	return header != NULL && header->fileExists("*"%_name+".mim") && header->fileExists("*"%_name+".map");
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

	header = new FsArchive(archivePath);
	if(!header->isOpen()) {
		qWarning() << "fieldData pas ouvert" << _name;
		delete header;
		header = NULL;
		return false;
	}

	/* MSD */

	if(header->fileExists("*.msd"))
	{
		openMsdFile(header->fileData("*.msd"));
	}

	/* JSM & SYM */

	if(header->fileExists("*.jsm"))
	{
		if(header->fileExists("*.sym"))
			openJsmFile(header->fileData("*.jsm"), header->fileData("*.sym"));
		else
			openJsmFile(header->fileData("*.jsm"));
	}

	/* ID & CA */

	if(header->fileExists("*.id"))
	{
		if(header->fileExists("*.ca"))
			openWalkmeshFile(header->fileData("*.id"), header->fileData("*.ca"));
		else
			openWalkmeshFile(header->fileData("*.id"));
	}

	/* RAT & MRT */

	if(header->fileExists("*.rat") || header->fileExists("*.mrt"))
	{
		openEncounterFile(header->fileData("*.rat"), header->fileData("*.mrt"));
	}

	/* INF, PMP, PMD & PVP */

	if(header->fileExists("*.inf") || header->fileExists("*.pmp")
			|| header->fileExists("*.pmd") || header->fileExists("*.pvp"))
	{
		openMiscFile(header->fileData("*.inf"),
					 header->fileData("*.pmp"),
					 header->fileData("*.pmd"),
					 header->fileData("*.pvp"));
	}

	setOpen(true);
	return true;
}

bool FieldPC::open(FsArchive *archive)
{
	setOpen(false);

	header = new FsArchive(archive->fileData("*"%_name%".fl"), archive->fileData("*"%_name%".fi"));
	if(!header->isOpen()) {
		qWarning() << "fieldData pas ouvert" << _name;
		delete header;
		header = NULL;
		return false;
	}

	FsHeader *msd_infos, *jsm_infos, *sym_infos, *id_infos, *ca_infos, *inf_infos, *rat_infos, *mrt_infos, *pmp_infos, *pmd_infos, *pvp_infos;

	msd_infos = header->getFile("*"%_name%".msd");
	jsm_infos = header->getFile("*"%_name%".jsm");
	sym_infos = header->getFile("*"%_name%".sym");
	id_infos = header->getFile("*"%_name%".id");
	ca_infos = header->getFile("*"%_name%".ca");
	inf_infos = header->getFile("*"%_name%".inf");
	rat_infos = header->getFile("*"%_name%".rat");
	mrt_infos = header->getFile("*"%_name%".mrt");
	pmp_infos = header->getFile("*"%_name%".pmp");
	pmd_infos = header->getFile("*"%_name%".pmd");
	pvp_infos = header->getFile("*"%_name%".pvp");

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

	if(maxSize==0) {
		qWarning() << "No files !" << _name;
		delete header;
		header = NULL;
		return false;
	}

	QByteArray fs_data = archive->fileData("*"%_name%".fs", true, maxSize);

	if(fs_data.isEmpty()) {
		qWarning() << "No data !" << _name << maxSize;
		delete header;
		header = NULL;
		return false;
	}

	/* MSD */

	if(msd_infos!=NULL)
	{
		openMsdFile(msd_infos->data(fs_data));
	}

	/* JSM & SYM */

	if(jsm_infos!=NULL)
	{
		if(sym_infos!=NULL)
			openJsmFile(jsm_infos->data(fs_data), sym_infos->data(fs_data));
		else
			openJsmFile(jsm_infos->data(fs_data));
	}

	/* ID & CA */

	if(id_infos!=NULL)
	{
		if(ca_infos!=NULL)
			openWalkmeshFile(id_infos->data(fs_data), ca_infos->data(fs_data));
		else
			openWalkmeshFile(id_infos->data(fs_data));
	}

	/* RAT & MRT */

	if(rat_infos!=NULL || mrt_infos!=NULL)
	{
		openEncounterFile(rat_infos!=NULL ? rat_infos->data(fs_data) : QByteArray(),
						  mrt_infos!=NULL ? mrt_infos->data(fs_data) : QByteArray());
	}

	/* INF, PMP, PMD & PVP */

	if(inf_infos!=NULL || pmp_infos!=NULL
			|| pmd_infos!=NULL || pvp_infos!=NULL)
	{
		openMiscFile(inf_infos!=NULL ? inf_infos->data(fs_data) : QByteArray(),
					 pmp_infos!=NULL ? pmp_infos->data(fs_data) : QByteArray(),
					 pmd_infos!=NULL ? pmd_infos->data(fs_data) : QByteArray(),
					 pvp_infos!=NULL ? pvp_infos->data(fs_data) : QByteArray());
	}

	setOpen(true);
	return true;
}

bool FieldPC::open2()
{
	if(header==NULL)
		return false;

	if(header->fileExists("*"%name()%".map") && header->fileExists("*"%name()%".mim")) {
		openBackgroundFile(header->fileData("*"%name()%".map"), header->fileData("*"%name()%".mim"));
	}
	openTdwFile(header->fileData("*"%name()%".tdw"));
//	chara_data = header->fileData("*chara.one");

	return true;
}

bool FieldPC::open2(FsArchive *archive)
{
	if(header==NULL)
		return false;

	FsHeader *fi_infos_mim = header->getFile("*"%name()%".mim");
	FsHeader *fi_infos_map = header->getFile("*"%name()%".map");
	FsHeader *fi_infos_tdw = header->getFile("*"%name()%".tdw");
//	FsHeader *fi_infos_one = header->getFile("*chara.one");

	if(fi_infos_mim==NULL || fi_infos_map==NULL)
		return false;

	quint32 maxPos = qMax(fi_infos_mim->position()+fi_infos_mim->uncompressed_size(),
						  fi_infos_map->position()+fi_infos_map->uncompressed_size());
	if(fi_infos_tdw!=NULL) {
		maxPos = qMax(maxPos, fi_infos_tdw->position()+fi_infos_tdw->uncompressed_size());
	}

	QByteArray fs_data = archive->fileData(path(), true, maxPos);

	if(fs_data.isEmpty())	 return false;

	if(fi_infos_mim!=NULL && fi_infos_map!=NULL) {
		openBackgroundFile(fi_infos_map->data(fs_data), fi_infos_mim->data(fs_data));
	}
	if(fi_infos_tdw!=NULL) {
		openTdwFile(fi_infos_tdw->data(fs_data));
	}
//	if(fi_infos_one!=NULL) {
//		chara_data = fi_infos_one->data(fs_data);
//	}

	return true;
}

void FieldPC::save(QByteArray &fs_data, QByteArray &fl_data, QByteArray &fi_data)
{
	if(header == NULL)	return;

	if(msdFile!=NULL && msdFile->isModified())
		header->setFileData("*"%_name%".msd", fs_data, msdFile->save());
	if(jsmFile!=NULL && jsmFile->isModified()) {
		QByteArray sym;
		header->setFileData("*"%_name%".jsm", fs_data, jsmFile->save(sym));
		if(!sym.isEmpty()) {
			header->setFileData("*"%_name%".sym", fs_data, sym);
		}
	}
	if(encounterFile!=NULL && encounterFile->isModified()) {
		QByteArray rat, mrt;
		encounterFile->save(rat, mrt);
		header->setFileData("*"%_name%".rat", fs_data, rat);
		header->setFileData("*"%_name%".mrt", fs_data, mrt);
	}
	if(miscFile!=NULL && miscFile->isModified()) {
		QByteArray inf, pmp, pmd, pvp;
		miscFile->save(inf, pmp, pmd, pvp);
		header->setFileData("*"%_name%".inf", fs_data, inf);
		header->setFileData("*"%_name%".pmp", fs_data, pmp);
		header->setFileData("*"%_name%".pmd", fs_data, pmd);
		header->setFileData("*"%_name%".pvp", fs_data, pvp);
	}
	if(walkmeshFile!=NULL && walkmeshFile->isModified()) {
		QByteArray ca;
		walkmeshFile->save(ca);
		header->setFileData("*"%_name%".ca", fs_data, ca);
	}
	header->save(fl_data, fi_data);
}

void FieldPC::optimize(QByteArray &fs_data, QByteArray &fl_data, QByteArray &fi_data)
{
	if(header == NULL)	return;

	header->fileToTheEnd("*"%_name%".tdw", fs_data);
	header->fileToTheEnd("*"%_name%".map", fs_data);
	header->fileToTheEnd("*"%_name%".mim", fs_data);
	header->fileToTheEnd("*chara.one", fs_data);

	header->save(fl_data, fi_data);
}
