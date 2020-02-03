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
#ifndef JSMDATA_H
#define JSMDATA_H

#include <QtCore>
#include "JsmOpcode.h"

class JsmData
{
public:
	JsmData();
	JsmData(const QByteArray &scriptData, bool demo);
	int nbOpcode() const;
	JsmData &append(const JsmOpcode &opcode);
	JsmData &append(const JsmData &jsmData);
	JsmOpcode opcode(int opcodeID) const;
	JsmOpcode *opcodep(int opcodeID) const;
	QList<JsmOpcode *> opcodesp(int opcodeID=0, int nbOpcode=-1) const;
	JsmData &setOpcode(int opcodeID, const JsmOpcode &opcode);
	JsmData &insertOpcode(int opcodeID, const JsmOpcode &opcode);
	JsmData &insert(int opcodeID, const JsmData &data);
	JsmData &remove(int opcodeID, int nbOpcode);
	JsmData &replace(int opcodeID, int nbOpcode, const JsmData &after);
	JsmData mid(int opcodeID, int nbOpcode=-1) const;
	const QByteArray &constData() const;
	JsmData &operator+=(const JsmOpcode &opcode);
	JsmData &operator+=(const JsmData &data);
	bool operator==(const JsmData &data) const;
	bool operator!=(const JsmData &data) const;
	JsmOpcode operator[](int opcodeID) const;
private:
	QByteArray scriptData;
	bool _demo;
};

#endif // JSMDATA_H
