#ifndef FSPREVIEWWIDGET_H
#define FSPREVIEWWIDGET_H

#include <QtGui>
#include "BGPreview2.h"

class FsPreviewWidget : public QStackedWidget
{
public:
	enum Pages { EmptyPage, ImagePage, TextPage };
    explicit FsPreviewWidget(QWidget *parent = 0);
	void clearPreview();
	void imagePreview(const QPixmap &image, const QString &name=QString());
	void textPreview(const QString &text);
private:
	QWidget *imageWidget();
	QWidget *textWidget();
	QScrollArea *scrollArea;
};

#endif // FSPREVIEWWIDGET_H
