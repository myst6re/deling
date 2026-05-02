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
#include "JsmExpression.h"
#include "Field.h"
#include "Config.h"
#include "Data.h"
#include "files/JsmFile.h"

JsmInstruction::JsmInstruction(JsmExpression *expression) :
    _type(Expression), _position(expression->position())
{
	_instr.expression = expression;
}

QString JsmInstruction::toString(const JsmFile *jsm, int indent, int highlightOpcodeID) const
{
	QString mark;

	if (_position == highlightOpcodeID) {
		mark = "[X]";
	}

	switch (type()) {
	case Opcode:
		return QString("%1%2%3%1")
		        .arg(mark)
		        .arg(opcode() && opcode()->isLabel()
		             ? "" : QString(indent, QChar('\t')),
		             opcodeToString(opcode()));
	case Expression:
		return QString("%1%2%3%1")
		        .arg(mark)
		        .arg(QString(indent, QChar('\t')),
		             expression()->toString(jsm, highlightOpcodeID, 10));
	case Control:
		return mark + control()->toString(jsm, indent, highlightOpcodeID);
	case Application:
		return QString("%1%2%3%1")
		        .arg(mark)
		        .arg(QString(indent, QChar('\t')),
		             application()->toString(jsm, highlightOpcodeID));
	}
	return QString(); // Never happen
}

int JsmInstruction::opcodeCount() const
{
	switch (type()) {
	case Opcode:
		return 1;
	case Expression:
		return expression()->opcodeCount();
	case Control:
		return control()->opcodeCount();
	case Application:
		return application()->opcodeCount();
	}
	return 0; // Never happen
}

QString JsmInstruction::opcodeToString(const JsmOpcode *opcode)
{
	if (!opcode) {
		return "nil";
	}
	if (opcode->isLabel()) {
		return QString("label %1").arg(opcode->param());
	} else if (opcode->isGoto()) {
		return QString("goto %1").arg(((JsmOpcodeGoto *)opcode)->label());
	}
	return QString("%1(%2)").arg(opcode->name().toLower(),
	                             opcode->hasParam() ? opcode->paramStr() : "");
}

QStringList JsmProgram::toStringList(const JsmFile *jsm, int indent, int highlightOpcodeID) const
{
	QStringList ret;

	for (const JsmInstruction &instr: *this) {
		QString instrStr = instr.toString(jsm, indent, highlightOpcodeID);
		if (!instrStr.isEmpty()) {
			ret.append(instrStr);
		}
	}

	return ret;
}

int JsmProgram::opcodeCount() const
{
	int ret = 0;

	for (const JsmInstruction &instr: *this) {
		ret += instr.opcodeCount();
	}

	return ret;
}

QString JsmExpression::getToStringMark(int highlightOpcodeID) const
{
	QString mark;

	if (_position == highlightOpcodeID) {
		mark = "[X]";
	}

	return mark;
}

int JsmExpression::eval(bool *ok) const
{
	if (ok) {
		*ok = false;
	}
	return 0;
}

QString JsmExpression::stripParenthesis(const QString &exprStr)
{
	if (exprStr.startsWith('(')
	        && exprStr.endsWith(')')) {
		return exprStr.mid(1, exprStr.size() - 2);
	}
	return exprStr;
}

