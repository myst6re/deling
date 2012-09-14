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
#include "FieldArchive.h"
#include "LZS.h"
#include "Data.h"

FieldArchive::FieldArchive()
	: readOnly(false)
{
}

FieldArchive::~FieldArchive()
{
	foreach(Field *field, fields)		delete field;
	foreach(CharaModel *model, models)	delete model;
}

void FieldArchive::clearFields()
{
	foreach(Field *field, fields)	delete field;
	fields.clear();
	foreach(CharaModel *model, models)	delete model;
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
//	if(field == NULL)	return NULL;
//	if(!field->isOpen() && openField(field)) {
//		return field;
//	}
//	if(!field->isOpen())	return NULL;

	return fields.value(id, NULL);
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
	return models.value(id, NULL);
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

	foreach(Field *field, fields) {
		if(field->isOpen() && field->isModified() && field->hasJsmFile()) {
			if(!field->getJsmFile()->compileAll(errorGroupID, errorMethodID, errorLine, errorStr)) {
				return false;
			}
		}
		++errorFieldID;
	}

	return true;
}

int FieldArchive::searchText(const QString &text, int &fieldID, int &textID, int from, Sorting sorting, Qt::CaseSensitivity cs, bool regExp) const
{
	QMap<QString, int>::const_iterator i, end;
	int index;

	switch(sorting) {
	case SortByName:
		i = fieldsSortByName.constFind(fieldsSortByName.key(fieldID), fieldID);
		end = fieldsSortByName.constEnd();
		if(i==end) {
			i = fieldsSortByName.constBegin();
		}
		break;
	case SortByDesc:
		i = fieldsSortByDesc.constFind(fieldsSortByDesc.key(fieldID), fieldID);
		end = fieldsSortByDesc.constEnd();
		if(i==end) {
			i = fieldsSortByDesc.constBegin();
		}
		break;
	case SortByMapId:
		i = fieldsSortByMapId.constFind(fieldsSortByMapId.key(fieldID), fieldID);
		end = fieldsSortByMapId.constEnd();
		if(i==end) {
			i = fieldsSortByMapId.constBegin();
		}
		break;
	}

	for( ; i != end ; ++i) {
		fieldID = i.value();
		Field *field = getField(fieldID);
		if(field && field->hasMsdFile() && (index=field->getMsdFile()->searchText(text, textID, from, cs, regExp))!=-1)
			return index;
		textID = from = 0;
	}

	return -1;
}

int FieldArchive::searchTextReverse(const QString &text, int &fieldID, int &textID, int from, Sorting sorting, Qt::CaseSensitivity cs, bool regExp) const
{
	QMap<QString, int>::const_iterator i, begin;
	int index;

	switch(sorting) {
	case SortByName:
		begin = fieldsSortByName.constBegin();
		i = fieldsSortByName.constFind(fieldsSortByName.key(fieldID), fieldID);
		if(i==fieldsSortByName.constEnd()) {
			--i;
		}
		break;
	case SortByDesc:
		begin = fieldsSortByDesc.constBegin();
		i = fieldsSortByDesc.constFind(fieldsSortByDesc.key(fieldID), fieldID);
		if(i==fieldsSortByDesc.constEnd()) {
			--i;
		}
		break;
	case SortByMapId:
		begin = fieldsSortByMapId.constBegin();
		i = fieldsSortByMapId.constFind(fieldsSortByMapId.key(fieldID), fieldID);
		if(i==fieldsSortByMapId.constEnd()) {
			--i;
		}
		break;
	}

	if(i == begin-1)	return -1;

	if(textID < 0)	--i;

	for( ; i != begin-1 ; --i) {
		fieldID = i.value();
		Field *field = getField(fieldID);
		if(field && field->hasMsdFile() && (index=field->getMsdFile()->searchTextReverse(text, textID, from, cs, regExp))!=-1)
			return index;
		textID = from = -1;
	}

	return -1;
}

