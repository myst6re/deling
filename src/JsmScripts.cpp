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

void JsmMethod::setDecompiledScript(const QString &text, bool moreDecompiled)
{
	if (moreDecompiled) {
		_decompiledScriptMore = text;
	} else {
		_decompiledScript = text;
	}
}

const QString &JsmMethod::decompiledScript(bool moreDecompiled) const
{
	return moreDecompiled ? _decompiledScriptMore : _decompiledScript;
}

void JsmMethod::setScriptData(const JsmData &scriptData)
{
	if (scriptData.nbOpcode() > 0 && scriptData.opcode(0).key() == JsmOpcode::LBL) {
		_scriptData = scriptData.mid(1); // remove LBL
	} else {
		_scriptData = scriptData;
	}
}

JsmGroup::JsmGroup(quint16 absMethodId, const QList<JsmMethod> &methodList, Type groupType, const QString &name)
	: _type(groupType), _character(-1), _modelID(-1), _name(name), _methodList(methodList),
	  _absMethodId(absMethodId)
{
}

QString JsmGroup::absoluteMethodName(int absMethodID) const
{
	if (absMethodID < _absMethodId || absMethodID >= _absMethodId + methodCount()) {
		return QString();
	}

	return method(absMethodID - _absMethodId).name();
}

int JsmGroup::findAbsoluteMethodIDByName(const QString &methodName) const
{
	int absMethodID = _absMethodId;

	for (const JsmMethod &method: _methodList) {
		if (method.name() == methodName) {
			return absMethodID;
		}
		absMethodID += 1;
	}

	return -1;
}

JsmGroup::Type JsmGroup::toNativeType(Type type)
{
	switch (type) {
		case JsmGroup::No:
		case JsmGroup::Main:
		case JsmGroup::Model:
			return JsmGroup::No;
		case JsmGroup::Location:
			return JsmGroup::Location;
		case JsmGroup::Door:
			return JsmGroup::Door;
		case JsmGroup::Background:
			return JsmGroup::Background;
	}

	return JsmGroup::No;
}

JsmGroup::Type JsmGroup::nativeType() const
{
	return toNativeType(_type);
}

JsmScripts::JsmScripts(const QList<JsmGroup> &groupListOrderedById) :
	_groupListOrderedById(groupListOrderedById)
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

