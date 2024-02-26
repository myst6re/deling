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

class FF8Text : public QString
{
public:
	FF8Text();
	FF8Text(const QString &str);
	FF8Text(const char *str);
	FF8Text(const QByteArray &ba);
	FF8Text(const QByteArray &ba, const QList<QStringList> &tables);
	static QByteArray toFF8(const QString &string, bool jp);
	QByteArray toFF8() const;
	QString caract(quint8 ord, quint8 table = 0) const;
	static QString getCaract(quint8 ord, quint8 table = 0, bool jp = false);
	void setTables(const QList<QStringList> &tables);
private:
	QString fromFF8(const QByteArray &ff8str);
	QList<QStringList> tables;
	static const char *names[14];
	static const char *colors[16];
	static const char *locations[8];
};
