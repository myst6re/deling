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

#include <QtCore>

class JsmFile;

// Scripts of a JSM file as text files, one per method:
// <dir>/<groupID>-<group name>/<methodID>-<method name>.txt
// The IDs identify the method, the names are only there to be read by humans.
class ScriptFiles
{
public:
	enum Format {
		PseudoCode, // Same text as the "pseudo-code" view of Deling
		Opcodes     // Same text as the opcode view of Deling
	};

	explicit ScriptFiles(Format format);
	bool toDir(JsmFile *jsm, const QDir &dir, bool overwrite);
	// Compiles the files that differ from the current scripts.
	// Returns the number of compiled methods, or -1 on error (the JSM file can be partially modified then)
	int fromDir(JsmFile *jsm, const QDir &dir);
	QString methodText(JsmFile *jsm, int groupID, int methodID) const;
	static QString entryName(int id, const QString &name);
	// Compiled methods and warnings of the last fromDir()
	inline const QStringList &messages() const {
		return _messages;
	}
	inline const QString &errorString() const {
		return _lastErrorString;
	}
private:
	static QString normalized(const QString &text);
	static bool listEntries(const QDir &dir, QDir::Filters filters, const QStringList &nameFilters,
	                        QMap<int, QString> &entries, QString &errorStr);

	Format _format;
	QStringList _messages;
	QString _lastErrorString;
};
