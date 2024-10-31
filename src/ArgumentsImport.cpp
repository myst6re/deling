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
#include "ArgumentsImport.h"

ArgumentsImport::ArgumentsImport() : ArgumentsImportExport()
{
	_ADD_ARGUMENT(_OPTION_NAMES("c", "column"),
	          "Column (starting at 1) to use as text.", "COLUMN", "1");

	_parser.addPositionalArgument("archive", QCoreApplication::translate("ArgumentsImport", "Input Field FS archive."));
	_parser.addPositionalArgument("file", QCoreApplication::translate("ArgumentsImport", "Input CSV file path."));

	parse();
	
	assertParameters();
}

int ArgumentsImport::column() const
{
	if (!_parser.isSet("column")) {
		return 0;
	}

	bool ok = false;
	int ret = _parser.value("column").toInt(&ok);

	if (!ok || ret <= 0) {
		qWarning() << qPrintable(
		    QCoreApplication::translate("Arguments", "Error: column should be an integer value >= 1"));
		exit(1);
	}

	return ret - 1;
}

void ArgumentsImport::parse()
{
	_parser.process(*qApp);

	if (_parser.positionalArguments().size() > 3) {
		qWarning() << qPrintable(
		    QCoreApplication::translate("Arguments", "Error: too much parameters"));
		exit(1);
	}

	QStringList paths = wilcardParse();
	if (paths.size() == 2) {
		_destination = paths.last();
		
		if (!QFile::exists(_destination)) {
			qWarning() << qPrintable(
			    QCoreApplication::translate("Arguments", "Error: CSV file does not exist"));
			exit(1);
		}

		_path = paths.first();
		
		if (!QFile::exists(_path)) {
			qWarning() << qPrintable(
			    QCoreApplication::translate("Arguments", "Error: Field archive does not exist"));
			exit(1);
		}
	}
}
