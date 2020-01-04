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
#ifndef JSMSCRIPTS_H
#define JSMSCRIPTS_H

#include <QtCore>
#include "JsmData.h"
#include "JsmExpression.h"

class JsmScript
{
public:
	JsmScript();
	JsmScript(quint16 pos, bool flag);
	JsmScript(quint16 pos, const QString &name);
	void setName(const QString &name);
//	void setPos(quint16);
	void incPos(int=1);
	void setFlag(bool);
	void setDecompiledScript(const QString &script, bool moreDecompiled);
	const QString &name() const;
	quint16 pos() const;
	bool flag() const;
	const QString &decompiledScript(bool moreDecompiled) const;
private:
	QString _name, _decompiledScript, _decompiledScriptMore;
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
	void setType(Type type);
	void setCharacter(int character);
	void setModelId(int modelId);
	void setBackgroundParamId(int paramId);
	void setName(const QString &name);
	void incLabel(int inc = 1);
	void incScriptCount(int inc = 1);
	Type type() const;
	int character() const;
	int modelId() const;
	int backgroundParamId() const;
	const QString &name() const;
	quint16 execOrder() const;
	quint16 label() const;
	quint8 scriptCount() const;
private:
	Type _type;
	int _character;
	int _modelID, _backgroundParamID;
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
	void setGroupModelId(int groupID, int modelID);
	void setGroupBackgroundParamId(int groupID, int backgroundParamID);
	void setGroupType(int groupID, JsmGroup::Type type);
	int firstMethodID(int groupID) const;
	// Scripts
	const QList<JsmScript> &getScriptList() const;
	const JsmScript &script(int absoluteMethodID) const;
	const JsmScript &script(int groupID, int methodID) const;
	int findGroup(int absoluteMethodID) const;
	int nbScript() const;
	int nbScript(int groupID) const;
	void setScriptName(int groupID, int methodID, const QString &name);
	int posScript(int groupID, int methodID) const;
	int posScript(int groupID, int methodID, int *nbOpcode) const;
	int posOpcode(int groupID, int methodID, int opcodeID) const;
	QList<int> searchJumps(const QList<JsmOpcode *> &opcodes) const;
	QList<JsmOpcode *> opcodesp(int groupID, int methodID,
	                            bool withLabels) const;
	JsmProgram program(int groupID, int methodID,
	                   QSet<void *> &collectPointers) const;
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
//	void insertGroup(int groupID);
//	void removeGroup(int groupID);
	// Decompiled data
	void setDecompiledScript(int groupID, int methodID, const QString &text, bool moreDecompiled);
private:
	/* methodID can not be valid */
	int absoluteMethodID(int groupID, int methodID) const;
	void shiftGroupsAfter(int groupID, int methodID, int shiftGroup, int shiftScript);
	void shiftScriptsAfter(int groupID, int methodID, int shift);
	static void mergeAndConditions(JsmControl *control, int pos, int posEnd,
	                               QSet<void *> &collectPointers,
	                               QSet<int> &usedLabels);
	static JsmProgram program(const QList<JsmOpcode *>::const_iterator &constBegin, 
	                          QList<JsmOpcode *>::const_iterator it,
	                          const QList<JsmOpcode *>::const_iterator &end,
	                          const QList<int> &labels,
	                          QSet<void *> &collectPointers,
	                          QSet<int> &usedLabels);
	static JsmProgram &program2ndPass(JsmProgram &program,
	                                  QSet<void *> &collectPointers,
	                                  const QSet<int> &usedLabels);

	QList<JsmGroup> groupList;
	QList<JsmScript> scriptList;
	JsmData scriptData;
	quint8 _countDoors, _countLines, _countBackgrounds, _countOthers;
};

#endif // JSMSCRIPTS_H
