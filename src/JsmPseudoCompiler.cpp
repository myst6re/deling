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
#include "JsmPseudoCompiler.h"
#include "files/JsmFile.h"

QMap<QString, int> JsmPseudoCompiler::_opcodeMap;

JsmPseudoCompiler::JsmPseudoCompiler()
{
	initOpcodeMap();
}

void JsmPseudoCompiler::initOpcodeMap()
{
	if (!_opcodeMap.isEmpty()) return;
	for (int i = 0; i < JSM_OPCODE_COUNT; ++i) {
		_opcodeMap.insert(QString(JsmOpcode::opcodes[i]).toLower(), i);
	}
}

int JsmPseudoCompiler::resolveOpcode(const QString &name) const
{
	return _opcodeMap.value(name.toLower(), -1);
}

// ============================================================
// Tokenizer
// ============================================================

QList<JsmPseudoCompiler::Token> JsmPseudoCompiler::tokenize(
    const QString &input, QString &errorStr, int &errorLine)
{
	QList<Token> tokens;
	int i = 0, line = 1, len = input.length();

	while (i < len) {
		QChar c = input[i];

		// Skip spaces/tabs
		if (c == ' ' || c == '\t') { ++i; continue; }

		// Comments: // to end of line
		if (c == '/' && i + 1 < len && input[i + 1] == '/') {
			while (i < len && input[i] != '\n') ++i;
			continue;
		}

		// Newlines
		if (c == '\n') {
			tokens.append({Token::Newline, "\n", line});
			++line; ++i; continue;
		}
		if (c == '\r') {
			tokens.append({Token::Newline, "\n", line});
			++i;
			if (i < len && input[i] == '\n') ++i;
			++line; continue;
		}

		// Numbers: 0x hex, b binary, or decimal (possibly negative handled as unary)
		// But check if number is followed by _ (making it a variable like 474_ubyte)
		if (c.isDigit() || (c == '-' && i + 1 < len && input[i + 1].isDigit())) {
			int start = i;
			if (c == '-') ++i;
			if (i + 1 < len && input[i] == '0' && (input[i + 1] == 'x' || input[i + 1] == 'X')) {
				i += 2;
				while (i < len && (input[i].isDigit() || (input[i] >= 'a' && input[i] <= 'f')
				       || (input[i] >= 'A' && input[i] <= 'F'))) ++i;
				tokens.append({Token::HexNumber, input.mid(start, i - start), line});
			} else {
				while (i < len && input[i].isDigit()) ++i;
				// Check if followed by _ making it a variable name (e.g. 474_ubyte, 1024_uword)
				if (i < len && input[i] == '_') {
					while (i < len && (input[i].isLetterOrNumber() || input[i] == '_')) ++i;
					tokens.append({Token::Identifier, input.mid(start, i - start), line});
				} else {
					tokens.append({Token::Number, input.mid(start, i - start), line});
				}
			}
			continue;
		}

		// Binary: b01010
		if (c == 'b' && i + 1 < len && (input[i + 1] == '0' || input[i + 1] == '1')) {
			int start = i; ++i;
			while (i < len && (input[i] == '0' || input[i] == '1')) ++i;
			// Check it's not an identifier continuation
			if (i < len && (input[i].isLetterOrNumber() || input[i] == '_')) {
				// It's an identifier starting with 'b'
				while (i < len && (input[i].isLetterOrNumber() || input[i] == '_')) ++i;
				tokens.append({Token::Identifier, input.mid(start, i - start), line});
			} else {
				tokens.append({Token::BinNumber, input.mid(start, i - start), line});
			}
			continue;
		}

		// Identifiers and keywords
		if (c.isLetter() || c == '_') {
			int start = i;
			while (i < len && (input[i].isLetterOrNumber() || input[i] == '_' || input[i] == '#')) ++i;
			QString word = input.mid(start, i - start);
			// Check for "and" / "or" keywords
			if (word.toLower() == "and") {
				tokens.append({Token::OpLogAnd, word, line});
			} else if (word.toLower() == "or") {
				tokens.append({Token::OpLogOr, word, line});
			} else {
				tokens.append({Token::Identifier, word, line});
			}
			continue;
		}

		// Two-char operators
		if (i + 1 < len) {
			QString two = input.mid(i, 2);
			if (two == "==") { tokens.append({Token::OpEq, two, line}); i += 2; continue; }
			if (two == "!=") { tokens.append({Token::OpNt, two, line}); i += 2; continue; }
			if (two == ">=") { tokens.append({Token::OpGe, two, line}); i += 2; continue; }
			if (two == "<=") { tokens.append({Token::OpLe, two, line}); i += 2; continue; }
			if (two == ">>") { tokens.append({Token::OpRsh, two, line}); i += 2; continue; }
			if (two == "<<") { tokens.append({Token::OpLsh, two, line}); i += 2; continue; }
			if (two == "+=") { tokens.append({Token::OpAddAssign, two, line}); i += 2; continue; }
			if (two == "-=") { tokens.append({Token::OpSubAssign, two, line}); i += 2; continue; }
			if (two == "*=") { tokens.append({Token::OpMulAssign, two, line}); i += 2; continue; }
			if (two == "/=") { tokens.append({Token::OpDivAssign, two, line}); i += 2; continue; }
			if (two == "%=") { tokens.append({Token::OpModAssign, two, line}); i += 2; continue; }
			if (two == "&=") { tokens.append({Token::OpAndAssign, two, line}); i += 2; continue; }
			if (two == "|=") { tokens.append({Token::OpOrAssign, two, line}); i += 2; continue; }
			if (two == "^=") { tokens.append({Token::OpEorAssign, two, line}); i += 2; continue; }
		}
		// Three-char compound assigns
		if (i + 2 < len) {
			QString three = input.mid(i, 3);
			if (three == ">>=") { tokens.append({Token::OpRshAssign, three, line}); i += 3; continue; }
			if (three == "<<=") { tokens.append({Token::OpLshAssign, three, line}); i += 3; continue; }
		}

		// Single-char operators
		if (c == '(') { tokens.append({Token::LParen, "(", line}); ++i; continue; }
		if (c == ')') { tokens.append({Token::RParen, ")", line}); ++i; continue; }
		if (c == ',') { tokens.append({Token::Comma, ",", line}); ++i; continue; }
		if (c == '.') { tokens.append({Token::Dot, ".", line}); ++i; continue; }
		if (c == '=') { tokens.append({Token::OpAssign, "=", line}); ++i; continue; }
		if (c == '>') { tokens.append({Token::OpGt, ">", line}); ++i; continue; }
		if (c == '<') { tokens.append({Token::OpLs, "<", line}); ++i; continue; }
		if (c == '+') { tokens.append({Token::OpAdd, "+", line}); ++i; continue; }
		if (c == '-') { tokens.append({Token::OpSub, "-", line}); ++i; continue; }
		if (c == '*') { tokens.append({Token::OpMul, "*", line}); ++i; continue; }
		if (c == '/') { tokens.append({Token::OpDiv, "/", line}); ++i; continue; }
		if (c == '%') { tokens.append({Token::OpMod, "%", line}); ++i; continue; }
		if (c == '&') { tokens.append({Token::OpAnd, "&", line}); ++i; continue; }
		if (c == '|') { tokens.append({Token::OpOr, "|", line}); ++i; continue; }
		if (c == '^') { tokens.append({Token::OpEor, "^", line}); ++i; continue; }
		if (c == '~') { tokens.append({Token::OpNot, "~", line}); ++i; continue; }
		if (c == '!') { tokens.append({Token::OpLogNot, "!", line}); ++i; continue; }

		errorStr = QObject::tr("Unexpected character: '%1'").arg(c);
		errorLine = line;
		return QList<Token>();
	}

	tokens.append({Token::EndOfInput, "", line});
	return tokens;
}

