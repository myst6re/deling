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
#include "JsmScripts.h"

JsmScript::JsmScript() {
}

JsmScript::JsmScript(quint16 pos, bool flag) :
	_pos(pos), _flag(flag)
{
}

JsmScript::JsmScript(quint16 pos, const QString &name) :
	_name(name), _pos(pos), _flag(false)
{

}

void JsmScript::setName(const QString &name) {
	_name = name;
}

//void JsmScript::setPos(quint16 pos) {
//	_pos = pos;
//}

void JsmScript::incPos(int inc) {
	_pos += inc;
//	qDebug() << name() << "incPos" << inc << "=>" << _pos;
}

void JsmScript::setFlag(bool flag) {
	_flag = flag;
}

void JsmScript::setDecompiledScript(const QString &text) {
	_decompiledScript = text;
}

const QString &JsmScript::name() const {
	return _name;
}

quint16 JsmScript::pos() const {
	return _pos;
}

bool JsmScript::flag() const {
	return _flag;
}

const QString &JsmScript::decompiledScript() const {
	return _decompiledScript;
}

JsmGroup::JsmGroup() {
}

JsmGroup::JsmGroup(quint16 exec_order, quint16 label, quint8 script_count)
	: _type(No), _character(-1), _modelID(-1), _exec_order(exec_order), _label(label), _script_count(script_count)
{
}

void JsmGroup::setType(Type type) {
	_type = type;
}

void JsmGroup::setCharacter(int character) {
	_character = character;
}

void JsmGroup::setModelId(int modelID) {
	_modelID = modelID;
}

void JsmGroup::setName(const QString &name) {
	_name = name;
}

void JsmGroup::incLabel(int inc)
{
	_label += inc;
//	qDebug() << name() << "incLabel" << inc << "=>" << _label;
}

void JsmGroup::incScriptCount(int inc)
{
	_script_count += inc;
//	qDebug() << name() << "incScriptCount" << inc << "=>" << _script_count;
}

JsmGroup::Type JsmGroup::type() const {
	return _type;
}

int JsmGroup::character() const {
	return _character;
}

int JsmGroup::modelId() const {
	return _modelID;
}

const QString &JsmGroup::name() const {
	return _name;
}

quint16 JsmGroup::execOrder() const {
	return _exec_order;
}

quint16 JsmGroup::label() const {
	return _label;
}

quint8 JsmGroup::scriptCount() const {
	return _script_count + 1;//count + constructor
}

JsmScripts::JsmScripts()
{
}

JsmScripts::JsmScripts(const QList<JsmGroup> &groupList, const QList<JsmScript> &scriptList, const JsmData &scriptData,
					   quint8 countDoors, quint8 countLines, quint8 countBackgrounds, quint8 countOthers) :
	groupList(groupList), scriptList(scriptList), scriptData(scriptData), _countDoors(countDoors), _countLines(countLines), _countBackgrounds(countBackgrounds), _countOthers(countOthers)
{
}

quint8 JsmScripts::countDoors()
{
	return _countDoors;
}

quint8 JsmScripts::countLines()
{
	return _countLines;
}

quint8 JsmScripts::countBackgrounds()
{
	return _countBackgrounds;
}

quint8 JsmScripts::countOthers()
{
	return _countOthers;
}

int JsmScripts::nbEntitiesForSym()
{
	return _countLines + _countBackgrounds + _countOthers;
}

const QList<JsmGroup> &JsmScripts::getGroupList() const
{
	return groupList;
}

const JsmGroup &JsmScripts::group(int groupID) const
{
	return groupList.at(groupID);
}

int JsmScripts::nbGroup() const
{
	return groupList.size();
}

void JsmScripts::setGroupName(int groupID, const QString &name)
{
	groupList[groupID].setName(name);
}

void JsmScripts::setGroupCharacter(int groupID, int character)
{
	groupList[groupID].setCharacter(character);
}

void JsmScripts::setGroupModelId(int groupID, int modelID)
{
	groupList[groupID].setModelId(modelID);
}

void JsmScripts::setGroupType(int groupID, JsmGroup::Type type)
{
	groupList[groupID].setType(type);
}

int JsmScripts::firstMethodID(int groupID) const
{
	return group(groupID).label();
}

int JsmScripts::absoluteMethodID(int groupID, int methodID) const
{
	return firstMethodID(groupID) + methodID;
}

const QList<JsmScript> &JsmScripts::getScriptList() const
{
	return scriptList;
}

const JsmScript &JsmScripts::script(int groupID, int methodID) const
{
	return scriptList.at(absoluteMethodID(groupID, methodID));
}

int JsmScripts::nbScript() const
{
	return scriptList.size();
}

int JsmScripts::nbScript(int groupID) const
{
	return group(groupID).scriptCount();
}

void JsmScripts::setScriptName(int groupID, int methodID, const QString &name)
{
	scriptList[absoluteMethodID(groupID, methodID)].setName(name);
}

const JsmData &JsmScripts::data() const
{
	return scriptData;
}

int JsmScripts::posScript(int groupID, int methodID) const
{
	return script(groupID, methodID).pos();
}

int JsmScripts::posScript(int groupID, int methodID, int *nbOpcode) const
{
	int position = posScript(groupID, methodID);
	*nbOpcode = posScript(groupID, methodID + 1) - position;
	return position;
}

