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
#pragma once

#include "Search.h"

class MiscSearch : public QDialog
{
	Q_OBJECT
public:
	MiscSearch(FieldArchive *fieldArchive, QWidget *parent = nullptr);

private slots:
	void search(int);
private:
	void fillList();
    static int getMapId(const QList<Field *> &fields);

	QListWidget *list;
	QPlainTextEdit *textEdit;
	FieldArchive *fieldArchive;
};
