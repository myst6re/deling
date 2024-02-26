/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2024 Arzel Jérôme <myst6re@gmail.com>
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

#include "FieldThread.h"
#include "FF8Image.h"
#include "FieldArchive.h"
#include "Field.h"

void FieldThread::run()
{
	qDebug() << "thread started";

	if (fieldArchive && field && fieldArchive->openBG(field)
            && field->hasBackgroundFile()) {
		qDebug() << "field opened";
		QImage bg = field->getBackgroundFile()->background();
		qDebug() << "background generated";
        emit background(bg);
    } else {
		emit background(FF8Image::errorImage());
    }
}

void FieldThread::setData(FieldArchive *fieldArchive, Field *field)
{
	this->fieldArchive = fieldArchive;
	this->field = field;
}
