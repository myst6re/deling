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
#include "FieldArchive.h"
#include "Data.h"
#include "Field.h"

FieldArchive::FieldArchive()
	: readOnly(false)
{
}

FieldArchive::~FieldArchive()
{
	for (Field *field: fields)		delete field;
	for (CharaModel *model: models)	delete model;
}

void FieldArchive::clearFields()
{
	for (Field *field: fields)	delete field;
	fields.clear();
	for (CharaModel *model: models)	delete model;
	models.clear();
	fieldsSortByName.clear();
	fieldsSortByDesc.clear();
	fieldsSortByMapId.clear();
}

const QString &FieldArchive::errorMessage() const
{
	return errorMsg;
}

Field *FieldArchive::getField(int id) const
{
//	Field *field = fields.value(id, NULL);
//	if (field == NULL)	return NULL;
//	if (!field->isOpen() && openField(field)) {
//		return field;
//	}
//	if (!field->isOpen())	return NULL;

	return fields.value(id, nullptr);
}

Field *FieldArchive::getFieldFromMapId(int mapId) const
{
	int fieldId = fieldsSortByMapId.value(QString("%1").arg(mapId, 3, 10, QChar('0')), -1);

	return getField(fieldId);
}

const QList<Field *> &FieldArchive::getFields() const
{
	return fields;
}

int FieldArchive::nbFields() const
{
	return fields.size();
}

CharaModel *FieldArchive::getModel(int id) const
{
	return models.value(id, nullptr);
}

QHash<int, CharaModel *> *FieldArchive::getModels()
{
	return &models;
}

bool FieldArchive::isReadOnly() const
{
	return readOnly;
}

bool FieldArchive::compileScripts(int &errorFieldID, int &errorGroupID, int &errorMethodID, int &errorLine, QString &errorStr)
{
	errorFieldID = 0;

	for (Field *field: fields) {
		if (field->isOpen() && field->isModified() && field->hasJsmFile()) {
			if (!field->getJsmFile()->compileAll(errorGroupID, errorMethodID, errorLine, errorStr)) {
				return false;
			}
		}
		++errorFieldID;
	}

	return true;
}

bool FieldArchive::searchIterators(QMultiMap<QString, int>::const_iterator &i, QMultiMap<QString, int>::const_iterator &end, int fieldID, Sorting sorting) const
{
	if (fieldID >= fields.size())		return false;

	switch (sorting) {
	case SortByName:
		i = fieldsSortByName.constFind(fieldsSortByName.key(fieldID), fieldID);
		end = fieldsSortByName.constEnd();
		if (i==end) {
			i = fieldsSortByName.constBegin();
		}
		return true;
	case SortByDesc:
		i = fieldsSortByDesc.constFind(fieldsSortByDesc.key(fieldID), fieldID);
		end = fieldsSortByDesc.constEnd();
		if (i==end) {
			i = fieldsSortByDesc.constBegin();
		}
		return true;
	case SortByMapId:
		i = fieldsSortByMapId.constFind(fieldsSortByMapId.key(fieldID), fieldID);
		end = fieldsSortByMapId.constEnd();
		if (i==end) {
			i = fieldsSortByMapId.constBegin();
		}
		return true;
	}
	return true;
}

bool FieldArchive::searchIteratorsP(QMultiMap<QString, int>::const_iterator &i, QMultiMap<QString, int>::const_iterator &begin, int fieldID, Sorting sorting) const
{
	if (fieldID < 0)		return false;

	switch (sorting) {
	case SortByName:
		begin = fieldsSortByName.constBegin();
		i = fieldsSortByName.constFind(fieldsSortByName.key(fieldID), fieldID);
		if (i==fieldsSortByName.constEnd()) {
			--i;
		}
		return true;
	case SortByDesc:
		begin = fieldsSortByDesc.constBegin();
		i = fieldsSortByDesc.constFind(fieldsSortByDesc.key(fieldID), fieldID);
		if (i==fieldsSortByDesc.constEnd()) {
			--i;
		}
		break;
	case SortByMapId:
		begin = fieldsSortByMapId.constBegin();
		i = fieldsSortByMapId.constFind(fieldsSortByMapId.key(fieldID), fieldID);
		if (i==fieldsSortByMapId.constEnd()) {
			--i;
		}
		return true;
	}
	return true;
}

bool FieldArchive::searchText(const QRegularExpression &text, int &fieldID, int &textID, int &from, int &size, Sorting sorting) const
{
	QMultiMap<QString, int>::const_iterator i, end;
	if (!searchIterators(i, end, fieldID, sorting))	return false;

	for ( ; i != end ; ++i) {
		Field *field = getField(fieldID = i.value());
		if (field && field->hasMsdFile() && field->getMsdFile()->searchText(text, textID, from, size))
			return true;
		textID = from = 0;
	}

	return false;
}

bool FieldArchive::searchTextReverse(const QRegularExpression &text, int &fieldID, int &textID, int &from, int &size, Sorting sorting) const
{
	QMultiMap<QString, int>::const_iterator i, begin;
	if (!searchIteratorsP(i, begin, fieldID, sorting))	return false;

	for ( ; i != begin ; --i) {
		Field *field = getField(fieldID = i.value());
		if (field && field->hasMsdFile() && field->getMsdFile()->searchTextReverse(text, textID, from, size))
			return true;
		textID = from = 2147483647;
	}

	return false;
}

bool FieldArchive::searchScript(JsmFile::SearchType type, quint64 value, int &fieldID, int &groupID, int &methodID, int &opcodeID, Sorting sorting) const
{
	QMultiMap<QString, int>::const_iterator i, end;
	if (!searchIterators(i, end, fieldID, sorting))	return false;

	for ( ; i != end ; ++i) {
		Field *field = getField(fieldID = i.value());
		if (field && field->hasJsmFile() && field->getJsmFile()->search(type, value, groupID, methodID, opcodeID))
			return true;
		groupID = methodID = opcodeID = 0;
	}

	return false;
}

