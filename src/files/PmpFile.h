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
#ifndef PMPFILE_H
#define PMPFILE_H

#include "files/File.h"
#include "FF8Image.h"

class PmpFile : public File
{
public:
	static QString currentFieldName;
	PmpFile();
	bool open(const QByteArray &pmp);
	bool save(QByteArray &pmp);
	inline QString filterText() const {
		return QObject::tr("Fichier données particules écran PC (*.pmp)");
	}
	QByteArray getPmpData() const;
	void setPmpData(const QByteArray &pmp);
	QImage palette() const;
	QImage image(quint8 deph=4, quint8 palID=0) const;
private:
	QByteArray pmp;
};

#endif // PMPFILE_H
