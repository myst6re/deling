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

#include "JsmOpcode.h"

class JsmControl;
class JsmExpression;
class JsmApplication;
class JsmFile;

class JsmInstruction
{
public:
	union Instruction {
		JsmOpcode *opcode;
		JsmExpression *expression;
		JsmControl *control;
		JsmApplication *application;
	};
	enum InstructionType {
		Opcode,
		Expression,
		Control,
		Application
	};

	JsmInstruction(Instruction instr, InstructionType type, int pos) :
	    _instr(instr), _type(type), _position(pos) {}
	explicit JsmInstruction(JsmOpcode *opcode, int pos) :
	    _type(Opcode), _position(pos) {
		_instr.opcode = opcode;
	}
	explicit JsmInstruction(JsmExpression *expression);
	explicit JsmInstruction(JsmControl *control, int pos) :
	    _type(Control), _position(pos) {
		_instr.control = control;
	}
	explicit JsmInstruction(JsmApplication *application, int pos) :
	    _type(Application), _position(pos) {
		_instr.application = application;
	}
	inline JsmOpcode *opcode() const {
		return _instr.opcode;
	}
	inline JsmExpression *expression() const {
		return _instr.expression;
	}
	inline JsmControl *control() const {
		return _instr.control;
	}
	inline JsmApplication *application() const {
		return _instr.application;
	}
	inline InstructionType type() const {
		return _type;
	}
	QString toString(const JsmFile *jsm, int indent, int highlightOpcodeID) const;
	int opcodeCount() const;
	inline void setPosition(int position) {
		_position = position;
	}
private:
	static QString opcodeToString(const JsmOpcode *opcode);
	inline Instruction instruction() const {
		return _instr;
	}
	Instruction _instr;
	InstructionType _type;
	int _position;
};

class JsmProgram : public QList<JsmInstruction>
{
public:
	JsmProgram() {}
	JsmProgram(const QList<JsmInstruction> &other) :
		QList<JsmInstruction>(other) {}
	JsmProgram(QList<JsmInstruction> &&other) :
	    QList<JsmInstruction>(other) {}
	QStringList toStringList(const JsmFile *jsm, int indent, int highlightOpcodeID = -2) const;
	int opcodeCount() const;
};

class JsmExpression
{
public:
	enum ExpressionType {
		Val,
		Var,
		Char,
		Temp,
		Unary,
		Binary
	};

	JsmExpression() : _position(-1) {}
	virtual ~JsmExpression() {}
	virtual QString toString(const JsmFile *jsm, int highlightOpcodeID, int base) const=0;
	virtual int opcodeCount() const=0;
	virtual int eval(bool *ok) const;
	virtual ExpressionType type() const=0;
	inline void setPosition(int position) {
		_position = position;
	}
	inline int position() const {
		return _position;
	}
	static QString stripParenthesis(const QString &exprStr);
	static JsmExpression *factory(const JsmOpcode *op,
	                              QStack<JsmExpression *> &stack, int pos);
protected:
	QString getToStringMark(int highlightOpcodeID) const;
private:
	int _position;
};

class JsmExpressionVal : public JsmExpression
{
public:
	explicit JsmExpressionVal(int val) : _val(val) {}
	virtual ~JsmExpressionVal() {}
	virtual QString toString(const JsmFile *jsm, int highlightOpcodeID, int base) const;
	virtual inline int opcodeCount() const {
		return 1;
	}
	virtual int eval(bool *ok) const;
	inline ExpressionType type() const {
		return Val;
	}
private:
	int _val;
};

class JsmExpressionVar : public JsmExpression
{
public:
	explicit JsmExpressionVar(int var) : _var(var) {}
	virtual ~JsmExpressionVar() {}
	virtual inline int opcodeCount() const {
		return 1;
	}
	inline ExpressionType type() const {
		return Var;
	}
	inline int var() const {
		return _var;
	}
	QString varName() const;
protected:
	int _var;
};

class JsmExpressionVarUByte : public JsmExpressionVar
{
public:
	explicit JsmExpressionVarUByte(int var) : JsmExpressionVar(var) {}
	virtual ~JsmExpressionVarUByte() {}
	virtual QString toString(const JsmFile *jsm, int highlightOpcodeID, int base) const;
};

class JsmExpressionVarUWord : public JsmExpressionVar
{
public:
	explicit JsmExpressionVarUWord(int var) : JsmExpressionVar(var) {}
	virtual ~JsmExpressionVarUWord() {}
	virtual QString toString(const JsmFile *jsm, int highlightOpcodeID, int base) const;
};

class JsmExpressionVarULong : public JsmExpressionVar
{
public:
	explicit JsmExpressionVarULong(int var) : JsmExpressionVar(var) {}
	virtual ~JsmExpressionVarULong() {}
	virtual QString toString(const JsmFile *jsm, int highlightOpcodeID, int base) const;
};

