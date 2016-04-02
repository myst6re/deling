#ifndef JSMEXPRESSION_H
#define JSMEXPRESSION_H

#include <QtCore>
#include "JsmOpcode.h"

class JsmControl;
class JsmExpression;
class JsmApplication;
class Field;

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

	JsmInstruction(Instruction instr, InstructionType type) :
	    _instr(instr), _type(type) {}
	explicit JsmInstruction(JsmOpcode *opcode) :
	    _type(Opcode) {
		_instr.opcode = opcode;
	}
	explicit JsmInstruction(JsmExpression *expression) :
	    _type(Expression) {
		_instr.expression = expression;
	}
	explicit JsmInstruction(JsmControl *control) :
	    _type(Control) {
		_instr.control = control;
	}
	explicit JsmInstruction(JsmApplication *application) :
	    _type(Application) {
		_instr.application = application;
	}
	inline Instruction instruction() const {
		return _instr;
	}
	inline InstructionType type() const {
		return _type;
	}
	QString toString(const Field *field, int indent) const;
private:
	Instruction _instr;
	InstructionType _type;
};

typedef QList<JsmInstruction> JsmProgram;

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

	JsmExpression() {}
	virtual QString toString(const Field *field) const=0;
	virtual int opcodeCount() const=0;
	virtual int eval(bool *ok) const;
	virtual ExpressionType type() const=0;
	static JsmExpression *factory(const JsmOpcode *op,
	                              QStack<JsmExpression *> &stack);
};

class JsmExpressionVal : public JsmExpression
{
public:
	explicit JsmExpressionVal(int val) : _val(val) {}
	virtual QString toString(const Field *field) const;
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
	virtual inline int opcodeCount() const {
		return 1;
	}
	inline ExpressionType type() const {
		return Var;
	}
protected:
	int _var;
};

class JsmExpressionVarUByte : public JsmExpressionVar
{
public:
	explicit JsmExpressionVarUByte(int var) : JsmExpressionVar(var) {}
	virtual QString toString(const Field *field) const;
};

class JsmExpressionVarUWord : public JsmExpressionVar
{
public:
	explicit JsmExpressionVarUWord(int var) : JsmExpressionVar(var) {}
	virtual QString toString(const Field *field) const;
};

class JsmExpressionVarULong : public JsmExpressionVar
{
public:
	explicit JsmExpressionVarULong(int var) : JsmExpressionVar(var) {}
	virtual QString toString(const Field *field) const;
};

class JsmExpressionVarSByte : public JsmExpressionVar
{
public:
	explicit JsmExpressionVarSByte(int var) : JsmExpressionVar(var) {}
	virtual QString toString(const Field *field) const;
};

class JsmExpressionVarSWord : public JsmExpressionVar
{
public:
	explicit JsmExpressionVarSWord(int var) : JsmExpressionVar(var) {}
	virtual QString toString(const Field *field) const;
};

class JsmExpressionVarSLong : public JsmExpressionVar
{
public:
	explicit JsmExpressionVarSLong(int var) : JsmExpressionVar(var) {}
	virtual QString toString(const Field *field) const;
};

class JsmExpressionChar : public JsmExpression
{
public:
	explicit JsmExpressionChar(int c) : _char(c) {}
	virtual QString toString(const Field *field) const;
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
	virtual QString toString(const Field *field) const;
	virtual inline int opcodeCount() const {
		return 1;
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
		Min = 5,
		Not = 15
	};
	JsmExpressionUnary(Operation op, JsmExpression *first) :
	    _op(op), _first(first) {}
	virtual QString toString(const Field *field) const;
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
		Add = 0,
		Sub = 1,
		Mul = 2,
		Mod = 3,
		Div = 4,
		// Min
		Eq = 6,
		Gt = 7,
		Ge = 8,
		Ls = 9,
		Le = 10,
		Nt = 11,
		And = 12,
		Or = 13,
		Eor = 14
	};
	JsmExpressionBinary(Operation op, JsmExpression *first,
	                    JsmExpression *second) :
	    _op(op), _first(first), _second(second) {}
	virtual QString toString(const Field *field) const;
	virtual int opcodeCount() const;
	virtual int eval(bool *ok) const;
	inline ExpressionType type() const {
		return Binary;
	}
	inline JsmExpression *leftOperand() const {
		return _first;
	}
	inline JsmExpression *rightOperand() const {
		return _second;
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
	virtual QString toString(const Field *field, int indent) const=0;
	virtual ControlType type() const=0;
	inline JsmExpression *condition() const {
		return _condition;
	}
	inline const JsmProgram &block() const {
		return _block;
	}
	inline JsmInstruction blockTakeLast() {
		return _block.takeLast();
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
	JsmControlIfElse(const JsmControl &other,
	                 const JsmProgram &blockElse = JsmProgram()) :
	    JsmControl(other), _blockElse(blockElse) {}
	QString toString(const Field *field, int indent, bool elseIf) const;
	inline virtual QString toString(const Field *field, int indent) const {
		return toString(field, indent, false);
	}
	inline ControlType type() const {
		return IfElse;
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
	virtual QString toString(const Field *field, int indent) const;
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
	virtual QString toString(const Field *field, int indent) const;
	inline ControlType type() const {
		return RepeatUntil;
	}
};

class JsmApplication
{
public:
	JsmApplication(const QStack<JsmExpression *> &stack, JsmOpcode *opcode) :
	    _stack(stack), _opcode(opcode) {}
	virtual QString toString(const Field *field) const;
protected:
	QStringList stackToStringList(const Field *field) const;
	QString paramsToString(const Field *field) const;
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
	QString toString(const Field *field) const;
private:
	JsmExpression *opcodeExpression() const;
};

class JsmApplicationExec : public JsmApplication
{
public:
	JsmApplicationExec(JsmExpression *stackFirst,
	                   JsmExpression *stackSecond,
	                   JsmOpcode *opcode) :
	    JsmApplication(QStack<JsmExpression *>(), opcode) {
		_stack.push(stackSecond);
		_stack.push(stackFirst);
	}
	QString toString(const Field *field) const;
private:
	QString execType() const;
};

#endif // JSMEXPRESSION_H
