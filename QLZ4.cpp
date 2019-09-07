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
