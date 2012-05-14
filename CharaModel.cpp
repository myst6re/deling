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
#include "CharaModel.h"

CharaModel::CharaModel(const QString &name, const TimFile &texture) :
	_name(name), _texture(texture)
{
}

CharaModel::CharaModel(const QString &name) :
	_name(name)
{
}

CharaModel::CharaModel(const TimFile &texture) :
	_texture(texture)
{
}

CharaModel::CharaModel()
{
}

bool CharaModel::isEmpty() const
{
	return _texture.image().isNull();
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

const TimFile &CharaModel::texture() const
{
	return _texture;
}
