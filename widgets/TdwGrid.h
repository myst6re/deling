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
#ifndef TDWGRID_H
#define TDWGRID_H

#include <QtGui>
#include "widgets/TdwDisplay.h"

class TdwGrid : public TdwDisplay
{
	Q_OBJECT
public:
	explicit TdwGrid(QWidget *parent=0);
	virtual ~TdwGrid();
	//virtual void setTdwFile(TdwFile *tdwFile);
	static QPoint getPos(int letter);
signals:
	void letterClicked(int letter);
public slots:
	void updateLetter(const QRect &rect);
protected:
	virtual QSize sizeHint() const;
	virtual QSize minimumSizeHint() const;
	virtual void mousePressEvent(QMouseEvent * e);
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void paintEvent(QPaintEvent *e);
	virtual void focusInEvent(QFocusEvent *);
	virtual void focusOutEvent(QFocusEvent *);
private:
	static int getLetter(const QPoint &pos);
	//QPixmap copyGrid;
};

#endif // TDWGRID_H
