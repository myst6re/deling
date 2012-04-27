#include "MsdHighlighter.h"

MsdHighlighter::MsdHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{
	HighlightingRule rule;

	rule.pattern = QRegExp("\\{x([\\da-fA-F]{2}){1,2}\\}");
	rule.color = Qt::darkRed;
	highlightingRules.append(rule);

	QStringList names;
	names << "\\{Squall\\}" << "\\{Zell\\}" << "\\{Irvine\\}" << "\\{Quistis\\}" << "\\{Rinoa\\}"
		  << "\\{Selphie\\}" << "\\{Seifer\\}" << "\\{Edea\\}" << "\\{Laguna\\}" << "\\{Kiros\\}"
		  << "\\{Ward\\}" << "\\{Angelo\\}" << "\\{Griever\\}" << "\\{Boko\\}"
		  << "\\{Galbadia\\}" << "\\{Esthar\\}" << "\\{Balamb\\}" << "\\{Dollet\\}"
		  << "\\{Timber\\}" << "\\{Trabia\\}" << "\\{Centra\\}" << "\\{Horizon\\}";

	foreach(const QString &name, names) {
		rule.pattern = QRegExp(name);
		rule.color = Qt::darkGreen;
		highlightingRules.append(rule);
	}

	QStringList syst;
	syst << "\\{Darkgrey\\}" << "\\{Grey\\}" << "\\{Yellow\\}" << "\\{Red\\}"
		  << "\\{Green\\}" << "\\{Blue\\}" << "\\{Purple\\}" << "\\{White\\}"
		  << "\\{DarkgreyBlink\\}" << "\\{GreyBlink\\}" << "\\{YellowBlink\\}"
		  << "\\{RedBlink\\}" << "\\{GreenBlink\\}" << "\\{BlueBlink\\}"
		  << "\\{PurpleBlink\\}" << "\\{WhiteBlink\\}" << "\\{Wait\\d\\d\\d\\}"
		  << "\\{Var[0b]?[0-7]\\}" << "^\\{NewPage\\}$" << "\\{jp\\d\\d\\d\\}";

	foreach(const QString &s, syst) {
		rule.pattern = QRegExp(s, Qt::CaseInsensitive);
		rule.color = Qt::darkBlue;
		highlightingRules.append(rule);
	}

	QStringList doublet;
	doublet << "\\{in\\}" << "\\{e \\}" << "\\{ne\\}" << "\\{to\\}" << "\\{re\\}" << "\\{HP\\}" << "\\{l \\}" << "\\{ll\\}" <<
			"\\{GF\\}" << "\\{nt\\}" << "\\{il\\}" << "\\{o \\}" << "\\{ef\\}" << "\\{on\\}" << "\\{ w\\}" << "\\{ r\\}" <<
			"\\{wi\\}" << "\\{fi\\}" << "\\{EC\\}" << "\\{s \\}" << "\\{ar\\}" << "\\{FE\\}" << "\\{ S\\}" << "\\{ag\\}";

	foreach(const QString &d, doublet) {
		rule.pattern = QRegExp(d);
		rule.color = Qt::darkBlue;
		highlightingRules.append(rule);
	}
}

void MsdHighlighter::highlightBlock(const QString &text)
{
	foreach(const HighlightingRule &rule, highlightingRules) {
		QRegExp expression(rule.pattern);
		int index = expression.indexIn(text);
		while(index >= 0) {
			int length = expression.matchedLength();
			setFormat(index, length, rule.color);
			index = expression.indexIn(text, index + length);
		}
	}
}