bool FieldArchive::searchScriptText(const QRegularExpression &text, int &fieldID, int &groupID, int &methodID, int &opcodeID, Sorting sorting) const
{
	QMultiMap<QString, int>::const_iterator i, end;
	if (!searchIterators(i, end, fieldID, sorting))	return false;

	for ( ; i != end ; ++i) {
		Field *field = getField(fieldID = i.value());
		if (field && field->hasMsdFile() && field->hasJsmFile()) {
			MsdFile *msd = field->getMsdFile();
			JsmFile *jsm = field->getJsmFile();
			QList<quint64> textIDs;
			for (int textID = 0; textID < msd->nbText(); ++textID) {
				if (text.match(msd->text(textID)).hasMatch()) {
					textIDs.append(quint64(textID));
				}
			}
			if (jsm->search(JsmFile::SearchText, textIDs, groupID, methodID, opcodeID)) {
				return true;
			}
		}
		groupID = methodID = opcodeID = 0;
	}

	return false;
}

bool FieldArchive::searchScriptReverse(JsmFile::SearchType type, quint64 value, int &fieldID, int &groupID, int &methodID, int &opcodeID, Sorting sorting) const
{
	QMultiMap<QString, int>::const_iterator i, begin;
	if (!searchIteratorsP(i, begin, fieldID, sorting))	return false;

	for ( ; i != begin ; --i) {
		Field *field = getField(fieldID = i.value());
		if (field && field->hasJsmFile() && field->getJsmFile()->searchReverse(type, value, groupID, methodID, opcodeID))
			return true;
		groupID = methodID = opcodeID = 2147483647;
	}

	return false;
}

bool FieldArchive::searchScriptTextReverse(const QRegularExpression &text, int &fieldID, int &groupID, int &methodID, int &opcodeID, Sorting sorting) const
{
	QMultiMap<QString, int>::const_iterator i, begin;
	if (!searchIteratorsP(i, begin, fieldID, sorting))	return false;

	for ( ; i != begin ; --i) {
		Field *field = getField(fieldID = i.value());
		if (field && field->hasMsdFile() && field->hasJsmFile()) {
			MsdFile *msd = field->getMsdFile();
			JsmFile *jsm = field->getJsmFile();
			QList<quint64> textIDs;
			for (int textID = 0; textID < msd->nbText(); ++textID) {
				if (text.match(msd->text(textID)).hasMatch()) {
					textIDs.append(quint64(textID));
				}
			}
			if (jsm->searchReverse(JsmFile::SearchText, textIDs, groupID, methodID, opcodeID)) {
				return true;
			}
		}
		groupID = methodID = opcodeID = 2147483647;
	}

	return false;
}

QMultiMap<int, QString> FieldArchive::searchAllVars() const
{
	QMultiMap<int, QString> vars;

	for (Field *field: fields) {
		if (field->hasJsmFile()) {
			QList<int> v = field->getJsmFile()->searchAllVars();
			for (int var: v)
				vars.insert(var, field->name());
		}
	}

	return vars;
}

QList<int> FieldArchive::searchAllSpells(int fieldID) const
{
	Field *field = getField(fieldID);

	if (field && field->hasJsmFile()) {
		return field->getJsmFile()->searchAllSpells(field->name());
	}

	return QList<int>();
}

QList<int> FieldArchive::searchAllCards(int fieldID) const
{
	Field *field = getField(fieldID);

	if (field && field->hasJsmFile()) {
		return field->getJsmFile()->searchAllCards(field->name());
	}

	return QList<int>();
}

QList<int> FieldArchive::searchAllCardPlayers(int fieldID) const
{
	Field *field = getField(fieldID);

	if (field && field->hasJsmFile()) {
		return field->getJsmFile()->searchAllCardPlayers(field->name());
	}

	return QList<int>();
}

QMap<Field *, QList<int> > FieldArchive::searchAllBattles() const
{
	QMap<Field *, QList<int> > battles;

	for (Field *field: fields) {
		if (field->hasMrtFile()) {
			battles.insert(field, field->getMrtFile()->searchAllBattles());
		}
	}

	return battles;
}

QMultiMap<int, Field *> FieldArchive::searchAllMoments() const
{
	QMultiMap<int, Field *> moments;

	QStringList mapList = Data::maplist();

	for (Field *field: fields) {
		if (field->hasJsmFile() && (mapList.indexOf(field->name()) > 88 || mapList.indexOf(field->name()) == 73)) {
			for (int moment: field->getJsmFile()->searchAllMoments()) {
				moments.insert(moment, field);
			}
		}
	}

	return moments;
}

QMap<int, int> FieldArchive::searchAllOpcodeTypes() const
{
	QMap<int, int> ret;

	for (Field *field: fields) {
		if (field->hasJsmFile()) {
			field->getJsmFile()->searchAllOpcodeTypes(ret);
		}
	}

	return ret;
}

QList<Vertex> FieldArchive::searchAllSavePoints() const
{
	QList<Vertex> ret;

	for (Field *field: fields) {
		if (field->hasJsmFile()) {
			//field->getJsmFile()->searchAllSavePoints(ret);
		}
	}

	return ret;
}

QStringList FieldArchive::fieldList() const
{
	QStringList list;

	for (Field *field: fields) {
		list.append(field->name());
	}

	return list;
}

const QStringList &FieldArchive::mapList() const
{
	return _mapList;
}

void FieldArchive::setMapList(const QStringList &mapList)
{
	_mapList = mapList.isEmpty() ? Data::maplist() : mapList;
}