bool FieldArchive::searchScript(quint8 type, quint64 value, int &fieldID, int &groupID, int &methodID, int &opcodeID, Sorting sorting) const
{
	QMap<QString, int>::const_iterator i, end;

	switch(sorting) {
	case SortByName:
		i = fieldsSortByName.constFind(fieldsSortByName.key(fieldID), fieldID);
		end = fieldsSortByName.constEnd();
		if(i==end) {
			i = fieldsSortByName.constBegin();
		}
		break;
	case SortByDesc:
		i = fieldsSortByDesc.constFind(fieldsSortByDesc.key(fieldID), fieldID);
		end = fieldsSortByDesc.constEnd();
		if(i==end) {
			i = fieldsSortByDesc.constBegin();
		}
		break;
	case SortByMapId:
		i = fieldsSortByMapId.constFind(fieldsSortByMapId.key(fieldID), fieldID);
		end = fieldsSortByMapId.constEnd();
		if(i==end) {
			i = fieldsSortByMapId.constBegin();
		}
		break;
	}

	for( ; i != end ; ++i) {
		fieldID = i.value();
		Field *field = getField(fieldID);
		if(field && field->hasJsmFile() && field->getJsmFile()->search(type, value, groupID, methodID, opcodeID))
			return true;
		groupID = methodID = opcodeID = 0;
	}

	return false;
}

bool FieldArchive::searchScriptText(const QString &text, Qt::CaseSensitivity sensitivity, int &fieldID, int &groupID, int &methodID, int &opcodeID, Sorting sorting) const
{
	QMap<QString, int>::const_iterator i, end;
	int textID = 0;

	switch(sorting) {
	case SortByName:
		i = fieldsSortByName.constFind(fieldsSortByName.key(fieldID), fieldID);
		end = fieldsSortByName.constEnd();
		if(i==end) {
			i = fieldsSortByName.constBegin();
		}
		break;
	case SortByDesc:
		i = fieldsSortByDesc.constFind(fieldsSortByDesc.key(fieldID), fieldID);
		end = fieldsSortByDesc.constEnd();
		if(i==end) {
			i = fieldsSortByDesc.constBegin();
		}
		break;
	case SortByMapId:
		i = fieldsSortByMapId.constFind(fieldsSortByMapId.key(fieldID), fieldID);
		end = fieldsSortByMapId.constEnd();
		if(i==end) {
			i = fieldsSortByMapId.constBegin();
		}
		break;
	}

	for( ; i != end ; ++i) {
		fieldID = i.value();
		Field *field = getField(fieldID);
		if(field && field->hasMsdFile() && field->hasJsmFile()) {
			MsdFile *msd = field->getMsdFile();
			while(msd->searchText(text, textID, 0, sensitivity) != -1)
			{
				if(field->getJsmFile()->search(0, textID, groupID, methodID, opcodeID))
					return true;
				++textID;
			}
		}
		textID = groupID = methodID = opcodeID = 0;
	}

	return false;
}

bool FieldArchive::searchScriptReverse(quint8 type, quint64 value, int &fieldID, int &groupID, int &methodID, int &opcodeID, Sorting sorting) const
{
	QMap<QString, int>::const_iterator i, begin;

	switch(sorting) {
	case SortByName:
		begin = fieldsSortByName.constBegin();
		i = fieldsSortByName.constFind(fieldsSortByName.key(fieldID), fieldID);
		if(i==fieldsSortByName.constEnd()) {
			--i;
		}
		break;
	case SortByDesc:
		begin = fieldsSortByDesc.constBegin();
		i = fieldsSortByDesc.constFind(fieldsSortByDesc.key(fieldID), fieldID);
		if(i==fieldsSortByDesc.constEnd()) {
			--i;
		}
		break;
	case SortByMapId:
		begin = fieldsSortByMapId.constBegin();
		i = fieldsSortByMapId.constFind(fieldsSortByMapId.key(fieldID), fieldID);
		if(i==fieldsSortByMapId.constEnd()) {
			--i;
		}
		break;
	}

	for( ; i != begin-1 ; --i) {
		fieldID = i.value();
		Field *field = getField(fieldID);
		if(field && field->hasJsmFile() && field->getJsmFile()->searchReverse(type, value, groupID, methodID, opcodeID))
			return true;
		groupID = methodID = opcodeID = -1;
	}

	return false;
}

