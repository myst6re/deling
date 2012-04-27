#ifndef WALKMESHWIDGET_H
#define WALKMESHWIDGET_H

#include <QtGui>
#include "PageWidget.h"
#include "WalkmeshFile.h"
#include "WalkmeshGLWidget.h"

class WalkmeshWidget : public PageWidget
{
public:
	WalkmeshWidget(QWidget *parent=0);
	void clear();
	void fill(WalkmeshFile *walkmeshFile);
private:
	void build();
	WalkmeshFile *walkmeshFile;
	WalkmeshGLWidget *walkmeshGL;
};

#endif // WALKMESHWIDGET_H
