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
#ifndef CHARAMODEL_H
#define CHARAMODEL_H

#include <QtCore>
#include <QPixmap>
#include "files/TimFile.h"

class CharaModel
{
public:
	CharaModel(const QString &name, const QList<quint32> &toc, const QByteArray &data);
	CharaModel(const QString &name);
	CharaModel();
	bool open(const QList<quint32> &toc, const QByteArray &data);
	bool isEmpty() const;
	QString name() const;
	int id() const;
	const TimFile &texture(int id) const;
	int textureCount() const;
private:
	QString _name;
	QList<TimFile> _textures;
};

#endif // CHARAMODEL_H
