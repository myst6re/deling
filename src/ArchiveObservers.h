#ifndef ARCHIVEOBSERVERS_H
#define ARCHIVEOBSERVERS_H

#include "ArchiveObserver.h"
#include <QList>

class ArchiveObservers : public ArchiveObserver
{
public:
	ArchiveObservers(ArchiveObserver *observer);
	ArchiveObservers(const QList<ArchiveObserver *> &observers);
	virtual bool observerWasCanceled() const;
	virtual void setObserverCanCancel(bool canCancel) const;
	virtual void setObserverMaximum(unsigned int max);
	virtual void setObserverValue(int value);
private:
	QList<ArchiveObserver *> _observers;
};

#endif // ARCHIVEOBSERVERS_H
