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
#ifndef FSARCHIVE_H
#define FSARCHIVE_H

#include <QtCore>
#include <QtGui/QProgressDialog>
#include "parameters.h"
#include "LZS.h"
#include "Data.h"
#include "Field.h"
#include "files/MchFile.h"

class FieldArchive
{
public:
	enum Sorting{
		SortByName, SortByDesc, SortByMapId
	};

	FieldArchive();
	~FieldArchive();
	void clearFields();
	const QString &errorMessage() const;
	virtual QString archivePath() const=0;
	Field *getField(int id) const;
	const QList<Field *> &getFields() const;
	int nbFields() const;
	CharaModel *getModel(int id) const;
	QHash<int, CharaModel *> *getModels();
	bool isReadOnly() const;
	virtual int open(const QString &, QProgressDialog *progress)=0;
	virtual bool openModels()=0;
	virtual bool openBG(Field *field) const=0;
	bool compileScripts(int &errorFieldID, int &errorGroupID, int &errorMethodID, int &errorLine, QString &errorStr);
	int searchText(const QString &text, int &fieldID, int &textID, int from=0, Sorting=SortByMapId, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool regExp=false) const;
	int searchTextReverse(const QString &text, int &fieldID, int &textID, int from=0, Sorting=SortByMapId, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool regExp=false) const;
	bool searchScript(quint8 type, quint64 value, int &fieldID, int &groupID, int &methodID, int &opcodeID, Sorting=SortByMapId) const;
	bool searchScriptText(const QString &text, Qt::CaseSensitivity sensitivity, int &fieldID, int &groupID, int &methodID, int &opcodeID, Sorting=SortByMapId) const;
	bool searchScriptReverse(quint8 type, quint64 value, int &fieldID, int &groupID, int &methodID, int &opcodeID, Sorting=SortByMapId) const;
	bool searchScriptTextReverse(const QString &text, Qt::CaseSensitivity sensitivity, int &fieldID, int &groupID, int &methodID, int &opcodeID, Sorting=SortByMapId) const;
	QMultiMap<int, QString> searchAllVars() const;
	QList<int> searchAllSpells(int fieldID) const;
	QMap<Field *, QList<int> > searchAllBattles() const;
	QMultiMap<int, Field *> searchAllMoments() const;
	QMap<int, int> searchAllOpcodeTypes() const;
	QStringList fieldList() const;
	const QStringList &mapList() const;
	void setMapList(QStringList mapList);
protected:
	QString errorMsg;
	QList<Field *> fields;
	QHash<int, CharaModel *> models;
	QStringList _mapList;
	QMultiMap<QString, int> fieldsSortByName;
	QMultiMap<QString, int> fieldsSortByDesc;
	QMultiMap<QString, int> fieldsSortByMapId;
	bool readOnly;
};

#endif // FSARCHIVE_H
