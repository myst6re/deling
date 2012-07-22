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
#ifndef MISCFILE_H
#define MISCFILE_H

#include <QtCore>

class MiscFile
{
public:
	MiscFile();
    bool open(const QByteArray &pmp, const QByteArray &pmd, const QByteArray &pvp);
    bool save(QByteArray &pmp, QByteArray &pmd, QByteArray &pvp);
	bool isModified() const;
	const QByteArray &getPvpData() const;
	void setPvpData(const QByteArray &pvp);
	const QByteArray &getPmpData() const;
	void setPmpData(const QByteArray &pmp);
	const QByteArray &getPmdData() const;
	void setPmdData(const QByteArray &pmd);
private:
    QByteArray pvp, pmp, pmd;
	bool modified;
};

#endif // MISCFILE_H
