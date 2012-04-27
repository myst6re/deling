#ifndef MISCWIDGET_H
#define MISCWIDGET_H

#include <QtGui>
#include "PageWidget.h"
#include "MiscFile.h"
#include "WalkmeshFile.h"

class MiscWidget : public PageWidget
{
	Q_OBJECT
public:
	MiscWidget(QWidget *parent=0);
	void build();
	void clear();
//	bool filled();
	void setReadOnly(bool readOnly);
	void fill(MiscFile *miscFile, WalkmeshFile *walkmeshFile);
private slots:
	void editName(const QString &);
	void editRat(const QString &);
	void editMrt(const QString &);
	void editPmp(const QString &);
	void editPmd(const QString &);
	void editPvp(const QString &);
	void editCaVector(int value);
	void editCaPos(double value);
private:
	void editCaVector(int id, int id2, int value);
	void editCaPos(int id, int value);

	MiscFile *miscFile;
	WalkmeshFile *walkmeshFile;
	QLineEdit *nameEdit, *ratEdit, *mrtEdit, *pmpEdit, *pmdEdit, *pvpEdit;
	QListWidget *gateList;
	QSpinBox *caVectorX1Edit, *caVectorX2Edit, *caVectorX3Edit, *caVectorY1Edit, *caVectorY2Edit, *caVectorY3Edit, *caVectorZ1Edit, *caVectorZ2Edit, *caVectorZ3Edit;
	QDoubleSpinBox *caSpaceXEdit, *caSpaceYEdit, *caSpaceZEdit;
};

#endif // MISCWIDGET_H
