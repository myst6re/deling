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
#ifndef FIELDTHREAD_H
#define FIELDTHREAD_H

#include <QtCore>
#include "FieldArchive.h"
#include "Field.h"

class FieldThread : public QThread
{
	Q_OBJECT
public:
	explicit FieldThread(QObject *parent = 0)
		: QThread(parent), fieldArchive(0), field(0) { }
	void run();
	void setData(FieldArchive *fieldArchive, Field *field);
signals:
	void background(QImage);
private:
	FieldArchive *fieldArchive;
	Field *field;
};

#endif // FIELDTHREAD_H
