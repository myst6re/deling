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

#include <QRegularExpression>
#include <QtGlobal>

#if QT_VERSION < QT_VERSION_CHECK(6, 6, 0)

// --- Custom enum to replace QRegularExpression::WildcardConversionOption ---
enum RegexWildcardConversionOption {
    RegexNoWildcardConversion = 0x0,
    RegexUnanchoredWildcardConversion = 0x1,
    RegexNonPathWildcardConversion = 0x2
};

Q_DECLARE_FLAGS(RegexWildcardConversionOptions, RegexWildcardConversionOption)
Q_DECLARE_OPERATORS_FOR_FLAGS(RegexWildcardConversionOptions)

QRegularExpression fromWildcard(
    const QString &wildcard,
    Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive,
    RegexWildcardConversionOptions options = RegexNoWildcardConversion
);

QString wildcardToRegularExpression(
    const QString &wildcard,
    RegexWildcardConversionOptions options = RegexNoWildcardConversion
);

using WildcardConversionOptions = RegexWildcardConversionOptions;
constexpr auto NonPathWildcardConversion = RegexNonPathWildcardConversion;

#else

// Qt ≥ 6.6 — Use built-in Qt API
inline QRegularExpression fromWildcard(
    const QString &wildcard,
    Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive,
    QRegularExpression::WildcardConversionOptions options = QRegularExpression::WildcardConversionOptions()
) {
    return QRegularExpression::fromWildcard(wildcard, caseSensitivity, options);
}

inline QString wildcardToRegularExpression(
    const QString &wildcard,
    QRegularExpression::WildcardConversionOptions options = QRegularExpression::WildcardConversionOptions()
) {
    return QRegularExpression::wildcardToRegularExpression(wildcard, options);
}

using WildcardConversionOptions = QRegularExpression::WildcardConversionOptions;
constexpr auto NonPathWildcardConversion = QRegularExpression::NonPathWildcardConversion;

#endif // QT_VERSION
