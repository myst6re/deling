#ifndef JSMDATA_H
#define JSMDATA_H

#include <QtCore>
#include "JsmOpcode.h"

class JsmData
{
public:
	JsmData();
	JsmData(const QByteArray &scriptData);
	int nbOpcode() const;
	JsmData &append(const JsmOpcode &opcode);
	JsmData &append(const JsmData &jsmData);
	JsmOpcode opcode(int opcodeID) const;
	JsmOpcode *opcodep(int opcodeID) const;
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
};

#endif // JSMDATA_H
