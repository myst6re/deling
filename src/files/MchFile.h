/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2024 Arzel Jérôme <myst6re@gmail.com>
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
#pragma once

#include "files/File.h"
#include "CharaModel.h"

class MchFile : public File
{
public:
	MchFile();
	bool open(const QByteArray &mch, const QString &name);
	virtual bool save(QByteArray &data) const override;
	static bool readFullModel(const char *constData, int size, const QString &name, CharaModel &model);
	static QByteArray writeFullModel(const CharaModel &model);
	static QList<Animation> readAnimations(const char *constData, int size, QByteArray *unknownData = nullptr);
	static QByteArray writeAnimations(const CharaModel &model);
	inline QString filterText() const {
		return QObject::tr("Field 3D model file (*.mch)");
	}
	bool hasModel() const;
	const CharaModel &model() const;
private:
	using File::open;
	CharaModel _model;
};