bool FieldArchive::searchScriptTextReverse(const QString &text, Qt::CaseSensitivity sensitivity, int &fieldID, int &groupID, int &methodID, int &opcodeID, Sorting sorting) const
{
	QMap<QString, int>::const_iterator i, begin;
	int textID = 0;

	switch(sorting) {
	case SortByName:
		begin = fieldsSortByName.constBegin();
		i = fieldsSortByName.constFind(fieldsSortByName.key(fieldID), fieldID);
		if(i==fieldsSortByName.constEnd()) {
			--i;
		}
		break;
	case SortByDesc:
		begin = fieldsSortByDesc.constBegin();
		i = fieldsSortByDesc.constFind(fieldsSortByDesc.key(fieldID), fieldID);
		if(i==fieldsSortByDesc.constEnd()) {
			--i;
		}
		break;
	case SortByMapId:
		begin = fieldsSortByMapId.constBegin();
		i = fieldsSortByMapId.constFind(fieldsSortByMapId.key(fieldID), fieldID);
		if(i==fieldsSortByMapId.constEnd()) {
			--i;
		}
		break;
	}

	for( ; i != begin-1 ; --i) {
		fieldID = i.value();
		Field *field = getField(fieldID);
		if(field && field->hasMsdFile() && field->hasJsmFile()) {
			MsdFile *msd = field->getMsdFile();
			while(msd->searchText(text, textID, 0, sensitivity) != -1)
			{
				if(field->getJsmFile()->searchReverse(0, textID, groupID, methodID, opcodeID))
					return true;
				++textID;
			}
		}
		textID = 0;
		groupID = methodID = opcodeID = -1;
	}

	return false;
}

QMultiMap<int, QString> FieldArchive::searchAllVars() const
{
	QMultiMap<int, QString> vars;

	foreach(Field *field, fields) {
		if(field->hasJsmFile()) {
			QList<int> v = field->getJsmFile()->searchAllVars();
			foreach(int var, v)
				vars.insert(var, field->name());
		}
	}

	return vars;
}

QList<int> FieldArchive::searchAllSpells(int fieldID) const
{
	Field *field = getField(fieldID);

	if(field && field->hasJsmFile()) {
		return field->getJsmFile()->searchAllSpells(field->name());
	}

	return QList<int>();
}

QMap<Field *, QList<int> > FieldArchive::searchAllBattles() const
{
	QMap<Field *, QList<int> > battles;

	foreach(Field *field, fields) {
		if(field->hasMrtFile()) {
			battles.insert(field, field->getMrtFile()->searchAllBattles());
		}
	}

	return battles;
}

QMultiMap<int, Field *> FieldArchive::searchAllMoments() const
{
	QMultiMap<int, Field *> moments;

	QStringList mapList = Data::maplist();

	foreach(Field *field, fields) {
		if(field->hasJsmFile() && (mapList.indexOf(field->name()) > 88 || mapList.indexOf(field->name()) == 73)) {
			foreach(int moment, field->getJsmFile()->searchAllMoments()) {
				moments.insert(moment, field);
			}
		}
	}

	return moments;
}

QMap<int, int> FieldArchive::searchAllOpcodeTypes() const
{
	QMap<int, int> ret;

	foreach(Field *field, fields) {
		if(field->hasJsmFile()) {
			field->getJsmFile()->searchAllOpcodeTypes(ret);
		}
	}

	return ret;
}

QStringList FieldArchive::fieldList() const
{
	QStringList list;

	foreach(Field *field, fields) {
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
