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
#include "JsmData.h"

JsmData::JsmData() :
    _demo(false)
{
}

JsmData::JsmData(const QByteArray &scriptData, bool demo) :
    scriptData(scriptData), _demo(demo)
{
	if(this->scriptData.size() % 4 != 0) {
		qWarning() << "JsmData::JsmData : Incorrect size" << this->scriptData.size();
		this->scriptData.resize(this->scriptData.size() - (this->scriptData.size() % 4));
	}
}

int JsmData::nbOpcode() const
{
	return scriptData.size()/4;
}

JsmData &JsmData::append(const JsmOpcode &opcode)
{
	quint32 op = opcode.opcode();
	scriptData.append((char *)&op, 4);
	return *this;
}

JsmData &JsmData::append(const JsmData &jsmData)
{
	scriptData.append(jsmData.constData());
	return *this;
}

JsmOpcode JsmData::opcode(int opcodeID) const
{
	const char *constData = scriptData.constData();
	quint32 op=0;

	memcpy(&op, constData + opcodeID * 4, 4);

	JsmOpcode ret(op);

	// CANIME and CANIMEKEEP does not exist in early demo version
	if(_demo && ret.key() >= JsmOpcode::CANIME) {
		ret.setKey(ret.key() + 2);
	}

	return ret;
}

JsmOpcode *JsmData::opcodep(int opcodeID) const
{
	JsmOpcode op = opcode(opcodeID);

	switch(op.key()) {
	case JsmOpcode::CAL:
		return new JsmOpcodeCal(op);
	case JsmOpcode::PSHN_L:
	case JsmOpcode::PSHI_L:
	case JsmOpcode::PSHM_B:
	case JsmOpcode::PSHM_W:
	case JsmOpcode::PSHM_L:
	case JsmOpcode::PSHSM_B:
	case JsmOpcode::PSHSM_W:
	case JsmOpcode::PSHSM_L:
	case JsmOpcode::PSHAC:
		return new JsmOpcodePsh(op);
	case JsmOpcode::POPI_L:
	case JsmOpcode::POPM_B:
	case JsmOpcode::POPM_W:
	case JsmOpcode::POPM_L:
		return new JsmOpcodePop(op);
	default:
		return new JsmOpcode(op);
	}
}

QList<JsmOpcode *> JsmData::opcodesp(int opcodeID, int nbOpcode) const
{
	QList<JsmOpcode *> ret;
	int count;
	if(nbOpcode >= 0) {
		count = nbOpcode;
	} else {
		count = this->nbOpcode() - opcodeID;
	}
	for(int i = 0 ; i < count ; ++i) {
		ret.append(opcodep(opcodeID + i));
	}
	return ret;
}

JsmData &JsmData::setOpcode(int opcodeID, const JsmOpcode &opcode)
{
	quint32 op = opcode.opcode();
	scriptData.replace(opcodeID*4, 4, (char *)&op, 4);
	return *this;
}

JsmData &JsmData::insertOpcode(int opcodeID, const JsmOpcode &opcode)
{
	quint32 op = opcode.opcode();
	scriptData.insert(opcodeID*4, (char *)&op, 4);
	return *this;
}

JsmData &JsmData::insert(int opcodeID, const JsmData &data)
{
	scriptData.insert(opcodeID*4, data.constData());
	return *this;
}

JsmData &JsmData::remove(int opcodeID, int nbOpcode)
{
	scriptData.remove(opcodeID*4, nbOpcode*4);
	return *this;
}

JsmData &JsmData::replace(int opcodeID, int nbOpcode, const JsmData &after)
{
	scriptData.replace(opcodeID*4, nbOpcode*4, after.constData());
	return *this;
}

JsmData JsmData::mid(int opcodeID, int nbOpcode) const
{
	return JsmData(scriptData.mid(opcodeID*4, nbOpcode==-1 ? nbOpcode : nbOpcode*4), _demo);
}

const QByteArray &JsmData::constData() const
{
	return scriptData;
}

JsmData &JsmData::operator+=(const JsmOpcode &opcode)
{
	return append(opcode);
}

JsmData &JsmData::operator+=(const JsmData &data)
{
	return append(data);
}

bool JsmData::operator==(const JsmData &data) const
{
	return this->constData() == data.constData();
}

bool JsmData::operator!=(const JsmData &data) const
{
	return !(*this == data);
}

JsmOpcode JsmData::operator[](int opcodeID) const
{
	return opcode(opcodeID);
}
