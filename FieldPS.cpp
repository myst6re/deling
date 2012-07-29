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

FieldPS::FieldPS(const QByteArray &dat, quint32 isoFieldID)
	: Field(QString()), _isoFieldID(isoFieldID)
{
	open(dat);
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
	quint32 posSections[12];
	const char *constData = dat.constData();
	int memoryPos;

	memcpy(posSections, constData, 48);// 12 * 4

	memoryPos = posSections[Inf] - 48;

	for(int i=0 ; i<12 ; ++i)
		posSections[i] -= memoryPos;

	if((int)posSections[Last] != dat.size()) {
		qWarning() << "Mauvaise position section 1" << posSections[Last] << memoryPos << dat.size();
		return false;
	}

	setName(dat.mid(48, 8));

	/* MSD */
	openFile(Field::Msd, dat.mid(posSections[Msd], posSections[Msd+1]-posSections[Msd]));

	/* JSM */
	openJsmFile(dat.mid(posSections[Jsm], posSections[Jsm+1]-posSections[Jsm]));

	/* ID */
	openFile(Field::Id, dat.mid(posSections[Id], posSections[Id+1]-posSections[Id]));

	/* CA */
	openFile(Field::Ca, dat.mid(posSections[Ca], posSections[Ca+1]-posSections[Ca]));

	/* MSK */
	openFile(Field::Msk, dat.mid(posSections[Msk], posSections[Msk+1]-posSections[Msk]));

	/* RAT */
	openFile(Field::Rat, dat.mid(posSections[Rat], posSections[Rat+1]-posSections[Rat]));

	/* MRT */
	openFile(Field::Mrt, dat.mid(posSections[Mrt], posSections[Mrt+1]-posSections[Mrt]));

	/* INF */
	openFile(Field::Inf, dat.mid(posSections[Inf], posSections[Inf+1]-posSections[Inf]));

	/* AKAO */
	openFile(Field::AkaoList, dat.mid(posSections[AKAO], posSections[AKAO+1]-posSections[AKAO]));

	/* PMD */
	openFile(Field::Pmd, dat.mid(posSections[Pmd], posSections[Pmd+1]-posSections[Pmd]));

	setOpen(true);
	return true;
}

