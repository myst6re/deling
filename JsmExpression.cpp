#include "JsmExpression.h"

QString JsmInstruction::toString(int indent) const
{
	switch(type()) {
	case Opcode:
		return QString("%1%2")
		        .arg(QString(indent, QChar('\t')),
		             instruction().opcode->toString());
	case Expression:
		return QString("%1%2")
		        .arg(QString(indent, QChar('\t')),
		             instruction().expression->toString());
	case Control:
		return instruction().control->toString(indent);
	case Application:
		return QString("%1%2")
		        .arg(QString(indent, QChar('\t')),
		             instruction().application->toString());
	}
	return QString(); // Never happen
}

int JsmExpression::eval(bool *ok) const
{
	if(ok) {
		*ok = false;
	}
	return 0;
}

JsmExpression *JsmExpression::factory(const JsmOpcode *op,
                                      QStack<JsmExpression *> &stack)
{
	JsmExpression *ret = 0;

	switch(op->key()) {
	case JsmOpcode::CAL:
		if(op->param() == JsmExpressionUnary::Min) {
			if(!stack.isEmpty()) {
				ret = new JsmExpressionUnary(
				            JsmExpressionUnary::Operation(op->param()),
				            stack.pop());
			}
		} else if(op->param() <= JsmExpressionBinary::Eor) {
			if(stack.size() >= 2) {
				ret = new JsmExpressionBinary(
				            JsmExpressionBinary::Operation(op->param()),
				            stack.pop(),
				            stack.pop());
			}
		}
		break;
	case JsmOpcode::PSHN_L:
		ret = new JsmExpressionVal(op->param());
		break;
	case JsmOpcode::PSHI_L:
		ret = new JsmExpressionTemp(op->param());
		break;
	case JsmOpcode::PSHM_B:
		ret = new JsmExpressionVarUByte(op->param());
		break;
	case JsmOpcode::PSHM_W:
		ret = new JsmExpressionVarUWord(op->param());
		break;
	case JsmOpcode::PSHM_L:
		ret = new JsmExpressionVarULong(op->param());
		break;
	case JsmOpcode::PSHSM_B:
		ret = new JsmExpressionVarSByte(op->param());
		break;
	case JsmOpcode::PSHSM_W:
		ret = new JsmExpressionVarSWord(op->param());
		break;
	case JsmOpcode::PSHSM_L:
		ret = new JsmExpressionVarSLong(op->param());
		break;
	case JsmOpcode::PSHAC:
		ret = new JsmExpressionChar(op->param());
		break;
	default:
		break;
	}

	if(ret) {
		stack.push(ret);
	}

	return ret;
}

QString JsmExpressionVal::toString() const
{
	return QString::number(_val);
}

int JsmExpressionVal::eval(bool *ok) const
{
	if(ok) {
		*ok = true;
	}
	return _val;
}

QString JsmExpressionVarUByte::toString() const
{
	return QString("var_%1_ubyte").arg(_var);
}

QString JsmExpressionVarUWord::toString() const
{
	return QString("var_%1_uword").arg(_var);
}

QString JsmExpressionVarULong::toString() const
{
	return QString("var_%1_ulong").arg(_var);
}

QString JsmExpressionVarSByte::toString() const
{
	return QString("var_%1_sbyte").arg(_var);
}

QString JsmExpressionVarSWord::toString() const
{
	return QString("var_%1_sword").arg(_var);
}

QString JsmExpressionVarSLong::toString() const
{
	return QString("var_%1_slong").arg(_var);
}

QString JsmExpressionChar::toString() const
{
	return QString("char_%1").arg(_char);
}

QString JsmExpressionTemp::toString() const
{
	return QString("temp_%1").arg(_temp);
}

QString JsmExpressionUnary::toString() const
{
	return QString("-%1").arg(_first->toString());
}

int JsmExpressionUnary::opcodeCount() const
{
	return 1 + _first->opcodeCount();
}

int JsmExpressionUnary::eval(bool *ok) const
{
	int v = _first->eval(ok);
	if(ok && *ok) {
		return -v;
	}
	return JsmExpression::eval(ok);
}

QString JsmExpressionBinary::toString() const
{
	return QString("%1 %2 %3")
	        .arg(_first->toString())
	        .arg(operationToString())
	        .arg(_second->toString());
}

