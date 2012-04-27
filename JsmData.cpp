#include "JsmData.h"

JsmData::JsmData()
{
}

JsmData::JsmData(const QByteArray &scriptData) :
	scriptData(scriptData)
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

	memcpy(&op, &constData[opcodeID * 4], 4);

	return JsmOpcode(op);
}

JsmOpcode *JsmData::opcodep(int opcodeID) const
{
	JsmOpcode op = opcode(opcodeID);

	switch(op.key()) {
	case JsmOpcode::CAL:
		return new JsmOpcodeCal(op.opcode());
	case JsmOpcode::PSHN_L:
	case JsmOpcode::PSHI_L:
	case JsmOpcode::PSHM_B:
	case JsmOpcode::PSHM_W:
	case JsmOpcode::PSHM_L:
	case JsmOpcode::PSHSM_B:
	case JsmOpcode::PSHSM_W:
	case JsmOpcode::PSHSM_L:
	case JsmOpcode::PSHAC:
		return new JsmOpcodePsh(op.opcode());
	case JsmOpcode::POPI_L:
	case JsmOpcode::POPM_B:
	case JsmOpcode::POPM_W:
	case JsmOpcode::POPM_L:
		return new JsmOpcodePop(op.opcode());
	default:
		return new JsmOpcode(op.opcode());
	}
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
	return JsmData(scriptData.mid(opcodeID*4, nbOpcode==-1 ? nbOpcode : nbOpcode*4));
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
