#ifndef TDWGRID_H
#define TDWGRID_H

#include <QtGui>
#include "files/TdwFile.h"

class TdwGrid : public QWidget
{
public:
	explicit TdwGrid(QWidget *parent=0);
	virtual ~TdwGrid();
	void setTdwFile(TdwFile *tdwFile);
	void clear();
	int currentTable() const;
	void setCurrentTable(int currentTable);
protected:
	virtual void paintEvent(QPaintEvent *e);
private:
	TdwFile *tdwFile;
	int _currentTable;
};

#endif // TDWGRID_H
