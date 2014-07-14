#ifndef PROGRESSWIDGET_H
#define PROGRESSWIDGET_H

#include <QProgressDialog>
#include "ArchiveObserverProgressDialog.h"

class ProgressWidget : public QObject, public ArchiveObserverProgressDialog
{
	Q_OBJECT
public:
	enum ButtonLabel {
		Cancel,
		Stop
	};

	ProgressWidget(const QString &labelText, ButtonLabel buttonLabel, QWidget *parent);
	virtual bool observerWasCanceled() const;
	virtual void setObserverCanCancel(bool canCancel) const;
	virtual void setObserverMaximum(unsigned int max);
	virtual void setObserverValue(int value);
private:
	QProgressDialog _progress;
};

#endif // PROGRESSWIDGET_H
