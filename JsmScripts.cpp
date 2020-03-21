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

void JsmScript::setName(const QString &name)
{
	_name = name;
}

//void JsmScript::setPos(quint16 pos)
//{
//	_pos = pos;
//}

void JsmScript::incPos(int inc)
{
	_pos += inc;
//	qDebug() << name() << "incPos" << inc << "=>" << _pos;
}

void JsmScript::setFlag(bool flag)
{
	_flag = flag;
}

void JsmScript::setDecompiledScript(const QString &text, bool moreDecompiled)
{
	if(moreDecompiled) {
		_decompiledScriptMore = text;
	} else {
		_decompiledScript = text;
	}
}

const QString &JsmScript::name() const
{
	return _name;
}

quint16 JsmScript::pos() const
{
	return _pos;
}

bool JsmScript::flag() const
{
	return _flag;
}

const QString &JsmScript::decompiledScript(bool moreDecompiled) const
{
	if(moreDecompiled) {
		return _decompiledScriptMore;
	}
	return _decompiledScript;
}

JsmGroup::JsmGroup() {
}

JsmGroup::JsmGroup(quint16 exec_order, quint16 label, quint8 script_count)
	: _type(No), _character(-1), _modelID(-1), _backgroundParamID(-1),
	  _exec_order(exec_order), _label(label), _script_count(script_count)
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

