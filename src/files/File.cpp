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
#include "File.h"

QString File::lastError;

File::File() :
	modified(false)
{
}

bool File::fromFile(const QString &path)
{
	QFile f(path);
	if (f.open(QIODevice::ReadOnly)) {
		bool ok = open(f.readAll());
		if (!ok) {
			lastError = QObject::tr("Format de fichier invalide");
		}
		f.close();
		return ok;
	}
	lastError = f.errorString();
	return false;
}

bool File::toFile(const QString &path)
{
	QFile f(path);
	if (f.open(QIODevice::WriteOnly)) {
		QByteArray data;
		bool ok;
		if ((ok = save(data))) {
			f.write(data);
		} else {
			lastError = QObject::tr("Erreur inconnue");
		}
		f.close();
		return ok;
	}
	lastError = f.errorString();
	return false;
}

bool File::open(const QByteArray &)
{
	return false;
}

bool File::save(QByteArray &)
{
	return false;
}

bool File::isModified() const
{
	return modified;
}

void File::setModified(bool modified)
{
	this->modified = modified;
}

const QString &File::errorString() const
{
	return lastError;
}
