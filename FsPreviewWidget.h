#ifndef FSPREVIEWWIDGET_H
#define FSPREVIEWWIDGET_H

#include <QtGui>

class FsPreviewWidget : public QStackedWidget
{
public:
	enum Pages { EmptyPage, ImagePage, TextPage };
    explicit FsPreviewWidget(QWidget *parent = 0);
	void clearPreview();
	void imagePreview(const QPixmap &image);
	void textPreview(const QString &text);
private:
	QWidget *imageWidget();
	QWidget *textWidget();
	QScrollArea *scrollArea;
};

#endif // FSPREVIEWWIDGET_H