// ============================================================
// Parser helpers
// ============================================================

JsmPseudoCompiler::Token JsmPseudoCompiler::peek() const
{
	if (_pos < _tokens.size()) return _tokens[_pos];
	return {Token::EndOfInput, "", _currentLine};
}

JsmPseudoCompiler::Token JsmPseudoCompiler::advance()
{
	Token t = peek();
	if (_pos < _tokens.size()) {
		_currentLine = t.line;
		++_pos;
	}
	return t;
}

bool JsmPseudoCompiler::match(Token::Type type)
{
	if (peek().type == type) { advance(); return true; }
	return false;
}

bool JsmPseudoCompiler::expect(Token::Type type, QString &errorStr)
{
	if (match(type)) return true;
	errorStr = QObject::tr("Expected '%1', got '%2'")
	           .arg(type == Token::Newline ? "newline" :
	                type == Token::RParen ? ")" :
	                type == Token::LParen ? "(" :
	                type == Token::Comma ? "," : "token",
	                peek().text.isEmpty() ? "end of input" : peek().text);
	return false;
}

void JsmPseudoCompiler::skipNewlines()
{
	while (peek().type == Token::Newline) advance();
}

// ============================================================
// Main compile entry
// ============================================================

bool JsmPseudoCompiler::compile(const QString &pseudoCode, JsmData &result,
                                QString &errorStr, int &errorLine)
{
	errorStr.clear();
	errorLine = 0;

	_tokens = tokenize(pseudoCode, errorStr, errorLine);
	if (!errorStr.isEmpty()) return false;

	_pos = 0;
	_currentLine = 1;

	return parseStatements(result, errorStr, errorLine, {});
}

// ============================================================
// Statement-level parsing
// ============================================================

bool JsmPseudoCompiler::parseStatements(JsmData &result, QString &errorStr, int &errorLine,
                                        const QStringList &allowedTerminators)
{
	skipNewlines();
	while (peek().type != Token::EndOfInput) {
		QString kw = peek().text.toLower();
		if (allowedTerminators.contains(kw)) break;
		// Check for block-ending keywords that aren't valid in this context
		if (kw == "end" || kw == "else" || kw == "until") {
			errorStr = QObject::tr("Unexpected '%1' in this block").arg(kw);
			errorLine = _currentLine;
			return false;
		}

		if (!parseStatement(result, errorStr)) {
			errorLine = _currentLine;
			return false;
		}
		skipNewlines();
	}
	return true;
}

