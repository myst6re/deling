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
#include "JsmOpcode.h"

class JsmData
{
public:
	JsmData();
	JsmData(const QByteArray &scriptData, bool demo);
	inline int nbOpcode() const {
		return _scriptData.size() / 4;
	}
	JsmData &append(const JsmOpcode &opcode);
	JsmData &prepend(const JsmOpcode &opcode);
	JsmData &append(const JsmData &jsmData);
	JsmOpcode opcode(int opcodeID) const;
	JsmOpcode *opcodep(int opcodeID) const;
	QList<JsmOpcode *> opcodesp(int opcodeID = 0, int nbOpcode = -1) const;
	JsmData &setOpcode(int opcodeID, const JsmOpcode &opcode);
	JsmData &insertOpcode(int opcodeID, const JsmOpcode &opcode);
	JsmData &insert(int opcodeID, const JsmData &data);
	JsmData &remove(int opcodeID, int nbOpcode);
	JsmData &replace(int opcodeID, int nbOpcode, const JsmData &after);
	JsmData mid(int opcodeID, int nbOpcode = -1) const;
	JsmData copy() const;
	inline const QByteArray &constData() const {
		return _scriptData;
	}
	inline JsmData &operator+=(const JsmOpcode &opcode) {
		return append(opcode);
	}
	inline JsmData &operator+=(const JsmData &data) {
		return append(data);
	}
	inline bool operator==(const JsmData &data) const {
		return this->constData() == data.constData();
	}
	inline bool operator!=(const JsmData &data) const {
		return !(*this == data);
	}
	inline JsmOpcode operator[](int opcodeID) const {
		return opcode(opcodeID);
	}
private:
	QByteArray _scriptData;
	bool _demo;
};
