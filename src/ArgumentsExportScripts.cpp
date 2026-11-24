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
#include "ArgumentsExportScripts.h"

ArgumentsExportScripts::ArgumentsExportScripts() : ArgumentsScripts()
{
	_ADD_FLAG(_OPTION_NAMES("f", "force"),
	          "Overwrite existing script files.");

	_parser.addPositionalArgument("file", QCoreApplication::translate("ArgumentsExportScripts", "Input field FS archive (field.fs) or JSM file (the SYM file next to it is used for names)."));
	_parser.addPositionalArgument("directory", QCoreApplication::translate("ArgumentsExportScripts", "Output directory, one text file per script method: <field>/<groupID>-<group>/<methodID>-<method>.txt"));

	parse();
}

bool ArgumentsExportScripts::force() const
{
	return _parser.isSet("force");
}

void ArgumentsExportScripts::parse()
{
	_parser.process(*qApp);

	if (_parser.positionalArguments().size() > 3) {
		qWarning() << qPrintable(
		    QCoreApplication::translate("Arguments", "Error: too much parameters"));
		exit(1);
	}

	QStringList paths = wilcardParse();
	if (paths.size() == 2) {
		_directory = paths.takeLast();
		_path = paths.first();
	}
}
