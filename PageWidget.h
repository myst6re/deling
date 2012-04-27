#ifndef PAGEWIDGET_H
#define PAGEWIDGET_H

#include <QtGui>

class PageWidget : public QWidget
{
	Q_OBJECT
public:
	explicit PageWidget(QWidget *parent = 0);
	bool isBuilded() const;
	virtual void build();
	bool isFilled() const;
	virtual void fill();
	virtual void clear();
	virtual void setReadOnly(bool ro);
	bool isReadOnly() const;
signals:
	void modified(bool);
private:
	bool builded, filled, readOnly;
};

#endif // PAGEWIDGET_H
