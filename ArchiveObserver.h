#ifndef ARCHIVEOBSERVER_H
#define ARCHIVEOBSERVER_H

struct ArchiveObserver
{
	ArchiveObserver() {}
	virtual bool observerWasCanceled() const=0;
	virtual void setObserverCanCancel(bool canCancel) const=0;
	virtual void setObserverMaximum(unsigned int max)=0;
	virtual void setObserverValue(int value)=0;
};

#endif // ARCHIVEOBSERVER_H
