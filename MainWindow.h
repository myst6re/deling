/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2012 Arzel J�r�me <myst6re@gmail.com>
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "parameters.h"
#include "Config.h"
#include "Data.h"
#include "files/MsdFile.h"
#include "FieldArchivePC.h"
#include "FieldArchivePS.h"
#include "FieldThread.h"
#include "Search.h"
#include "BGPreview.h"
#include "TextPreview.h"
#include "widgets/MsdWidget.h"
#include "widgets/JsmWidget.h"
#include "widgets/CharaWidget.h"
#include "widgets/WalkmeshWidget.h"
#include "widgets/BackgroundWidget.h"
#include "widgets/EncounterWidget.h"
#include "widgets/TdwWidget.h"
#include "widgets/MiscWidget.h"
#include "VarManager.h"
#include "MiscSearch.h"
#include "FsDialog.h"
#include "ConfigDialog.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	enum Pages {
		TextPage, ScriptPage, ModelPage, WalkmeshPage, BackgroundPage, EncounterPage, FontPage, MiscPage
	};

	MainWindow();
public slots:
	void openFile(QString path=QString());
private slots:
	void filterMap();
	void fillPage();
	void fillBackground(const QImage &image);
	void about();
	int closeFiles(bool quit=false);
	void setModified(bool modified=true);
	void save();
	void saveAs(QString path=QString());
	void optimizeArchive();
	void manageArchive();
	void configDialog();
	void gotoField(int fieldID);
	void gotoText(const QString &text, int fieldID, int textID, Qt::CaseSensitivity cs, bool reverse, bool regexp);
	void gotoScript(int fieldID, int groupID, int methodID, int opcodeID);
	void search();
	void varManager();
	void runFF8();
	void miscSearch();
	void bgPage();
	void setCurrentPage(int index);
	void fullScreen();
private:
	bool openArchive(const QString &path);
	bool openFsArchive(const QString &);
	bool openMsdFile(const QString &);
	bool openJsmFile(const QString &);
	bool openIsoArchive(const QString &);
	void setReadOnly(bool);

	FieldArchive *fieldArchive;
	FieldPC *field;
	FieldThread *fieldThread;//TODO

	MiscSearch *miscSearchD;
	QLabel *currentPath;
	Search *searchDialog;
	QAction *actionSave;
	QAction *actionSaveAs;
	QAction *actionOpti;
	QAction *actionClose;
	QAction *actionFind;
	QAction *actionRun;
	QMenu *menuLang;
	QAction *actionEncode;
	QToolBar *toolBar;
	QList<PageWidget *> pageWidgets;
	QTreeWidget *list1;
	QLineEdit *lineSearch;
	BGPreview *bgPreview;
	QTabBar *tabBar;
	QStackedWidget *mainStackedWidget, *stackedWidget;
	FsDialog *fsDialog;
	VarManager *_varManager;
	bool firstShow;
protected:
	void showEvent(QShowEvent *);
	void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
