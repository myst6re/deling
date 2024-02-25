/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2024 Arzel Jérôme <myst6re@gmail.com>
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
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
	for (ArchiveObserver *obs: _observers) {
		if (obs->observerWasCanceled()) {
			return true;
		}
	}

	return false;
}

void ArchiveObservers::setObserverCanCancel(bool canCancel) const
{
	for (ArchiveObserver *obs: _observers) {
		obs->setObserverCanCancel(canCancel);
	}
}

void ArchiveObservers::setObserverMaximum(unsigned int max)
{
	for (ArchiveObserver *obs: _observers) {
		obs->setObserverMaximum(max);
	}
}

void ArchiveObservers::setObserverValue(int value)
{
	for (ArchiveObserver *obs: _observers) {
		obs->setObserverValue(value);
	}
}

