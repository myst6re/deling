#ifndef TDWMANAGERDIALOG_H
#define TDWMANAGERDIALOG_H

#include <QtGui>
#include "widgets/TdwWidget2.h"

class TdwManagerDialog : public QDialog
{
public:
	explicit TdwManagerDialog(QWidget *parent=0);
private:
	QListWidget *list1;
	TdwWidget2 *tdwWidget;
};

#endif // TDWMANAGERDIALOG_H
