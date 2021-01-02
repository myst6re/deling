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
#ifndef FF8FONT_H
#define FF8FONT_H

#include "files/TdwFile.h"

class FF8Font
{
public:
	FF8Font(TdwFile *tdw, const QByteArray &txtFileData);
	TdwFile *tdw() const;
	const QList<QStringList> &tables() const;
	void setTables(const QList<QStringList> &tables);
	void setChar(int tableId, int charId, const QString &c);
	bool isValid() const;
	bool isModified() const;
	void setModified(bool modified);
	const QString &name() const;
	void setName(const QString &name);
	void setPaths(const QString &txtPath, const QString &tdwPath);
	const QString &txtPath() const;
	const QString &tdwPath() const;
	void setReadOnly(bool ro);
	bool isReadOnly() const;
	QString saveTxt();

	static bool listFonts();
	static QStringList fontList();
	static void registerFont(const QString &name, FF8Font *font);
	static void deregisterFont(const QString &name);
	static FF8Font *font(QString name);
	static FF8Font *getCurrentConfigFont();
	static bool saveFonts();
	static bool copyFont(const QString &name, const QString &from, const QString &name2);
	static bool removeFont(const QString &name);
	static const QString &fontDirPath();
private:
	void openTxt(const QString &data);
	void print();
	TdwFile *_tdw;
	QString _name;
	QString _txtPath, _tdwPath;
	bool modified, readOnly;
	QList<QStringList> _tables;

	static FF8Font *openFont(const QString &tdwPath, const QString &txtPath);
	static QString font_dirPath;
	static QMap<QString, FF8Font *> fonts;
};

#endif // FF8FONT_H
