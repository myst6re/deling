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
#ifndef FSWIDGET_H
#define FSWIDGET_H

#include <QtWidgets>

#define FILE_TYPE_ROLE		Qt::UserRole
#define FILE_NAME_ROLE		Qt::UserRole + 1
#define DIR		0
#define FILE	1

class TreeWidgetItem : public QTreeWidgetItem {
public:
	TreeWidgetItem(const QStringList &strings):QTreeWidgetItem(strings){}
private:
	bool operator<(const QTreeWidgetItem &other)const {
		int column = treeWidget()->sortColumn();

		if (column == 0) {
			int fileType = data(0, FILE_TYPE_ROLE).toInt(), otherFileType = other.data(0, FILE_TYPE_ROLE).toInt();
			if (fileType == DIR && otherFileType == FILE)	return true;
			if (fileType == FILE && otherFileType == DIR)	return false;
		} else if (column == 1) {
			return text(column).toInt() < other.text(column).toInt();
		}

		return text(column).toLower() < other.text(column).toLower();
	}
};

class FsWidget : public QTreeWidget
{
    Q_OBJECT
public:
	explicit FsWidget(QWidget *parent=0);
	virtual ~FsWidget();

	static QIcon getFileIcon();
	static QIcon getDirIcon();
	QIcon getFileIcon(const QString &file);
signals:
	void fileDropped(const QStringList &paths);
private:
	QFile tmp;
	static QIcon dirIcon, fileIcon;
	static QMap<QString, QIcon> cacheIcon;
protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dropEvent(QDropEvent *event);
};

#endif // FSWIDGET_H
