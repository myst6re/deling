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
#include "CharaModel.h"

CharaModel::CharaModel()
{
}

CharaModel::CharaModel(const QString &name, const QList<TimFile> &textures) :
    _name(name), _textures(textures)
{
}

bool CharaModel::isEmpty() const
{
	return _textures.isEmpty();
}

QString CharaModel::name() const
{
	return _name.left(4);
}

int CharaModel::id() const
{
	bool ok;
	int id = _name.mid(1, 3).toInt(&ok);
	return ok ? id : -1;
}

const TimFile &CharaModel::texture(int id) const
{
	return _textures.at(id);
}

int CharaModel::textureCount() const
{
	return _textures.size();
}