JsmExpression *JsmExpression::factory(const JsmOpcode *op,
                                      QStack<JsmExpression *> &stack, int pos)
{
	JsmExpression *ret = 0;

	switch (op->key()) {
	case JsmOpcode::CAL:
		if (op->param() == JsmExpressionUnary::Min
		        || op->param() == JsmExpressionUnary::Not) {
			if (!stack.isEmpty()) {
				ret = new JsmExpressionUnary(
				            JsmExpressionUnary::Operation(op->param()),
				            stack.pop());
			}
		} else if (op->param() <= JsmExpressionBinary::Lsh) {
			if (stack.size() >= 2) {
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

	if (ret) {
		ret->setPosition(pos);
		stack.push(ret);
	}

	return ret;
}

QString JsmExpressionVal::toString(const JsmFile *jsm, int highlightOpcodeID, int base) const
{
	QString mark = getToStringMark(highlightOpcodeID);

	Q_UNUSED(jsm)
	switch (base) {
	case 2:
		return mark + QString("b%1").arg(quint32(_val), 0, base) + mark;
	case 16:
		return mark + QString("0x%1").arg(QString::number(quint32(_val), base)
		                           .toUpper()) + mark;
	default:
		return mark + QString::number(_val, base) + mark;
	}
}

int JsmExpressionVal::eval(bool *ok) const
{
	if (ok) {
		*ok = true;
	}
	return _val;
}

QString JsmExpressionVar::varName() const
{
	QString name = Config::value(QString("var%1").arg(_var)).toString();
	if (!name.isEmpty() && _var < 1024) {
		return QString("%1_%2").arg(_var).arg(name
		                                      .replace(QRegularExpression("\\W"), "_")
		                                      .replace(QRegularExpression("_+"), "_"));
	}
	return QString::number(_var);
}

QString JsmExpressionVarUByte::toString(const JsmFile *jsm, int highlightOpcodeID, int base) const
{
	Q_UNUSED(jsm)
	Q_UNUSED(base)
	QString mark = getToStringMark(highlightOpcodeID);
	return mark + QString("%1_ubyte").arg(varName()) + mark;
}

QString JsmExpressionVarUWord::toString(const JsmFile *jsm, int highlightOpcodeID, int base) const
{
	Q_UNUSED(jsm)
	Q_UNUSED(base)
	QString mark = getToStringMark(highlightOpcodeID);
	return mark + QString("%1_uword").arg(varName()) + mark;
}

QString JsmExpressionVarULong::toString(const JsmFile *jsm, int highlightOpcodeID, int base) const
{
	Q_UNUSED(jsm)
	Q_UNUSED(base)
	QString mark = getToStringMark(highlightOpcodeID);
	return mark + QString("%1_ulong").arg(varName()) + mark;
}

QString JsmExpressionVarSByte::toString(const JsmFile *jsm, int highlightOpcodeID, int base) const
{
	Q_UNUSED(jsm)
	Q_UNUSED(base)
	QString mark = getToStringMark(highlightOpcodeID);
	return mark + QString("%1_sbyte").arg(varName()) + mark;
}

QString JsmExpressionVarSWord::toString(const JsmFile *jsm, int highlightOpcodeID, int base) const
{
	Q_UNUSED(jsm)
	Q_UNUSED(base)
	QString mark = getToStringMark(highlightOpcodeID);
	return mark + QString("%1_sword").arg(varName()) + mark;
}

QString JsmExpressionVarSLong::toString(const JsmFile *jsm, int highlightOpcodeID, int base) const
{
	Q_UNUSED(jsm)
	Q_UNUSED(base)
	QString mark = getToStringMark(highlightOpcodeID);
	return mark + QString("%1_slong").arg(varName()) + mark;
}

QString JsmExpressionChar::toString(const JsmFile *jsm, int highlightOpcodeID, int base) const
{
	Q_UNUSED(jsm)
	Q_UNUSED(base)
	QString mark = getToStringMark(highlightOpcodeID);
	return mark + QString("model_%1").arg(_char) + mark;
}

QString JsmExpressionTemp::toString(const JsmFile *jsm, int highlightOpcodeID, int base) const
{
	Q_UNUSED(jsm)
	Q_UNUSED(base)
	QString mark = getToStringMark(highlightOpcodeID);
	return mark + QString("temp_%1").arg(_temp) + mark;
}

QString JsmExpressionUnary::toString(const JsmFile *jsm, int highlightOpcodeID, int base) const
{
	if (_op == Not) {
		base = 16;
	}
	QString mark = getToStringMark(highlightOpcodeID);
	return mark + QString("%1%2").arg(operationToString(),
	                           _first->toString(jsm, highlightOpcodeID, base)) + mark;
}

int JsmExpressionUnary::opcodeCount() const
{
	return 1 + _first->opcodeCount();
}

int JsmExpressionUnary::eval(bool *ok) const
{
	bool ok2;
	int v = _first->eval(&ok2);
	if (ok2) {
		if (ok) {
			*ok = true;
		}
		switch (_op) {
		case Min:
			return -v;
		case Not:
			return ~v;
		case LogNot:
			return int(!v);
		}
	}
	return JsmExpression::eval(ok);
}

QString JsmExpressionUnary::operationToString(Operation op)
{
	switch (op) {
	case Min:
		return "-";
	case Not:
		return "~";
	case LogNot:
		return "!";
	}
	return QString();
}

QString JsmExpressionBinary::toString(const JsmFile *jsm, int highlightOpcodeID, int base) const
{
	if (_op == And
	        || _op == Or
	        || _op == Eor
	        || _op == Lsh
	        || _op == Rsh) {
		base = 16;
	}
	QString mark = getToStringMark(highlightOpcodeID);
	return mark + QString("(%1 %2 %3)")
	        .arg(_first->toString(jsm, highlightOpcodeID, base),
	             operationToString(),
	             _second->toString(jsm, highlightOpcodeID, base)) + mark;
}

int JsmExpressionBinary::eval(bool *ok) const
{
	int v1 = _first->eval(ok);
	if (ok && *ok) {
		int v2 = _second->eval(ok);
		if (ok && *ok) {
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
				return int(v1 == v2);
			case Gt:
				return int(v1 > v2);
			case Ge:
				return int(v1 >= v2);
			case Ls:
				return int(v1 < v2);
			case Le:
				return int(v1 <= v2);
			case Nt:
				return int(v1 != v2);
			case And:
				return v1 & v2;
			case Or:
				return v1 | v2;
			case Eor:
				return v1 ^ v2;
			case Rsh:
				return v1 >> v2;
			case Lsh:
				return v1 << v2;
			case LogAnd:
				return int(v1 && v2);
			case LogOr:
				return int(v1 || v2);
			}
		}
	}
	return JsmExpression::eval(ok);
}

JsmExpressionBinary::Operation JsmExpressionBinary::logicalNot(bool *ok) const
{
	if (ok) {
		*ok = true;
	}
	switch (_op) {
	case Eq:
		return Nt;
	case Gt:
		return Le;
	case Ge:
		return Ls;
	case Ls:
		return Ge;
	case Le:
		return Gt;
	case Nt:
		return Eq;
	case LogAnd:
		return LogOr;
	case LogOr:
		return LogAnd;
	default:
		break;
	}
	if (ok) {
		*ok = false;
	}
	return _op;
}

bool JsmExpressionBinary::logicalNot()
{
	bool ok;
	_op = logicalNot(&ok);
	return ok;
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
	case Rsh:
		return ">>";
	case Lsh:
		return "<<";
	case LogAnd:
		return "and";
	case LogOr:
		return "or";
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

QString JsmControlIfElse::toString(bool elseIf, const JsmFile *jsm, int indent,
                                   int highlightOpcodeID) const
{
	// TODO: if block is empty, then blockElse will be executed unless condition
	QString condStr = JsmExpression::stripParenthesis(
	                      _condition->toString(jsm, highlightOpcodeID, 10));
	QStringList lines(indentString("if %1 begin", elseIf ? 0 : indent)
	                  .arg(condStr));

	if (!_block.isEmpty()) {
		lines.append(_block.toStringList(jsm, indent + 1, highlightOpcodeID));
	}

	if (!_blockElse.isEmpty()) {
		if (_blockElse.size() == 1) {
			const JsmInstruction &instr = _blockElse.first();
			if (instr.type() == JsmInstruction::Control
			        && instr.control()->type() == JsmControl::IfElse) {
				JsmControlIfElse *ifElse =
				        static_cast<JsmControlIfElse *>(instr.control());
				QString elseStr = ifElse->toString(true, jsm, indent, highlightOpcodeID);
				if (elseStr.isEmpty()) {
					lines.append(indentString("else begin", indent));
				} else {
					lines.append(indentString("els%1", indent)
								.arg(elseStr));
				}
				goto JsmControlIfElse_toString_end;
			}
		}

		lines.append(indentString("else begin", indent));
		lines.append(_blockElse.toStringList(jsm, indent + 1, highlightOpcodeID));
	}

JsmControlIfElse_toString_end:
	if (!elseIf) {
		lines.append(indentString("end", indent));
	}

	return lines.join("\n");
}

QString JsmControlWhile::toString(const JsmFile *jsm, int indent, int highlightOpcodeID) const
{
	QString prefix, firstLine;
	bool ok, noBlock = _block.isEmpty();

	if (noBlock) {
		prefix = "wait ";
	}

	int value = condition()->eval(&ok);
	if (ok && value == 1) {
		// Forever
		firstLine = "forever";
	} else {
		// While
		QString condStr = JsmExpression::stripParenthesis(
		                      _condition->toString(jsm, highlightOpcodeID, 10));
		firstLine = QString("while %1").arg(condStr);
		if (!noBlock) {
			firstLine.append(" begin");
		}
	}
	QStringList lines(indentString(prefix.append(firstLine), indent));

	if (!noBlock) {
		lines.append(_block.toStringList(jsm, indent + 1, highlightOpcodeID));
		lines.append(indentString("end", indent));
	}

	return lines.join("\n");
}

QString JsmControlRepeatUntil::toString(const JsmFile *jsm, int indent, int highlightOpcodeID) const
{
	QStringList lines(indentString("repeat", indent));
	lines.append(_block.toStringList(jsm, indent + 1, highlightOpcodeID));

	QString condStr = JsmExpression::stripParenthesis(
	                      _condition->toString(jsm, highlightOpcodeID, 10));
	lines.append(indentString("until %1 end", indent)
	             .arg(condStr));

	return lines.join("\n");
}

QStringList JsmApplication::stackToStringList(const JsmFile *jsm, int highlightOpcodeID) const
{
	if (_opcode == nullptr) {
		return QStringList();
	}
	
	QStringList params;
	bool isText = false, isMap = false, isChara = false, isParty = false,
	        isMagic = false, isItem = false, isItem2 = false, isKeys = false;

	switch (_opcode->key()) {
	case JsmOpcode::AMESW:
	case JsmOpcode::AMES:
	case JsmOpcode::RAMESW:
	case JsmOpcode::AASK:
		isText = true;
		break;
	case JsmOpcode::MAPJUMP:
	case JsmOpcode::MAPJUMP3:
	case JsmOpcode::MAPJUMPO:
	case JsmOpcode::PREMAPJUMP:
	case JsmOpcode::PREMAPJUMP2:
		isMap = true;
		break;
	case JsmOpcode::ADDPARTY:
	case JsmOpcode::SUBPARTY:
	case JsmOpcode::ISPARTY:
	case JsmOpcode::ADDMEMBER:
	case JsmOpcode::SUBMEMBER:
	case JsmOpcode::ISMEMBER:
	case JsmOpcode::RESETGF:
	case JsmOpcode::SETDRESS:
	case JsmOpcode::GETDRESS:
	case JsmOpcode::SETPC:
		isChara = true;
		break;
	case JsmOpcode::ADDMAGIC:
		isMagic = isChara = true;
		break;
	case JsmOpcode::SETPARTY:
		isParty = true;
		break;
	case JsmOpcode::HASITEM:
		isItem = true;
		break;
	case JsmOpcode::ADDITEM:
		isItem2 = true;
		break;
	case JsmOpcode::KEYSCAN:
	case JsmOpcode::KEYON:
	case JsmOpcode::KEYSCAN2:
	case JsmOpcode::KEYON2:
		isKeys = true;
		break;
	}

	int i = 0;
	QStack<JsmExpression *> stackCpy = _stack;
	while (!stackCpy.isEmpty()) {
		QString text;
		JsmExpression *expr = stackCpy.pop();

		if (isText || isMap || isChara || isParty || isItem || isItem2 || isKeys) {
			if (isText && i == 1) {
				bool ok = false;
				int id = expr->eval(&ok);

				if (ok) {
					text = QString("text_%1").arg(id);
				}
			} else if (isMap && i == 0) {
				bool ok = false;
				int id = expr->eval(&ok);

				if (ok) {
					text = QString("map_%1").arg(id);
				}
			} else if ((isChara && i == 0) || isParty) {
				bool ok = false;
				int id = expr->eval(&ok);

				if (ok) {
					switch (id) {
					case 0:
						text = "squall";
						break;
					case 1:
						text = "zell";
						break;
					case 2:
						text = "irvine";
						break;
					case 3:
						text = "quistis";
						break;
					case 4:
						text = "rinoa";
						break;
					case 5:
						text = "selphie";
						break;
					case 6:
						text = "seifer";
						break;
					case 7:
						text = "edea";
						break;
					case 8:
						text = "laguna";
						break;
					case 9:
						text = "kiros";
						break;
					case 10:
						text = "ward";
						break;
					default:
						text = QString("chara_%1").arg(id);
						break;
					}
				}
			} else if (isMagic && i == 1) {
				bool ok = false;
				int id = expr->eval(&ok);

				if (ok) {
					text = QString("magic_%1").arg(id);
				}
			} else if (isItem && i == 0) {
				bool ok = false;
				int id = expr->eval(&ok);

				if (ok) {
					text = QString("item_%1").arg(id);
				}
			} else if (isItem2 && i == 1) {
				bool ok = false;
				int id = expr->eval(&ok);

				if (ok) {
					text = QString("item_%1").arg(id);
				}
			} else if (isKeys && i == 0) {
				bool ok = false;
				int id = expr->eval(&ok);

				if (ok) {
					switch (id) {
					case 0x1:
						text = "KeyL1";
						break;
					case 0x2:
						text = "KeyR1";
						break;
					case 0x4:
						text = "KeyL2";
						break;
					case 0x8:
						text = "KeyR2";
						break;
					case 0x10:
						text = "KeyCancel";
						break;
					case 0x20:
						text = "KeyMenu";
						break;
					case 0x40:
						text = "KeyChoose";
						break;
					case 0x80:
						text = "KeyCard";
						break;
					case 0x100:
						text = "KeySelect";
						break;
					case 0x800:
						text = "KeyStart";
						break;
					case 0x1000:
						text = "KeyUp";
						break;
					case 0x2000:
						text = "KeyRight";
						break;
					case 0x4000:
						text = "KeyDown";
						break;
					case 0x8000:
						text = "KeyLeft";
						break;
					}
				}
			}

			++i;
		}

		if (text.isEmpty()) {
			text = expr->toString(jsm, highlightOpcodeID, 10);
		} else if (expr->position() == highlightOpcodeID) {
			text.prepend("[X]");
			text.append("[X]");
		}

		params.append(text);
	}

	return params;
}

QString JsmApplication::paramsToString(const JsmFile *jsm, int highlightOpcodeID) const
{
	QStringList params;

	if (_opcode != nullptr && _opcode->hasParam()) {
		params.append(_opcode->paramStr());
	}

	params.append(stackToStringList(jsm, highlightOpcodeID));

	return params.join(", ");
}

QString JsmApplication::toString(const JsmFile *jsm, int highlightOpcodeID) const
{
	return QString("%1(%2)").arg(_opcode != nullptr ? _opcode->name().toLower() : "nil",
	                             paramsToString(jsm, highlightOpcodeID));
}

int JsmApplication::opcodeCount() const
{
	int ret = 1;

	for (JsmExpression *expr: _stack) {
		ret += expr->opcodeCount();
	}

	return ret;
}

JsmExpression *JsmApplicationAssignment::opcodeExpression() const
{
	if (_opcode == nullptr) {
		return 0;
	}
	switch (_opcode->key()) {
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

QString JsmApplicationAssignment::toString(const JsmFile *jsm, int highlightOpcodeID) const
{
	QString ret;
	JsmExpression *opExpr = opcodeExpression(), // New instance
	        *expr = _stack.top();
	if (expr->type() == JsmExpression::Binary) {
		JsmExpressionBinary *binaryExpr
		        = static_cast<JsmExpressionBinary *>(expr);
		JsmExpression *leftOp = binaryExpr->leftOperand(),
		        *rightOp = binaryExpr->rightOperand();
		if (opExpr->type() == leftOp->type()
		        && opExpr->type() == JsmExpression::Var
		        && (((JsmExpressionVar *)opExpr)->var() ==
		            ((JsmExpressionVar *)leftOp)->var()
		         || ((JsmExpressionTemp *)opExpr)->temp() ==
		            ((JsmExpressionTemp *)leftOp)->temp())) {
			int base = 10;
			if (binaryExpr->operation() == JsmExpressionBinary::And
			        || binaryExpr->operation() == JsmExpressionBinary::Or
			        || binaryExpr->operation() == JsmExpressionBinary::Eor
			        || binaryExpr->operation() == JsmExpressionBinary::Rsh
			        || binaryExpr->operation() == JsmExpressionBinary::Lsh) {
				base = 16;
			}
			delete opExpr;
			opExpr = leftOp;
			ret = QString("%1 %2= %3")
			      .arg(opExpr->toString(jsm, highlightOpcodeID, base),
			           binaryExpr->operationToString(),
			           JsmExpression::stripParenthesis(
			               rightOp->toString(jsm, highlightOpcodeID, base)));
			opExpr = nullptr;
		} else if ((binaryExpr->operation() == JsmExpressionBinary::Add
		          || binaryExpr->operation() == JsmExpressionBinary::Mul)
		          && opExpr->type() == rightOp->type()
		          && opExpr->type() == JsmExpression::Var
		          && (((JsmExpressionVar *)opExpr)->var() ==
		              ((JsmExpressionVar *)rightOp)->var()
		           || ((JsmExpressionTemp *)opExpr)->temp() ==
		              ((JsmExpressionTemp *)rightOp)->temp())) {
			delete opExpr;
			opExpr = rightOp;
			ret = QString("%1 %2= %3")
			      .arg(opExpr->toString(jsm, highlightOpcodeID, 10),
			           binaryExpr->operationToString(),
			           JsmExpression::stripParenthesis(
			               leftOp->toString(jsm, highlightOpcodeID, 10)));
			opExpr = nullptr;
		}
	}
	if (ret.isEmpty() && opExpr != nullptr) {
		ret = QString("%1 = %2").arg(opExpr->toString(jsm, highlightOpcodeID, 10),
		                             JsmExpression::stripParenthesis(
		                                 expr->toString(jsm, highlightOpcodeID, 10)));
	}
	if (opExpr != nullptr) {
		delete opExpr;
	}
	return ret;
}

QString JsmApplicationExec::execType() const
{
	if (_opcode == nullptr) {
		return QString();
	}
	switch (_opcode->key()) {
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

QString JsmApplicationExec::toString(const JsmFile *jsm, int highlightOpcodeID) const
{
	if (_opcode == nullptr) {
		return JsmApplication::toString(jsm, highlightOpcodeID);
	}

	QString groupName, methodName;
	QStack<JsmExpression *> stackCpy = _stack;
	bool ok;
	int groupId = _opcode->param(),
	    absMethodId = stackCpy.pop()->eval(&ok);

	if (ok && jsm != nullptr && groupId >= 0 && absMethodId >= 0) {
		const JsmScripts &scripts = jsm->getScripts();
		if (groupId < scripts.nbGroup()
		        && absMethodId < scripts.nbScript()) {
			groupName = scripts.group(groupId).name();
			methodName = scripts.script(absMethodId).name();
		}
	}

	if (groupName.isEmpty() || methodName.isEmpty()) {
		return JsmApplication::toString(jsm, highlightOpcodeID);
	}

	QStringList params(stackCpy.pop()->toString(jsm, highlightOpcodeID, 10));
	QString eType = execType();
	if (!eType.isEmpty()) {
		params.append(eType);
	}

	return QString("%1.%2(%3)").arg(groupName,
	                                methodName,
	                                params.join(", "));
}
