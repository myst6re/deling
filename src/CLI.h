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

#include <QList>
#include <QRegularExpression>
#include <QString>
#include "ArchiveObserver.h"

class FsArchive;
class JsmFile;

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
	virtual void flush() override {
		setPercent(_lastPercent);
	}
private:
	void setPercent(quint8 percent);
	qint64 _maximum;
	quint8 _lastPercent;
	QString _filename;
};

// Used instead of CLIObserver when --no-progress is passed
struct SilentObserver : public ArchiveObserver
{
	inline bool observerWasCanceled() const override {
		return false;
	}
	inline void setObserverCanCancel(bool canCancel) const override {
		Q_UNUSED(canCancel)
	}
	inline void setObserverMaximum(unsigned int max) override {
		Q_UNUSED(max)
	}
	inline void setObserverValue(int value) override {
		Q_UNUSED(value)
	}
};

class CLI
{
public:
	static void exec();
private:
	static void commandExport();
	static void commandImport();
	static void commandUnpack();
	static void commandPack();
	static void commandExportScripts();
	static void commandImportScripts();
	static FsArchive *openArchive(const QString &ext, const QString &path);
	static QStringList filteredFiles(const QStringList &fileList, const QStringList &includePatterns, const QStringList &excludePatterns);
	static void printLine(const QString &line);
	static bool isJsmPath(const QString &path);
	static QString jsmName(const QString &jsmPath);
	static QString symPath(const QString &jsmPath);
	static bool openJsm(const QString &path, JsmFile &jsm);
	static bool writeFile(const QString &path, const QByteArray &data);
	static bool saveJsm(const QString &path, JsmFile &jsm);
	static QList<QRegularExpression> fieldFilters(const QStringList &patterns);
	static bool matchesField(const QString &name, const QList<QRegularExpression> &filters);
	static CLIObserver observer;
};
