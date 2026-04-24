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
#include "JsmData.h"
#include "JsmExpression.h"
#include "JsmOpcode.h"

class JsmScript
{
public:
	inline explicit JsmScript(quint16 opcodeIDStart, const QString &name = QString()) :
		_name(name), _opcodeIDStart(opcodeIDStart) {}
	inline void setName(const QString &name) {
		_name = name;
	}
	inline void incOpcodeIDStart(int inc = 1) {
		_opcodeIDStart += inc;
	}
	void setDecompiledScript(const QString &script, bool moreDecompiled);
	inline const QString &name() const {
		return _name;
	}
	inline quint16 opcodeIDStart() const {
		return _opcodeIDStart;
	}
	const QString &decompiledScript(bool moreDecompiled) const;
private:
	QString _name, _decompiledScript, _decompiledScriptMore;
	quint16 _opcodeIDStart;
};

class JsmGroup
{
public:
	enum Type {
		No, Main, Model, Location, Door, Background
	};

	JsmGroup(quint16 label, quint8 scriptCount, Type groupType, int groupTypeRelativeID);
	inline void setType(Type type) {
		_type = type;
	}
	inline void setCharacter(int character) {
		_character = character;
	}
	inline void setModelId(int modelID) {
		_modelID = modelID;
	}
	inline void setGroupTypeRelativeId(int groupTypeRelativeID) {
		_groupTypeRelativeID = groupTypeRelativeID;
	}
	inline void setName(const QString &name) {
		_name = name;
	}
	inline void incLabel(int inc = 1) {
		_label += inc;
	}
	inline void incScriptCount(int inc = 1) {
		_scriptCount += inc;
	}
	inline Type type() const {
		return _type;
	}
	inline int character() const {
		return _character;
	}
	inline int modelId() const {
		return _modelID;
	}
	int groupTypeRelativeId() const {
		return _groupTypeRelativeID;
	}
	const QString &name() const {
		return _name;
	}
	quint16 label() const {
		return _label;
	}
	quint8 scriptCount() const {
		return _scriptCount;
	}
private:
	Type _type;
	int _character;
	int _modelID, _groupTypeRelativeID;
	QString _name;
	quint16 _label;
	quint8 _scriptCount;
};

class JsmScripts
{
public:
	/* In the following methods, groupID and methodID must be valid! */
	JsmScripts() {}
	JsmScripts(const QList<JsmGroup> &groupListOrderedById, const QList<JsmScript> &methodList, const JsmData &scriptData);
	// Header
	void countTypes(quint8 &doors, quint8 &lines, quint8 &backgrounds, quint8 &others) const;
	int countModels() const;
	// Groups
	inline int nbGroup() const {
		return _groupListOrderedById.size();
	}
	inline const QList<JsmGroup> &groups() const {
		return _groupListOrderedById;
	}
	QList<int> groupIDsSortedByLabel() const;
	inline const JsmGroup &group(int groupID) const {
		return _groupListOrderedById.at(groupID);
	}
	void setGroupName(int groupID, const QString &name);
	void setGroupCharacter(int groupID, int character);
	void setGroupModelId(int groupID, int modelID);
	void setGroupType(int groupID, JsmGroup::Type type);
	int firstMethodID(int groupID) const;
	int findGroupIDByName(const QString &groupName) const;
	// Scripts
	inline const QList<JsmScript> &scripts() const {
		return _methodList;
	}
	inline const JsmScript &script(int absoluteMethodID) const {
		return _methodList.at(absoluteMethodID);
	}
	const JsmScript &script(int groupID, int methodID) const;
	inline int nbScript() const {
		return _methodList.size();
	}
	int nbScript(int groupID) const;
	void setScriptName(int groupID, int methodID, const QString &name);
	int opcodeIDStartScript(int groupID, int methodID) const;
	int opcodeIDStartScript(int groupID, int methodID, int *nbOpcode) const;
	int findMethodIDByName(int groupID, const QString &methodName) const;
	QList<int> searchJumps(const QList<JsmOpcode *> &opcodes) const;
	QList<JsmOpcode *> opcodesp(int groupID, int methodID,
	                            bool withLabels) const;
	JsmProgram program(int groupID, int methodID,
	                   QSet<void *> &collectPointers) const;
	// Data
	inline const JsmData &data() const {
		return scriptData;
	}
	unsigned int key(int groupID, int methodID, int opcodeID) const;
	inline unsigned int key(int absoluteOpcodeID) const {
		return opcode(absoluteOpcodeID).key();
	}
	int param(int groupID, int methodID, int opcodeID) const;
	inline int param(int absoluteOpcodeID) const {
		return opcode(absoluteOpcodeID).param();
	}
	void setParam(int groupID, int methodID, int opcodeID, int param);
	void setParam(int opcodeID, int param);
	JsmOpcode opcode(int groupID, int methodID, int opcodeID) const;
	inline JsmOpcode opcode(int absoluteOpcodeID) const {
		return scriptData.opcode(absoluteOpcodeID);
	}
	void setOpcode(int groupID, int methodID, int opcodeID, const JsmOpcode &value);
	inline void setOpcode(int absoluteOpcodeID, const JsmOpcode &value) {
		scriptData.setOpcode(absoluteOpcodeID, value);
	}
	void insertScript(int groupID, int methodID, const QString &name = QString());
	void removeScript(int groupID, int methodID);
	void replaceScript(int groupID, int methodID, const JsmData &data);
	void insertGroup(int groupID, int groupTypeRelativeID);
	void removeGroup(int groupID);
	// Decompiled data
	void setDecompiledScript(int groupID, int methodID, const QString &text, bool moreDecompiled);
private:
	/* methodID can not be valid */
	int absoluteMethodID(int groupID, int methodID) const;
	int absoluteOpcodeID(int groupID, int methodID, int opcodeID) const;
	void propagateGroupCountChange(int groupID, int groupCountChange);
	void propagateScriptCountChange(int groupID, int absMethodID, int scriptCountChange);
	void propagateOpcodeCountChange(int absMethodID, int opcodeCountChange);
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

	QList<JsmGroup> _groupListOrderedById;
	QList<JsmScript> _methodList;
	JsmData scriptData;
};