bool JsmPseudoCompiler::parseStatement(JsmData &result, QString &errorStr)
{
	skipNewlines();
	Token t = peek();
	if (t.type == Token::EndOfInput) return true;

	QString kw = t.text.toLower();

	// Control flow keywords
	if (kw == "if") return parseIf(result, errorStr);
	if (kw == "while") return parseWhile(result, errorStr);
	if (kw == "wait") {
		// 'wait' is both a keyword (wait while, wait forever) and an opcode (wait(frames))
		advance(); // consume 'wait'
		if (peek().type == Token::LParen) {
			return parseFunctionCall("wait", result, errorStr, true);
		}
		// Keyword form — put back and delegate to parseWait
		--_pos;
		return parseWait(result, errorStr);
	}
	if (kw == "forever") return parseForever(result, errorStr);
	if (kw == "repeat") return parseRepeat(result, errorStr);
	if (kw == "ret") {
		advance();
		// Check if ret has arguments: ret(N)
		if (peek().type == Token::LParen) {
			return parseFunctionCall("ret", result, errorStr, true);
		}
		result.append(JsmOpcode(JsmOpcode::RET, 0));
		return true;
	}
	if (kw == "goto") {
		advance();
		// We don't support goto in pseudo-code compilation
		errorStr = QObject::tr("'goto' is not supported in pseudo-code mode. Use if/while/repeat instead.");
		return false;
	}
	if (kw == "label") {
		advance();
		// Labels are not supported in pseudo-code compilation
		errorStr = QObject::tr("'label' is not supported in pseudo-code mode.");
		return false;
	}

	// Must be an identifier: could be assignment, function call, or method call
	if (t.type != Token::Identifier) {
		errorStr = QObject::tr("Expected statement, got '%1'").arg(t.text);
		return false;
	}

	Token ident = advance();
	Token next = peek();

	// Method call: entity.method(args)
	if (next.type == Token::Dot) {
		advance(); // consume dot
		if (peek().type != Token::Identifier) {
			errorStr = QObject::tr("Expected method name after '.'");
			return false;
		}
		Token methodToken = advance();
		return parseMethodCall(ident.text, methodToken.text, result, errorStr);
	}

	// Assignment: var = expr, var += expr, etc.
	if (next.type == Token::OpAssign || next.type == Token::OpAddAssign ||
	    next.type == Token::OpSubAssign || next.type == Token::OpMulAssign ||
	    next.type == Token::OpDivAssign || next.type == Token::OpModAssign ||
	    next.type == Token::OpAndAssign || next.type == Token::OpOrAssign ||
	    next.type == Token::OpEorAssign || next.type == Token::OpRshAssign ||
	    next.type == Token::OpLshAssign) {
		return parseAssignment(ident.text, result, errorStr);
	}

	// Function call: name(args)
	if (next.type == Token::LParen) {
		return parseFunctionCall(ident.text, result, errorStr, true);
	}

	// Bare opcode name with no parens (for zero-arg opcodes)
	int opcode = resolveOpcode(ident.text);
	if (opcode >= 0) {
		result.append(JsmOpcode(opcode));
		return true;
	}

	errorStr = QObject::tr("Unknown statement: '%1'").arg(ident.text);
	return false;
}

// ============================================================
// Variable push/pop helpers
// ============================================================

bool JsmPseudoCompiler::emitPushVar(const QString &varName, JsmData &result, QString &errorStr)
{
	QString lower = varName.toLower();

	// temp_N
	if (lower.startsWith("temp_")) {
		bool ok;
		int idx = lower.mid(5).toInt(&ok);
		if (!ok) { errorStr = QObject::tr("Invalid temp index: '%1'").arg(varName); return false; }
		result.append(JsmOpcode(JsmOpcode::PSHI_L, idx));
		return true;
	}
	// model_N
	if (lower.startsWith("model_")) {
		bool ok;
		int idx = lower.mid(6).toInt(&ok);
		if (!ok) { errorStr = QObject::tr("Invalid model index: '%1'").arg(varName); return false; }
		result.append(JsmOpcode(JsmOpcode::PSHAC, idx));
		return true;
	}
	// N_ubyte, N_uword, N_ulong, N_sbyte, N_sword, N_slong
	// or VARNAME_ubyte etc.
	QRegularExpression varRe("^(.+)_(ubyte|uword|ulong|sbyte|sword|slong)$");
	QRegularExpressionMatch m = varRe.match(lower);
	if (m.hasMatch()) {
		QString varPart = m.captured(1);
		QString typePart = m.captured(2);
		// Extract the numeric var ID (could be "123" or "123_somename")
		bool ok;
		int varId = varPart.section('_', 0, 0).toInt(&ok);
		if (!ok) varId = varPart.toInt(&ok);
		if (!ok) { errorStr = QObject::tr("Cannot resolve variable: '%1'").arg(varName); return false; }

		int opKey;
		if (typePart == "ubyte") opKey = JsmOpcode::PSHM_B;
		else if (typePart == "uword") opKey = JsmOpcode::PSHM_W;
		else if (typePart == "ulong") opKey = JsmOpcode::PSHM_L;
		else if (typePart == "sbyte") opKey = JsmOpcode::PSHSM_B;
		else if (typePart == "sword") opKey = JsmOpcode::PSHSM_W;
		else opKey = JsmOpcode::PSHSM_L;

		result.append(JsmOpcode(opKey, varId));
		return true;
	}

	errorStr = QObject::tr("Cannot resolve variable for push: '%1'. "
	                        "Use temp_N, model_N, or N_ubyte/uword/ulong/sbyte/sword/slong format.")
	           .arg(varName);
	return false;
}

