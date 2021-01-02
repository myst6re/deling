#include "ArchiveObservers.h"

ArchiveObservers::ArchiveObservers(ArchiveObserver *observer)
{
	_observers.append(observer);
}

ArchiveObservers::ArchiveObservers(const QList<ArchiveObserver *> &observers) :
	_observers(observers)
{
}

bool ArchiveObservers::observerWasCanceled() const
{
	foreach (ArchiveObserver *obs, _observers) {
		if (obs->observerWasCanceled()) {
			return true;
		}
	}

	return false;
}

void ArchiveObservers::setObserverCanCancel(bool canCancel) const
{
	foreach (ArchiveObserver *obs, _observers) {
		obs->setObserverCanCancel(canCancel);
	}
}

void ArchiveObservers::setObserverMaximum(unsigned int max)
{
	foreach (ArchiveObserver *obs, _observers) {
		obs->setObserverMaximum(max);
	}
}

void ArchiveObservers::setObserverValue(int value)
{
	foreach (ArchiveObserver *obs, _observers) {
		obs->setObserverValue(value);
	}
}

