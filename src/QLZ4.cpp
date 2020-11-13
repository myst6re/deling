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
#include "QLZ4.h"
#include <lz4.h>

QByteArray QLZ4::result;

const QByteArray &QLZ4::decompressAll(const char *data, int size, bool *ok)
{
	if (ok != nullptr) {
		*ok = false;
	}

	if (size <= 8) {
		return result;
	}

	int dstCapacity;

	memcpy(&dstCapacity, data + 4, 4);

	// Impossible ratio
	if (dstCapacity > size * 2000) {
		return result;
	}

	result.resize(dstCapacity * 1000);

	int decSize = LZ4_decompress_safe(data + 8, result.data(), size - 8, result.size());

	if (decSize < 0) {
		return result;
	}

	if (ok != nullptr) {
		*ok = true;
	}

	result.resize(decSize);

	return result;
}

const QByteArray &QLZ4::decompress(const char *data, int size, int max, bool *ok)
{
	if (ok != nullptr) {
		*ok = false;
	}

	if (size <= 8) {
		return result;
	}

	result.resize(max + 10);

	int decSize = LZ4_decompress_safe_partial(data + 8, result.data(), size - 8, max, result.size());

	if (decSize < 0) {
		return result;
	}

	if (ok != nullptr) {
		*ok = true;
	}

	result.resize(decSize);

	return result;
}

const QByteArray &QLZ4::compress(const char *data, int size)
{
	if (size <= 0) {
		result.resize(0);

		return result;
	}

	result.resize(size * 2);

	int compSize = LZ4_compress_default(data, result.data() + 8, size, result.size() - 8);

	if (compSize > 0) {
		result.resize(8 + compSize);
		memcpy(result.data(), "4ZL_", 4);
		memcpy(result.data() + 4, &size, 4);
	}

	return result;
}
