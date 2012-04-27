#ifndef JSMSCRIPTS_H
#define JSMSCRIPTS_H

#include <QtCore>
#include "JsmData.h"

class JsmScript
{
public:
	JsmScript();
	JsmScript(quint16 pos, bool flag);
	JsmScript(quint16 pos, const QString &name);
	void setName(const QString &);
//	void setPos(quint16);
	void incPos(int=1);
	void setFlag(bool);
	void setDecompiledScript(const QString &);
	const QString &name() const;
	quint16 pos() const;
	bool flag() const;
	const QString &decompiledScript() const;
private:
	QString _name, _decompiledScript;
	quint16 _pos;
	bool _flag;
};

class JsmGroup
{
public:
	enum Type {
		No, Main, Model, Location, Door, Background
	};

	JsmGroup();
	JsmGroup(quint16 exec_order, quint16 label, quint8 script_count);
	void setType(Type);
	void setCharacter(int);
	void setName(const QString &);
	void incLabel(int=1);
	void incScriptCount(int=1);
	Type type() const;
	int character() const;
	const QString &name() const;
	quint16 execOrder() const;
	quint16 label() const;
	quint8 scriptCount() const;
private:
	Type _type;
	int _character;
	QString _name;
	quint16 _exec_order, _label;
	quint8 _script_count;
};

class JsmScripts
{
public:
	/* In the following methods, groupID and methodID must be valid! */
	JsmScripts();
	JsmScripts(const QList<JsmGroup> &groupList, const QList<JsmScript> &scriptList, const JsmData &scriptData,
			   quint8 countDoors, quint8 countLines, quint8 countBackgrounds, quint8 countOthers);
	// Header
	quint8 countDoors();
	quint8 countLines();
	quint8 countBackgrounds();
	quint8 countOthers();
	int nbEntitiesForSym();
	// Groups
	const QList<JsmGroup> &getGroupList() const;
	const JsmGroup &group(int groupID) const;
	int nbGroup() const;
	void setGroupName(int groupID, const QString &name);
	void setGroupCharacter(int groupID, int character);
	void setGroupType(int groupID, JsmGroup::Type type);
	int firstMethodID(int groupID) const;
	// Scripts
	const QList<JsmScript> &getScriptList() const;
	const JsmScript &script(int groupID, int methodID) const;
	int nbScript() const;
	int nbScript(int groupID) const;
	void setScriptName(int groupID, int methodID, const QString &name);
	int posScript(int groupID, int methodID) const;
	int posScript(int groupID, int methodID, int *nbOpcode) const;
	int posOpcode(int groupID, int methodID, int opcodeID) const;
	// Data
	const JsmData &data() const;
	unsigned int key(int groupID, int methodID, int opcodeID) const;
	unsigned int key(int opcodeID) const;
//	void setKey(int groupID, int methodID, int opcodeID, unsigned int key);
//	void setKey(int opcodeID, unsigned int key);
	int param(int groupID, int methodID, int opcodeID) const;
	int param(int opcodeID) const;
	void setParam(int groupID, int methodID, int opcodeID, int param);
	void setParam(int opcodeID, int param);
	JsmOpcode opcode(int groupID, int methodID, int opcodeID) const;
	JsmOpcode opcode(int opcodeID) const;
	JsmOpcode *opcodep(int opcodeID) const;
	void setOpcode(int groupID, int methodID, int opcodeID, const JsmOpcode &value);
	void setOpcode(int opcodeID, const JsmOpcode &value);
	void insertOpcode(int groupID, int methodID, int opcodeID, const JsmOpcode &value);
	void removeOpcode(int groupID, int methodID, int opcodeID);
//	void addScript(int groupID, const JsmData &data);
	void insertScript(int groupID, int methodID, const JsmData &data, const QString &name=QString());
	void removeScript(int groupID, int methodID);
	void replaceScript(int groupID, int methodID, const JsmData &data);
	// Decompiled data
	void setDecompiledScript(int groupID, int methodID, const QString &text);
private:
	/* methodID can not be valid */
	int absoluteMethodID(int groupID, int methodID) const;
	void shiftGroupsAfter(int groupID, int methodID, int shiftGroup, int shiftScript);
	void shiftScriptsAfter(int groupID, int methodID, int shift);

	QList<JsmGroup> groupList;
	QList<JsmScript> scriptList;
	JsmData scriptData;
	quint8 _countDoors, _countLines, _countBackgrounds, _countOthers;
};

#endif // JSMSCRIPTS_H
