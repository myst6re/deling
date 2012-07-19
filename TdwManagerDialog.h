#ifndef TDWMANAGERDIALOG_H
#define TDWMANAGERDIALOG_H

#include <QtGui>
#include "widgets/TdwWidget2.h"

class TdwManagerDialog : public QDialog
{
	Q_OBJECT
public:
	explicit TdwManagerDialog(QWidget *parent=0);
private slots:
	void setTdw(int id);
private:
	void fillList1();
	QListWidget *list1;
	TdwWidget2 *tdwWidget;
};

#endif // TDWMANAGERDIALOG_H
