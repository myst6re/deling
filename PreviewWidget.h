#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QWidget>
#include <QStackedLayout>
#include "files/TdwFile.h"
#include "BGPreview.h"
#include "TextPreview.h"

class PreviewWidget : public QWidget
{
	Q_OBJECT
public:
	explicit PreviewWidget(QWidget *parent = nullptr);

	void showText(const QByteArray &textData, FF8Window ff8Window, TdwFile *tdwFile = nullptr);
	void showBackground(const QPixmap &background);

private:
	void buildLayout();

	QStackedLayout *layout;
	TextPreview *textPreview;
	BGPreview *bgPreview;
};

#endif // PREVIEWWIDGET_H
