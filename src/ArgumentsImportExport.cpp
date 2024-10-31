/****************************************************************************
 ** Copyright (C) 2009-2021 Arzel Jérôme <myst6re@gmail.com>
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
#include "ArgumentsImportExport.h"

ArgumentsImportExport::ArgumentsImportExport() : CommonArguments()
{
	_ADD_ARGUMENT(QStringList("separator"),
	          "CSV field separator.", "CHARACTER", ",");
	_ADD_ARGUMENT(QStringList("quote"),
	          "CSV quote character.", "CHARACTER", "\"");
}

QChar ArgumentsImportExport::separator() const
{
	if (!_parser.isSet("separator")) {
		return QChar(',');
	}
	
	QString separator = _parser.value("separator");
	
	if (separator == "\\t") {
		return QChar('\t');
	}
	if (separator == "\\0") {
		return QChar('\0');
	}
	
	if (separator.size() != 1) {
		qWarning() << qPrintable(
		    QCoreApplication::translate("Arguments", "Error: separator value should be specified with exactly one character"));
		exit(1);
	}
	
	if (separator.at(0) == '\n' || separator.at(0) == '\r') {
		qWarning() << qPrintable(
		    QCoreApplication::translate("Arguments", "Error: separator value cannot be a line break character"));
		exit(1);
	}
	
	return separator.at(0);
}

QChar ArgumentsImportExport::quoteCharacter() const
{
	if (!_parser.isSet("quote")) {
		return QChar('"');
	}
	
	QString quote = _parser.value("quote");
	
	if (quote.size() != 1) {
		qWarning() << qPrintable(
		    QCoreApplication::translate("Arguments", "Error: quote character value should be specified with exactly one character"));
		exit(1);
	}
	
	if (quote.at(0) == '\n' || quote.at(0) == '\r') {
		qWarning() << qPrintable(
		    QCoreApplication::translate("Arguments", "Error: quote value cannot be a line break character"));
		exit(1);
	}
	
	return quote.at(0);
}

void ArgumentsImportExport::assertParameters() const
{
	if (separator() == quoteCharacter()) {
		qWarning() << qPrintable(
		    QCoreApplication::translate("Arguments", "Error: separator and quote character cannot be the same character"));
		exit(1);
	}
}
