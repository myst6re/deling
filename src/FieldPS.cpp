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
#include "FieldPS.h"

HeaderPS::HeaderPS(const QByteArray &data, int count) :
    sectionCount(count)
{
	const int size = count * 4;
	posSections = new quint32[count + 1];
	memcpy(posSections, data.constData(), size);

	memoryPos = posSections[0] - size;

	for(int i=0 ; i<sectionCount ; ++i) {
		posSections[i] -= memoryPos;
	}
	posSections[count] = data.size();
}

HeaderPS::HeaderPS(const HeaderPS &other) :
    memoryPos(other.memoryPos), sectionCount(other.sectionCount)
{
	posSections = new quint32[other.sectionCount + 1];
	memcpy(posSections, other.posSections, (other.sectionCount + 1) * 4);
}

HeaderPS::~HeaderPS()
{
	delete[] posSections;
}

bool HeaderPS::isValid(int size) const
{
	if (posSections[sectionCount - 1] != quint32(size)) {
		return false;
	}

	for(int i=0 ; i<sectionCount ; ++i) {
		if(posSections[i] > posSections[i+1]) {
			return false;
		}
	}

	return true;
}

quint32 HeaderPS::position(int section) const
{
	return posSections[section];
}

quint32 HeaderPS::size(int section) const
{
	return posSections[section+1] - posSections[section];
}

QByteArray HeaderPS::sectionData(int section, const QByteArray &data) const
{
	return data.mid(position(section), size(section));
}

void HeaderPS::setSize(int section, quint32 newSize)
{
	int diff = newSize - size(section);

	for(int i=section+1 ; i<sectionCount ; ++i) {
		posSections[i] += diff;
	}
}

QByteArray HeaderPS::save() const
{
	QByteArray header;

	for(int i=0 ; i<sectionCount ; ++i) {
		quint32 posSection = posSections[i] + memoryPos;
		header.append((char *)&posSection, 4);
	}

	return header;
}

FieldDatHeader::FieldDatHeader(const QByteArray &data) :
    HeaderPS(data, 12)
{
}

FieldDatJpDemoHeader::FieldDatJpDemoHeader(const QByteArray &data) :
    HeaderPS(data, 9)
{
}

FieldPS::FieldPS(quint32 isoFieldID)
	: Field(QString()), _isoFieldID(isoFieldID)
{
}

FieldPS::~FieldPS()
{
}

bool FieldPS::isPc() const
{
	return false;
}

bool FieldPS::hasFiles2() const
{
	return true;
}

//void FieldPS::setIsoFieldID(quint32 isoFieldID)
//{
//	this->isoFieldID = isoFieldID;
//}

quint32 FieldPS::isoFieldID() const
{
	return _isoFieldID;
}

bool FieldPS::open(const QByteArray &dat)
{
	setOpen(false);
	// pvp + mim + tdw + pmp (MIM)
	// inf + ca + id + map + msk + rat + mrt + AKAO + msd + pmd + jsm (DAT)
	FieldDatHeader header(dat);

	if(!header.isValid(dat.size())) {
		qWarning() << "FieldPS::open Bad dat header" << dat.size();
		return false;
	}

	setName(dat.mid(48, 8));

	/* INF */
	openFile(Field::Inf, header.sectionData(FieldDatHeader::Inf, dat));

	/* CA */
	openFile(Field::Ca, header.sectionData(FieldDatHeader::Ca, dat));

	/* ID */
	openFile(Field::Id, header.sectionData(FieldDatHeader::Id, dat));

	/* MSK */
	openFile(Field::Msk, header.sectionData(FieldDatHeader::Msk, dat));

	/* RAT */
	openFile(Field::Rat, header.sectionData(FieldDatHeader::Rat, dat));

	/* MRT */
	openFile(Field::Mrt, header.sectionData(FieldDatHeader::Mrt, dat));

	/* AKAO */
	openFile(Field::AkaoList, header.sectionData(FieldDatHeader::AKAO, dat));

	/* MSD */
	openFile(Field::Msd, header.sectionData(FieldDatHeader::Msd, dat));

	/* PMD */
	openFile(Field::Pmd, header.sectionData(FieldDatHeader::Pmd, dat));

	/* JSM */
	openJsmFile(header.sectionData(FieldDatHeader::Jsm, dat));

	setOpen(true);
	return true;
}