bool FieldPS::open2(const QByteArray &dat, const QByteArray &mim, const QByteArray &lzk)
{
	const char *constData = mim.constData();

	quint32 posSectionPvp = 8, posSectionMim = posSectionPvp + 4;

	if(!hasTdwFile()) {
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

bool FieldPS::save(QByteArray &dat)
{
	// pvp + mim + tdw + pmp (MIM)
	// inf + ca + id + map + msk + rat + mrt + AKAO + msd + pmd + jsm (DAT)
	quint32 posSections[12];
	const char *constData = dat.constData();
	int memoryPos, diff;

	memcpy(posSections, constData, 48);

	memoryPos = posSections[Inf] - 48;

	for(int i=0 ; i<12 ; ++i)
		posSections[i] -= memoryPos;

	if((int)posSections[Last] != dat.size()) {
		qWarning() << "Mauvaise position section 1" << posSections[Last] << memoryPos << dat.size();
		return false;
	}

	if(hasMsdFile() && getMsdFile()->isModified()) {
		QByteArray msd;
		if(getMsdFile()->save(msd)) {
			dat.replace(posSections[Msd], posSections[Msd+1] - posSections[Msd], msd);
			diff = msd.size() - (posSections[Msd+1] - posSections[Msd]);
			for(int i=Msd+1 ; i<12 ; ++i)	posSections[i] += diff;
		}
	}
	if(hasJsmFile() && getJsmFile()->isModified()) {
		QByteArray sym, jsm;
		if(getJsmFile()->save(jsm, sym)) {
			dat.replace(posSections[Jsm], posSections[Jsm+1] - posSections[Jsm], jsm);
			diff = jsm.size() - (posSections[Jsm+1] - posSections[Jsm]);
			for(int i=Jsm+1 ; i<12 ; ++i)	posSections[i] += diff;
		}
	}
	if(hasRatFile() && getRatFile()->isModified()) {
		QByteArray rat;
		if(getRatFile()->save(rat)) {
			dat.replace(posSections[Rat], posSections[Rat+1] - posSections[Rat], rat);
			diff = rat.size() - (posSections[Rat+1] - posSections[Rat]);
			for(int i=Rat+1 ; i<12 ; ++i)	posSections[i] += diff;
		}
	}
	if(hasMrtFile() && getMrtFile()->isModified()) {
		QByteArray mrt;
		if(getMrtFile()->save(mrt)) {
			dat.replace(posSections[Mrt], posSections[Mrt+1] - posSections[Mrt], mrt);
			diff = mrt.size() - (posSections[Mrt+1] - posSections[Mrt]);
			for(int i=Mrt+1 ; i<12 ; ++i)	posSections[i] += diff;
		}
	}
	if(hasInfFile() && getInfFile()->isModified()) {
		QByteArray inf;
		if(getInfFile()->save(inf)) {
			dat.replace(posSections[Inf], posSections[Inf+1] - posSections[Inf], inf);
			diff = inf.size() - (posSections[Inf+1] - posSections[Inf]);
			for(int i=Inf+1 ; i<12 ; ++i)	posSections[i] += diff;
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
			dat.replace(posSections[Pmd], posSections[Pmd+1] - posSections[Pmd], pmd);
			diff = pmd.size() - (posSections[Pmd+1] - posSections[Pmd]);
			for(int i=Pmd+1 ; i<12 ; ++i)	posSections[i] += diff;
		}
	}
	if(hasPvpFile() && getPvpFile()->isModified()) {
		QByteArray pvp;
		if(getPvpFile()->save(pvp)) {
			// TODO: pvp
		}
	}
	if(hasIdFile() && getIdFile()->isModified()) {
		QByteArray id;
		if(getIdFile()->save(id)) {
			dat.replace(posSections[Id], posSections[Id+1] - posSections[Id], id);
			diff = id.size() - (posSections[Id+1] - posSections[Id]);
			for(int i=Id+1 ; i<12 ; ++i)	posSections[i] += diff;
		}
	}
	if(hasCaFile() && getCaFile()->isModified()) {
		QByteArray ca;
		if(getCaFile()->save(ca)) {
			dat.replace(posSections[Ca], posSections[Ca+1] - posSections[Ca], ca);
			diff = ca.size() - (posSections[Ca+1] - posSections[Ca]);
			for(int i=Ca+1 ; i<12 ; ++i)	posSections[i] += diff;
		}
	}
	if(hasMskFile() && getMskFile()->isModified()) {
		QByteArray msk;
		if(getMskFile()->save(msk)) {
			dat.replace(posSections[Msk], posSections[Msk+1] - posSections[Msk], msk);
			diff = msk.size() - (posSections[Msk+1] - posSections[Msk]);
			for(int i=Msk+1 ; i<12 ; ++i)	posSections[i] += diff;
		}
	}
	if(hasTdwFile() && getTdwFile()->isModified()) {
		QByteArray tdw;
		if(getTdwFile()->save(tdw)) {
			dat.replace(posSections[Tdw], posSections[Tdw+1] - posSections[Tdw], tdw);
			diff = tdw.size() - (posSections[Tdw+1] - posSections[Tdw]);
			for(int i=Tdw+1 ; i<12 ; ++i)	posSections[i] += diff;
		}
	}
	if(hasAkaoListFile() && getAkaoListFile()->isModified()) {
		QByteArray akao;
		if(getAkaoListFile()->save(akao)) {
			dat.replace(posSections[AKAO], posSections[AKAO+1] - posSections[AKAO], akao);
			diff = akao.size() - (posSections[AKAO+1] - posSections[AKAO]);
			for(int i=AKAO+1 ; i<12 ; ++i)	posSections[i] += diff;
		}
	}

	for(int i=0 ; i<12 ; ++i)
		posSections[i] += memoryPos;

	dat.replace(0, 48, (char *)posSections, 48);

	return true;
}