void JsmGroup::setBackgroundParamId(int backgroundParamID) {
	_backgroundParamID = backgroundParamID;
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

int JsmGroup::backgroundParamId() const {
	return _backgroundParamID;
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
	groupList(groupList), scriptList(scriptList), scriptData(scriptData), _countDoors(countDoors),
	_countLines(countLines), _countBackgrounds(countBackgrounds), _countOthers(countOthers)
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

void JsmScripts::setGroupBackgroundParamId(int groupID, int backgroundParamID)
{
	groupList[groupID].setBackgroundParamId(backgroundParamID);
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

const JsmScript &JsmScripts::script(int absoluteMethodID) const
{
	return scriptList.at(absoluteMethodID);
}

const JsmScript &JsmScripts::script(int groupID, int methodID) const
{
	return scriptList.at(absoluteMethodID(groupID, methodID));
}

int JsmScripts::findGroup(int absoluteMethodID) const
{
	int groupId = 0;

	foreach (const JsmGroup &group, groupList) {
		if(group.label() > absoluteMethodID) {
			return groupId - 1;
		}

		groupId += 1;
	}

	return groupId - 1;
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

QList<int> JsmScripts::searchJumps(const QList<JsmOpcode *> &opcodes) const
{
	QMap<int, bool> labels;
	int i = 0, nbOpcode = opcodes.size();

	foreach(JsmOpcode *op, opcodes) {
		quint32 key = op->key();

		if(key >= JsmOpcode::JMP && key <= JsmOpcode::GJMP) {
			int jumpTo = i + op->param();
			if(jumpTo >= 0 && jumpTo < nbOpcode) {
				labels.insert(jumpTo, true);
			}
		}
		++i;
	}

	// Sort labels by jumpTo
	return labels.keys();
}

QList<JsmOpcode *> JsmScripts::opcodesp(int groupID, int methodID,
                                        bool withLabels) const
{
	int nbOpcode, position;

	position = posScript(groupID, methodID, &nbOpcode);

	QList<JsmOpcode *> opcodes = scriptData.opcodesp(position, nbOpcode);

	if(withLabels) {
		QList<int> labels = searchJumps(opcodes);
		QMutableListIterator<JsmOpcode *> it(opcodes);
		int i = 0;
		while(it.hasNext()) {
			JsmOpcode *op = it.next();
			int lbl = labels.indexOf(i);
			if(lbl != -1) {
				// Add label lbl
				it.previous();
				it.insert(new JsmOpcodeLabel(lbl));
				it.next();
			}

			unsigned int key = op->key();
			if(key >= JsmOpcode::JMP && key <= JsmOpcode::GJMP) {
				int param = op->param(),
				    lbl = labels.indexOf(i + param);
				if(lbl != -1) {
					it.setValue(new JsmOpcodeGoto(*op, lbl));
					delete op;
				}
			}

			++i;
		}
	}

	return opcodes;
}

void JsmScripts::mergeAndConditions(JsmControl *control, int pos, int posEnd,
                                    QSet<void *> &collectPointers,
                                    QSet<int> &usedLabels)
{
	JsmProgram programCopy = control->block();
	foreach(const JsmInstruction &instr, programCopy) {
		// We're looking for a ifElse with one goto in the else block
		if(instr.type() == JsmInstruction::Control) {
			JsmControl *subControl = instr.control();
			if(subControl->block().isEmpty() &&
			        subControl->type() == JsmControl::IfElse &&
			        ((JsmControlIfElse *)subControl)->blockElse().size() == 1) {
				const JsmInstruction &subInstr =
				        ((JsmControlIfElse *)subControl)->blockElse().first();
				if(subInstr.type() == JsmInstruction::Opcode) {
					JsmOpcode *opcode = subInstr.opcode();
					pos += subControl->condition()->opcodeCount();
					// The goto must go to the start of the next else
					if(opcode->isGoto() && pos + opcode->param() == posEnd) {
						pos += 1;
						// This goto will be converted to and condition
						JsmExpression *newCondition =
						        new JsmExpressionBinary(
						            JsmExpressionBinary::LogAnd,
						            control->condition(),
						            subControl->condition());
						control->block().removeFirst();
						control->setCondition(newCondition);
						collectPointers.insert(newCondition);
						usedLabels.remove(((JsmOpcodeGoto *)opcode)->label());
						continue;
					}
				}
			}
		}
		break;
	}
}

JsmProgram JsmScripts::program(const QList<JsmOpcode *>::const_iterator &constBegin,
                               QList<JsmOpcode *>::const_iterator it,
                               const QList<JsmOpcode *>::const_iterator &end,
                               const QList<int> &labels,
                               QSet<void *> &collectPointers,
                               QSet<int> &usedLabels)
{
	JsmProgram ret;
	QStack<JsmExpression *> stack;

	while(it < end) {
		JsmOpcode *op = *it;
		int pos = it - constBegin;
		int lbl = labels.indexOf(pos);
		if(lbl != -1) {
			// Add label lbl
			JsmOpcode *opLabel = new JsmOpcodeLabel(lbl);
			ret.append(JsmInstruction(opLabel));
			collectPointers.insert(opLabel);
		}

		++it;

		// Compute expressions from PUSH and CAL, will modify the stack
		JsmExpression *expression = JsmExpression::factory(op, stack);

		if(expression) {
			collectPointers.insert(expression);
			delete op; // Delete push/cal
		} else {
			bool instructionAppended = false;
			// Use the stack
			if(!stack.isEmpty()) {
				if(op->popCount() >= 0) {
					// Show elements of the stack directly
					while(stack.size() > op->popCount()) {
						// FIXME: back to Opcode
						ret.append(JsmInstruction(stack.pop()));
					}
				}

				if(op->key() >= JsmOpcode::JMP && op->key() <= JsmOpcode::GJMP) {
					if(op->key() == JsmOpcode::JPF) {
						JsmExpression *condition = stack.pop();
						JsmControl *toAppend = 0;

						if(op->param() > 0 && it - 1 + op->param() <= end) {
							QList<JsmOpcode *>::const_iterator ifElsePos = it,
									ifElseEnd = it + op->param() - 1;
							JsmOpcode *lastOpOfBlock = *(ifElseEnd - 1);

							// Else or While
							if(lastOpOfBlock->key() == JsmOpcode::JMP) {
								if(lastOpOfBlock->param() > 0
										&& ifElseEnd - 1 + lastOpOfBlock->param() <= end) {
									// If Else
									toAppend = new JsmControlIfElse(
									               condition,
									               // Block If
									               program(constBegin, ifElsePos,
									                       // Remove JMP
									                       ifElseEnd - 1, labels,
									                       collectPointers,
									                       usedLabels),
									               // Block Else
									               program(constBegin, ifElseEnd,
									                       ifElseEnd +
									                       lastOpOfBlock->param() -
									                       1, labels,
									                       collectPointers,
									                       usedLabels));

									ifElseEnd += lastOpOfBlock->param() - 1;
								} else if(lastOpOfBlock->param() < 0
										  && ifElseEnd + lastOpOfBlock->param() ==
										  ifElsePos - condition->opcodeCount()) {
									// While
									toAppend = new JsmControlWhile(
									               condition,
									               // Block loop
									               program(constBegin, ifElsePos,
									                       // Remove JMP
									                       ifElseEnd - 1, labels,
									                       collectPointers,
									                       usedLabels));
								}
							}
							if(!toAppend) {
								// If
								toAppend = new JsmControlIfElse(
								               condition,
								               // Block If
								               program(constBegin, ifElsePos,
								                       ifElseEnd, labels,
								                       collectPointers,
								                       usedLabels));
							}

							mergeAndConditions(toAppend, ifElsePos - constBegin,
							                   (it - constBegin) + op->param() - 1,
							                   collectPointers, usedLabels);

							it = ifElseEnd;
						} // else if(op->param() <= 0 0 && it - 1 + op->param() >= 0) {
							// TODO: repeatUntil: remove appended opcodes from ret
							// toAppend = new JsmControlRepeatUntil(
							//							stack.pop(),
							//							program(constBegin, it - 1 + op->param(),
							//									it - 1, labels,
							//									collectPointers));
						// }
						if(!toAppend) { // JPF
							int lbl = labels.indexOf(pos + op->param());
							if(lbl == -1) {
								qDebug() << labels << op->toString();
								qFatal(qPrintable(QString("JsmScripts::program 1 label for pos %1 + %2 not found")
								                  .arg(pos).arg(op->param())));
							}
							JsmOpcodeGoto *jmp = new JsmOpcodeGoto(*op, lbl);
							jmp->setKey(JsmOpcode::JMP);
							collectPointers.insert(jmp);
							toAppend = new JsmControlIfElse(condition, JsmProgram(),
							                                JsmProgram()
							                                << JsmInstruction(jmp));
							usedLabels.insert(lbl);
						}
						ret.append(JsmInstruction(toAppend));
						collectPointers.insert(toAppend);
					} else { // JMP/GJMP
						int lbl = labels.indexOf(pos + op->param());
						if(lbl == -1) {
							qDebug() << labels << op->toString();
							qFatal(qPrintable(QString("JsmScripts::program 3 label for pos %1 + %2 not found")
							                  .arg(pos).arg(op->param())));
						}
						JsmOpcode *toAppendOp = new JsmOpcodeGoto(*op, lbl);
						ret.append(JsmInstruction(toAppendOp));
						collectPointers.insert(toAppendOp);
						usedLabels.insert(lbl);
					}
					delete op; // Remove JPF/JMP/GJMP
					instructionAppended = true;
				} else if(op->key() == JsmOpcode::POPI_L
				          || op->key() == JsmOpcode::POPM_B
				          || op->key() == JsmOpcode::POPM_W
				          || op->key() == JsmOpcode::POPM_L) {
					JsmApplication *application = new JsmApplicationAssignment(
					                                  stack.pop(), op);
					ret.append(JsmInstruction(application));
					instructionAppended = true;
					collectPointers.insert(application);
					collectPointers.insert(op);
				} else if(stack.size() >= 2
				          && op->key() >= JsmOpcode::REQ
				          && op->key() <= JsmOpcode::REQEW) {
					JsmApplication *exec = new JsmApplicationExec(
					                           stack.pop(),
					                           stack.pop(),
					                           op);
					ret.append(JsmInstruction(exec));
					instructionAppended = true;
					collectPointers.insert(exec);
					collectPointers.insert(op);
				}
			}

			if(!instructionAppended) {
				if(!stack.isEmpty()) {
					QStack<JsmExpression *> invertedStack;
					while(!stack.isEmpty()) {
						// FIXME: only pop what will be used by the game for this application
						invertedStack.push(stack.pop());
					}
					JsmApplication *application = new JsmApplication(invertedStack, op);
					ret.append(JsmInstruction(application));
					collectPointers.insert(application);
				} else {
					ret.append(JsmInstruction(op));
				}
				collectPointers.insert(op);
			}
		}
	}

	if(!stack.isEmpty()) {
		QStack<JsmExpression *> invertedStack;
		while(!stack.isEmpty()) {
			// FIXME: only pop what will be used by the game for this application
			invertedStack.push(stack.pop());
		}
		JsmApplication *application = new JsmApplication(invertedStack, NULL);
		ret.append(JsmInstruction(application));
		collectPointers.insert(application);
	}

	return ret;
}

JsmProgram &JsmScripts::program2ndPass(JsmProgram &program,
                                       QSet<void *> &collectPointers,
                                       const QSet<int> &usedLabels)
{
	QMutableListIterator<JsmInstruction> it(program);
	while(it.hasNext()) {
		const JsmInstruction &instr = it.next();

		if(instr.type() == JsmInstruction::Control) {
			JsmControl *control = instr.control();
			program2ndPass(control->block(), collectPointers, usedLabels);
			if(control->type() == JsmControl::IfElse) {
				JsmControlIfElse *ifElse =
				        static_cast<JsmControlIfElse *>(control);
				program2ndPass(ifElse->blockElse(), collectPointers,
				               usedLabels);
				if(ifElse->block().isEmpty()) {
					if(ifElse->blockElse().isEmpty()) { // Useless if/Else
						
					} else { // Not
						JsmExpression *cond = ifElse->condition();
						if(cond->type() != JsmExpression::Binary
						        || !static_cast<JsmExpressionBinary *>(cond)->
						        logicalNot()) {
							// Explicit Not
							JsmExpression *newCondition =
							        new JsmExpressionUnary(
							            JsmExpressionUnary::LogNot,
							            cond);
							// It is unecessary to delete old pointers here
							collectPointers.insert(newCondition);
							ifElse->setCondition(newCondition);
						}
						// Else invert binary expression

						ifElse->setBlock(ifElse->blockElse());
						ifElse->setBlockElse(JsmProgram());
					}
				} else if(ifElse->block().size() == 1
				          && ifElse->blockElse().isEmpty()) { // If block only
					const JsmInstruction &subInstr = ifElse->block().first();
					if(subInstr.type() == JsmInstruction::Control) {
						JsmControl *subControl = subInstr.control();
						// Contains only one if block
						if(subControl->type() == JsmControl::IfElse) {
							JsmControlIfElse *subIfElse =
							        static_cast<JsmControlIfElse *>(subControl);
							// If block without else
							if(subIfElse->blockElse().isEmpty()) { // And
								JsmExpression *newCondition =
								        new JsmExpressionBinary(
								            JsmExpressionBinary::LogAnd,
								            ifElse->condition(),
								            subIfElse->condition());
								// It is unecessary to delete old pointers here
								collectPointers.insert(newCondition);
								ifElse->setCondition(newCondition);
								ifElse->setBlock(subIfElse->block());
							}
						}
					}
				}
			}
		} else if(instr.type() == JsmInstruction::Opcode) {
			// Removing unused labels
			JsmOpcode *opcode = instr.opcode();
			if(opcode->isLabel() && !usedLabels.contains(opcode->param())) {
				it.remove(); // FIXME: we can also delete the pointer and clean collectPointers
			}
		}
	}

	return program;
}

JsmProgram JsmScripts::program(int groupID, int methodID,
                               QSet<void *> &collectPointers) const
{
	QList<JsmOpcode *> opcodes = opcodesp(groupID, methodID, false);
	if(opcodes.isEmpty()) {
		return JsmProgram();
	}
	JsmOpcode *firstOp = opcodes.first();
	QList<int> labels = searchJumps(opcodes);
	QList<JsmOpcode *>::const_iterator begin = opcodes.constBegin();
	// Ignore label if correct at the beginning
	if(firstOp->key() == JsmOpcode::LBL
	        && firstOp->param() == absoluteMethodID(groupID, methodID)) {
		begin += 1;
		delete firstOp;
	}
	QSet<int> usedLabels; // Filled by program()
	JsmProgram p = program(opcodes.constBegin(), begin, opcodes.constEnd(),
	                       labels, collectPointers, usedLabels);
	return program2ndPass(p, collectPointers, usedLabels);
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

void JsmScripts::setDecompiledScript(int groupID, int methodID, const QString &text, bool moreDecompiled)
{
	scriptList[absoluteMethodID(groupID, methodID)].setDecompiledScript(text, moreDecompiled);
}
