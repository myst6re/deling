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
#include "ArgumentsPackUnpack.h"

ArgumentsPackUnpack::ArgumentsPackUnpack() : CommonArguments()
{
	_ADD_ARGUMENT("include", "Include file name (without prefix c:\\ff8\\data\\). Repeat this argument to include multiple names. "
	                         "If empty, all will be included by default.", "include", "");
	_ADD_ARGUMENT("exclude", "Exclude file name (without prefix c:\\ff8\\data\\). Repeat this argument to exclude multiple names. "
	                         "Exclude has the priority over the --include argument.", "exclude", "");
	_ADD_ARGUMENT("include-from", "Include file names from file. The file format is one name per line.", "include", "");
	_ADD_ARGUMENT("exclude-from", "Exclude file names from file. The file format is one name per line.", "exclude", "");
}

QStringList ArgumentsPackUnpack::includes() const
{
	return _parser.values("include") + _includesFromFile;
}

QStringList ArgumentsPackUnpack::excludes() const
{
	return _parser.values("exclude") + _excludesFromFile;
}

QStringList ArgumentsPackUnpack::mapNamesFromFile(const QString &path)
{
	QFile f(path);
	if (!f.open(QIODevice::ReadOnly)) {
		qWarning() << qPrintable(
		    QCoreApplication::translate("Arguments", "Warning: cannot open file"))
		           << qPrintable(path) << qPrintable(f.errorString());
		exit(1);
	}

	QStringList ret;

	while (f.canReadLine()) {
		ret.append(QString::fromUtf8(f.readLine()));
	}

	return ret;
}

void ArgumentsPackUnpack::mapNamesFromFiles()
{
	QStringList pathsInclude = _parser.values("include-from");

	for (const QString &path: pathsInclude) {
		_includesFromFile.append(mapNamesFromFile(path));
	}

	QStringList pathsExclude = _parser.values("exclude-from");

	for (const QString &path: pathsExclude) {
		_excludesFromFile.append(mapNamesFromFile(path));
	}
}
