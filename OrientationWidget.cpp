#include "OrientationWidget.h"

OrientationWidget::OrientationWidget(QWidget *parent) :
	QWidget(parent)
{
//	setAutoFillBackground(true);
}

OrientationWidget::OrientationWidget(quint8 value, QWidget *parent) :
	QWidget(parent)
{
//	setAutoFillBackground(true);
	byte2degree(value);
}

quint8 OrientationWidget::value() const
{
	return degree2byte();
}

void OrientationWidget::setValue(int value)
{
	byte2degree(value);
	emit valueChanged(value);
	update();
}

void OrientationWidget::byte2degree(quint8 v)
{
	_value = (256 - v) * 360 / 256;
}

quint8 OrientationWidget::degree2byte() const
{
	return (360 - _value) * 256 / 360;
}

QSize OrientationWidget::minimumSizeHint() const
{
	return QSize(100, 100);
}

QSize OrientationWidget::sizeHint() const
{
	return minimumSizeHint();
}

void OrientationWidget::paintEvent(QPaintEvent *e)
{
	QPainter p(this);

	p.setPen(Qt::black);
	p.setBrush(Qt::gray);

	int size = qMin(width(), height());
	QPoint pos((width() - size) / 2, (height() - size) / 2);

	p.drawEllipse(QRect(pos, QSize(size-1, size-1)));

	p.translate(QPoint(width()/2, height()/2));

	p.rotate(_value);

	p.drawLines(QVector<QLine>()
				<< QLine(QPoint(-size/2, 0), QPoint(size/2, 0))
				<< QLine(QPoint(0, -size/2), QPoint(0, size/2)));

	p.drawText(QPoint(-size/2+4, -4), tr("Droite"));
}

void OrientationWidget::mousePressEvent(QMouseEvent *e)
{
	if(isInCircle(e->posF())) {
		moveCursor(e->posF());
	}
}

bool OrientationWidget::isInCircle(const QPointF &pos)
{
	double radius = qMin(width(), height())/2.0;
	QPointF centerCircle(width()/2.0, height()/2.0);
	qreal sizeX=pos.x() - centerCircle.x(), sizeY=pos.y() - centerCircle.y();
	double distance = sqrt(sizeX*sizeX + sizeY*sizeY);

	return distance <= radius;
}

void OrientationWidget::moveCursor(const QPointF &pos)
{
	QPointF centerCircle(width()/2.0, height()/2.0);
	qreal sizeX=pos.x() - centerCircle.x(), sizeY=pos.y() - centerCircle.y();
	double angle;

	if(sizeX != 0) {
		angle = atan2(abs(sizeY), abs(sizeX)) * 57.29577951;// rad2deg
	} else {
		angle = 0;
	}

	if(sizeX == 0 && sizeY == 0) {
		return;
	} else if(sizeX < 0 && sizeY <= 0) {
		_value = 0 + angle;
	} else if(sizeX == 0 && sizeY < 0) {
		_value = 90;
	} else if(sizeX > 0 && sizeY < 0) {
		_value = 180 - angle;
	} else if(sizeX == 0 && sizeY > 0) {
		_value = 270;
	} else if(sizeX < 0 && sizeY > 0) {
		_value = 360 - angle;
	} else if(sizeX > 0 && sizeY >= 0) {
		_value = 180 + angle;
	}

	quint8 value = degree2byte();
	setValue(value);
	emit valueEdited(value);
}

void OrientationWidget::mouseMoveEvent(QMouseEvent *e)
{
	if(isInCircle(e->posF())) {
		moveCursor(e->posF());
	}
}