bool JsmPseudoCompiler::emitPopVar(const QString &varName, JsmData &result, QString &errorStr)
{
	QString lower = varName.toLower();

	if (lower.startsWith("temp_")) {
		bool ok;
		int idx = lower.mid(5).toInt(&ok);
		if (!ok) { errorStr = QObject::tr("Invalid temp index: '%1'").arg(varName); return false; }
		result.append(JsmOpcode(JsmOpcode::POPI_L, idx));
		return true;
	}

	QRegularExpression varRe("^(.+)_(ubyte|uword|ulong|sbyte|sword|slong)$");
	QRegularExpressionMatch m = varRe.match(lower);
	if (m.hasMatch()) {
		QString varPart = m.captured(1);
		QString typePart = m.captured(2);
		bool ok;
		int varId = varPart.section('_', 0, 0).toInt(&ok);
		if (!ok) varId = varPart.toInt(&ok);
		if (!ok) { errorStr = QObject::tr("Cannot resolve variable: '%1'").arg(varName); return false; }

		int opKey;
		if (typePart == "ubyte") opKey = JsmOpcode::POPM_B;
		else if (typePart == "uword") opKey = JsmOpcode::POPM_W;
		else if (typePart == "ulong") opKey = JsmOpcode::POPM_L;
		// Signed types still pop to unsigned storage
		else if (typePart == "sbyte") opKey = JsmOpcode::POPM_B;
		else if (typePart == "sword") opKey = JsmOpcode::POPM_W;
		else opKey = JsmOpcode::POPM_L;

		result.append(JsmOpcode(opKey, varId));
		return true;
	}

	errorStr = QObject::tr("Cannot resolve variable for assignment: '%1'. "
	                        "Use temp_N or N_ubyte/uword/ulong/sbyte/sword/slong format.")
	           .arg(varName);
	return false;
}

// ============================================================
// Assignment: var = expr, var += expr, etc.
// ============================================================

bool JsmPseudoCompiler::parseAssignment(const QString &varName, JsmData &result, QString &errorStr)
{
	Token op = advance(); // consume the assignment operator

	if (op.type == Token::OpAssign) {
		// var = expr  =>  push expr, pop var
		if (!parseExpression(result, errorStr)) return false;
		return emitPopVar(varName, result, errorStr);
	}

	// Compound assignment: var += expr  =>  push var, push expr, CAL op, pop var
	int calOp = -1;
	switch (op.type) {
	case Token::OpAddAssign: calOp = 0; break; // ADD
	case Token::OpSubAssign: calOp = 1; break; // SUB
	case Token::OpMulAssign: calOp = 2; break; // MUL
	case Token::OpDivAssign: calOp = 3; break; // DIV
	case Token::OpModAssign: calOp = 4; break; // MOD
	case Token::OpAndAssign: calOp = 12; break; // AND
	case Token::OpOrAssign:  calOp = 13; break; // OR
	case Token::OpEorAssign: calOp = 14; break; // EOR
	case Token::OpRshAssign: calOp = 16; break; // RSH
	case Token::OpLshAssign: calOp = 17; break; // LSH
	default:
		errorStr = QObject::tr("Unknown assignment operator");
		return false;
	}

	if (!emitPushVar(varName, result, errorStr)) return false;
	if (!parseExpression(result, errorStr)) return false;
	result.append(JsmOpcode(JsmOpcode::CAL, calOp));
	return emitPopVar(varName, result, errorStr);
}

// ============================================================
// Function call: name(arg1, arg2, ...)
// ============================================================

bool JsmPseudoCompiler::parseFunctionCall(const QString &name, JsmData &result,
                                          QString &errorStr, bool isStatement)
{
	Q_UNUSED(isStatement)

	if (!expect(Token::LParen, errorStr)) return false;

	// Collect arguments into a temporary buffer
	QList<JsmData> argDatas;
	if (peek().type != Token::RParen) {
		do {
			JsmData argData;
			if (!parseExpression(argData, errorStr)) return false;
			argDatas.append(argData);
		} while (match(Token::Comma));
	}
	if (!expect(Token::RParen, errorStr)) return false;

	int opcode = resolveOpcode(name);
	if (opcode < 0) {
		errorStr = QObject::tr("Unknown function: '%1'").arg(name);
		return false;
	}

	// Opcodes 0x00 through 0x38 (NOP through DISCJUMP) use an inline parameter
	// encoded in the opcode word. The first argument becomes the inline param,
	// remaining arguments are pushed onto the stack in reverse order.
	// Opcodes > 0x38 have no inline param — all args are stack args.
	bool hasInlineParam = (opcode <= 0x38);

	if (hasInlineParam) {
		if (argDatas.isEmpty()) {
			// No args — emit opcode with no param
			result.append(JsmOpcode(opcode));
		} else {
			// Push stack args (all except first) in forward order
			// The decompiler inverts the stack before display, so first pushed = first displayed
			for (int i = 1; i < argDatas.size(); ++i) {
				result += argDatas[i];
			}
			// First arg is the inline param — try to extract a literal
			const JsmData &firstArg = argDatas[0];
			if (firstArg.nbOpcode() == 1) {
				JsmOpcode firstOp = firstArg.opcode(0);
				if (firstOp.key() == JsmOpcode::PSHN_L) {
					result.append(JsmOpcode(opcode, firstOp.param()));
				} else {
					// Not a literal — push it and emit opcode without inline param
					result += firstArg;
					result.append(JsmOpcode(opcode));
				}
			} else {
				result += firstArg;
				result.append(JsmOpcode(opcode));
			}
		}
	} else {
		// No inline param — all args are stack args, push in forward order
		for (int i = 0; i < argDatas.size(); ++i) {
			result += argDatas[i];
		}
		result.append(JsmOpcode(opcode));
	}

	return true;
}

