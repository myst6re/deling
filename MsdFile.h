#ifndef MSDFILE_H
#define MSDFILE_H

#include <QtCore>
#include "FF8Text.h"
#include "Data.h"

class MsdFile
{
public:
	MsdFile();
	bool open(const QString &path);
	bool open(const QByteArray &msd);
	bool save(const QString &path);
	QByteArray save();
	static QString lastError;
	QByteArray data(int id);
	FF8Text text(int);
	void setText(int, const FF8Text &);
	void insertText(int);
	void removeText(int);
	int nbText();
	int searchText(const QString &text, int &textID, int from=0, Qt::CaseSensitivity cs=Qt::CaseInsensitive, bool regExp=false);
	int searchTextReverse(const QString &text, int &textID, int from=0, Qt::CaseSensitivity cs=Qt::CaseInsensitive, bool regExp=false);
	bool isModified();
	bool isJp();
private:
//	QString path;
	QList<QByteArray> texts;
	QList<bool> needEndOfString;
	bool modified;
};

#endif // MSDFILE_H
