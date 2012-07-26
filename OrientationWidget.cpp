#include "OrientationWidget.h"

OrientationWidget::OrientationWidget(QWidget *parent) :
	QWidget(parent)
{
}

OrientationWidget::OrientationWidget(quint8 value, QWidget *parent) :
	QWidget(parent)
{
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

	double radius = radiusCircle();
	QPointF centerCircle = this->centerCircle();

	p.drawEllipse(centerCircle, radius, radius);

	p.translate(centerCircle);

	p.rotate(_value);

	p.drawLines(QVector<QLineF>()
				<< QLineF(QPointF(-radius, 0), QPointF(radius, 0))
				<< QLineF(QPointF(0, -radius), QPointF(0, radius)));

	if(_value >= 90 && _value < 270) {
		p.rotate(180);
		p.drawText(QRectF(0.0, 0.0, radius-4, radius-4), tr("Droite"), QTextOption(Qt::AlignRight));
	} else {
		p.drawText(QPointF(-radius+4, -4), tr("Droite"));
	}
}

void OrientationWidget::mousePressEvent(QMouseEvent *e)
{
	if(isInCircle(e->posF())) {
		moveCursor(e->posF());
	}
}

QPointF OrientationWidget::centerCircle() const
{
	return QPointF(width()/2.0, height()/2.0);
}

double OrientationWidget::radiusCircle() const
{
	return (qMin(width(), height()) - 1)/2.0;
}

bool OrientationWidget::isInCircle(const QPointF &pos)
{
	QPointF centerCircle = this->centerCircle();
	qreal sizeX=pos.x() - centerCircle.x(), sizeY=pos.y() - centerCircle.y();
	double distance = sqrt(sizeX*sizeX + sizeY*sizeY);

	return distance <= radiusCircle();
}

void OrientationWidget::moveCursor(const QPointF &pos)
{
	QPointF centerCircle = this->centerCircle();
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
