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
#include "ArchiveObserver.h"
#include "Field.h"
#include "files/MchFile.h"

class FieldArchive
{
public:
	enum Sorting{
		SortByName, SortByDesc, SortByMapId
	};

	FieldArchive();
	virtual ~FieldArchive();
	void clearFields();
	const QString &errorMessage() const;
	virtual QString archivePath() const=0;
	virtual Field *getField(int id) const;
	Field *getFieldFromMapId(int mapId) const;
	const QList<Field *> &getFields() const;
	int nbFields() const;
	CharaModel *getModel(int id) const;
	QHash<int, CharaModel *> *getModels();
	bool isReadOnly() const;
	virtual int open(const QString &, ArchiveObserver *progress)=0;
	virtual bool openModels()=0;
	virtual bool openBG(Field *field) const=0;
	bool compileScripts(int &errorFieldID, int &errorGroupID, int &errorMethodID, int &errorLine, QString &errorStr);
	bool searchText(const QRegExp &text, int &fieldID, int &textID, int &from, int &size, Sorting=SortByMapId) const;
	bool searchTextReverse(const QRegExp &text, int &fieldID, int &textID, int &from, int &index, int &size, Sorting=SortByMapId) const;
	bool searchScript(JsmFile::SearchType type, quint64 value, int &fieldID, int &groupID, int &methodID, int &opcodeID, Sorting=SortByMapId) const;
	bool searchScriptText(const QRegExp &text, int &fieldID, int &groupID, int &methodID, int &opcodeID, Sorting=SortByMapId) const;
	bool searchScriptReverse(JsmFile::SearchType type, quint64 value, int &fieldID, int &groupID, int &methodID, int &opcodeID, Sorting=SortByMapId) const;
	bool searchScriptTextReverse(const QRegExp &text, int &fieldID, int &groupID, int &methodID, int &opcodeID, Sorting=SortByMapId) const;
	QMultiMap<int, QString> searchAllVars() const;
	QList<int> searchAllSpells(int fieldID) const;
	QList<int> searchAllCards(int fieldID) const;
	QMap<Field *, QList<int> > searchAllBattles() const;
	QMultiMap<int, Field *> searchAllMoments() const;
	QMap<int, int> searchAllOpcodeTypes() const;
	QList<Vertex_s> searchAllSavePoints() const;
	QStringList fieldList() const;
	const QStringList &mapList() const;
	void setMapList(const QStringList &mapList);
protected:
	QString errorMsg;
	QList<Field *> fields;
	QHash<int, CharaModel *> models;
	QStringList _mapList;
	QMultiMap<QString, int> fieldsSortByName;
	QMultiMap<QString, int> fieldsSortByDesc;
	QMultiMap<QString, int> fieldsSortByMapId;
	bool readOnly;
private:
	bool searchIterators(QMap<QString, int>::const_iterator &i, QMap<QString, int>::const_iterator &end, int fieldID, Sorting sorting) const;
	bool searchIteratorsP(QMap<QString, int>::const_iterator &i, QMap<QString, int>::const_iterator &begin, int fieldID, Sorting sorting) const;
};

#endif // FSARCHIVE_H