// ============================================================
// Method call: entity.method(priority, ...)  =>  REQ/REQSW/REQEW
// ============================================================

bool JsmPseudoCompiler::parseMethodCall(const QString &objectName, const QString &methodName,
                                        JsmData &result, QString &errorStr)
{
	if (!expect(Token::LParen, errorStr)) return false;

	// Parse arguments, but watch for SW/EW exec type identifiers
	QList<JsmData> argDatas;
	int reqOpcode = JsmOpcode::REQ;

	if (peek().type != Token::RParen) {
		do {
			// Check if this argument is an exec type modifier (SW or EW)
			if (peek().type == Token::Identifier) {
				QString upper = peek().text.toUpper();
				if (upper == "SW" || upper == "EW") {
					// Check if followed by ) or , — confirms it's a modifier, not a variable
					Token execToken = advance();
					if (peek().type == Token::RParen || peek().type == Token::Comma) {
						reqOpcode = (upper == "SW") ? JsmOpcode::REQSW : JsmOpcode::REQEW;
						continue;
					}
					// Not followed by ) or , — put it back and parse as expression
					--_pos;
				}
			}
			JsmData argData;
			if (!parseExpression(argData, errorStr)) return false;
			argDatas.append(argData);
		} while (match(Token::Comma));
	}
	if (!expect(Token::RParen, errorStr)) return false;

	// Resolve entity group ID — must be numeric or name#N format
	bool ok;
	int groupId = objectName.toInt(&ok);
	if (!ok) {
		int hashIdx = objectName.indexOf('#');
		if (hashIdx >= 0) {
			groupId = objectName.mid(hashIdx + 1).toInt(&ok);
		}
		if (!ok) {
			errorStr = QObject::tr("Cannot resolve entity '%1' to a group ID. "
			                        "Use numeric IDs for entity.method() calls.")
			           .arg(objectName);
			return false;
		}
	}

	// Emit: push method label, push priority, REQ/REQSW/REQEW(groupID)
	if (argDatas.isEmpty()) {
		result.append(JsmOpcode(JsmOpcode::PSHN_L, 0));
		result.append(JsmOpcode(JsmOpcode::PSHN_L, 0));
	} else if (argDatas.size() == 1) {
		result.append(JsmOpcode(JsmOpcode::PSHN_L, 0));
		result += argDatas[0];
	} else {
		result += argDatas[0];
		result += argDatas[1];
	}

	result.append(JsmOpcode(reqOpcode, groupId));
	return true;
}

// ============================================================
// Control flow: if/else/end, while/end, wait while, forever, repeat/until
// ============================================================

bool JsmPseudoCompiler::parseIf(JsmData &result, QString &errorStr)
{
	advance(); // consume 'if'

	// Parse condition
	JsmData condData;
	if (!parseExpression(condData, errorStr)) return false;

	// Expect 'begin'
	skipNewlines();
	if (peek().text.toLower() != "begin") {
		errorStr = QObject::tr("Expected 'begin' after if condition");
		return false;
	}
	advance();

	// Parse if-block
	JsmData ifBlock;
	int dummy;
	if (!parseStatements(ifBlock, errorStr, dummy, {"end", "else"})) return false;

	skipNewlines();
	QString kw = peek().text.toLower();

	if (kw == "else") {
		advance();
		skipNewlines();

		// Check for "else if"
		if (peek().text.toLower() == "if") {
			JsmData elseBlock;
			if (!parseIf(elseBlock, errorStr)) return false;

			// Emit: condition, JPF over if-block+JMP, if-block, JMP over else-block, else-block
			result += condData;
			int ifSize = ifBlock.nbOpcode() + 1; // +1 for JMP
			result.append(JsmOpcode(JsmOpcode::JPF, ifSize + 1));
			result += ifBlock;
			result.append(JsmOpcode(JsmOpcode::JMP, elseBlock.nbOpcode() + 1));
			result += elseBlock;
		} else {
			// Parse else-block
			JsmData elseBlock;
			if (!parseStatements(elseBlock, errorStr, dummy, {"end"})) return false;

			skipNewlines();
			if (peek().text.toLower() != "end") {
				errorStr = QObject::tr("Expected 'end' to close if/else block");
				return false;
			}
			advance();

			result += condData;
			int ifSize = ifBlock.nbOpcode() + 1;
			result.append(JsmOpcode(JsmOpcode::JPF, ifSize + 1));
			result += ifBlock;
			result.append(JsmOpcode(JsmOpcode::JMP, elseBlock.nbOpcode() + 1));
			result += elseBlock;
		}
	} else if (kw == "end") {
		advance();
		// Simple if without else
		result += condData;
		result.append(JsmOpcode(JsmOpcode::JPF, ifBlock.nbOpcode() + 1));
		result += ifBlock;
	} else {
		errorStr = QObject::tr("Expected 'else' or 'end' after if block, got '%1'").arg(peek().text);
		return false;
	}

	return true;
}