int JsmScripts::posOpcode(int groupID, int methodID, int opcodeID) const
{
	return posScript(groupID, methodID) + opcodeID;
}

unsigned int JsmScripts::key(int groupID, int methodID, int opcodeID) const
{
	return key(posOpcode(groupID, methodID, opcodeID));
}

unsigned int JsmScripts::key(int opcodeID) const
{
	return opcode(opcodeID).key();
}

//void JsmScripts::setKey(int groupID, int methodID, int opcodeID, unsigned int key)
//{
//	setKey(posOpcode(groupID, methodID, opcodeID), key);
//}

//void JsmScripts::setKey(int opcodeID, unsigned int key)
//{
//	JsmOpcode op = opcode(opcodeID);
//	op.setKey(key);
//	setOpcode(opcodeID, op);
//}

int JsmScripts::param(int groupID, int methodID, int opcodeID) const
{
	return param(posOpcode(groupID, methodID, opcodeID));
}

int JsmScripts::param(int opcodeID) const
{
	return opcode(opcodeID).param();
}

void JsmScripts::setParam(int groupID, int methodID, int opcodeID, int param)
{
	setParam(posOpcode(groupID, methodID, opcodeID), param);
}

void JsmScripts::setParam(int opcodeID, int param)
{
	JsmOpcode op = opcode(opcodeID);
	op.setParam(param);
	setOpcode(opcodeID, op);
}

JsmOpcode JsmScripts::opcode(int groupID, int methodID, int opcodeID) const
{
	return opcode(posOpcode(groupID, methodID, opcodeID));
}

JsmOpcode JsmScripts::opcode(int opcodeID) const
{
	return scriptData.opcode(opcodeID);
}

JsmOpcode *JsmScripts::opcodep(int opcodeID) const
{
	return scriptData.opcodep(opcodeID);
}

void JsmScripts::setOpcode(int groupID, int methodID, int opcodeID, const JsmOpcode &value)
{
	setOpcode(posOpcode(groupID, methodID, opcodeID), value);
}

void JsmScripts::setOpcode(int opcodeID, const JsmOpcode &value)
{
	scriptData.setOpcode(opcodeID, value);
}

void JsmScripts::insertOpcode(int groupID, int methodID, int opcodeID, const JsmOpcode &value)
{
	scriptData.insertOpcode(posOpcode(groupID, methodID, opcodeID), value);

	shiftScriptsAfter(groupID, methodID, +1);
}

void JsmScripts::removeOpcode(int groupID, int methodID, int opcodeID)
{
	scriptData.remove(posOpcode(groupID, methodID, opcodeID), 1);

	shiftScriptsAfter(groupID, methodID, -1);
}

//void JsmScripts::addScript(int groupID, const JsmData &data)
//{
//	insertScript(groupID, nbScript(groupID), data);
//}

void JsmScripts::insertScript(int groupID, int methodID, const JsmData &data, const QString &name)
{
	int position = posScript(groupID, methodID);
	scriptData.insert(position, data);
	scriptList.insert(absoluteMethodID(groupID, methodID), JsmScript(position, name));

	shiftGroupsAfter(groupID, methodID, 1, data.nbOpcode());
}

void JsmScripts::removeScript(int groupID, int methodID)
{
	int nbOpcode, position = posScript(groupID, methodID, &nbOpcode);
	scriptData.remove(position, nbOpcode);
	scriptList.removeAt(absoluteMethodID(groupID, methodID));

	shiftGroupsAfter(groupID, methodID, -1, nbOpcode);
}

void JsmScripts::replaceScript(int groupID, int methodID, const JsmData &data)
{
	int nbOpcode, position = posScript(groupID, methodID, &nbOpcode);
	scriptData.replace(position, nbOpcode, data);

	shiftScriptsAfter(groupID, methodID, data.nbOpcode() - nbOpcode);
}

//void JsmScripts::insertGroup(int groupID)
//{
//	groupList.insert(groupID, JsmGroup(firstMethodID(groupID), 0));

//	shiftGroupsAfter(groupID, methodID, 1, data.nbOpcode());
//}

//void JsmScripts::removeGroup(int groupID)
//{
//	int nbOpcode, position = posScript(groupID, methodID, &nbOpcode);
//	scriptData.remove(position, nbOpcode);

//	shiftGroupsAfter(groupID, methodID, -1, nbOpcode);
//}

void JsmScripts::shiftGroupsAfter(int groupID, int methodID, int shiftGroup, int shiftScript)
{
	if(shiftGroup == 0 || shiftScript == 0)	return;

	groupList[groupID].incScriptCount(shiftGroup);

	for(int i = qMax(groupID+shiftGroup, 0) ; i<nbGroup() ; ++i)
		groupList[i].incLabel(shiftGroup);

	shiftScriptsAfter(groupID, methodID, shiftScript);
}

void JsmScripts::shiftScriptsAfter(int groupID, int methodID, int shift)
{
	if(shift == 0)	return;

	for(int i = absoluteMethodID(groupID, methodID+1) ; i<nbScript() ; ++i)
		scriptList[i].incPos(shift);
}

void JsmScripts::setDecompiledScript(int groupID, int methodID, const QString &text)
{
	scriptList[absoluteMethodID(groupID, methodID)].setDecompiledScript(text);
}