QList<int> JsmScripts::groupIDsSortedByAbsMethodId() const
{
	QMultiMap<int, int> groupIDByLabel;
	int groupID = 0;
	for (const JsmGroup &group: _groupListOrderedById) {
		groupIDByLabel.insert(group.absMethodId(), groupID);
		groupID += 1;
	}

	return groupIDByLabel.values();
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

int JsmScripts::calcGroupTypeRelativeId(int groupID) const
{
	int relativeId = 0, i = 0;
	JsmGroup::Type type = _groupListOrderedById.at(groupID).type();

	for (const JsmGroup &group: _groupListOrderedById) {
		if (i == groupID) {
			break;
		}

		if (group.type() == type) {
			relativeId += 1;
		}

		i += 1;
	}

	return relativeId;
}

bool JsmScripts::groupBounds(JsmGroup::Type groupType, int &firstGroupID, int &lastGroupID) const
{
	JsmGroup::Type nativeGroupType = JsmGroup::toNativeType(groupType);
	firstGroupID = -1;
	lastGroupID = -1;

	for (int groupID = 0; groupID < _groupListOrderedById.size(); ++groupID) {
		const JsmGroup &jsmGroup = _groupListOrderedById.at(groupID);

		if (jsmGroup.nativeType() == nativeGroupType) {
			if (firstGroupID < 0) {
				firstGroupID = groupID;
			}
			lastGroupID = groupID;
		} else if (firstGroupID >= 0) {
			break;
		}
	}

	return firstGroupID != -1 && lastGroupID != -1;
}

int JsmScripts::insertGroup(int indicativeGroupID, JsmGroup::Type groupType, const QString &name)
{
	if (!canSetGroupName(name)) {
		return -1;
	}

	const JsmGroup &lastGroup = _groupListOrderedById.last();
	int absMethodID = lastGroup.absMethodId() + lastGroup.methodCount();
	int firstGroupID = -1, lastGroupID = -1;
	bool ok = groupBounds(groupType, firstGroupID, lastGroupID);

	// Force groupID inside range
	int groupID = indicativeGroupID;

	if (ok) {
		if (indicativeGroupID < firstGroupID) {
			groupID = firstGroupID;
		} else if (indicativeGroupID > lastGroupID) {
			groupID = lastGroupID + 1;
		}
	}

	propagateGroupCountChange(groupID, +1);
	_groupListOrderedById.insert(groupID, JsmGroup(absMethodID, QList<JsmMethod>(), groupType, name));
	insertMethod(groupID, 0, "init");

	return groupID;
}

int JsmScripts::insertGroup(int indicativeGroupID, const JsmGroup &jsmGroup)
{
	int groupID = insertGroup(indicativeGroupID, jsmGroup.type(), jsmGroup.name());
	if (groupID < 0) {
		return -1;
	}

	JsmGroup &newGroup = group(groupID);

	newGroup.setCharacter(jsmGroup.character());
	newGroup.setModelId(jsmGroup.modelId());

	int methodCount = jsmGroup.methodCount();
	if (methodCount > 0) {
		newGroup.method(0).setScriptData(jsmGroup.method(0).scriptData());

		for (int methodID = 1; methodID < methodCount; ++methodID) {
			insertMethod(groupID, methodID, jsmGroup.method(methodID));
		}
	}

	return groupID;
}

void JsmScripts::removeGroup(int groupID)
{
	const JsmGroup &grp = _groupListOrderedById.at(groupID);

	for (int methodID = grp.methodCount() - 1; methodID >= 0; --methodID) {
		removeMethod(groupID, methodID);
	}

	_groupListOrderedById.removeAt(groupID);
	propagateGroupCountChange(groupID, -1);
}

bool JsmScripts::moveGroup(int groupID, bool down)
{
	if (groupID < 0 || groupID >= _groupListOrderedById.size()) {
		return false;
	}

	int firstGroupID = -1, lastGroupID = -1;
	bool ok = groupBounds(_groupListOrderedById.at(groupID).type(), firstGroupID, lastGroupID);

	if (down) {
		if (groupID + 1 >= _groupListOrderedById.size() || (ok && groupID >= lastGroupID)) {
			return false;
		}
		_groupListOrderedById.swapItemsAt(groupID, groupID + 1);
	} else {
		if (groupID - 1 < 0 || (ok && groupID <= firstGroupID)) {
			return false;
		}
		_groupListOrderedById.swapItemsAt(groupID, groupID - 1);
	}

	return true;
}

bool JsmScripts::canSetGroupName(const QString &name) const
{
	if (name.isEmpty()) {
		return false;
	}

	QStringList existingNames;
	for (const JsmGroup &jsmGroup: _groupListOrderedById) {
		existingNames.append(jsmGroup.name());
	}

	return !existingNames.contains(name);
}

bool JsmScripts::setGroupName(int groupID, const QString &name)
{
	JsmGroup &jsmGroup = group(groupID);
	QString oldGroupName = jsmGroup.name();

	if (oldGroupName == name) {
		return true;
	}

	if (!canSetGroupName(name)) {
		return false;
	}
	
	jsmGroup.setName(name);

	if (oldGroupName.isEmpty()) {
		return true;
	}

	QRegularExpression search(QString("\\b%1\\.([A-Za-z0-9_]+)\\b").arg(QRegularExpression::escape(oldGroupName)));

	for (JsmGroup &jsmGroup: _groupListOrderedById) {
		for (int methodID = 0; methodID < jsmGroup.methodCount(); ++methodID) {
			JsmMethod &jsmMethod = jsmGroup.method(methodID);

			QString decompiledScript = jsmMethod.decompiledScript(true);
			if (!decompiledScript.isEmpty()) {
				QRegularExpressionMatchIterator match = search.globalMatch(decompiledScript);
				QStringList methodNames;
				while (match.hasNext()) {
					match.next();

					QString methodName = match.next().captured(1);
					if (!methodNames.contains(methodName)) {
						methodNames.append(methodName);
					}
				}

				for (const QString &methodName: methodNames) {
					QRegularExpression methodSearch(QString("\\b%1\\.%2\\b").arg(QRegularExpression::escape(oldGroupName), QRegularExpression::escape(methodName)));
					decompiledScript.replace(methodSearch, QString("%1.%2").arg(name, methodName));
				}

				if (!methodNames.isEmpty()) {
					jsmMethod.setDecompiledScript(decompiledScript, true);
				}
			}
		}
	}

	return true;
}

int JsmScripts::countMethods() const
{
	int ret = 0;

	for (const JsmGroup &group: _groupListOrderedById) {
		ret += group.methodCount();
	}

	return ret;
}

QList<JsmMethod> JsmScripts::methods() const
{
	QList<int> groupIDs = groupIDsSortedByAbsMethodId();
	QList<JsmMethod> methods;

	for (int groupID: groupIDs) {
		methods.append(_groupListOrderedById.at(groupID).methods());
	}

	return methods;
}

QList<int> JsmScripts::searchJumps(const QList<JsmOpcode *> &opcodes)
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
	QList<JsmOpcode *> opcodes = group(groupID).method(methodID).scriptData().opcodesp();

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
	QList<int> labels = searchJumps(opcodes);
	QList<JsmOpcode *>::const_iterator begin = opcodes.constBegin();
	QSet<int> usedLabels; // Filled by program()
	JsmProgram p = program(opcodes.constBegin(), begin, opcodes.constEnd(),
	                       labels, collectPointers, usedLabels);
	// Doing twice because "if !" can be created in the first 2nd pass and "and" conditions on the second 2nd pass 
	return program2ndPass(program2ndPass(p, collectPointers, usedLabels), collectPointers, usedLabels);
}