bool JsmPseudoCompiler::parseWhile(JsmData &result, QString &errorStr)
{
	advance(); // consume 'while'

	JsmData condData;
	if (!parseExpression(condData, errorStr)) return false;

	skipNewlines();
	if (peek().text.toLower() != "begin") {
		// "while <cond>" without begin = wait while (single-line)
		// Emit: condition, JPF +2, JMP back to condition
		int condSize = condData.nbOpcode();
		result += condData;
		result.append(JsmOpcode(JsmOpcode::JPF, 2));
		result.append(JsmOpcode(JsmOpcode::JMP, -(condSize + 1)));
		return true;
	}
	advance(); // consume 'begin'

	JsmData bodyBlock;
	int dummy;
	if (!parseStatements(bodyBlock, errorStr, dummy, {"end"})) return false;

	skipNewlines();
	if (peek().text.toLower() != "end") {
		errorStr = QObject::tr("Expected 'end' to close while block");
		return false;
	}
	advance();

	// Emit: condition, JPF over body+JMP, body, JMP back to condition
	int condSize = condData.nbOpcode();
	int bodySize = bodyBlock.nbOpcode();
	result += condData;
	result.append(JsmOpcode(JsmOpcode::JPF, bodySize + 2)); // +1 for JMP back
	result += bodyBlock;
	result.append(JsmOpcode(JsmOpcode::JMP, -(condSize + bodySize + 1)));

	return true;
}

bool JsmPseudoCompiler::parseWait(JsmData &result, QString &errorStr)
{
	advance(); // consume 'wait'

	skipNewlines();
	QString kw = peek().text.toLower();

	if (kw == "while") {
		advance(); // consume 'while'
		JsmData condData;
		if (!parseExpression(condData, errorStr)) return false;

		// wait while <cond> = empty-body while loop
		int condSize = condData.nbOpcode();
		result += condData;
		result.append(JsmOpcode(JsmOpcode::JPF, 2));
		result.append(JsmOpcode(JsmOpcode::JMP, -(condSize + 1)));
		return true;
	}

	if (kw == "forever") {
		advance(); // consume 'forever'
		// wait forever = while(1) with empty body
		result.append(JsmOpcode(JsmOpcode::PSHN_L, 1));
		result.append(JsmOpcode(JsmOpcode::JPF, 2));
		result.append(JsmOpcode(JsmOpcode::JMP, -2));
		return true;
	}

	errorStr = QObject::tr("Expected 'while' or 'forever' after 'wait'");
	return false;
}

bool JsmPseudoCompiler::parseForever(JsmData &result, QString &errorStr)
{
	advance(); // consume 'forever'

	skipNewlines();
	// Check for 'begin' — if present, it's a forever loop with body
	if (peek().text.toLower() == "begin") {
		advance();
		JsmData bodyBlock;
		int dummy;
		if (!parseStatements(bodyBlock, errorStr, dummy, {"end"})) return false;

		skipNewlines();
		if (peek().text.toLower() != "end") {
			errorStr = QObject::tr("Expected 'end' to close forever block");
			return false;
		}
		advance();

		// forever begin ... end = while(1) begin ... end
		int bodySize = bodyBlock.nbOpcode();
		result.append(JsmOpcode(JsmOpcode::PSHN_L, 1));
		result.append(JsmOpcode(JsmOpcode::JPF, bodySize + 2));
		result += bodyBlock;
		result.append(JsmOpcode(JsmOpcode::JMP, -(bodySize + 2)));
		return true;
	}

	// Bare "forever" = wait forever
	result.append(JsmOpcode(JsmOpcode::PSHN_L, 1));
	result.append(JsmOpcode(JsmOpcode::JPF, 2));
	result.append(JsmOpcode(JsmOpcode::JMP, -2));
	return true;
}

bool JsmPseudoCompiler::parseRepeat(JsmData &result, QString &errorStr)
{
	advance(); // consume 'repeat'

	JsmData bodyBlock;
	int dummy;
	if (!parseStatements(bodyBlock, errorStr, dummy, {"until"})) return false;

	skipNewlines();
	if (peek().text.toLower() != "until") {
		errorStr = QObject::tr("Expected 'until' to close repeat block");
		return false;
	}
	advance();

	JsmData condData;
	if (!parseExpression(condData, errorStr)) return false;

	skipNewlines();
	if (peek().text.toLower() == "end") {
		advance();
	}

	// repeat ... until <cond> end
	// Emit: body, condition, JPF back to body start
	int bodySize = bodyBlock.nbOpcode();
	int condSize = condData.nbOpcode();
	result += bodyBlock;
	result += condData;
	result.append(JsmOpcode(JsmOpcode::JPF, -(bodySize + condSize)));

	return true;
}

