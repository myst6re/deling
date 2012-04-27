#include "JsmHighlighter.h"

JsmHighlighter::JsmHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{
}

void JsmHighlighter::highlightBlock(const QString &text)
{
	QStringList rows = text.split(QRegExp("[\\t ]+"), QString::SkipEmptyParts);
	int opcode;
	bool ok;

	if(rows.isEmpty())	return;

	QString name=rows.first();

	if((opcode = JsmFile::opcodeName.indexOf(name.toUpper())) != -1) {
		if(opcode==JsmOpcode::CAL) {
			setFormat(text.indexOf(name), name.size(), QColor(0x00,0x66,0xcc));
		}
		else {
			if(opcode>0x01 && opcode<0x05) {
				setFormat(text.indexOf(name), name.size(), QColor(0x66,0xcc,0x00));
			}

			if(opcode==JsmOpcode::LBL) {
				setFormat(text.indexOf(name), name.size(), QColor(0xcc,0x00,0x00));
			}
			else if(opcode>0x05 && opcode<20)
				setFormat(text.indexOf(name), name.size(), QColor(0x66,0x66,0x66));
			else if(opcode>19 && opcode<26)
				setFormat(text.indexOf(name), name.size(), QColor(0xcc,0x66,0x00));
			else if(opcode==JsmOpcode::MES || opcode==JsmOpcode::ASK || opcode==JsmOpcode::AMESW
					|| opcode==JsmOpcode::AMES || opcode==JsmOpcode::AASK || opcode==JsmOpcode::RAMESW) {
				QTextCharFormat textFormat;
				textFormat.setBackground(QColor(0xFF,0xFF,0x00));
				setFormat(text.indexOf(name), name.size(), textFormat);
			}
		}
	} else if(name.startsWith("LABEL", Qt::CaseInsensitive)) {
		name.mid(5).toInt(&ok);
		if(ok) {
			setFormat(text.indexOf(name), name.size(), QColor(0x66,0xcc,0x00));
		}
		return;
	}

	if(rows.size()==1)	return;

	QString param=rows.at(1);

	if(opcode==JsmOpcode::CAL && JsmFile::opcodeNameCalc.contains(param.toUpper())) {
		setFormat(text.indexOf(param), param.size(), QColor(0x00,0x66,0xcc));
	} else if(opcode>1 && opcode<5 && param.startsWith("LABEL", Qt::CaseInsensitive)) {
		param.mid(5).toInt(&ok);
		if(ok) {
			setFormat(text.indexOf(param), param.size(), QColor(0x66,0xcc,0x00));
		}
	} else {
//		param.toInt(&ok);
//		if(ok) {
//			setFormat(text.indexOf(param), param.size(), QColor(0x00,0x66,0xcc));
//		}
	}
}
