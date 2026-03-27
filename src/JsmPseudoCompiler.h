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
#include "JsmData.h"
#include "JsmOpcode.h"

class JsmPseudoCompiler
{
public:
	JsmPseudoCompiler();

	bool compile(const QString &pseudoCode, JsmData &result, QString &errorStr, int &errorLine);

private:
	struct Token {
		enum Type {
			Identifier, Number, HexNumber, BinNumber,
			LParen, RParen, Comma, Dot,
			OpAssign, OpAddAssign, OpSubAssign, OpMulAssign, OpDivAssign,
			OpModAssign, OpAndAssign, OpOrAssign, OpEorAssign, OpRshAssign, OpLshAssign,
			OpEq, OpNt, OpGt, OpGe, OpLs, OpLe,
			OpAdd, OpSub, OpMul, OpDiv, OpMod,
			OpAnd, OpOr, OpEor, OpNot, OpRsh, OpLsh,
			OpLogAnd, OpLogOr, OpLogNot,
			Newline, EndOfInput
		};
		Type type;
		QString text;
		int line;
	};

	// Lexer
	QList<Token> tokenize(const QString &input, QString &errorStr, int &errorLine);

	// Parser state
	QList<Token> _tokens;
	int _pos;
	int _currentLine;

	Token peek() const;
	Token advance();
	bool match(Token::Type type);
	bool expect(Token::Type type, QString &errorStr);
	void skipNewlines();

	// Parser — statement level
	bool parseStatements(JsmData &result, QString &errorStr, int &errorLine);
	bool parseStatement(JsmData &result, QString &errorStr);
	bool parseIf(JsmData &result, QString &errorStr);
	bool parseWhile(JsmData &result, QString &errorStr);
	bool parseWait(JsmData &result, QString &errorStr);
	bool parseForever(JsmData &result, QString &errorStr);
	bool parseRepeat(JsmData &result, QString &errorStr);

	// Parser — expression level (for conditions and arguments)
	bool parseExpression(JsmData &result, QString &errorStr);
	bool parseLogicalOr(JsmData &result, QString &errorStr);
	bool parseLogicalAnd(JsmData &result, QString &errorStr);
	bool parseBitwiseOr(JsmData &result, QString &errorStr);
	bool parseBitwiseEor(JsmData &result, QString &errorStr);
	bool parseBitwiseAnd(JsmData &result, QString &errorStr);
	bool parseEquality(JsmData &result, QString &errorStr);
	bool parseRelational(JsmData &result, QString &errorStr);
	bool parseShift(JsmData &result, QString &errorStr);
	bool parseAdditive(JsmData &result, QString &errorStr);
	bool parseMultiplicative(JsmData &result, QString &errorStr);
	bool parseUnary(JsmData &result, QString &errorStr);
	bool parsePrimary(JsmData &result, QString &errorStr);

	// Helpers
	bool parseAssignment(const QString &varName, JsmData &result, QString &errorStr);
	bool parseFunctionCall(const QString &name, JsmData &result, QString &errorStr, bool isStatement);
	bool parseMethodCall(const QString &objectName, const QString &methodName,
	                     JsmData &result, QString &errorStr);
	bool emitPushVar(const QString &varName, JsmData &result, QString &errorStr);
	bool emitPopVar(const QString &varName, JsmData &result, QString &errorStr);
	int resolveOpcode(const QString &name) const;

	static QMap<QString, int> _opcodeMap;
	static void initOpcodeMap();
};