// ============================================================
// Expression parsing (recursive descent with precedence)
// ============================================================

bool JsmPseudoCompiler::parseExpression(JsmData &result, QString &errorStr)
{
	return parseLogicalOr(result, errorStr);
}

bool JsmPseudoCompiler::parseLogicalOr(JsmData &result, QString &errorStr)
{
	if (!parseLogicalAnd(result, errorStr)) return false;
	while (peek().type == Token::OpLogOr) {
		advance();
		JsmData right;
		if (!parseLogicalAnd(right, errorStr)) return false;
		// We can't directly emit LogOr as a CAL — it's a virtual op in the decompiler
		// Instead: left, right, OR (bitwise) — close enough for most uses
		// Actually the decompiler uses LogOr/LogAnd as virtual ops. For compilation,
		// we use the pattern: left != 0, right != 0, OR
		result += right;
		result.append(JsmOpcode(JsmOpcode::CAL, 13)); // OR
		return true;
	}
	return true;
}

bool JsmPseudoCompiler::parseLogicalAnd(JsmData &result, QString &errorStr)
{
	if (!parseBitwiseOr(result, errorStr)) return false;
	while (peek().type == Token::OpLogAnd) {
		advance();
		JsmData right;
		if (!parseBitwiseOr(right, errorStr)) return false;
		result += right;
		result.append(JsmOpcode(JsmOpcode::CAL, 12)); // AND
	}
	return true;
}

bool JsmPseudoCompiler::parseBitwiseOr(JsmData &result, QString &errorStr)
{
	if (!parseBitwiseEor(result, errorStr)) return false;
	while (peek().type == Token::OpOr) {
		advance();
		JsmData right;
		if (!parseBitwiseEor(right, errorStr)) return false;
		result += right;
		result.append(JsmOpcode(JsmOpcode::CAL, 13)); // OR
	}
	return true;
}

bool JsmPseudoCompiler::parseBitwiseEor(JsmData &result, QString &errorStr)
{
	if (!parseBitwiseAnd(result, errorStr)) return false;
	while (peek().type == Token::OpEor) {
		advance();
		JsmData right;
		if (!parseBitwiseAnd(right, errorStr)) return false;
		result += right;
		result.append(JsmOpcode(JsmOpcode::CAL, 14)); // EOR
	}
	return true;
}

bool JsmPseudoCompiler::parseBitwiseAnd(JsmData &result, QString &errorStr)
{
	if (!parseEquality(result, errorStr)) return false;
	while (peek().type == Token::OpAnd) {
		advance();
		JsmData right;
		if (!parseEquality(right, errorStr)) return false;
		result += right;
		result.append(JsmOpcode(JsmOpcode::CAL, 12)); // AND
	}
	return true;
}

bool JsmPseudoCompiler::parseEquality(JsmData &result, QString &errorStr)
{
	if (!parseRelational(result, errorStr)) return false;
	while (peek().type == Token::OpEq || peek().type == Token::OpNt) {
		Token op = advance();
		JsmData right;
		if (!parseRelational(right, errorStr)) return false;
		result += right;
		result.append(JsmOpcode(JsmOpcode::CAL, op.type == Token::OpEq ? 6 : 11));
	}
	return true;
}

bool JsmPseudoCompiler::parseRelational(JsmData &result, QString &errorStr)
{
	if (!parseShift(result, errorStr)) return false;
	while (peek().type == Token::OpGt || peek().type == Token::OpGe ||
	       peek().type == Token::OpLs || peek().type == Token::OpLe) {
		Token op = advance();
		JsmData right;
		if (!parseShift(right, errorStr)) return false;
		result += right;
		int calOp;
		switch (op.type) {
		case Token::OpGt: calOp = 7; break;
		case Token::OpGe: calOp = 8; break;
		case Token::OpLs: calOp = 9; break;
		default:          calOp = 10; break; // LE
		}
		result.append(JsmOpcode(JsmOpcode::CAL, calOp));
	}
	return true;
}

bool JsmPseudoCompiler::parseShift(JsmData &result, QString &errorStr)
{
	if (!parseAdditive(result, errorStr)) return false;
	while (peek().type == Token::OpRsh || peek().type == Token::OpLsh) {
		Token op = advance();
		JsmData right;
		if (!parseAdditive(right, errorStr)) return false;
		result += right;
		result.append(JsmOpcode(JsmOpcode::CAL, op.type == Token::OpRsh ? 16 : 17));
	}
	return true;
}

bool JsmPseudoCompiler::parseAdditive(JsmData &result, QString &errorStr)
{
	if (!parseMultiplicative(result, errorStr)) return false;
	while (peek().type == Token::OpAdd || peek().type == Token::OpSub) {
		Token op = advance();
		JsmData right;
		if (!parseMultiplicative(right, errorStr)) return false;
		result += right;
		result.append(JsmOpcode(JsmOpcode::CAL, op.type == Token::OpAdd ? 0 : 1));
	}
	return true;
}