int JsmExpressionBinary::eval(bool *ok) const
{
	int v1 = _first->eval(ok);
	if(ok && *ok) {
		int v2 = _second->eval(ok);
		if(ok && *ok) {
			switch (_op) {
			case Add:
				return v1 + v2;
			case Sub:
				return v1 - v2;
			case Mul:
				return v1 * v2;
			case Mod:
				return v1 % v2;
			case Div:
				return v1 / v2;
			case Eq:
				return v1 == v2;
			case Gt:
				return v1 > v2;
			case Ge:
				return v1 >= v2;
			case Ls:
				return v1 < v2;
			case Le:
				return v1 <= v2;
			case Nt:
				return v1 != v2;
			case And:
				return v1 & v2;
			case Or:
				return v1 | v2;
			case Eor:
				return v1 ^ v2;
			}
		}
	}
	return JsmExpression::eval(ok);
}

QString JsmExpressionBinary::operationToString(Operation op)
{
	switch (op) {
	case Add:
		return "+";
	case Sub:
		return "-";
	case Mul:
		return "*";
	case Mod:
		return "%";
	case Div:
		return "/";
	case Eq:
		return "==";
	case Gt:
		return ">";
	case Ge:
		return ">=";
	case Ls:
		return "<";
	case Le:
		return "<=";
	case Nt:
		return "!=";
	case And:
		return "&";
	case Or:
		return "|";
	case Eor:
		return "^";
	}
	return QString();
}

int JsmExpressionBinary::opcodeCount() const
{
	return 1 + _first->opcodeCount() + _second->opcodeCount();
}

QString &JsmControl::indentString(QString &str, int indent)
{
	return str.prepend(QString(indent, QChar('\t')));
}

QString JsmControl::indentString(const char *str, int indent)
{
	QString s(str);
	return indentString(s, indent);
}

QString JsmControlIfElse::toString(int indent) const
{
	if(_block.isEmpty() && _blockElse.isEmpty()) {
		return QString();
	}
	// TODO: if block is empty, then blockElse will be executed unless condition

	QStringList lines(indentString("if %1 begin", indent)
	                  .arg(_condition->toString()));

	foreach(const JsmInstruction &instr, _block) {
		lines.append(instr.toString(indent + 1));
	}

	if(!_blockElse.isEmpty()) {
		lines.append(indentString("else", indent));
		foreach(const JsmInstruction &instr, _blockElse) {
			lines.append(instr.toString(indent + 1));
		}
	}

	lines.append(indentString("end", indent));

	return lines.join("\n");
}

QString JsmControlWhile::toString(int indent) const
{
	QString firstLine;
	bool ok;
	int value = condition()->eval(&ok);
	if(ok && value == 1) {
		// Forever
		firstLine = indentString("forever", indent);
	} else {
		// While
		firstLine = indentString("while %1 begin", indent)
		            .arg(_condition->toString());
	}
	QStringList lines(firstLine);

	foreach(const JsmInstruction &instr, _block) {
		lines.append(instr.toString(indent + 1));
	}

	lines.append(indentString("end", indent));

	return lines.join("\n");
}

QString JsmControlRepeatUntil::toString(int indent) const
{
	QStringList lines(indentString("repeat", indent));

	foreach(const JsmInstruction &instr, _block) {
		lines.append(instr.toString(indent + 1));
	}

	lines.append(indentString("until %1 end", indent)
	             .arg(_condition->toString()));

	return lines.join("\n");
}

QString JsmApplication::toString() const
{
	QStringList params;

	if(_opcode->hasParam()) {
		params.append(_opcode->paramStr());
	}

	QStack<JsmExpression *> stackCpy = _stack;
	while(!stackCpy.isEmpty()) {
		params.append(stackCpy.pop()->toString());
	}

	return QString("%1(%2)").arg(_opcode->name().toLower(),
	                             params.join(", "));
}

QString JsmApplicationAssignment::opcodeToString() const
{
	switch(_opcode->key()) {
	case JsmOpcode::POPI_L:
		return JsmExpressionTemp(_opcode->param()).toString();
	case JsmOpcode::POPM_B:
		return JsmExpressionVarUByte(_opcode->param()).toString();
	case JsmOpcode::POPM_W:
		return JsmExpressionVarUWord(_opcode->param()).toString();
	case JsmOpcode::POPM_L:
		return JsmExpressionVarULong(_opcode->param()).toString();
	default:
		break;
	}
	return QString();
}

QString JsmApplicationAssignment::toString() const
{
	return QString("%1 = %2").arg(opcodeToString(),
	                               _stack.first()->toString());
}
