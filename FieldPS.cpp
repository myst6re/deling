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

FieldPS::FieldPS(const QByteArray &data, quint32 isoFieldID)
	: Field(QString()), _isoFieldID(isoFieldID)
{
	open(data);
}

FieldPS::~FieldPS()
{
}

bool FieldPS::hasMapMimFiles() const
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

bool FieldPS::open(const QByteArray &dat_data)
{
	// pvp + mim + tdw + pmp (MIM)
	// inf + ca + id + map + msk + rat + mrt + AKAO + msd + pmd + jsm (DAT)
	quint32 posSections[12];
	const char *constData = dat_data.constData();
	int memoryPos;

	memcpy(posSections, constData, 48);

	memoryPos = posSections[Inf] - 48;

	for(int i=0 ; i<12 ; ++i)
		posSections[i] -= memoryPos;

	if((int)posSections[Last] != dat_data.size()) {
		qWarning() << "Mauvaise position section 1" << posSections[Last] << memoryPos << dat_data.size();
		return false;
	}

	_name = dat_data.mid(48, 8);

	/* MSD */

	openMsdFile(dat_data.mid(posSections[Msd], posSections[Msd+1]-posSections[Msd]));

	/* JSM */

	openJsmFile(dat_data.mid(posSections[Jsm], posSections[Jsm+1]-posSections[Jsm]));

	/* ID & CA */
	openWalkmeshFile(
				dat_data.mid(posSections[Id], posSections[Id+1]-posSections[Id]),
				dat_data.mid(posSections[Ca], posSections[Ca+1]-posSections[Ca]));

	/* RAT & MRT */
	openEncounterFile(
				dat_data.mid(posSections[Rat], posSections[Rat+1]-posSections[Rat]),
				dat_data.mid(posSections[Mrt], posSections[Mrt+1]-posSections[Mrt]));

	/* INF & PMD */
	openMiscFile(
				dat_data.mid(posSections[Inf], posSections[Inf+1]-posSections[Inf]),
				QByteArray(),
				dat_data.mid(posSections[Pmd], posSections[Pmd+1]-posSections[Pmd]),
				QByteArray());

	setOpen(true);
	return true;
}

bool FieldPS::open2(const QByteArray &dat, const QByteArray &mim, const QByteArray &lzk)
{
	const char *constData = mim.constData();
	quint32 posSectionTdw, posSectionPmp;

	memcpy(&posSectionTdw, constData, 4);
	memcpy(&posSectionPmp, &constData[4], 4);

	openTdwFile(mim.mid(0x0c + 438272, posSectionPmp-posSectionTdw));

	constData = dat.constData();
	quint32 posSectionInf, posSectionMap, posSectionMsk, memoryPos;

	memcpy(&posSectionInf, constData, 4);
	memcpy(&posSectionMap, &constData[12], 4);
	memcpy(&posSectionMsk, &constData[16], 4);

	memoryPos = posSectionInf - 48;

	openBackgroundFile(dat.mid(posSectionMap - memoryPos, posSectionMsk-posSectionMap), mim.mid(0x0c, 438272));

	openCharaFile(lzk, true);

	return true;
}

bool FieldPS::save(QByteArray &dat_data)
{
	// pvp + mim + tdw + pmp (MIM)
	// inf + ca + id + map + msk + rat + mrt + AKAO + msd + pmd + jsm (DAT)
	quint32 posSections[12];
	const char *constData = dat_data.constData();
	int memoryPos, diff;

	memcpy(posSections, constData, 48);

	memoryPos = posSections[Inf] - 48;

	for(int i=0 ; i<12 ; ++i)
		posSections[i] -= memoryPos;

	if((int)posSections[Last] != dat_data.size()) {
		qWarning() << "Mauvaise position section 1" << posSections[Last] << memoryPos << dat_data.size();
		return false;
	}

	if(msdFile!=NULL && msdFile->isModified()) {
		QByteArray msd = msdFile->save();
		dat_data.replace(posSections[Msd], posSections[Msd+1] - posSections[Msd], msd);
		diff = msd.size() - (posSections[Msd+1] - posSections[Msd]);
		for(int i=Msd+1 ; i<12 ; ++i)	posSections[i] += diff;
	}
	if(jsmFile!=NULL && jsmFile->isModified()) {
		QByteArray sym, jsm = jsmFile->save(sym);
		dat_data.replace(posSections[Jsm], posSections[Jsm+1] - posSections[Jsm], jsm);
		diff = jsm.size() - (posSections[Jsm+1] - posSections[Jsm]);
		for(int i=Jsm+1 ; i<12 ; ++i)	posSections[i] += diff;
	}
	if(encounterFile!=NULL && encounterFile->isModified()) {
		QByteArray rat, mrt;
		encounterFile->save(rat, mrt);
		dat_data.replace(posSections[Rat], posSections[Rat+1] - posSections[Rat], rat);
		diff = rat.size() - (posSections[Rat+1] - posSections[Rat]);
		for(int i=Rat+1 ; i<12 ; ++i)	posSections[i] += diff;
		dat_data.replace(posSections[Mrt], posSections[Mrt+1] - posSections[Mrt], mrt);
		diff = mrt.size() - (posSections[Mrt+1] - posSections[Mrt]);
		for(int i=Mrt+1 ; i<12 ; ++i)	posSections[i] += diff;
	}
	if(miscFile!=NULL && miscFile->isModified()) {
		QByteArray inf, pmp, pmd, pvp;
		miscFile->save(inf, pmp, pmd, pvp);
		dat_data.replace(posSections[Inf], posSections[Inf+1] - posSections[Inf], inf);
		diff = inf.size() - (posSections[Inf+1] - posSections[Inf]);
		for(int i=Inf+1 ; i<12 ; ++i)	posSections[i] += diff;
		// TODO pmp
		dat_data.replace(posSections[Pmd], posSections[Pmd+1] - posSections[Pmd], pmd);
		diff = pmd.size() - (posSections[Pmd+1] - posSections[Pmd]);
		for(int i=Pmd+1 ; i<12 ; ++i)	posSections[i] += diff;
		// TODO pvp
	}
	if(walkmeshFile!=NULL && walkmeshFile->isModified()) {
		QByteArray ca;
		walkmeshFile->save(ca);
		dat_data.replace(posSections[Ca], posSections[Ca+1] - posSections[Ca], ca);
		diff = ca.size() - (posSections[Ca+1] - posSections[Ca]);
		for(int i=Ca+1 ; i<12 ; ++i)	posSections[i] += diff;
	}

	for(int i=0 ; i<12 ; ++i)
		posSections[i] += memoryPos;

	dat_data.replace(0, 48, (char *)posSections, 48);

	return true;
}
