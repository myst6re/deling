#ifndef JSMHIGHLIGHTER_H
#define JSMHIGHLIGHTER_H

#include <QtGui>
#include "JsmFile.h"

class JsmHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
public:
	JsmHighlighter(QTextDocument *parent = 0);

protected:
	void highlightBlock(const QString &text);

private:
};

#endif // JSMHIGHLIGHTER_H
