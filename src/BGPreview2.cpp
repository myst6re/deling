/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2012 Arzel Jérôme <myst6re@gmail.com>
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
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
	QString path = QFileDialog::getSaveFileName(this, tr("Enregistrer l'image"), name + ".png", tr("Image PNG (*.png);;Image JPG (*.jpg);;Image BMP (*.bmp);;Portable Pixmap (*.ppm)"));
	if(path.isEmpty())	return;

	pixmap()->save(path);
}
