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
#ifndef FIELDPS_H
#define FIELDPS_H

#include "Field.h"

class FieldPS : public Field
{
public:
	enum MimSections {
		Pvp, Mim, Tdw, Pmp
	};
	enum DatSections {
		Inf, Ca, Id, Map, Msk, Rat, Mrt, AKAO, Msd, Pmd, Jsm, Last
	};

	FieldPS(const QByteArray &data, quint32 isoFieldID);
	~FieldPS();

	bool hasMapFiles() const;
//	void setIsoFieldID(quint32 isoFieldID);
	quint32 isoFieldID() const;
	bool open(const QByteArray &dat_data);
	bool open2(const QByteArray &dat, const QByteArray &mim);
	bool save(QByteArray &dat_data);

private:
	quint32 _isoFieldID;
};

#endif // FIELDPS_H
