/****************************************************************************
 ** Makou Reactor Final Fantasy VII Field Script Editor
 ** Copyright (C) 2009-2021 Arzel Jérôme <myst6re@gmail.com>
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
#pragma once

#include <QString>
#include "ArchiveObserver.h"

class FsArchive;

struct CLIObserver : public ArchiveObserver
{
	CLIObserver() {}
	inline void setFilename(const QString &filename) {
		_filename = filename;
	}
	inline bool observerWasCanceled() const override {
		return false;
	}
	inline void setObserverMaximum(unsigned int max) override {
		_maximum = max;
	}
	virtual void setObserverValue(int value) override;
	virtual void setObserverCanCancel(bool canCancel) const override;
private:
	void setPercent(quint8 percent);
	qint64 _maximum;
	quint8 _lastPercent;
	QString _filename;
};

class CLI
{
public:
	static void exec();
private:
	static void commandExport();
	static void commandImport();
	static FsArchive *openArchive(const QString &ext, const QString &path);
	static QStringList filteredFiles(const QStringList &fileList, int offset, const QStringList &includePatterns, const QStringList &excludePatterns);
	static CLIObserver observer;
};
