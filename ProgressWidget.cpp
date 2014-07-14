#include "ProgressWidget.h"

ProgressWidget::ProgressWidget(const QString &labelText, ButtonLabel buttonLabel, QWidget *parent) :
	ArchiveObserverProgressDialog(&_progress),
	_progress(parent, Qt::Dialog | Qt::WindowCloseButtonHint)
{
	_progress.setLabelText(labelText);
	_progress.setCancelButtonText(buttonLabel == Cancel ? tr("Annuler") : tr("Arrêter"));
	_progress.setRange(0, 0);
	_progress.setWindowModality(Qt::WindowModal);
	_progress.show();
}

bool ProgressWidget::observerWasCanceled() const
{
	return ArchiveObserverProgressDialog::observerWasCanceled();
}

void ProgressWidget::setObserverCanCancel(bool canCancel) const
{
	ArchiveObserverProgressDialog::setObserverCanCancel(canCancel);
}

void ProgressWidget::setObserverMaximum(unsigned int max)
{
	ArchiveObserverProgressDialog::setObserverMaximum(max);
}

void ProgressWidget::setObserverValue(int value)
{
	ArchiveObserverProgressDialog::setObserverValue(value);
}