bool JsmPseudoCompiler::parseMultiplicative(JsmData &result, QString &errorStr)
{
	if (!parseUnary(result, errorStr)) return false;
	while (peek().type == Token::OpMul || peek().type == Token::OpDiv || peek().type == Token::OpMod) {
		Token op = advance();
		JsmData right;
		if (!parseUnary(right, errorStr)) return false;
		result += right;
		int calOp;
		switch (op.type) {
		case Token::OpMul: calOp = 2; break;
		case Token::OpDiv: calOp = 3; break;
		default:           calOp = 4; break; // MOD
		}
		result.append(JsmOpcode(JsmOpcode::CAL, calOp));
	}
	return true;
}

bool JsmPseudoCompiler::parseUnary(JsmData &result, QString &errorStr)
{
	if (peek().type == Token::OpSub) {
		advance();
		if (!parseUnary(result, errorStr)) return false;
		result.append(JsmOpcode(JsmOpcode::CAL, 5)); // MIN (negate)
		return true;
	}
	if (peek().type == Token::OpNot) {
		advance();
		if (!parseUnary(result, errorStr)) return false;
		result.append(JsmOpcode(JsmOpcode::CAL, 15)); // NOT (bitwise)
		return true;
	}
	if (peek().type == Token::OpLogNot) {
		advance();
		if (!parseUnary(result, errorStr)) return false;
		// Logical not: compare with 0 using EQ
		result.append(JsmOpcode(JsmOpcode::PSHN_L, 0));
		result.append(JsmOpcode(JsmOpcode::CAL, 6)); // EQ
		return true;
	}
	return parsePrimary(result, errorStr);
}

bool JsmPseudoCompiler::parsePrimary(JsmData &result, QString &errorStr)
{
	Token t = peek();

	// Parenthesized expression
	if (t.type == Token::LParen) {
		advance();
		if (!parseExpression(result, errorStr)) return false;
		return expect(Token::RParen, errorStr);
	}

	// Numeric literals
	if (t.type == Token::Number) {
		advance();
		result.append(JsmOpcode(JsmOpcode::PSHN_L, t.text.toInt()));
		return true;
	}
	if (t.type == Token::HexNumber) {
		advance();
		bool ok;
		int val = t.text.toInt(&ok, 16);
		if (!ok) val = (int)t.text.toUInt(&ok, 16);
		result.append(JsmOpcode(JsmOpcode::PSHN_L, val));
		return true;
	}
	if (t.type == Token::BinNumber) {
		advance();
		bool ok;
		int val = t.text.mid(1).toInt(&ok, 2); // skip 'b' prefix
		result.append(JsmOpcode(JsmOpcode::PSHN_L, val));
		return true;
	}

	// Identifier: variable, function call, or named constant
	if (t.type == Token::Identifier) {
		Token ident = advance();
		QString lower = ident.text.toLower();

		// Named constants from pseudo-code: text_N, map_N, item_N, magic_N
		// These are just numeric values in the opcode stream
		QRegularExpression constRe("^(text|map|item|magic)_(\\d+)$");
		QRegularExpressionMatch cm = constRe.match(lower);
		if (cm.hasMatch()) {
			int val = cm.captured(2).toInt();
			result.append(JsmOpcode(JsmOpcode::PSHN_L, val));
			return true;
		}

		// Key constants
		if (lower == "keyl1") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 0x1)); return true; }
		if (lower == "keyr1") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 0x2)); return true; }
		if (lower == "keyl2") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 0x4)); return true; }
		if (lower == "keyr2") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 0x8)); return true; }
		if (lower == "keycancel") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 0x10)); return true; }
		if (lower == "keymenu") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 0x20)); return true; }
		if (lower == "keychoose") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 0x40)); return true; }
		if (lower == "keycard") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 0x80)); return true; }
		if (lower == "keyselect") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 0x100)); return true; }
		if (lower == "keystart") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 0x800)); return true; }
		if (lower == "keyup") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 0x1000)); return true; }
		if (lower == "keyright") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 0x2000)); return true; }
		if (lower == "keydown") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 0x4000)); return true; }
		if (lower == "keyleft") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 0x8000)); return true; }

		// Character name constants
		if (lower == "squall") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 0)); return true; }
		if (lower == "zell") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 1)); return true; }
		if (lower == "irvine") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 2)); return true; }
		if (lower == "quistis") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 3)); return true; }
		if (lower == "rinoa") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 4)); return true; }
		if (lower == "selphie") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 5)); return true; }
		if (lower == "seifer") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 6)); return true; }
		if (lower == "edea") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 7)); return true; }
		if (lower == "laguna") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 8)); return true; }
		if (lower == "kiros") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 9)); return true; }
		if (lower == "ward") { result.append(JsmOpcode(JsmOpcode::PSHN_L, 10)); return true; }

		// Function call
		if (peek().type == Token::LParen) {
			return parseFunctionCall(ident.text, result, errorStr, false);
		}

		// Variable reference
		return emitPushVar(ident.text, result, errorStr);
	}

	errorStr = QObject::tr("Unexpected token: '%1'").arg(t.text.isEmpty() ? "end of input" : t.text);
	return false;
}
