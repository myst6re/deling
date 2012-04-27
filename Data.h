#ifndef DATA_H
#define DATA_H

#include <QtCore>
#include "Config.h"

class Data
{
public:
	static QString AppPath();
	static QString magic(int id);
	static QStringList locations();
	static QStringList names();
	static QStringList maplist();
private:
	static QStringList _locations;
	static QStringList _names;
	static QStringList _magic;
	static QString AppPath_cache;
};

#endif // DATA_H
