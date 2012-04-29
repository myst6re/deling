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
#ifndef FIELD_H
#define FIELD_H

#include <QtCore>
#include "MsdFile.h"
#include "JsmFile.h"
#include "WalkmeshFile.h"
#include "MiscFile.h"

class Field
{
public:
	Field(const QString &name);
	~Field();

	bool isOpen() const;
	void setOpen(bool open);
	void openMsdFile(const QByteArray &msd);
	void openJsmFile(const QByteArray &jsm, const QByteArray &sym=QByteArray());
	void openWalkmeshFile(const QByteArray &id, const QByteArray &ca=QByteArray());
	void openMiscFile(const QByteArray &inf, const QByteArray &rat, const QByteArray &mrt, const QByteArray &pmp, const QByteArray &pmd, const QByteArray &pvp);
	bool hasMsdFile() const;
	bool hasJsmFile() const;
	bool hasWalkmeshFile() const;
	bool hasMiscFile() const;
	virtual bool hasMapFiles() const=0;
	bool hasFiles() const;
	MsdFile *getMsdFile() const;
	JsmFile *getJsmFile() const;
	WalkmeshFile *getWalkmeshFile() const;
	MiscFile *getMiscFile() const;
	bool isModified() const;
	const QString &name() const;

protected:
	void deleteMsdFile();
	void deleteJsmFile();
	void deleteWalkmeshFile();
	void deleteMiscFile();

	bool _isOpen;
	QString _name;
	MsdFile *msdFile;
	JsmFile *jsmFile;
	WalkmeshFile *walkmeshFile;
	MiscFile *miscFile;
};

#endif // FIELD_H
