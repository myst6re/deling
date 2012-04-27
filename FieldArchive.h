#ifndef FSARCHIVE_H
#define FSARCHIVE_H

#include <QtCore>
#include <QtGui/QProgressDialog>
#include "parameters.h"
#include "LZS.h"
#include "MsdFile.h"
#include "Data.h"
#include "Field.h"

class FieldArchive
{
public:
	enum Sorting{
		SortByName, SortByDesc, SortByMapId
	};

	FieldArchive();
	~FieldArchive();
	void clearFields();
	virtual QString archivePath() const=0;
	Field *getField(int id) const;
	const QList<Field *> &getFields() const;
	int nbFields() const;
	bool isReadOnly() const;
	virtual bool open(const QString &, QProgressDialog *progress)=0;
	virtual bool openBG(Field *field, QByteArray &map_data, QByteArray &mim_data, QByteArray &tdw_data, QByteArray &chara_data) const=0;
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
	QList<Field *> fields;
	QStringList _mapList;
	QMultiMap<QString, int> fieldsSortByName;
	QMultiMap<QString, int> fieldsSortByDesc;
	QMultiMap<QString, int> fieldsSortByMapId;
	bool readOnly;
};

#endif // FSARCHIVE_H
