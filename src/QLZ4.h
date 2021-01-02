/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2020 Arzel Jérôme <myst6re@gmail.com>
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
#ifndef QLZ4_H
#define QLZ4_H

#include <QByteArray>

class QLZ4
{
public:
	static const QByteArray &decompressAll(const QByteArray &data, bool *ok = nullptr) {
		return decompressAll(data.constData(), data.size(), ok);
	}
	static const QByteArray &decompressAll(const char *data, int size, bool *ok = nullptr);
	static const QByteArray &decompress(const QByteArray &data, int max, bool *ok = nullptr) {
		return decompress(data.constData(), data.size(), max, ok);
	}
	static const QByteArray &decompress(const char *data, int size, int max, bool *ok = nullptr);
	static const QByteArray &compress(const QByteArray &data) {
		return compress(data.constData(), data.size());
	}
	static const QByteArray &compress(const char *data, int size);
private:
	static QByteArray result;
};

#endif // QLZ4_H