bool FieldPS::open2(const QByteArray &dat, const QByteArray &mim, const QByteArray &lzk)
{
	const char *constData = mim.constData();

	quint32 posSectionPvp = 8, posSectionMim = posSectionPvp + 4;

	if(!hasTdwFile()) {
		if (mim.size() < 8) {
			return false;
		}

		quint32 posSectionTdwRam, posSectionTdw, posSectionPmpRam, posSectionPmp;

		memcpy(&posSectionTdwRam, constData, 4);
		memcpy(&posSectionPmpRam, &constData[4], 4);
		posSectionTdw = posSectionMim + 438272;
		posSectionPmp = posSectionPmpRam * posSectionTdw / posSectionTdwRam;

		/* PVP */
		openFile(Field::Pvp, mim.mid(posSectionPvp, posSectionMim-posSectionPvp));
		/* TDW */
		openFile(Field::Tdw, mim.mid(posSectionTdw, posSectionPmpRam-posSectionTdwRam));
		/* PMP */
		openFile(Field::Pmp, mim.mid(posSectionPmp));
	}

	if (dat.size() < 48) {
		return false;
	}

	constData = dat.constData();
	quint32 posSectionInf, posSectionMap, posSectionMsk, memoryPos;

	memcpy(&posSectionInf, constData, 4);
	memcpy(&posSectionMap, &constData[12], 4);
	memcpy(&posSectionMsk, &constData[16], 4);

	memoryPos = posSectionInf - 48;

	openBackgroundFile(dat.mid(posSectionMap - memoryPos, posSectionMsk-posSectionMap), mim.mid(posSectionMim, 438272));

	openCharaFile(lzk);

	return true;
}

bool FieldPS::save(QByteArray &dat, QByteArray &mim)
{
	// pvp + mim + tdw + pmp (MIM)
	// inf + ca + id + map + msk + rat + mrt + AKAO + msd + pmd + jsm (DAT)
	FieldDatHeader header(dat);

	if(hasMsdFile() && getMsdFile()->isModified()) {
		QByteArray msd;
		if(getMsdFile()->save(msd)) {
			dat.replace(header.position(FieldDatHeader::Msd), header.size(FieldDatHeader::Msd), msd);
			header.setSize(FieldDatHeader::Msd, msd.size());
		}
	}
	if(hasJsmFile() && getJsmFile()->isModified()) {
		QByteArray sym, jsm;
		if(getJsmFile()->save(jsm, sym)) {
			dat.replace(header.position(FieldDatHeader::Jsm), header.size(FieldDatHeader::Jsm), jsm);
			header.setSize(FieldDatHeader::Jsm, jsm.size());
		}
	}
	if(hasRatFile() && getRatFile()->isModified()) {
		QByteArray rat;
		if(getRatFile()->save(rat)) {
			dat.replace(header.position(FieldDatHeader::Rat), header.size(FieldDatHeader::Rat), rat);
			header.setSize(FieldDatHeader::Rat, rat.size());
		}
	}
	if(hasMrtFile() && getMrtFile()->isModified()) {
		QByteArray mrt;
		if(getMrtFile()->save(mrt)) {
			dat.replace(header.position(FieldDatHeader::Mrt), header.size(FieldDatHeader::Mrt), mrt);
			header.setSize(FieldDatHeader::Mrt, mrt.size());
		}
	}
	if(hasInfFile() && getInfFile()->isModified()) {
		QByteArray inf;
		if(getInfFile()->save(inf)) {
			dat.replace(header.position(FieldDatHeader::Inf), header.size(FieldDatHeader::Inf), inf);
			header.setSize(FieldDatHeader::Inf, inf.size());
		}
	}
	if(hasPmpFile() && getPmpFile()->isModified()) {
		QByteArray pmp;
		if(getPmpFile()->save(pmp)) {
			// TODO: pmp
		}
	}
	if(hasPmdFile() && getPmdFile()->isModified()) {
		QByteArray pmd;
		if(getPmdFile()->save(pmd)) {
			dat.replace(header.position(FieldDatHeader::Pmd), header.size(FieldDatHeader::Pmd), pmd);
			header.setSize(FieldDatHeader::Pmd, pmd.size());
		}
	}
	if(hasPvpFile() && getPvpFile()->isModified()) {
		QByteArray pvp;
		if(getPvpFile()->save(pvp)) {
			mim.replace(8, 4, pvp);
		}
	}
	if(hasIdFile() && getIdFile()->isModified()) {
		QByteArray id;
		if(getIdFile()->save(id)) {
			dat.replace(header.position(FieldDatHeader::Id), header.size(FieldDatHeader::Id), id);
			header.setSize(FieldDatHeader::Id, id.size());
		}
	}
	if(hasCaFile() && getCaFile()->isModified()) {
		QByteArray ca;
		if(getCaFile()->save(ca)) {
			dat.replace(header.position(FieldDatHeader::Ca), header.size(FieldDatHeader::Ca), ca);
			header.setSize(FieldDatHeader::Ca, ca.size());
		}
	}
	if(hasMskFile() && getMskFile()->isModified()) {
		QByteArray msk;
		if(getMskFile()->save(msk)) {
			dat.replace(header.position(FieldDatHeader::Msk), header.size(FieldDatHeader::Msk), msk);
			header.setSize(FieldDatHeader::Msk, msk.size());
		}
	}
	if(hasTdwFile() && getTdwFile()->isModified()) {
		QByteArray tdw;
		if(getTdwFile()->save(tdw)) {
			//TODO: tdw
		}
	}
	if(hasAkaoListFile() && getAkaoListFile()->isModified()) {
		QByteArray akao;
		if(getAkaoListFile()->save(akao)) {
			dat.replace(header.position(FieldDatHeader::AKAO), header.size(FieldDatHeader::AKAO), akao);
			header.setSize(FieldDatHeader::AKAO, akao.size());
		}
	}
	if(hasBackgroundFile() && getBackgroundFile()->isModified()) {
		QByteArray map;
		if(getBackgroundFile()->save(map)) {
			dat.replace(header.position(FieldDatHeader::Map), header.size(FieldDatHeader::Map), map);
			header.setSize(FieldDatHeader::Map, map.size());
		}
	}

	dat.replace(0, 48, header.save());

	return true;
}

