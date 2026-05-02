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
#include "files/PcbFile.h"

class CharaOneFile : public File
{
public:
	CharaOneFile();
	virtual ~CharaOneFile();
	bool open(const QByteArray &one, const QByteArray &pcb, bool ps);
	bool save(QByteArray &one, QByteArray &pcb) const;
	inline QString filterText() const override {
		return QString();
	}
	const CharaModel &model(int id) const {
		return models.at(id);
	}
	CharaModel &model(int id) {
		return models[id];
	}
	void setModel(int id, const CharaModel &model);
	inline int modelCount() const {
		return models.size();
	}
	inline QRgb defaultLightColor() const {
		return _defaultLightColor;
	}
	void setDefaultLightColor(QRgb defaultLightColor);
private:
	using File::open;
	QList<CharaModel> models;
	QRgb _defaultLightColor;
	bool _ps;
};
