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
