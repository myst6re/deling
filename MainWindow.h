#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "parameters.h"
#include "Config.h"
#include "Data.h"
#include "MsdFile.h"
#include "FieldArchivePC.h"
#include "FieldArchivePS.h"
#include "Search.h"
#include "BGPreview.h"
#include "BGThread.h"
#include "TextPreview.h"
#include "MsdWidget.h"
#include "JsmWidget.h"
#include "WalkmeshWidget.h"
#include "BackgroundWidget.h"
#include "MiscWidget.h"
#include "VarManager.h"
#include "MiscSearch.h"
#include "FsDialog.h"
#include "ConfigDialog.h"

#define TEXTPAGE		0
#define SCRIPTPAGE		1
#define WALKMESHPAGE	2
#define BACKGROUNDPAGE	3
#define MISCPAGE		4

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();
public slots:
	void openFile(QString path=QString());
private slots:
	void filterMap();
	void fillPage();
	void about();
	int closeFiles(bool quit=false);
	void setModified(bool modified);
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
	MsdFile *msdFile;
	JsmFile *jsmFile;
	WalkmeshFile *walkmeshFile;
	MiscFile *miscFile;

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
	MsdWidget *textPage;
	JsmWidget *scriptPage;
	WalkmeshWidget *walkmeshPage;
	BackgroundWidget *backgroundPage;
	MiscWidget *miscPage;
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
