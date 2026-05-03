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
#include "JsmScripts.h"

void JsmScript::setDecompiledScript(const QString &text, bool moreDecompiled)
{
	if (moreDecompiled) {
		_decompiledScriptMore = text;
	} else {
		_decompiledScript = text;
	}
}

const QString &JsmScript::decompiledScript(bool moreDecompiled) const
{
	return moreDecompiled ? _decompiledScriptMore : _decompiledScript;
}

JsmGroup::JsmGroup(quint16 label, quint8 scriptCount, Type groupType, int groupTypeRelativeID)
	: _type(groupType), _character(-1), _modelID(-1), _groupTypeRelativeID(groupTypeRelativeID),
	  _label(label), _scriptCount(scriptCount)
{
}

JsmScripts::JsmScripts(const QList<JsmGroup> &groupListOrderedById, const QList<JsmScript> &methodList, const JsmData &scriptData) :
	_groupListOrderedById(groupListOrderedById), _methodList(methodList), scriptData(scriptData)
{
}

void JsmScripts::countTypes(quint8 &doors, quint8 &lines, quint8 &backgrounds, quint8 &others) const
{
	doors = 0;
	lines = 0;
	backgrounds = 0;
	others = 0;

	for (const JsmGroup &group: _groupListOrderedById) {
		switch (group.type()) {
		case JsmGroup::Door:
			++doors;
			break;
		case JsmGroup::Location:
			++lines;
			break;
		case JsmGroup::Background:
			++backgrounds;
			break;
		default:
			++others;
			break;
		}
	}
}

int JsmScripts::countModels() const
{
	int ret = 0;
	for (const JsmGroup &group: _groupListOrderedById) {
		ret += int(group.type() == JsmGroup::Model);
	}
	return ret;
}

QList<int> JsmScripts::groupIDsSortedByLabel() const
{
	QMultiMap<int, int> groupIDByLabel;
	int groupID = 0;
	for (const JsmGroup &group: _groupListOrderedById) {
		groupIDByLabel.insert(group.label(), groupID);
		groupID += 1;
	}

	return groupIDByLabel.values();
}

void JsmScripts::setGroupName(int groupID, const QString &name)
{
	_groupListOrderedById[groupID].setName(name);
}

void JsmScripts::setGroupCharacter(int groupID, int character)
{
	_groupListOrderedById[groupID].setCharacter(character);
}

void JsmScripts::setGroupModelId(int groupID, int modelID)
{
	_groupListOrderedById[groupID].setModelId(modelID);
}

void JsmScripts::setGroupType(int groupID, JsmGroup::Type type)
{
	_groupListOrderedById[groupID].setType(type);
}

int JsmScripts::firstMethodID(int groupID) const
{
	return group(groupID).label();
}

int JsmScripts::absoluteMethodID(int groupID, int methodID) const
{
	return firstMethodID(groupID) + methodID;
}

int JsmScripts::findGroupIDByName(const QString &groupName) const
{
	int groupID = 0;

	for (const JsmGroup &group: _groupListOrderedById) {
		if (group.name() == groupName) {
			return groupID;
		}

		groupID += 1;
	}

	return -1;
}

const JsmScript &JsmScripts::script(int groupID, int methodID) const
{
	return _methodList.at(absoluteMethodID(groupID, methodID));
}

int JsmScripts::nbScript(int groupID) const
{
	return group(groupID).scriptCount();
}

void JsmScripts::setScriptName(int groupID, int methodID, const QString &name)
{
	_methodList[absoluteMethodID(groupID, methodID)].setName(name);
}

int JsmScripts::opcodeIDStartScript(int groupID, int methodID) const
{
	return script(groupID, methodID).opcodeIDStart();
}

int JsmScripts::opcodeIDStartScript(int groupID, int methodID, int *nbOpcode) const
{
	int opcodeID = opcodeIDStartScript(groupID, methodID);
	*nbOpcode = opcodeIDStartScript(groupID, methodID + 1) - opcodeID;
	return opcodeID;
}

