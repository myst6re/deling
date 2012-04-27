#ifndef MSDHIGHLIGHTER_H
#define MSDHIGHLIGHTER_H

#include <QtGui>
#include "FF8Text.h"

class MsdHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
public:
	MsdHighlighter(QTextDocument *parent = 0);

protected:
	void highlightBlock(const QString &text);

private:
	struct HighlightingRule
	{
		QRegExp pattern;
		QColor color;
	};
	QVector<HighlightingRule> highlightingRules;
};

#endif // MSDHIGHLIGHTER_H
