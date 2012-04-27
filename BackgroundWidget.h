#ifndef BACKGROUNDWIDGET_H
#define BACKGROUNDWIDGET_H

#include <QtGui>
#include "PageWidget.h"
#include "BGPreview.h"
#include "BGPreview2.h"
#include "FF8Image.h"

class BackgroundWidget : public PageWidget
{
	Q_OBJECT
public:
	BackgroundWidget(QWidget *parent=0);
	void clear();
//	bool filled();
	QPixmap error();
	QPixmap generate(const QString &, const QByteArray &, const QByteArray &);

private slots:
	void parameterChanged(int index);
	void enableState(QListWidgetItem *item);
	void enableLayer(QListWidgetItem *item);
//	void switchItem(QListWidgetItem *item);
private:
	void build();
	void fillWidgets();
	void fill();

	QByteArray map, mim;
	BGPreview2 *image;
	QComboBox *parametersWidget;
	QListWidget *statesWidget, *layersWidget;
	QMultiMap<quint8, quint8> params;
	QMultiMap<quint8, quint8> allparams;
	QMap<quint8, bool> layers;
};

#endif // BACKGROUNDWIDGET_H
