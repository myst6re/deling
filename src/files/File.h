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

#include <QtCore>

class File
{
public:
	File();
	virtual ~File() {}
	virtual bool open(const QByteArray &);
	virtual bool save(QByteArray &) const;
	virtual bool saveForExport(QByteArray &);
	bool fromFile(const QString &path);
	bool toFile(const QString &path);
	virtual QString filterText() const=0;
	inline bool isModified() const {
		return modified;
	}
	inline void setModified(bool modified) {
		this->modified = modified;
	}
	inline bool isOpen() const {
		return _open;
	}
	inline void setOpen(bool open) {
		_open = open;
	}
	const QString &errorString() const {
		return lastError;
	}
protected:
	bool modified;
	bool _open;
	static QString lastError;
};