bool JsmScripts::insertMethod(int groupID, int methodID, const QString &name)
{
	if (!canSetMethodName(groupID, name)) {
		return false;
	}

	JsmGroup &jsmGroup = group(groupID);
	int absMethodID = jsmGroup.absMethodId() + methodID;
	// Minimal script (RET) All groups have at last one script
	JsmData data = JsmData()
	    .append(JsmOpcode(JsmOpcode::RET, 8));
	jsmGroup.insertMethod(methodID, JsmMethod(data, name));
	
	propagateMethodCountChange(groupID, absMethodID, +1);

	return true;
}

bool JsmScripts::insertMethod(int groupID, int methodID, const JsmMethod &jsmMethod)
{
	if (!insertMethod(groupID, methodID, jsmMethod.name())) {
		return false;
	}

	group(groupID).method(methodID).setScriptData(jsmMethod.scriptData());

	return true;
}

void JsmScripts::removeMethod(int groupID, int methodID)
{
	JsmGroup &jsmGroup = group(groupID);
	jsmGroup.removeMethod(methodID);
	int absMethodID = jsmGroup.absMethodId() + methodID;

	propagateMethodCountChange(groupID, absMethodID, -1);
}

bool JsmScripts::canSetMethodName(int groupID, const QString &name)
{
	const JsmGroup &jsmGroup = group(groupID);

	// First method is immutable
	for (int methodID = 1; methodID < jsmGroup.methodCount(); ++methodID) {
		const JsmMethod &jsmMethod = jsmGroup.method(methodID);
		if (jsmMethod.name() == name) {
			return false;
		}
	}

	return true;
}

