/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2012 Arzel Jérôme <myst6re@gmail.com>
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
#include "JsmHighlighter.h"

JsmHighlighter::JsmHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{
	QStringList keywords;
	keywords << "if" << "begin" << "else" << "end"
	         << "while" << "wait while" << "forever"
	         << "wait forever" << "repeat" << "until" << "ret"
	         << "goto" << "label";
	// Don't forget to escape strings if necessary
	_regKeywords = QRegExp(QString("\\b(%1)\\b")
	                       .arg(keywords.join("|")));
	_regNumeric = QRegExp("\\b-?(b[01]+|0x[\\da-fA-F]+|\\d+)\\b");
	_regVar = QRegExp("\\b((model|temp)_\\d+|\\w+_[us](byte|word|long))\\b");
	_regConst = QRegExp("\\b((text|map|item|magic)_\\d+|[A-Z][a-zA-Z\\d]+)\\b");
	_regExec = QRegExp("\\b([\\w#]+\\.\\w+|req|reqsw|reqew|preq|preqsw|preqew)\\b");
}

void JsmHighlighter::applyReg(const QString &text, const QRegExp &regExp,
                              const QTextCharFormat &format)
{
	int index = regExp.indexIn(text);
	while (index >= 0) {
		int length = regExp.matchedLength();
		setFormat(index, length, format);
		index = regExp.indexIn(text, index + length);
	}
}

void JsmHighlighter::applyReg(const QString &text, const QRegExp &regExp,
                              const QColor &color)
{
	int index = regExp.indexIn(text);
	while (index >= 0) {
		int length = regExp.matchedLength();
		setFormat(index, length, color);
		index = regExp.indexIn(text, index + length);
	}
}

void JsmHighlighter::highlightBlock(const QString &text)
{
	if(_pseudoCode) {
		highlightBlockPseudoCode(text);
	} else {
		highlightBlockOpcodes(text);
	}
}

void JsmHighlighter::highlightBlockPseudoCode(const QString &text)
{
	// Keywords
	applyReg(text, _regKeywords, QColor(0x80, 0x80, 0x00)); // Yellow

	// Types
	applyReg(text, _regNumeric, QColor(0x00, 0x00, 0x80)); // blue
	applyReg(text, _regConst, QColor(0x00, 0x00, 0x80)); // blue
	applyReg(text, _regVar, QColor(0x80, 0x00, 0x00)); // red

	// Methods
	applyReg(text, _regExec, QColor(0x80, 0x00, 0x80)); // purple
}

void JsmHighlighter::highlightBlockOpcodes(const QString &text)
{
	QStringList rows = text.split(QRegExp("[\\t ]+"), QString::SkipEmptyParts);
	bool ok;

	if(rows.isEmpty()) {
		return;
	}

	const QString &name = rows.first();
	int opcode = JsmFile::opcodeName.indexOf(name.toUpper());

	if(opcode != -1) {
		if(opcode == JsmOpcode::CAL) {
			setFormat(text.indexOf(name), name.size(), QColor(0x00,0x66,0xcc));
		} else if(opcode >= JsmOpcode::JMP && opcode <= JsmOpcode::GJMP) {
			setFormat(text.indexOf(name), name.size(), QColor(0x66,0xcc,0x00));
		} else if(opcode == JsmOpcode::LBL) {
			setFormat(text.indexOf(name), name.size(), QColor(0xcc,0x00,0x00));
		} else if(opcode >= JsmOpcode::RET && opcode <= JsmOpcode::PSHAC) {
			setFormat(text.indexOf(name), name.size(), QColor(0x66,0x66,0x66));
		} else if(opcode >= JsmOpcode::REQ && opcode <= JsmOpcode::PREQEW) {
			setFormat(text.indexOf(name), name.size(), QColor(0xcc,0x66,0x00));
		} else if(opcode == JsmOpcode::MES || opcode == JsmOpcode::ASK
				  || opcode == JsmOpcode::AMESW || opcode == JsmOpcode::AMES
				  || opcode == JsmOpcode::AASK || opcode == JsmOpcode::RAMESW) {
			QTextCharFormat textFormat;
			textFormat.setBackground(QColor(0xFF,0xFF,0x00));
			setFormat(text.indexOf(name), name.size(), textFormat);
		}
	} else if(name.startsWith("LABEL", Qt::CaseInsensitive)) {
		name.mid(5).toInt(&ok);
		if(ok) {
			setFormat(text.indexOf(name), name.size(), QColor(0x66,0xcc,0x00));
		}
		return;
	}

	if(rows.size() == 1) {
		return;
	}

	const QString &param = rows.at(1);

	if(opcode == JsmOpcode::CAL && JsmFile::opcodeNameCalc.contains(param.toUpper())) {
		setFormat(text.indexOf(param), param.size(), QColor(0x00,0x66,0xcc));
	} else if(opcode >= JsmOpcode::JMP && opcode <= JsmOpcode::GJMP
	          && param.startsWith("LABEL", Qt::CaseInsensitive)) {
		param.mid(5).toInt(&ok);
		if(ok) {
			setFormat(text.indexOf(param), param.size(), QColor(0x66,0xcc,0x00));
		}
	} else if(opcode >= JsmOpcode::PSHI_L && opcode <= JsmOpcode::POPI_L
	          && param.startsWith("TEMP", Qt::CaseInsensitive)) {
		param.mid(4).toInt(&ok);
		if(ok) {
			setFormat(text.indexOf(param), param.size(), QColor(0x66,0x00,0xcc));
		}
	} else if(opcode >= JsmOpcode::PSHM_B && opcode <= JsmOpcode::PSHSM_L
	          && param.startsWith("VAR", Qt::CaseInsensitive)) {
		param.mid(3).toInt(&ok);
		if(ok) {
			setFormat(text.indexOf(param), param.size(), QColor(0x66,0x00,0xcc));
		}
	} else if(opcode == JsmOpcode::PSHAC
	          && param.startsWith("MODEL", Qt::CaseInsensitive)) {
		param.mid(5).toInt(&ok);
		if(ok) {
			setFormat(text.indexOf(param), param.size(), QColor(0x66,0x00,0xcc));
		}
	} else {
//		param.toInt(&ok);
//		if(ok) {
//			setFormat(text.indexOf(param), param.size(), QColor(0x00,0x66,0xcc));
//		}
	}
}
