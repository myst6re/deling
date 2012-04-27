#include "BGPreview2.h"

BGPreview2::BGPreview2(QWidget *parent) :
    QLabel(parent)
{
	showSave = false;
}

void BGPreview2::setName(const QString &name)
{
	this->name = name;
}

void BGPreview2::paintEvent(QPaintEvent *e)
{
	QLabel::paintEvent(e);

	if(isEnabled() && showSave) {
		QPainter painter(this);
		painter.drawPixmap(0, 0, QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton).pixmap(32));
		painter.end();
	}
}

void BGPreview2::enterEvent(QEvent *)
{
	showSave = true;
	update(0, 0, 32, 32);
}

void BGPreview2::leaveEvent(QEvent *)
{
	showSave = false;
	update(0, 0, 32, 32);
}

void BGPreview2::mousePressEvent(QMouseEvent *event)
{
	if(event->button() != Qt::LeftButton)	return;

	if(event->x()>=0 && event->x()<32 && event->y()>=0 && event->y()<32) {
		savePixmap();
	}
}

void BGPreview2::savePixmap()
{
	QString path = QFileDialog::getSaveFileName(this, tr("Enregistrer le background"), name, tr("Image PNG (*.png);;Image JPG (*.jpg);;Image BMP (*.bmp);;Portable Pixmap (*.ppm)"));
	if(path.isEmpty())	return;

	pixmap()->save(path);
}
