#ifndef MISCSEARCH_H
#define MISCSEARCH_H

#include "Search.h"

class MiscSearch : public QDialog
{
	Q_OBJECT
public:
	MiscSearch(FieldArchive *fieldArchive, QWidget *parent=0);

private slots:
	void search(int);
private:
	void fillList();
	static int getMapId(QList<Field *> fields);

	QListWidget *list;
	QPlainTextEdit *textEdit;
	FieldArchive *fieldArchive;
};

#endif // MISCSEARCH_H
