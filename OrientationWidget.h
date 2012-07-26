#ifndef ORIENTATIONWIDGET_H
#define ORIENTATIONWIDGET_H

#include <QtGui>

class OrientationWidget : public QWidget
{
	Q_OBJECT
public:
	explicit OrientationWidget(QWidget *parent = 0);
	explicit OrientationWidget(quint8 value, QWidget *parent = 0);
	quint8 value() const;
	virtual QSize minimumSizeHint() const;
	virtual QSize sizeHint() const;
signals:
	void valueChanged(int i);
	void valueEdited(int i);
public slots:
	void setValue(int value);
private:
	void byte2degree(quint8 v);
	quint8 degree2byte() const;
	QPointF centerCircle() const;
	double radiusCircle() const;
	bool isInCircle(const QPointF &pos);
	void moveCursor(const QPointF &pos);
	int _value;
protected:
	virtual void paintEvent(QPaintEvent *);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);
};

#endif // ORIENTATIONWIDGET_H
