#include "JsmExpression.h"
#include "Field.h"

QString JsmInstruction::toString(const Field *field, int indent) const
{
	switch(type()) {
	case Opcode:
		return QString("%1%2")
		        .arg(QString(indent, QChar('\t')),
		             instruction().opcode->toString());
	case Expression:
		return QString("%1%2")
		        .arg(QString(indent, QChar('\t')),
		             instruction().expression->toString(field));
	case Control:
		return instruction().control->toString(field, indent);
	case Application:
		return QString("%1%2")
		        .arg(QString(indent, QChar('\t')),
		             instruction().application->toString(field));
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
		if(op->param() == JsmExpressionUnary::Min
		        || op->param() == JsmExpressionUnary::Not) {
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

QString JsmExpressionVal::toString(const Field *field) const
{
	Q_UNUSED(field)
	return QString::number(_val);
}

int JsmExpressionVal::eval(bool *ok) const
{
	if(ok) {
		*ok = true;
	}
	return _val;
}

QString JsmExpressionVarUByte::toString(const Field *field) const
{
	Q_UNUSED(field)
	return QString("var_%1_ubyte").arg(_var);
}

QString JsmExpressionVarUWord::toString(const Field *field) const
{
	Q_UNUSED(field)
	return QString("var_%1_uword").arg(_var);
}

QString JsmExpressionVarULong::toString(const Field *field) const
{
	Q_UNUSED(field)
	return QString("var_%1_ulong").arg(_var);
}

QString JsmExpressionVarSByte::toString(const Field *field) const
{
	Q_UNUSED(field)
	return QString("var_%1_sbyte").arg(_var);
}

QString JsmExpressionVarSWord::toString(const Field *field) const
{
	Q_UNUSED(field)
	return QString("var_%1_sword").arg(_var);
}

QString JsmExpressionVarSLong::toString(const Field *field) const
{
	Q_UNUSED(field)
	return QString("var_%1_slong").arg(_var);
}

QString JsmExpressionChar::toString(const Field *field) const
{
	Q_UNUSED(field)
	return QString("char_%1").arg(_char);
}

QString JsmExpressionTemp::toString(const Field *field) const
{
	Q_UNUSED(field)
	return QString("temp_%1").arg(_temp);
}

QString JsmExpressionUnary::toString(const Field *field) const
{
	return QString("%1%2").arg(operationToString(),
	                           _first->toString(field));
}

int JsmExpressionUnary::opcodeCount() const
{
	return 1 + _first->opcodeCount();
}

int JsmExpressionUnary::eval(bool *ok) const
{
	int v = _first->eval(ok);
	if(ok && *ok) {
		switch (_op) {
		case Min:
			return -v;
		case Not:
			return ~v;
		}
	}
	return JsmExpression::eval(ok);
}

QString JsmExpressionUnary::operationToString(Operation op)
{
	switch (op) {
	case Min:
		return "+";
	case Not:
		return "~";
	}
	return QString();
}

QString JsmExpressionBinary::toString(const Field *field) const
{
	return QString("(%1 %2 %3)")
	        .arg(_first->toString(field),
	             operationToString(),
	             _second->toString(field));
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

QString JsmControlIfElse::toString(const Field *field, int indent,
                                   bool elseIf) const
{
	if(_block.isEmpty() && _blockElse.isEmpty()) {
		return QString();
	}
	// TODO: if block is empty, then blockElse will be executed unless condition

	QStringList lines(indentString("if %1 begin", elseIf ? 0 : indent)
	                  .arg(_condition->toString(field)));

	foreach(const JsmInstruction &instr, _block) {
		lines.append(instr.toString(field, indent + 1));
	}

	if(!_blockElse.isEmpty()) {
		if(_blockElse.size() == 1) {
			const JsmInstruction &instr = _blockElse.first();
			if (instr.type() == JsmInstruction::Control
			        && instr.instruction().control->type() ==
			        JsmControl::IfElse) {
				JsmControlIfElse *ifElse =
				        static_cast<JsmControlIfElse *>(instr.instruction()
				                                        .control);
				lines.append(indentString("else %1", indent)
				             .arg(ifElse->toString(field, indent, true)));
				goto JsmControlIfElse_toString_end;
			}
		}

		lines.append(indentString("else", indent));
		foreach(const JsmInstruction &instr, _blockElse) {
			lines.append(instr.toString(field, indent + 1));
		}
	}

JsmControlIfElse_toString_end:
	if(!elseIf) {
		lines.append(indentString("end", indent));
	}

	return lines.join("\n");
}

QString JsmControlWhile::toString(const Field *field, int indent) const
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
		            .arg(_condition->toString(field));
	}
	QStringList lines(firstLine);

	foreach(const JsmInstruction &instr, _block) {
		lines.append(instr.toString(field, indent + 1));
	}

	lines.append(indentString("end", indent));

	return lines.join("\n");
}

QString JsmControlRepeatUntil::toString(const Field *field, int indent) const
{
	QStringList lines(indentString("repeat", indent));

	foreach(const JsmInstruction &instr, _block) {
		lines.append(instr.toString(field, indent + 1));
	}

	lines.append(indentString("until %1 end", indent)
	             .arg(_condition->toString(field)));

	return lines.join("\n");
}

QStringList JsmApplication::stackToStringList(const Field *field) const
{
	QStringList params;

	QStack<JsmExpression *> stackCpy = _stack;
	while(!stackCpy.isEmpty()) {
		params.append(stackCpy.pop()->toString(field));
	}

	return params;
}

QString JsmApplication::paramsToString(const Field *field) const
{
	QStringList params;

	if(_opcode->hasParam()) {
		params.append(_opcode->paramStr());
	}

	params.append(stackToStringList(field));

	return params.join(", ");
}

QString JsmApplication::toString(const Field *field) const
{
	return QString("%1(%2)").arg(_opcode->name().toLower(),
	                             paramsToString(field));
}

JsmExpression *JsmApplicationAssignment::opcodeExpression() const
{
	switch(_opcode->key()) {
	case JsmOpcode::POPI_L:
		return new JsmExpressionTemp(_opcode->param());
	case JsmOpcode::POPM_B:
		return new JsmExpressionVarUByte(_opcode->param());
	case JsmOpcode::POPM_W:
		return new JsmExpressionVarUWord(_opcode->param());
	case JsmOpcode::POPM_L:
		return new JsmExpressionVarULong(_opcode->param());
	default:
		break;
	}
	return 0;
}

QString JsmApplicationAssignment::toString(const Field *field) const
{
	QString ret;
	JsmExpression *opExpr = opcodeExpression(), // New instance
	        *expr = _stack.top();
	if(expr->type() == JsmExpression::Binary) {
		JsmExpressionBinary *binaryExpr
		        = static_cast<JsmExpressionBinary *>(expr);
		if(opExpr->toString(field) ==
		        binaryExpr->leftOperand()->toString(field)) {
			ret = QString("%1 %2= %3")
			      .arg(opExpr->toString(field),
			           binaryExpr->operationToString(),
			           binaryExpr->rightOperand()->toString(field));
		}
	}
	if(ret.isEmpty()) {
		ret = QString("%1 = %2").arg(opExpr->toString(field),
		                             expr->toString(field));
	}
	delete expr;
	return ret;
}

QString JsmApplicationExec::execType() const
{
	switch(_opcode->key()) {
	case JsmOpcode::REQ:
		return QString();
	case JsmOpcode::REQSW:
		return "SW";
	case JsmOpcode::REQEW:
		return "EW";
	default:
		break;
	}
	return QString();
}

QString JsmApplicationExec::toString(const Field *field) const
{
	QString groupName, methodName;
	QStack<JsmExpression *> stackCpy = _stack;
	bool ok;
	int groupId = _opcode->param(),
	    methodId = stackCpy.pop()->eval(&ok);

	if(ok && field && groupId >= 0 && methodId >= 0 && field->hasJsmFile()) {
		JsmFile *jsm = field->getJsmFile();
		const JsmScripts &scripts = jsm->getScripts();
		if(groupId < scripts.nbGroup()
		        && methodId < scripts.nbScript()) {
			groupName = scripts.group(groupId).name();
			methodName = scripts.script(methodId).name();
		}
	}

	if(groupName.isEmpty() || methodName.isEmpty()) {
		return JsmApplication::toString(field);
	}

	QStringList params(stackCpy.pop()->toString(field));
	QString eType = execType();
	if(!eType.isEmpty()) {
		params.append(eType);
	}

	return QString("%1.%2(%3)").arg(groupName,
	                                methodName,
	                                params.join(", "));
}
