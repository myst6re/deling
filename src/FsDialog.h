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

#include <QtWidgets>
#include "FsArchive.h"

class FsPreviewWidget;
class FsWidget;

class FsDialog : public QWidget
{
	Q_OBJECT
public:
	explicit FsDialog(FsArchive *fsArchive, QWidget *parent = nullptr);
	virtual ~FsDialog();
	const QString &getCurrentPath() const;
	void setCurrentPath(const QString &path);
signals:
private slots:
	void setButtonsEnabled();
	void changePreview();
	void changeImageInPreview(int imageID);
	void changeImagePaletteInPreview(int palID);
	void doubleClicked(QTreeWidgetItem *item);
	void openDir();
	void parentDir();
	void extract(QStringList sources = QStringList());
	void replace(QString source = QString(), QString destination = QString());
	void addFile(QStringList sources = QStringList());
	void addDirectory(QString source = QString());
	void remove(QStringList destinations = QStringList());
	void rename();
	void renameOK(QTreeWidgetItem *item, int row);
	void exportImages();
private:
	void generatePreview();
	void add(QStringList sources, bool fromDir = false);
	void openDir(const QString &dirPath);
	QStringList listFilesInDir(QString dirPath);
	static FiCompression compressionMessage(QWidget *parent);

	QToolBar *toolBar;
	QAction *extractAction, *replaceAction, *_addFileAction, *_addDirAction, *removeAction, *renameAction;
	FsWidget *list;
	FsPreviewWidget *preview;
	QPushButton *up;
	QLineEdit *pathWidget;
	QString currentPath;
	FsArchive *fsArchive;
	int currentImage, currentPal;
};
