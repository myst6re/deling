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
#include "TextureFile.h"

TextureFile::TextureFile() :
	_currentColorTable(0)
{
}

const QImage &TextureFile::image() const
{
	return _image;
}

int TextureFile::currentColorTable() const
{
	return _currentColorTable;
}

QVector<QRgb> TextureFile::colorTable(int id) const
{
	return _colorTables.value(id);
}

void TextureFile::setCurrentColorTable(int id)
{
	qDebug() << "setCurrentColorTable" << id << _colorTables.size();
	if(id < _colorTables.size() && _currentColorTable != id) {
		_image.setColorTable(_colorTables.at(_currentColorTable = id));
	}
}

void TextureFile::setColorTable(int id, const QVector<QRgb> &colorTable)
{
	if(id < _colorTables.size()) {
		_colorTables.replace(id, colorTable);
	}
}
