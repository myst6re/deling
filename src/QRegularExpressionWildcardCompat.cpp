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

#include "QRegularExpressionWildcardCompat.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 6, 0)

QRegularExpression fromWildcard(
    const QString &wildcard,
    Qt::CaseSensitivity caseSensitivity,
    RegexWildcardConversionOptions options)
{
    QString pattern = wildcardToRegularExpression(wildcard, options);

    QRegularExpression::PatternOptions reOptions = QRegularExpression::NoPatternOption;
    if (caseSensitivity == Qt::CaseInsensitive)
        reOptions |= QRegularExpression::CaseInsensitiveOption;

    return QRegularExpression(pattern, reOptions);
}

QString wildcardToRegularExpression(
    const QString &wildcard,
    RegexWildcardConversionOptions options)
{
    const bool unanchored = options.testFlag(RegexUnanchoredWildcardConversion);
    const bool nonPath = options.testFlag(RegexNonPathWildcardConversion);

    QString pattern;
    pattern.reserve(wildcard.size() * 2);

    for (QChar ch : wildcard) {
        switch (ch.unicode()) {
            case '*':
                pattern += nonPath ? ".*" : "[^/]*";
                break;
            case '?':
                pattern += nonPath ? "." : "[^/]";
                break;
            case '.': case '\\': case '+': case '{': case '}':
            case '[': case ']': case '(': case ')': case '^':
            case '$': case '|':
                pattern += '\\';
                pattern += ch;
                break;
            default:
                pattern += ch;
                break;
        }
    }

    if (!unanchored) {
        pattern.prepend("^");
        pattern.append("$");
    }

    return pattern;
}

#endif // QT_VERSION < 6.6
