/****************************************************************************
 **
 ** Copyx (C) 2010 Arzel Jérôme <myst6re@gmail.com>
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
#include "BGPreview.h"

BGPreview::BGPreview(QWidget *parent)
	: QScrollArea(parent)
{
	setAlignment(Qt::AlignCenter);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	QPalette pal = palette();
	pal.setColor(QPalette::Active, QPalette::Window, Qt::black);
	pal.setColor(QPalette::Inactive, QPalette::Window, Qt::black);
	pal.setColor(QPalette::Disabled, QPalette::Window, pal.color(QPalette::Disabled, QPalette::Text));
	setPalette(pal);
	setFrameShape(QFrame::NoFrame);
}

void BGPreview::fill(const QPixmap &background)
{
	setCursor(QCursor(Qt::PointingHandCursor));

	label = new QLabel();

	if(background.width()>width() || background.height()>height()) {
		if(background.height()==height())
			label->setPixmap(background.scaled(background.width()*width()/background.height(), height(), Qt::KeepAspectRatio));
		else
			label->setPixmap(background.scaled(width(), height(), Qt::KeepAspectRatio));
	}
	else
		label->setPixmap(background);

	setWidget(label);
}

void BGPreview::clear()
{
	label = new QLabel();
	label->setPixmap(QPixmap());
	setWidget(label);
	setCursor(QCursor(Qt::ArrowCursor));
}

void BGPreview::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
		emit triggered();
}
