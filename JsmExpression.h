#ifndef JSMEXPRESSION_H
#define JSMEXPRESSION_H

#include <QtCore>
#include "JsmOpcode.h"

class JsmControl;
class JsmExpression;
class JsmApplication;

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
	QString toString(int indent) const;
private:
	Instruction _instr;
	InstructionType _type;
};

typedef QList<JsmInstruction> JsmProgram;

class JsmExpression
{
public:
	JsmExpression() {}
	virtual QString toString() const=0;
	static JsmExpression *factory(const JsmOpcode *op,
	                              QStack<JsmExpression *> &stack);
};

class JsmExpressionVal : public JsmExpression
{
public:
	explicit JsmExpressionVal(int val) : _val(val) {}
	virtual QString toString() const;
private:
	int _val;
};

class JsmExpressionVar : public JsmExpression
{
public:
	explicit JsmExpressionVar(int var) : _var(var) {}
protected:
	int _var;
};

class JsmExpressionVarUByte : public JsmExpressionVar
{
public:
	explicit JsmExpressionVarUByte(int var) : JsmExpressionVar(var) {}
	virtual QString toString() const;
};

class JsmExpressionVarUWord : public JsmExpressionVar
{
public:
	explicit JsmExpressionVarUWord(int var) : JsmExpressionVar(var) {}
	virtual QString toString() const;
};

class JsmExpressionVarULong : public JsmExpressionVar
{
public:
	explicit JsmExpressionVarULong(int var) : JsmExpressionVar(var) {}
	virtual QString toString() const;
};

class JsmExpressionVarSByte : public JsmExpressionVar
{
public:
	explicit JsmExpressionVarSByte(int var) : JsmExpressionVar(var) {}
	virtual QString toString() const;
};

class JsmExpressionVarSWord : public JsmExpressionVar
{
public:
	explicit JsmExpressionVarSWord(int var) : JsmExpressionVar(var) {}
	virtual QString toString() const;
};

class JsmExpressionVarSLong : public JsmExpressionVar
{
public:
	explicit JsmExpressionVarSLong(int var) : JsmExpressionVar(var) {}
	virtual QString toString() const;
};

class JsmExpressionChar : public JsmExpression
{
public:
	explicit JsmExpressionChar(int c) : _char(c) {}
	virtual QString toString() const;
private:
	int _char;
};

class JsmExpressionTemp : public JsmExpression
{
public:
	explicit JsmExpressionTemp(int temp) : _temp(temp) {}
	virtual QString toString() const;
private:
	int _temp;
};

class JsmExpressionUnary : public JsmExpression
{
public:
	enum Operation {
		Min = 5
	};
	JsmExpressionUnary(Operation op, JsmExpression *first) :
	    _op(op), _first(first) {}
	virtual QString toString() const;
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
	virtual QString toString() const;
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
	JsmControl(JsmExpression *condition, const JsmProgram &block) :
	    _condition(condition), _block(block) {}
	virtual QString toString(int indent) const=0;
	inline JsmExpression *condition() const {
		return _condition;
	}
	inline const JsmProgram &block() const {
		return _block;
	}
	inline void blockRemoveLast() {
		_block.removeLast();
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
	virtual QString toString(int indent) const;
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
	virtual QString toString(int indent) const;
};

class JsmControlRepeatUntil : public JsmControl
{
public:
	JsmControlRepeatUntil(JsmExpression *condition,
	                      const JsmProgram &block) :
	    JsmControl(condition, block) {}
	virtual QString toString(int indent) const;
};

class JsmApplication
{
public:
	JsmApplication(const QStack<JsmExpression *> &stack, JsmOpcode *opcode) :
	    _stack(stack), _opcode(opcode) {}
	virtual QString toString() const;
protected:
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
	QString toString() const;
};

#endif // JSMEXPRESSION_H
