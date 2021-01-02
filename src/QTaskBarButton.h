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
#ifndef QTASKBARBUTTON_H
#define QTASKBARBUTTON_H

#ifdef Q_OS_WIN32
#	include <QWinTaskbarButton>

typedef QWinTaskbarProgress QTaskbarProgress;
typedef QWinTaskbarButton QTaskbarButton;

#define QTASKBAR_WIN

#else

#include <QObject>
#include <QString>
#include <QIcon>

class QTaskbarProgress : public QObject
{
	Q_OBJECT

	enum State {
		None = 0x0,
		Paused = 0x1,
		Stopped = 0x2,
		Visible = 0x4
	};
public:
	explicit QTaskbarProgress(QObject *parent = nullptr);

	bool isPaused() const;
	bool isStopped() const;
	bool isVisible() const;
	int maximum() const;
	int minimum() const;
	int value() const;
public slots:
	void hide();
	void pause();
	void reset();
	void resume();
	void setMaximum(int maximum);
	void setMinimum(int minimum);
	void setPaused(bool paused);
	void setRange(int minimum, int maximum);
	void setValue(int value);
	void setVisible(bool visible);
	void show();
	void stop();
signals:
	void maximumChanged(int maximum);
	void minimumChanged(int minimum);
	void pausedChanged(bool paused);
	void stoppedChanged(bool stopped);
	void valueChanged(int value);
	void visibilityChanged(bool visible);

private:
	State _state;
	int _maximum, _minimum, _value;
};

class QTaskbarButton : public QObject
{
	Q_OBJECT
public:
	explicit QTaskbarButton(QObject *parent = nullptr);
	virtual ~QTaskbarButton();
	QString overlayAccessibleDescription() const;
	QIcon overlayIcon() const;
	QTaskbarProgress *progress() const;
public slots:
	void clearOverlayIcon();
	void setOverlayAccessibleDescription(const QString &description);
	void setOverlayIcon(const QIcon &icon);
private:
	QIcon _icon;
	QString _accessibleDescription;
	QTaskbarProgress *_progress;
};

#endif

#endif // QTASKBARBUTTON_H
