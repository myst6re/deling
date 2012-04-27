#ifndef DEF_FF8TEXT
#define DEF_FF8TEXT

#include <QtCore>
#include "Config.h"

class FF8Text : public QString
{
public:
	FF8Text();
	FF8Text(const QString &str);
	FF8Text(const char *str);
	FF8Text(const QByteArray &ba);
	QByteArray toFF8() const;
	static QString fromFF8(const QByteArray &ff8str);
	static QByteArray toFF8(const QString &string);
	static QString caract(quint8 ord, quint8 table=0);

private:
	static const char *_caract[240];
	static const char *_caractJp[240];
	static const char *_caractJp19[240];
	static const char *_caractJp1a[240];
	static const char *_caractJp1b[240];
	static const char *names[14];
	static const char *colors[16];
	static const char *locations[8];
};

#endif
