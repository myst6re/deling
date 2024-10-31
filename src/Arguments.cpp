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
#include "Arguments.h"

HelpArguments::HelpArguments()
{
	_ADD_FLAG(_OPTION_NAMES("h", "help"), "Displays help.");
}

bool HelpArguments::help() const
{
	return _parser.isSet("help");
}

[[ noreturn ]] void HelpArguments::showHelp(int exitCode)
{
	QRegularExpression usage("Usage: .* \\[options\\]");
	qInfo("%s", qPrintable(_parser.helpText().replace(usage, "Usage: %1 [options]")
	                     .arg(QFileInfo(qApp->arguments().first()).fileName())));
	::exit(exitCode);
}

CommonArguments::CommonArguments()
{
	_ADD_ARGUMENT("input-format", "Input format (fs).", "input-format", "");
	_ADD_FLAG("no-progress", "Hide progress bar.");
}

QString CommonArguments::inputFormat() const
{
	QString inputFormat = _parser.value("input-format");
	if(inputFormat.isEmpty() && !_path.isEmpty()) {
		qsizetype index = _path.lastIndexOf('.');
		if(index > -1) {
			return _path.mid(index + 1);
		}
		return QString();
	}
	return inputFormat;
}

bool CommonArguments::noProgress() const
{
	return _parser.isSet("no-progress");
}

QStringList CommonArguments::searchFiles(const QString &path)
{
	qsizetype index = path.lastIndexOf('/');
	QString dirname, filename;

	if (index > 0) {
		dirname = path.left(index);
		filename = path.mid(index + 1);
	} else {
		filename = path;
	}

	QDir dir(dirname);
	QStringList entryList = dir.entryList(QStringList(filename), QDir::Files);
	int i = 0;
	for (const QString &entry: entryList) {
		entryList.replace(i++, dir.filePath(entry));
	}
	return entryList;
}

QStringList CommonArguments::wilcardParse()
{
	QStringList paths, args = _parser.positionalArguments();

	args.removeFirst();

	for (const QString &path: args) {
		if (path.contains('*') || path.contains('?')) {
			paths.append(searchFiles(QDir::fromNativeSeparators(path)));
		} else {
			paths.append(QDir::fromNativeSeparators(path));
		}
	}

	return paths;
}

Arguments::Arguments() :
      _command(None)
{
	_parser.addVersionOption();
	_parser.addPositionalArgument(
	    "command", QCoreApplication::translate("Arguments", "Command to execute"), "<command>"
	);
	_parser.addPositionalArgument("args", "", "[<args>]");
	_parser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
	_parser.setApplicationDescription(
	    QCoreApplication::translate(
	        "Arguments",
	        "\nList of available commands:\n"
	        "  unpack           Unpack files from FS archive\n"
	        "  pack             Pack files from directory to FS archive\n"
	        "  export-texts     Export texts to CSV from FIELD FS archive\n"
	        "  import-texts     Import texts from a CSV file to existing FIELD FS archive\n"
	        "\n"
	        "\"%1 unpack --help\" to see help of the specific subcommand"
	    ).arg(QFileInfo(qApp->arguments().first()).fileName())
	);

	parse();
}

void Arguments::parse()
{
	_parser.process(*qApp);

	QStringList args = _parser.positionalArguments();

	if (args.isEmpty()) {
		qWarning() << qPrintable(QCoreApplication::translate("Arguments", "Please specify a command"));
		return;
	}

	const QString &command = args.first();

	if (command == "export-texts") {
		_command = Export;
	} else if (command == "import-texts") {
		_command = Import;
	} else if (command == "unpack") {
		_command = Unpack;
	} else if (command == "pack") {
		_command = Pack;
	} else {
		qWarning() << qPrintable(QCoreApplication::translate("Arguments", "Unknown command type:")) << qPrintable(command);
		return;
	}
}
