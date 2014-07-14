#include "ArchiveObserverProgressDialog.h"

ArchiveObserverProgressDialog::ArchiveObserverProgressDialog(QProgressDialog *progressDialog) :
	_progressDialog(progressDialog)
{
}

bool ArchiveObserverProgressDialog::observerWasCanceled() const
{
	return _progressDialog->wasCanceled();
}

void ArchiveObserverProgressDialog::setObserverCanCancel(bool canCancel) const
{
	_progressDialog->setEnabled(canCancel);
}

void ArchiveObserverProgressDialog::setObserverMaximum(unsigned int max)
{
	_progressDialog->setMaximum(max);
}

void ArchiveObserverProgressDialog::setObserverValue(int value)
{
	_progressDialog->setValue(value);
}
