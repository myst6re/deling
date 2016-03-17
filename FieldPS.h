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

class HeaderPS
{
	quint32 *posSections;
	int memoryPos;
	int sectionCount;
public:
	explicit HeaderPS(const QByteArray &data, int count);
	HeaderPS(const HeaderPS &other);
	virtual ~HeaderPS();
	bool isValid(int size) const;
	quint32 position(int section) const;
	quint32 size(int section) const;
	QByteArray sectionData(int section, const QByteArray &data) const;
	void setSize(int section, quint32 newSize);
	QByteArray save() const;
};

class FieldDatHeader : public HeaderPS
{
public:
	enum DatSections {
		Inf, Ca, Id, Map, Msk, Rat, Mrt, AKAO, Msd, Pmd, Jsm, Last
	};
	explicit FieldDatHeader(const QByteArray &data);
	virtual ~FieldDatHeader() {}
	inline quint32 position(DatSections section) const {
		return HeaderPS::position(section);
	}
	inline quint32 size(DatSections section) const {
		return HeaderPS::size(section);
	}
	inline void setSize(DatSections section, quint32 newSize) {
		HeaderPS::setSize(section, newSize);
	}
private:
	inline quint32 position(int section) const {
		return HeaderPS::position(section);
	}
	inline quint32 size(int section) const {
		return HeaderPS::size(section);
	}
	inline void setSize(int section, quint32 newSize) {
		HeaderPS::setSize(section, newSize);
	}
};

class FieldDatJpDemoHeader : public HeaderPS
{
public:
	enum DatSections {
		Inf, Ca, Id, Map, InfPartial, Msd, AKAO, Jsm, Last
	};
	explicit FieldDatJpDemoHeader(const QByteArray &data);
	virtual ~FieldDatJpDemoHeader() {}
	inline quint32 position(DatSections section) const {
		return HeaderPS::position(section);
	}
	inline quint32 size(DatSections section) const {
		return HeaderPS::size(section);
	}
	inline void setSize(DatSections section, quint32 newSize) {
		HeaderPS::setSize(section, newSize);
	}
private:
	inline quint32 position(int section) const {
		return HeaderPS::position(section);
	}
	inline quint32 size(int section) const {
		return HeaderPS::size(section);
	}
	inline void setSize(int section, quint32 newSize) {
		HeaderPS::setSize(section, newSize);
	}
};

class FieldPS : public Field
{
public:
	enum MimSections {
		Pvp, Mim, Tdw, Pmp
	};


	FieldPS(quint32 isoFieldID);
	virtual ~FieldPS();

	bool isPc() const;
	bool hasFiles2() const;
//	void setIsoFieldID(quint32 isoFieldID);
	quint32 isoFieldID() const;
	virtual bool open(const QByteArray &dat);
	virtual bool open2(const QByteArray &dat, const QByteArray &mim, const QByteArray &lzk);
	bool save(QByteArray &dat, QByteArray &mim);

private:
	quint32 _isoFieldID;
};

class FieldJpDemoPS : public FieldPS
{
public:
	FieldJpDemoPS(quint32 isoFieldID);
	virtual ~FieldJpDemoPS() {}

	bool open(const QByteArray &dat);
	bool open2(const QByteArray &dat, const QByteArray &mim, const QByteArray &lzk);
};

#endif // FIELDPS_H
