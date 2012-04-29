#ifndef FSDIALOG_H
#define FSDIALOG_H

#include <QtGui>
#include "FsArchive.h"
#include "Config.h"
#include "FsWidget.h"
#include "FsPreviewWidget.h"
#include "FF8Image.h"

class FsDialog : public QWidget
{
    Q_OBJECT
public:
	explicit FsDialog(FsArchive *fsArchive, QWidget *parent=0);
	~FsDialog();
signals:
private slots:
	void setButtonsEnabled();
	void generatePreview();
	void doubleClicked(QTreeWidgetItem *);
	void openDir();
	void parentDir();
	void extract(QStringList sources=QStringList());
	void replace(QString source=QString(), QString destination=QString());
	void add(QStringList sources=QStringList());
	void remove(QStringList destinations=QStringList());
	void rename();
	void renameOK(QTreeWidgetItem *, int);
private:
	void openDir(const QString &);
	QStringList listFilesInDir(QString dirPath);

	QToolBar *toolBar;
	QAction *extractAction, *replaceAction, *_addAction, *removeAction, *renameAction;
	FsWidget *list;
	FsPreviewWidget *preview;
	QPushButton *up;
	QLineEdit *pathWidget;
	QString currentPath;
	FsArchive *fsArchive;
};

#endif // FSDIALOG_H
