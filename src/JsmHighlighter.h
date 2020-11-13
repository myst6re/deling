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
#ifndef JSMHIGHLIGHTER_H
#define JSMHIGHLIGHTER_H

#include <QtGui>
#include "files/JsmFile.h"

class JsmHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
public:
	JsmHighlighter(QTextDocument *parent = 0);
	inline void setPseudoCode(bool pseudoCode) {
		_pseudoCode = pseudoCode;
		rehighlight();
	}
	inline bool isPseudoCode() const {
		return _pseudoCode;
	}
protected:
	void highlightBlock(const QString &text);

private:
	void highlightBlockPseudoCode(const QString &text);
	void highlightBlockOpcodes(const QString &text);
	void applyReg(const QString &text, const QRegExp &regExp,
	              const QTextCharFormat &format);
	void applyReg(const QString &text, const QRegExp &regExp,
	              const QColor &color);

	bool _pseudoCode;
	QRegExp _regKeywords, _regNumeric, _regVar, _regConst, _regExec;
};

#endif // JSMHIGHLIGHTER_H
