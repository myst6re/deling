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
#ifndef PVPFILE_H
#define PVPFILE_H

#include "files/File.h"

class PvpFile : public File
{
public:
	PvpFile();
	bool open(const QByteArray &pvp);
	bool save(QByteArray &pvp);
	inline QString filterText() const {
		return QObject::tr("Fichier inconnu écran PC (*.pvp)");
	}
	quint32 value() const;
	void setValue(quint32 value);
private:
	quint32 _value;
};

#endif // PVPFILE_H
