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
#include "FieldLoose.h"

/**
 * Extensions that carry a whole file on their own, and the FileType they build.
 * map/mim, jsm/sym and one/pcb are missing on purpose: they are handled as pairs.
 */
static const QMap<QString, Field::FileType> SINGLE_FILES{
    {"msd", Field::Msd}, {"id", Field::Id},   {"ca", Field::Ca},   {"inf", Field::Inf},
    {"rat", Field::Rat}, {"mrt", Field::Mrt}, {"pmp", Field::Pmp}, {"pmd", Field::Pmd},
    {"pvp", Field::Pvp}, {"tdw", Field::Tdw}, {"msk", Field::Msk}, {"sfx", Field::Sfx},
};

FieldLoose::FieldLoose(const QString &name) :
    Field(name)
{
}

const QStringList &FieldLoose::looseExtensions()
{
	static const QStringList exts = QStringList(SINGLE_FILES.keys())
	                                << "jsm" << "sym" << "map" << "mim" << "one" << "pcb";

	return exts;
}

bool FieldLoose::looseExtension(const QString &path, QString &ext)
{
	ext = QFileInfo(path).suffix().toLower();

	return looseExtensions().contains(ext);
}

bool FieldLoose::addFile(const QString &path)
{
	QString ext;

	if (!looseExtension(path, ext)) {
		_errorString = QObject::tr("Unknown file type");
		return false;
	}

	QFile f(path);
	if (!f.open(QIODevice::ReadOnly)) {
		_errorString = f.errorString();
		return false;
	}

	_data.insert(ext, f.readAll());
	_paths.removeAll(_filePaths.value(ext)); // a replaced file is no longer part of this field
	_filePaths.insert(ext, path);
	_paths.append(path);
	f.close();

	return true;
}

void FieldLoose::buildFiles()
{
	// Field::openFile keeps the File it already has, so a type being replaced is dropped first
	for (auto it = SINGLE_FILES.constBegin(); it != SINGLE_FILES.constEnd(); ++it) {
		if (_data.contains(it.key())) {
			deleteFile(it.value());
			openFile(it.value(), _data.value(it.key()));
		}
	}

	if (_data.contains("map") && _data.contains("mim")) {
		deleteFile(Background);
		openBackgroundFile(_data.value("map"), _data.value("mim"));
	}

	if (_data.contains("jsm")) {
		deleteFile(Jsm);
		openJsmFile(_data.value("jsm"), _data.value("sym"));
	}

	if (_data.contains("one")) {
		deleteFile(CharaOne);
		openCharaFile(_data.value("one"), _data.value("pcb"));
	}
}

bool FieldLoose::saveFiles()
{
	bool ok = true;

	// Only the single-file types: the background and the models are built from two files
	// each and cannot be written back as one.
	for (auto it = SINGLE_FILES.constBegin(); it != SINGLE_FILES.constEnd(); ++it) {
		if (!_filePaths.contains(it.key()) || !hasFile(it.value())) {
			continue;
		}

		File *file = getFile(it.value());
		if (file == nullptr || !file->isModified()) {
			continue;
		}

		QByteArray data;
		QFile f(_filePaths.value(it.key()));
		if (!file->save(data) || !f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
			_errorString = f.errorString();
			ok = false;
			continue;
		}
		f.write(data);
		f.close();
		file->setModified(false);
	}

	if (_filePaths.contains("jsm") && hasJsmFile() && getJsmFile()->isModified()) {
		QByteArray data;
		QFile f(_filePaths.value("jsm"));
		if (!getJsmFile()->save(data) || !f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
			_errorString = f.errorString();
			ok = false;
		} else {
			f.write(data);
			f.close();
			getJsmFile()->setModified(false);
		}
	}

	return ok;
}
