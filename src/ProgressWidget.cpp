#include "ProgressWidget.h"

ProgressWidget::ProgressWidget(const QString &labelText, ButtonLabel buttonLabel, QWidget *parent) :
	ArchiveObserverProgressDialog(&_progress),
	_progress(parent, Qt::Dialog | Qt::WindowCloseButtonHint),
	_taskBarButton(parent)
{
	_progress.setLabelText(labelText);
	_progress.setCancelButtonText(buttonLabel == Cancel ? tr("Annuler") : tr("Arrêter"));
	_progress.setRange(0, 0);
	_progress.setWindowModality(Qt::WindowModal);
	_progress.show();
	_taskBarButton.progress()->hide();
}

ProgressWidget::~ProgressWidget()
{
	_taskBarButton.progress()->hide();
}

void ProgressWidget::setObserverMaximum(unsigned int max)
{
	ArchiveObserverProgressDialog::setObserverMaximum(max);
	_taskBarButton.progress()->setRange(0, max);
	_taskBarButton.progress()->reset();
	_taskBarButton.progress()->show();
}

void ProgressWidget::setObserverValue(int value)
{
	ArchiveObserverProgressDialog::setObserverValue(value);
	_taskBarButton.progress()->setValue(value);
}
