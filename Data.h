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
#ifndef DATA_H
#define DATA_H

#include <QtCore>
#include "Config.h"
#include "FF8Font.h"

#define LOC_COUNT	256
#define NAM_COUNT	16
#define MAG_COUNT	57
#define MAP_COUNT	982

struct TranslateChar {
	const char *source;
	const char *comment;
};

class Data : public QObject
{
	Q_OBJECT
public:
    static bool ff8Found();
	static QString AppPath();
	static QString location(int i);
	static QString name(int i);
	static QString magic(int i);
	static QStringList maplist();
private:
	static TranslateChar locations[LOC_COUNT];
	static const char *_maplist[MAP_COUNT];
	static QString AppPathCache;
};

#endif // DATA_H
