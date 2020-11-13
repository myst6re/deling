#ifndef ARCHIVEOBSERVERPROGRESSDIALOG_H
#define ARCHIVEOBSERVERPROGRESSDIALOG_H

#include "ArchiveObserver.h"
#include <QProgressDialog>

class ArchiveObserverProgressDialog : public ArchiveObserver
{
public:
	ArchiveObserverProgressDialog(QProgressDialog *progressDialog);
	virtual bool observerWasCanceled() const;
	virtual void setObserverCanCancel(bool canCancel) const;
	virtual void setObserverMaximum(unsigned int max);
	virtual void setObserverValue(int value);
private:
	QProgressDialog *_progressDialog;
};

#endif // ARCHIVEOBSERVERPROGRESSDIALOG_H
