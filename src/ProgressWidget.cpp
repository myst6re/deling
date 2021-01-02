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
	_taskBarButton.setState(QTaskBarButton::Indeterminate);
}

ProgressWidget::~ProgressWidget()
{
	_taskBarButton.setState(QTaskBarButton::Invisible);
}

void ProgressWidget::setObserverMaximum(unsigned int max)
{
	ArchiveObserverProgressDialog::setObserverMaximum(max);
	_taskBarButton.setRange(0, max);
	_taskBarButton.setState(QTaskBarButton::Normal);
}

void ProgressWidget::setObserverValue(int value)
{
	ArchiveObserverProgressDialog::setObserverValue(value);
	_taskBarButton.setValue(value);
}
