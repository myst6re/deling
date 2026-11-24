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
#include "ArgumentsImportScripts.h"

ArgumentsImportScripts::ArgumentsImportScripts() : ArgumentsScripts()
{
	_ADD_FLAG("dry-run",
	          "Compile the scripts and report errors, without saving anything.");

	_parser.addPositionalArgument("file", QCoreApplication::translate("ArgumentsImportScripts", "Field FS archive (field.fs) or JSM file to modify."));
	_parser.addPositionalArgument("directory", QCoreApplication::translate("ArgumentsImportScripts", "Directory created by export-scripts. Only the files that differ from the current scripts are compiled."));

	parse();
}

bool ArgumentsImportScripts::dryRun() const
{
	return _parser.isSet("dry-run");
}

void ArgumentsImportScripts::parse()
{
	_parser.process(*qApp);

	if (_parser.positionalArguments().size() > 3) {
		qWarning() << qPrintable(
		    QCoreApplication::translate("Arguments", "Error: too much parameters"));
		exit(1);
	}

	QStringList paths = wilcardParse();
	if (paths.size() == 2) {
		if (QDir(paths.last()).exists()) {
			_directory = paths.takeLast();
		} else {
			qWarning() << qPrintable(
			    QCoreApplication::translate("Arguments", "Error: scripts directory does not exist:"))
			           << qPrintable(paths.last());
			exit(1);
		}
		_path = paths.first();
	}
}