class JsmExpressionVarSByte : public JsmExpressionVar
{
public:
	explicit JsmExpressionVarSByte(int var) : JsmExpressionVar(var) {}
	virtual ~JsmExpressionVarSByte() {}
	virtual QString toString(const JsmFile *jsm, int highlightOpcodeID, int base) const;
};

class JsmExpressionVarSWord : public JsmExpressionVar
{
public:
	explicit JsmExpressionVarSWord(int var) : JsmExpressionVar(var) {}
	virtual ~JsmExpressionVarSWord() {}
	virtual QString toString(const JsmFile *jsm, int highlightOpcodeID, int base) const;
};

class JsmExpressionVarSLong : public JsmExpressionVar
{
public:
	explicit JsmExpressionVarSLong(int var) : JsmExpressionVar(var) {}
	virtual ~JsmExpressionVarSLong() {}
	virtual QString toString(const JsmFile *jsm, int highlightOpcodeID, int base) const;
};

class JsmExpressionChar : public JsmExpression
{
public:
	explicit JsmExpressionChar(int c) : _char(c) {}
	virtual ~JsmExpressionChar() {}
	virtual QString toString(const JsmFile *jsm, int highlightOpcodeID, int base) const;
	virtual inline int opcodeCount() const {
		return 1;
	}
	inline ExpressionType type() const {
		return Char;
	}
private:
	int _char;
};

class JsmExpressionTemp : public JsmExpression
{
public:
	explicit JsmExpressionTemp(int temp) : _temp(temp) {}
	virtual ~JsmExpressionTemp() {}
	virtual QString toString(const JsmFile *jsm, int highlightOpcodeID, int base) const;
	virtual inline int opcodeCount() const {
		return 1;
	}
	inline int temp() const {
		return _temp;
	}
	inline ExpressionType type() const {
		return Temp;
	}
private:
	int _temp;
};

class JsmExpressionUnary : public JsmExpression
{
public:
	enum Operation {
		Min = JsmOpcodeCal::MIN,
		Not = JsmOpcodeCal::NOT,
		LogNot = 0x1000002
	};
	JsmExpressionUnary(Operation op, JsmExpression *first) :
	    _op(op), _first(first) {}
	virtual ~JsmExpressionUnary() {}
	virtual QString toString(const JsmFile *jsm, int highlightOpcodeID, int base) const;
	virtual int opcodeCount() const;
	virtual int eval(bool *ok) const;
	inline ExpressionType type() const {
		return Unary;
	}
	inline JsmExpression *leftOperand() const {
		return _first;
	}
	inline QString operationToString() const {
		return operationToString(_op);
	}
	static QString operationToString(Operation op);
private:
	Operation _op;
	JsmExpression *_first;
};

class JsmExpressionBinary : public JsmExpression
{
public:
	enum Operation {
		Add = JsmOpcodeCal::ADD,
		Sub = JsmOpcodeCal::SUB,
		Mul = JsmOpcodeCal::MUL,
		Div = JsmOpcodeCal::DIV,
		Mod = JsmOpcodeCal::MOD,
		// Min
		Eq = JsmOpcodeCal::EQ,
		Gt = JsmOpcodeCal::GT,
		Ge = JsmOpcodeCal::GE,
		Ls = JsmOpcodeCal::LS,
		Le = JsmOpcodeCal::LE,
		Nt = JsmOpcodeCal::NT,
		And = JsmOpcodeCal::AND,
		Or = JsmOpcodeCal::OR,
		Eor = JsmOpcodeCal::EOR,
		// Not
		Rsh = JsmOpcodeCal::RSH,
		Lsh = JsmOpcodeCal::LSH,
		LogAnd = 0x1000000,
		LogOr = 0x1000001
		// LogNot
	};
	JsmExpressionBinary(Operation op, JsmExpression *first,
	                    JsmExpression *second) :
	    _op(op), _first(first), _second(second) {}
	virtual ~JsmExpressionBinary() {}
	virtual QString toString(const JsmFile *jsm, int highlightOpcodeID, int base) const;
	virtual int opcodeCount() const;
	virtual int eval(bool *ok) const;
	Operation logicalNot(bool *ok) const;
	bool logicalNot();
	inline ExpressionType type() const {
		return Binary;
	}
	inline JsmExpression *leftOperand() const {
		return _first;
	}
	inline JsmExpression *rightOperand() const {
		return _second;
	}
	inline Operation operation() const {
		return _op;
	}
	inline QString operationToString() const {
		return operationToString(_op);
	}
	static QString operationToString(Operation op);
private:
	Operation _op;
	JsmExpression *_first, *_second;
};

class JsmControl
{
public:
	enum ControlType {
		IfElse,
		While,
		RepeatUntil
	};

