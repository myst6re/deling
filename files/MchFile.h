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
#ifndef MCHFILE_H
#define MCHFILE_H

#include "files/File.h"
#include "CharaModel.h"

class MchFile : public File
{
public:
	MchFile();
	bool open(const QByteArray &mch, const QString &name);
	inline QString filterText() const {
		return QObject::tr("Fichier modèle 3D écran (*.mch)");
	}
	bool hasModel() const;
	CharaModel *model() const;
	void setModel(CharaModel *model);
private:
	using File::open;
	CharaModel *_model;
};

#endif // MCHFILE_H
