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
#include "FsWidget.h"

QIcon FsWidget::fileIcon;
QIcon FsWidget::dirIcon;
QMap<QString, QIcon> FsWidget::cacheIcon;

FsWidget::FsWidget(QWidget *parent) :
	QTreeWidget(parent)
{
	fileIcon = QFileIconProvider().icon(QFileIconProvider::File);
	dirIcon = QFileIconProvider().icon(QFileIconProvider::Folder);
	tmp.setFileName(QDir::tempPath() % "/deling");
	if(!tmp.isOpen()) {
		tmp.open(QIODevice::WriteOnly);
	}
	setAcceptDrops(true);
	setIndentation(0);
	setUniformRowHeights(true);
	setHeaderLabels(QStringList() << tr("Fichiers") << tr("Taille") << tr("Compression"));
	setSortingEnabled(true);
	sortByColumn(0, Qt::AscendingOrder);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setAllColumnsShowFocus(true);
	setAlternatingRowColors(true);
}

FsWidget::~FsWidget()
{
	tmp.remove();
}

QIcon FsWidget::getFileIcon(const QString &fileName)
{
	int index = fileName.lastIndexOf('.');
	QString type;

	if(index == -1)
		return fileIcon;
	else
		type = fileName.mid(index + 1);

	if(cacheIcon.contains(type))
	{
		return cacheIcon.value(type);
	}
	else
	{
		tmp.rename(QDir::tempPath() % "/" % fileName);
		if(!tmp.isOpen()) {
			if(!tmp.open(QIODevice::ReadOnly))		return fileIcon;
		}

		QIcon icon = QFileIconProvider().icon(QFileInfo(tmp));
		cacheIcon.insert(type, icon);
		return icon;
	}
}

QIcon FsWidget::getFileIcon()
{
	return fileIcon;
}

QIcon FsWidget::getDirIcon()
{
	return dirIcon;
}

void FsWidget::dragEnterEvent(QDragEnterEvent *event)
{
	if(event->mimeData()->hasUrls())
		event->acceptProposedAction();
}

void FsWidget::dragMoveEvent(QDragMoveEvent *event)
{
	if(event->mimeData()->hasUrls())
		event->acceptProposedAction();
}

void FsWidget::dropEvent(QDropEvent *event)
{
	const QMimeData *mimeData = event->mimeData();

	if(mimeData->hasUrls())
	{
		QList<QUrl> urlList = mimeData->urls();
		QStringList pathList;
		QString path;

		foreach(const QUrl &url, urlList)
		{
			path = QDir::cleanPath(url.path().mid(1));
			if(path.startsWith(QDir::rootPath())) {
				pathList.append(path);
			}
		}

		emit fileDropped(pathList);
	}
	event->acceptProposedAction();
}

/* void FsWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		QTreeWidgetItem *item = itemAt(event->pos());
		if(item == NULL)	return;

		QFile tempFile(item->text(0));
		tempFile.open(QIODevice::WriteOnly);
		tempFile.write();

		QDrag *drag = new QDrag(this);
		QMimeData *mimeData = new QMimeData;

		mimeData->setUrls(QList<QUrl>() << item);
		drag->setMimeData(mimeData);
		drag->setPixmap(iconPixmap);

		Qt::DropAction dropAction = drag->exec();
	}
} */