	JsmControl(JsmExpression *condition, const JsmProgram &block) :
	    _condition(condition), _block(block) {}
	virtual ~JsmControl() {}
	virtual QString toString(const JsmFile *jsm, int indent, int highlightOpcodeID) const=0;
	virtual ControlType type() const=0;
	virtual int opcodeCount() const {
		return condition()->opcodeCount() +
		       block().opcodeCount();
	}
	inline JsmExpression *condition() const {
		return _condition;
	}
	inline void setCondition(JsmExpression *condition) {
		_condition = condition;
	}
	inline const JsmProgram &block() const {
		return _block;
	}
	inline JsmProgram &block() {
		return _block;
	}
	inline void setBlock(const JsmProgram &block) {
		_block = block;
	}
protected:
	// FIXME: move this in a QString extension (if possible)
	static QString &indentString(QString &str, int indent);
	static QString indentString(const char *str, int indent);
	JsmExpression *_condition;
	JsmProgram _block;
};

class JsmControlIfElse : public JsmControl
{
public:
	JsmControlIfElse(JsmExpression *condition,
	                 const JsmProgram &block,
	                 const JsmProgram &blockElse = JsmProgram()) :
	    JsmControl(condition, block), _blockElse(blockElse) {}
	explicit JsmControlIfElse(const JsmControl &other,
	                 const JsmProgram &blockElse = JsmProgram()) :
	    JsmControl(other), _blockElse(blockElse) {}
	virtual ~JsmControlIfElse() {}
	QString toString(bool elseIf, const JsmFile *jsm, int indent, int highlightOpcodeID) const;
	inline virtual QString toString(const JsmFile *jsm, int indent, int highlightOpcodeID) const {
		return toString(false, jsm, indent, highlightOpcodeID);
	}
	virtual int opcodeCount() const {
		return JsmControl::opcodeCount() +
		       blockElse().opcodeCount();
	}
	inline ControlType type() const {
		return IfElse;
	}
	inline JsmProgram &blockElse() {
		return _blockElse;
	}
	inline const JsmProgram &blockElse() const {
		return _blockElse;
	}
	inline void setBlockElse(const JsmProgram &blockElse) {
		_blockElse = blockElse;
	}
private:
	JsmProgram _blockElse;
};

class JsmControlWhile : public JsmControl
{
public:
	JsmControlWhile(JsmExpression *condition,
	                const JsmProgram &block) :
	    JsmControl(condition, block) {}
	JsmControlWhile(const JsmControl &other) :
	    JsmControl(other) {}
	virtual ~JsmControlWhile() {}
	virtual QString toString(const JsmFile *jsm, int indent, int highlightOpcodeID) const;
	inline ControlType type() const {
		return While;
	}
};

class JsmControlRepeatUntil : public JsmControl
{
public:
	JsmControlRepeatUntil(JsmExpression *condition,
	                      const JsmProgram &block) :
	    JsmControl(condition, block) {}
	JsmControlRepeatUntil(const JsmControl &other) :
	    JsmControl(other) {}
	virtual ~JsmControlRepeatUntil() {}
	virtual QString toString(const JsmFile *jsm, int indent, int highlightOpcodeID) const;
	inline ControlType type() const {
		return RepeatUntil;
	}
};

class JsmApplication
{
public:
	JsmApplication(const QStack<JsmExpression *> &stack, JsmOpcode *opcode) :
	    _stack(stack), _opcode(opcode) {}
	virtual ~JsmApplication() {}
	virtual QString toString(const JsmFile *jsm, int highlightOpcodeID) const;
	virtual int opcodeCount() const;
protected:
	QStringList stackToStringList(const JsmFile *jsm, int highlightOpcodeID) const;
	QString paramsToString(const JsmFile *jsm, int highlightOpcodeID) const;
	QStack<JsmExpression *> _stack;
	JsmOpcode *_opcode;
};

class JsmApplicationAssignment : public JsmApplication
{
public:
	JsmApplicationAssignment(JsmExpression *expression,
	                         JsmOpcode *opcode) :
	    JsmApplication(QStack<JsmExpression *>(), opcode) {
		_stack.push(expression);
	}
	virtual ~JsmApplicationAssignment() {}
	QString toString(const JsmFile *jsm, int highlightOpcodeID) const;
private:
	JsmExpression *opcodeExpression() const;
};

class JsmApplicationExec : public JsmApplication
{
public:
	JsmApplicationExec(JsmExpression *stackSecond,
	                   JsmExpression *stackLast,
	                   JsmOpcode *opcode) :
	    JsmApplication(QStack<JsmExpression *>(), opcode) {
		_stack.push(stackSecond);
		_stack.push(stackLast);
	}
	virtual ~JsmApplicationExec() {}
	QString toString(const JsmFile *jsm, int highlightOpcodeID) const;
private:
	QString execType() const;
};
