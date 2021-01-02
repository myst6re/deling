/****************************************************************************
 ** Makou Reactor Final Fantasy VII Field Script Editor
 ** Copyright (C) 2009-2013 Arzel Jérôme <myst6re@gmail.com>
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
#include "QTaskbarButton.h"

#ifndef QTASKBAR_WIN

QTaskbarProgress::QTaskbarProgress(QObject *parent) :
    QObject(parent)
{
}

bool QTaskbarProgress::isPaused() const
{
	return (int(_state) | int(Paused)) != 0;
}

bool QTaskbarProgress::isStopped() const
{
	return (int(_state) | int(Stopped)) != 0;
}

bool QTaskbarProgress::isVisible() const
{
	return (int(_state) | int(Visible)) != 0;
}

int QTaskbarProgress::maximum() const
{
	return _maximum;
}

int QTaskbarProgress::minimum() const
{
	return _minimum;
}

int QTaskbarProgress::value() const
{
	return _value;
}

void QTaskbarProgress::hide()
{
	setVisible(false);
}

void QTaskbarProgress::pause()
{
	setPaused(true);
}

void QTaskbarProgress::reset()
{
	_state = None;
	_value = _minimum;
}

void QTaskbarProgress::resume()
{
	setPaused(false);
	_state = State(int(_state) & ~int(Stopped));
	emit stoppedChanged(false);
}

void QTaskbarProgress::setMaximum(int maximum)
{
	_maximum = maximum;
	emit maximumChanged(maximum);
}

void QTaskbarProgress::setMinimum(int minimum)
{
	_minimum = minimum;
	emit minimumChanged(minimum);
}

void QTaskbarProgress::setPaused(bool paused)
{
	if (paused) {
		_state = State(int(_state) | int(Paused));
	} else {
		_state = State(int(_state) & ~int(Paused));
	}
	emit pausedChanged(paused);
}

void QTaskbarProgress::setRange(int minimum, int maximum)
{
	setMinimum(minimum);
	setMaximum(maximum);
}

void QTaskbarProgress::setValue(int value)
{
	_value = value;
	emit valueChanged(value);
}

void QTaskbarProgress::setVisible(bool visible)
{
	if (visible) {
		_state = State(int(_state) | int(Visible));
	} else {
		_state = State(int(_state) & ~int(Visible));
	}
	emit visibilityChanged(visible);
}

void QTaskbarProgress::show()
{
	setVisible(true);
}

void QTaskbarProgress::stop()
{
	_state = State(int(_state) | int(Stopped));
	emit stoppedChanged(true);
}

QTaskbarButton::QTaskbarButton(QObject *parent) :
    QObject(parent), _progress(new QTaskbarProgress(parent))
{
}

QTaskbarButton::~QTaskbarButton()
{
	delete _progress;
}

QString QTaskbarButton::overlayAccessibleDescription() const
{
	return _accessibleDescription;
}

QIcon QTaskbarButton::overlayIcon() const
{
	return _icon;
}

QTaskbarProgress *QTaskbarButton::progress() const
{
	return _progress;
}

void QTaskbarButton::clearOverlayIcon()
{
	_icon = QIcon();
}

void QTaskbarButton::setOverlayAccessibleDescription(const QString &description)
{
	_accessibleDescription = description;
}

void QTaskbarButton::setOverlayIcon(const QIcon &icon)
{
	_icon = icon;
}

#endif