FieldJpDemoPS::FieldJpDemoPS(quint32 isoFieldID)
	: FieldPS(isoFieldID)
{
}

bool FieldJpDemoPS::open(const QByteArray &dat)
{
	setOpen(false);
	// pvp + mim + tdw + pmp (MIM)
	// inf + ca + id + map + msk + rat + mrt + AKAO + msd + pmd + jsm (DAT)
	FieldDatJpDemoHeader header(dat);

	if(!header.isValid(dat.size())) {
		qWarning() << "FieldJpDemoPS::open Bad dat header" << dat.size();
		return false;
	}

	setName(dat.mid(36, 8));

	/* INF */
	openFile(Field::Inf, header.sectionData(FieldDatJpDemoHeader::Inf, dat));

	/* CA */
	openFile(Field::Ca, header.sectionData(FieldDatJpDemoHeader::Ca, dat));

	/* ID */
	openFile(Field::Id, header.sectionData(FieldDatJpDemoHeader::Id, dat));

	/* MSK */
	// openFile(Field::Msk, header.sectionData(FieldDatJpDemoHeader::Msk, dat));

	/* RAT */
	// openFile(Field::Rat, header.sectionData(FieldDatJpDemoHeader::Rat, dat));

	/* MRT */
	// openFile(Field::Mrt, header.sectionData(FieldDatJpDemoHeader::Mrt, dat));

	/* AKAO */
	openFile(Field::AkaoList, header.sectionData(FieldDatJpDemoHeader::AKAO, dat));

	/* MSD */
	openFile(Field::Msd, header.sectionData(FieldDatJpDemoHeader::Msd, dat));

	/* PMD */
	// openFile(Field::Pmd, header.sectionData(FieldDatJpDemoHeader::Pmd, dat));

	/* JSM */
	openJsmFile(header.sectionData(FieldDatJpDemoHeader::Jsm, dat), QByteArray(), true);

	setOpen(true);
	return true;
}

bool FieldJpDemoPS::open2(const QByteArray &dat, const QByteArray &mim, const QByteArray &lzk)
{
	if (dat.size() < 48 || mim.size() < 8) {
		return false;
	}

	const char *constData = dat.constData();
	quint32 posSectionInf, posSectionMap, posSectionMsk, memoryPos;

	memcpy(&posSectionInf, constData, 4);
	memcpy(&posSectionMap, constData + 12, 4);
	memcpy(&posSectionMsk, constData + 16, 4);

	memoryPos = posSectionInf - 36;

	openBackgroundFile(dat.mid(posSectionMap - memoryPos, posSectionMsk-posSectionMap), mim);

	openCharaFile(lzk);

	return true;
}
