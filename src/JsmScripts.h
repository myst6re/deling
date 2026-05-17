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

class JsmScripts;
class JsmFile;

class JsmMethod
{
	friend JsmScripts;
	friend JsmFile;
public:
	inline explicit JsmMethod(const JsmData &scriptData, const QString &name = QString()) :
		_name(name), _scriptData(scriptData) {}
	void setDecompiledScript(const QString &script, bool moreDecompiled);
	inline const QString &name() const {
		return _name;
	}
	const QString &decompiledScript(bool moreDecompiled) const;
	inline const JsmData &scriptData() const {
		return _scriptData;
	}
	inline JsmData &scriptData() {
		return _scriptData;
	}
	void setScriptData(const JsmData &scriptData);
	void setNameUnsafe(const QString &name) {
		setName(name);
	}
private:
	inline void setName(const QString &name) {
		_name = name;
	}
	QString _name, _decompiledScript, _decompiledScriptMore;
	JsmData _scriptData;
};

class JsmGroup
{
	friend JsmScripts;
	friend JsmFile;
public:
	enum Type {
		No, Main, Model, Location, Door, Background
	};

	JsmGroup(quint16 absMethodId, const QList<JsmMethod> &methodList, Type groupType, const QString &name = QString());
	inline void setCharacter(int character) {
		_character = character;
	}
	inline void setModelId(int modelID) {
		_modelID = modelID;
	}
	inline void incAbsMethodId(int inc = 1) {
		_absMethodId += inc;
	}
	inline Type type() const {
		return _type;
	}
	static Type toNativeType(Type type);
	Type nativeType() const;
	inline int character() const {
		return _character;
	}
	inline int modelId() const {
		return _modelID;
	}
	const QString &name() const {
		return _name;
	}
	quint16 absMethodId() const {
		return _absMethodId;
	}
	quint8 methodCount() const {
		return _methodList.size();
	}
	const QList<JsmMethod> &methods() const {
		return _methodList;
	}
	const JsmMethod &method(int methodID) const {
		return _methodList.at(methodID);
	}
	JsmMethod &method(int methodID) {
		return _methodList[methodID];
	}
	QString absoluteMethodName(int absMethodID) const;
	int findAbsoluteMethodIDByName(const QString &methodName) const;
	void setNameUnsafe(const QString &name) {
		setName(name);
	}
private:
	inline void setType(Type type) {
		_type = type;
	}
	inline void insertMethod(int methodID, const JsmMethod &jsmMethod) {
		_methodList.insert(methodID, jsmMethod);
	}
	inline void removeMethod(int methodID) {
		_methodList.removeAt(methodID);
	}
	inline void setName(const QString &name) {
		_name = name;
	}
	Type _type;
	int _character;
	int _modelID;
	QString _name;
	QList<JsmMethod> _methodList;
	quint16 _absMethodId;
};

class JsmScripts
{
public:
	/* In the following methods, groupID and methodID must be valid! */
	JsmScripts() {}
	JsmScripts(const QList<JsmGroup> &groupListOrderedById);
	// Header
	void countTypes(quint8 &doors, quint8 &lines, quint8 &backgrounds, quint8 &others) const;
	int countModels() const;
	// Groups
	inline const QList<JsmGroup> &groups() const {
		return _groupListOrderedById;
	}
	QList<int> groupIDsSortedByAbsMethodId() const;
	inline const JsmGroup &group(int groupID) const {
		return _groupListOrderedById.at(groupID);
	}
	inline JsmGroup &group(int groupID) {
		return _groupListOrderedById[groupID];
	}
	int findGroupIDByName(const QString &groupName) const;
	int calcGroupTypeRelativeId(int groupID) const;
	bool canSetGroupName(const QString &name) const;
	bool setGroupName(int groupID, const QString &name);
	bool groupBounds(JsmGroup::Type groupType, int &firstGroupID, int &lastGroupID) const;
	int insertGroup(int indicativeGroupID, JsmGroup::Type groupType, const QString &name);
	int insertGroup(int indicativeGroupID, const JsmGroup &jsmGroup);
	void removeGroup(int groupID);
	bool moveGroup(int groupID, bool down);
	// Scripts
	int countMethods() const;
	QList<JsmMethod> methods() const;
	static QList<int> searchJumps(const QList<JsmOpcode *> &opcodes);
	QList<JsmOpcode *> opcodesp(int groupID, int methodID,
	                            bool withLabels) const;
	JsmProgram program(int groupID, int methodID,
	                   QSet<void *> &collectPointers) const;
	bool insertMethod(int groupID, int methodID, const QString &name);
	bool insertMethod(int groupID, int methodID, const JsmMethod &jsmMethod);
	void removeMethod(int groupID, int methodID);
	bool canSetMethodName(int groupID, const QString &name);
	bool setMethodName(int groupID, int methodID, const QString &name);
private:
	/* methodID can not be valid */
	void propagateGroupCountChange(int groupID, int groupCountChange);
	void propagateMethodCountChange(int groupID, int absMethodID, int methodCountChange);
	static void mergeAndConditions(JsmControl *control, int pos, int posEnd,
	                               QSet<void *> &collectPointers,
	                               QSet<int> &usedLabels);
	static void addLabelIfExists(JsmProgram &ret, int pos, const QList<int> &labels,
                                 QSet<void *> &collectPointers);
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
};