bool JsmScripts::setMethodName(int groupID, int methodID, const QString &name)
{
	// First method is immutable
	if (methodID == 0) {
		return false;
	}

	JsmGroup &jsmGroup = group(groupID);
	JsmMethod &jsmMethod = jsmGroup.method(methodID);
	const QString &oldMethodName = jsmMethod.name();

	if (oldMethodName == name) {
		return true;
	}

	if (!canSetMethodName(groupID, name)) {
		return false;
	}

	jsmMethod.setName(name);

	if (oldMethodName.isEmpty()) {
		return true;
	}

	QRegularExpression search(QString("\\b%1\\.%2\\b")
		.arg(QRegularExpression::escape(jsmGroup.name()), QRegularExpression::escape(oldMethodName)));

	for (JsmGroup &jsmGroup: _groupListOrderedById) {
		for (int methodID = 0; methodID < jsmGroup.methodCount(); ++methodID) {
			JsmMethod &jsmMethod = jsmGroup.method(methodID);

			QString decompiledScript = jsmMethod.decompiledScript(true);
			if (!decompiledScript.isEmpty()) {
				decompiledScript.replace(search, QString("%1.%2").arg(jsmGroup.name(), name));
				jsmMethod.setDecompiledScript(decompiledScript, true);
			}
		}
	}

	return true;
}

void JsmScripts::propagateGroupCountChange(int groupID, int groupCountChange)
{
	if (groupCountChange == 0) {
		return;
	}

	// Update REQ group IDs
	for (JsmGroup &jsmGroup: _groupListOrderedById) {
		for (int methodID = 0; methodID < jsmGroup.methodCount(); ++methodID) {
			JsmMethod &jsmMethod = jsmGroup.method(methodID);
			JsmData &jsmData = jsmMethod.scriptData();
			int nbOpcodes = jsmData.nbOpcode();
			for (int opcodeID = 0; opcodeID < nbOpcodes; ++opcodeID) {
				JsmOpcode opcode = jsmData.opcode(opcodeID);
				int group = 0;

				switch (opcode.key()) {
					case JsmOpcode::REQ:
					case JsmOpcode::REQSW:
					case JsmOpcode::REQEW:
						group = opcode.param();
						if (group >= groupID) {
							opcode.setParam(group + groupCountChange);
							jsmData.setOpcode(opcodeID, opcode);
						}
						break;
					default:
						break;
				}
			}
		}
	}
}

void JsmScripts::propagateMethodCountChange(int groupID, int absMethodID, int methodCountChange)
{
	if (methodCountChange == 0) {
		return;
	}

	// Update Groups label
	int i = 0;
	for (const JsmGroup &group: _groupListOrderedById) {
		if (groupID != i && group.absMethodId() >= absMethodID) {
			_groupListOrderedById[i].incAbsMethodId(methodCountChange);
		}
		i += 1;
	}

	// Update REQ absolute method IDs
	for (JsmGroup &jsmGroup: _groupListOrderedById) {
		for (int methodID = 0; methodID < jsmGroup.methodCount(); ++methodID) {
			JsmMethod &jsmMethod = jsmGroup.method(methodID);
			JsmData &jsmData = jsmMethod.scriptData();
			int nbOpcodes = jsmData.nbOpcode();
			for (int opcodeID = 0; opcodeID < nbOpcodes; ++opcodeID) {
				JsmOpcode opcode = jsmData.opcode(opcodeID);

				switch (opcode.key()) {
					case JsmOpcode::REQ:
					case JsmOpcode::REQSW:
					case JsmOpcode::REQEW:
						if (opcodeID > 0) {
							JsmOpcode prevOpcode = jsmData.opcode(opcodeID - 1);
							if (prevOpcode.key() == JsmOpcode::PSHN_L) {
								int method = prevOpcode.param();
								if (method >= absMethodID) {
									prevOpcode.setParam(method + methodCountChange);
									jsmData.setOpcode(opcodeID - 1, prevOpcode);
								}
							}
						}
						break;
					default:
						break;
				}
			}
		}
	}
}