int JsmScripts::absoluteOpcodeID(int groupID, int methodID, int opcodeID) const
{
	return opcodeIDStartScript(groupID, methodID) + opcodeID;
}

int JsmScripts::findMethodIDByName(int groupID, const QString &methodName) const
{
	if (groupID >= _groupListOrderedById.size()) {
		return -1;
	}

	const JsmGroup &group = _groupListOrderedById.at(groupID);

	for (int methodID = group.label(); methodID < group.label() + group.scriptCount(); ++methodID) {
		const JsmScript &method = _methodList.at(methodID);
		if (method.name() == methodName) {
			return methodID;
		}
	}

	return -1;
}

QList<int> JsmScripts::searchJumps(const QList<JsmOpcode *> &opcodes) const
{
	QMap<int, bool> labels;
	int i = 0, nbOpcode = opcodes.size();

	for (JsmOpcode *op: opcodes) {
		quint32 key = op->key();

		if (key >= JsmOpcode::JMP && key <= JsmOpcode::GJMP) {
			int jumpTo = i + op->param();
			if (jumpTo >= 0 && jumpTo < nbOpcode) {
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
	int nbOpcode, opcodeID;

	opcodeID = opcodeIDStartScript(groupID, methodID, &nbOpcode);

	QList<JsmOpcode *> opcodes = scriptData.opcodesp(opcodeID, nbOpcode);

	if (withLabels) {
		QList<int> labels = searchJumps(opcodes);
		QMutableListIterator<JsmOpcode *> it(opcodes);
		int i = 0;
		while (it.hasNext()) {
			JsmOpcode *op = it.next();
			int lbl = labels.indexOf(i);
			if (lbl != -1) {
				// Add label lbl
				it.previous();
				it.insert(new JsmOpcodeLabel(lbl));
				it.next();
			}

			unsigned int key = op->key();
			if (key >= JsmOpcode::JMP && key <= JsmOpcode::GJMP) {
				int param = op->param(),
				    lbl = labels.indexOf(i + param);
				if (lbl != -1) {
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
	for (const JsmInstruction &instr: programCopy) {
		// We're looking for a ifElse with one goto in the else block
		if (instr.type() == JsmInstruction::Control) {
			JsmControl *subControl = instr.control();
			if (subControl->block().isEmpty() &&
			        subControl->type() == JsmControl::IfElse &&
			        ((JsmControlIfElse *)subControl)->blockElse().size() == 1) {
				const JsmInstruction &subInstr =
				        ((JsmControlIfElse *)subControl)->blockElse().first();
				if (subInstr.type() == JsmInstruction::Opcode) {
					JsmOpcode *opcode = subInstr.opcode();
					pos += subControl->condition()->opcodeCount();
					// The goto must go to the start of the next else
					if (opcode->isGoto() && pos + opcode->param() == posEnd) {
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

void JsmScripts::addLabelIfExists(JsmProgram &ret, int pos, const QList<int> &labels,
                                  QSet<void *> &collectPointers)
{
	int lbl = labels.indexOf(pos);
	if (lbl != -1) {
		// Add label lbl
		JsmOpcode *opLabel = new JsmOpcodeLabel(lbl);
		ret.append(JsmInstruction(opLabel, -1));
		collectPointers.insert(opLabel);
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

	while (it < end) {
		JsmOpcode *op = *it;
		int pos = it - constBegin;
		addLabelIfExists(ret, pos, labels, collectPointers);

		++it;

		// Compute expressions from PUSH and CAL, will modify the stack
		JsmExpression *expression = JsmExpression::factory(op, stack, pos);

		if (expression) {
			collectPointers.insert(expression);
			collectPointers.insert(op); // CAL
		} else {
			bool instructionAppended = false;
			// Use the stack
			if (op->popCount() >= 0) {
				// Show elements of the stack directly
				while (stack.size() > op->popCount()) {
					// FIXME: back to Opcode
					ret.append(JsmInstruction(stack.pop()));
				}
			}

			if (op->key() >= JsmOpcode::JMP && op->key() <= JsmOpcode::GJMP) {
				if (op->key() == JsmOpcode::JPF && !stack.isEmpty()) {
					JsmExpression *condition = stack.pop();
					JsmControl *toAppend = 0;

					if (op->param() > 0 && it - 1 + op->param() <= end) {
						QList<JsmOpcode *>::const_iterator ifElsePos = it,
								ifElseEnd = it + op->param() - 1;
						JsmOpcode *lastOpOfBlock = *(ifElseEnd - 1);

						// Else or While
						if (lastOpOfBlock->key() == JsmOpcode::JMP) {
							if (lastOpOfBlock->param() > 0
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
								// Check label for the skipped JMP
								addLabelIfExists(toAppend->block(), ifElseEnd - 1 - constBegin, labels, collectPointers);

								ifElseEnd += lastOpOfBlock->param() - 1;
							} else if (lastOpOfBlock->param() < 0
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
								// Check label for the skipped JMP
								addLabelIfExists(toAppend->block(), ifElseEnd - 1 - constBegin, labels, collectPointers);
							}
						}
						if (!toAppend) {
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
					} /* else if (op->param() < 0 && it - condition->opcodeCount() + op->param() >= constBegin) {
						QList<JsmOpcode *>::const_iterator repeatUntilPos = it - condition->opcodeCount() + op->param(),
								repeatUntilEnd = it - condition->opcodeCount() - 1;
						toAppend = new JsmControlRepeatUntil(
											condition,
											// Block Repeat
											program(constBegin, repeatUntilPos,
													repeatUntilEnd, labels,
													collectPointers,
													usedLabels));
					} */
					if (!toAppend) { // JPF
						int lbl = labels.indexOf(pos + op->param());
						if (lbl == -1) {
							qDebug() << labels << op->toString();
							qFatal("%s", qUtf8Printable(QString("JsmScripts::program 1 label for pos %1 + %2 not found").arg(pos).arg(op->param())));
						}
						JsmOpcodeGoto *jmp = new JsmOpcodeGoto(*op, lbl);
						jmp->setKey(JsmOpcode::JMP);
						collectPointers.insert(jmp);
						toAppend = new JsmControlIfElse(condition, JsmProgram(),
														JsmProgram()
														<< JsmInstruction(jmp, pos));
						usedLabels.insert(lbl);
					}
					ret.append(JsmInstruction(toAppend, pos));
					collectPointers.insert(toAppend);
					collectPointers.insert(op);
				} else { // JMP/GJMP
					int lbl = labels.indexOf(pos + op->param());
					if (lbl == -1) {
						ret.append(JsmInstruction(op, pos));
					} else {
						// TODO: forever loop
						JsmOpcode *toAppendOp = new JsmOpcodeGoto(*op, lbl);
						ret.append(JsmInstruction(toAppendOp, pos));
						collectPointers.insert(toAppendOp);
						collectPointers.insert(op);
						usedLabels.insert(lbl);
					}
				}
				instructionAppended = true;
			} else if ((op->key() == JsmOpcode::POPI_L
						|| op->key() == JsmOpcode::POPM_B
						|| op->key() == JsmOpcode::POPM_W
						|| op->key() == JsmOpcode::POPM_L) && !stack.isEmpty()) {
				JsmApplication *application = new JsmApplicationAssignment(
													stack.pop(), op);
				ret.append(JsmInstruction(application, pos));
				instructionAppended = true;
				collectPointers.insert(application);
				collectPointers.insert(op);
			} else if (stack.size() >= 2
						&& op->key() >= JsmOpcode::REQ
						&& op->key() <= JsmOpcode::REQEW) {
				JsmApplication *exec = new JsmApplicationExec(
											stack.pop(),
											stack.pop(),
											op);
				ret.append(JsmInstruction(exec, pos));
				instructionAppended = true;
				collectPointers.insert(exec);
				collectPointers.insert(op);
			}

			if (!instructionAppended) {
				if (!stack.isEmpty()) {
					QStack<JsmExpression *> invertedStack;
					while (!stack.isEmpty()) {
						// FIXME: only pop what will be used by the game for this application
						invertedStack.push(stack.pop());
					}
					JsmApplication *application = new JsmApplication(invertedStack, op);
					ret.append(JsmInstruction(application, pos));
					collectPointers.insert(application);
				} else {
					ret.append(JsmInstruction(op, pos));
				}
				collectPointers.insert(op);
			}
		}
	}

	if (!stack.isEmpty()) {
		QStack<JsmExpression *> invertedStack;
		while (!stack.isEmpty()) {
			// FIXME: only pop what will be used by the game for this application
			invertedStack.push(stack.pop());
		}
		JsmApplication *application = new JsmApplication(invertedStack, nullptr);
		ret.append(JsmInstruction(application, end - constBegin));
		collectPointers.insert(application);
	}

	return ret;
}

JsmProgram &JsmScripts::program2ndPass(JsmProgram &program,
                                       QSet<void *> &collectPointers,
                                       const QSet<int> &usedLabels)
{
	QMutableListIterator<JsmInstruction> it(program);
	while (it.hasNext()) {
		const JsmInstruction &instr = it.next();

		if (instr.type() == JsmInstruction::Control) {
			JsmControl *control = instr.control();
			program2ndPass(control->block(), collectPointers, usedLabels);
			if (control->type() == JsmControl::IfElse) {
				JsmControlIfElse *ifElse =
				        static_cast<JsmControlIfElse *>(control);
				program2ndPass(ifElse->blockElse(), collectPointers,
				               usedLabels);
				if (ifElse->block().isEmpty()) {
					if (ifElse->blockElse().isEmpty()) { // Useless if/Else
						
					} else { // Not
						JsmExpression *cond = ifElse->condition();
						if (cond->type() != JsmExpression::Binary
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
				} else if (ifElse->block().size() == 1
				          && ifElse->blockElse().isEmpty()) { // If block only
					const JsmInstruction &subInstr = ifElse->block().first();
					if (subInstr.type() == JsmInstruction::Control) {
						JsmControl *subControl = subInstr.control();
						// Contains only one if block
						if (subControl->type() == JsmControl::IfElse) {
							JsmControlIfElse *subIfElse =
							        static_cast<JsmControlIfElse *>(subControl);
							// If block without else
							if (subIfElse->blockElse().isEmpty()) { // And
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
		} else if (instr.type() == JsmInstruction::Opcode) {
			// Removing unused labels
			JsmOpcode *opcode = instr.opcode();
			if (opcode->isLabel() && !usedLabels.contains(opcode->param())) {
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
	if (opcodes.isEmpty()) {
		return JsmProgram();
	}
	JsmOpcode *firstOp = opcodes.first();
	QList<int> labels = searchJumps(opcodes);
	QList<JsmOpcode *>::const_iterator begin = opcodes.constBegin();
	// Ignore LBL (automatically generated)
	if (firstOp->key() == JsmOpcode::LBL) {
		begin += 1;
		delete firstOp;
	}
	QSet<int> usedLabels; // Filled by program()
	JsmProgram p = program(opcodes.constBegin(), begin, opcodes.constEnd(),
	                       labels, collectPointers, usedLabels);
	// Doing twice because "if !" can be created in the first 2nd pass and "and" conditions on the second 2nd pass 
	return program2ndPass(program2ndPass(p, collectPointers, usedLabels), collectPointers, usedLabels);
}

unsigned int JsmScripts::key(int groupID, int methodID, int opcodeID) const
{
	return key(absoluteOpcodeID(groupID, methodID, opcodeID));
}

int JsmScripts::param(int groupID, int methodID, int opcodeID) const
{
	return param(absoluteOpcodeID(groupID, methodID, opcodeID));
}

void JsmScripts::setParam(int groupID, int methodID, int opcodeID, int param)
{
	setParam(absoluteOpcodeID(groupID, methodID, opcodeID), param);
}

void JsmScripts::setParam(int absoluteOpcodeID, int param)
{
	JsmOpcode op = opcode(absoluteOpcodeID);
	op.setParam(param);
	setOpcode(absoluteOpcodeID, op);
}

JsmOpcode JsmScripts::opcode(int groupID, int methodID, int opcodeID) const
{
	return opcode(absoluteOpcodeID(groupID, methodID, opcodeID));
}

void JsmScripts::setOpcode(int groupID, int methodID, int opcodeID, const JsmOpcode &value)
{
	setOpcode(absoluteOpcodeID(groupID, methodID, opcodeID), value);
}

void JsmScripts::insertScript(int groupID, int methodID, const QString &name)
{
	int absMethodID = absoluteMethodID(groupID, methodID);
	// Minimal script (LBL + RET) All groups have at last one script
	JsmData data = JsmData()
	    .append(JsmOpcode(JsmOpcode::LBL, absMethodID))
	    .append(JsmOpcode(JsmOpcode::RET, 8));
	int opcodeID = opcodeIDStartScript(groupID, methodID);
	qDebug() << "JsmScripts::insertScript" << "groupID" << groupID << "methodID" << methodID << "nbOpcode" << data.nbOpcode() << "opcodeID" << opcodeID;
	scriptData.insert(opcodeID, data);
	_methodList.insert(absMethodID, JsmScript(opcodeID, name));

	propagateScriptCountChange(groupID, absMethodID, +1);
	propagateOpcodeCountChange(absMethodID, data.nbOpcode());
}

void JsmScripts::insertGroup(int groupID, int groupTypeRelativeID)
{
	int absMethodID = 0;

	if (groupID < _groupListOrderedById.size()) {
		absMethodID = firstMethodID(groupID);
	} else {
		const JsmGroup &lastGroup = _groupListOrderedById.last();
		absMethodID = lastGroup.label() + lastGroup.scriptCount();
	}

	qDebug() << "JsmScripts::insertGroup" << "groupID" << groupID << "absMethodID" << absMethodID;

	_groupListOrderedById.insert(groupID, JsmGroup(absMethodID, 0, JsmGroup::No, groupTypeRelativeID));
	insertScript(groupID, 0);
	propagateGroupCountChange(groupID, +1);
}

void JsmScripts::removeScript(int groupID, int methodID)
{
	int nbOpcode, opcodeID = opcodeIDStartScript(groupID, methodID, &nbOpcode),
		absMethodID = absoluteMethodID(groupID, methodID);
	scriptData.remove(opcodeID, nbOpcode);
	_methodList.removeAt(absMethodID);

	propagateScriptCountChange(groupID, absMethodID, -1);
	propagateOpcodeCountChange(absMethodID, -nbOpcode);
}

void JsmScripts::removeGroup(int groupID)
{
	const JsmGroup &grp = _groupListOrderedById.at(groupID);

	for (int methodID = grp.scriptCount() - 1; methodID >= 0; --methodID) {
		removeScript(groupID, methodID);
	}

	_groupListOrderedById.removeAt(groupID);
	propagateGroupCountChange(groupID, -1);
}

void JsmScripts::replaceScript(int groupID, int methodID, const JsmData &data)
{
	// Add LBL automatically
	JsmData dataCopy = data.copy();
	JsmOpcode lbl(JsmOpcode::LBL, absoluteMethodID(groupID, methodID));
	if (dataCopy.nbOpcode() > 0 && dataCopy.opcode(0).key() == JsmOpcode::LBL) {
		dataCopy.setOpcode(0, lbl);
	} else {
		dataCopy.prepend(lbl);
	}
	int nbOpcode, opcodeID = opcodeIDStartScript(groupID, methodID, &nbOpcode);
	scriptData.replace(opcodeID, nbOpcode, dataCopy);

	propagateOpcodeCountChange(absoluteMethodID(groupID, methodID), dataCopy.nbOpcode() - nbOpcode);
}

void JsmScripts::propagateGroupCountChange(int groupID, int groupCountChange)
{
	qDebug() << "JsmScripts::propagateGroupCountChange" << "groupID" << groupID << "groupCountChange" << groupCountChange;
	if (groupCountChange == 0) {
		return;
	}

	// Update REQ groups
	for (int opcodeID = 0; opcodeID < scriptData.nbOpcode(); ++opcodeID) {
		JsmOpcode opcode = scriptData.opcode(opcodeID);
		int group = 0;

		switch (opcode.key()) {
			case JsmOpcode::REQ:
			case JsmOpcode::REQSW:
			case JsmOpcode::REQEW:
				group = opcode.param();
				opcode.setParam(group + groupCountChange);
				scriptData.setOpcode(opcodeID, opcode);
				break;
			default:
				break;
		}
	}
}

void JsmScripts::propagateScriptCountChange(int groupID, int absMethodID, int scriptCountChange)
{
	qDebug() << "JsmScripts::propagateScriptCountChange" << "groupID" << groupID << "absMethodID" << absMethodID << "scriptCountChange" << scriptCountChange;
	if (scriptCountChange == 0) {
		return;
	}

	_groupListOrderedById[groupID].incScriptCount(scriptCountChange);

	// Update Groups label
	int i = 0;
	for (const JsmGroup &group: _groupListOrderedById) {
		if (groupID != i && group.label() >= absMethodID) {
			_groupListOrderedById[i].incLabel(scriptCountChange);
		}
		i += 1;
	}

	// Update LBL parameter
	int label = 0;
	for (const JsmScript &script: _methodList) {
		JsmOpcode opcode = scriptData.opcode(script.opcodeIDStart());
		if (opcode.key() == JsmOpcode::LBL && opcode.param() != label) {
			opcode.setParam(label);
			scriptData.setOpcode(script.opcodeIDStart(), opcode);
		}
		label += 1;
	}

	// Update REQ labels
	for (int opcodeID = 0; opcodeID < scriptData.nbOpcode(); ++opcodeID) {
		JsmOpcode opcode = scriptData.opcode(opcodeID);
		//int group = 0;

		switch (opcode.key()) {
			case JsmOpcode::REQ:
			case JsmOpcode::REQSW:
			case JsmOpcode::REQEW:
				//group = opcode.param();
				if (opcodeID > 0) {
					JsmOpcode prevOpcode = scriptData.opcode(opcodeID - 1);
					if (prevOpcode.key() == JsmOpcode::PSHN_L) {
						int script = prevOpcode.param();
						if (script >= absMethodID) {
							prevOpcode.setParam(script + scriptCountChange);
							scriptData.setOpcode(opcodeID - 1, prevOpcode);
						}
					}
				}
				break;
			default:
				break;
		}
	}
}

void JsmScripts::propagateOpcodeCountChange(int absMethodID, int opcodeCountChange)
{
	//qDebug() << "JsmScripts::propagateOpcodeCountChange" << "absMethodID" << absMethodID << "opcodeCountChange" << opcodeCountChange;
	if (opcodeCountChange == 0) {
		return;
	}

	for (int i = absMethodID + 1; i < nbScript(); ++i) {
		_methodList[i].incOpcodeIDStart(opcodeCountChange);
	}
}

void JsmScripts::setDecompiledScript(int groupID, int methodID, const QString &text, bool moreDecompiled)
{
	_methodList[absoluteMethodID(groupID, methodID)].setDecompiledScript(text, moreDecompiled);
}
