#ifndef PROGRESSWIDGET_H
#define PROGRESSWIDGET_H

#include <QProgressDialog>
#include "ArchiveObserverProgressDialog.h"
#include "QTaskbarButton.h"

class ProgressWidget : public QObject, public ArchiveObserverProgressDialog
{
	Q_OBJECT
public:
	enum ButtonLabel {
		Cancel,
		Stop
	};

	ProgressWidget(const QString &labelText, ButtonLabel buttonLabel, QWidget *parent);
	virtual ~ProgressWidget();
	virtual void setObserverMaximum(unsigned int max);
	virtual void setObserverValue(int value);
private:
	QProgressDialog _progress;
	QTaskbarButton _taskBarButton;
};

#endif // PROGRESSWIDGET_H
