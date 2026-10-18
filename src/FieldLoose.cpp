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

// The files MainWindow::fillPage unloads when another field is shown
static bool isHeavy(Field::FileType fileType)
{
	return fileType == Field::Background || fileType == Field::CharaOne
	       || fileType == Field::Tdw || fileType == Field::Pmp;
}

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

FieldLoose *FieldLoose::openNeighbour(const QStringList &knownPaths, const QString &name)
{
	if (name.isEmpty()) {
		return nullptr;
	}

	for (const QString &knownPath: knownPaths) {
		const QDir dir = QFileInfo(knownPath).absoluteDir();
		// mapdata/bc/bcgate1a/ -> mapdata/<prefix>/<name>/, then a sibling folder, then the same one
		const QStringList candidates = {
		    dir.absoluteFilePath(QString("../../%1/%2").arg(name.left(2), name)),
		    dir.absoluteFilePath(QString("../%1").arg(name)),
		    dir.absolutePath()
		};

		for (const QString &candidate: candidates) {
			const QDir fieldDir(candidate);

			if (!fieldDir.exists(name % ".id")) {
				continue;
			}

			FieldLoose *field = new FieldLoose(name);
			for (const QString &ext: looseExtensions()) {
				const QString path = fieldDir.absoluteFilePath(name % "." % ext);
				if (QFile::exists(path)) {
					field->addFile(path);
				}
			}
			field->buildFiles();
			field->setOpen(true);

			return field;
		}
	}

	return nullptr;
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
	f.close();

	_paths.removeAll(_filePaths.value(ext)); // a replaced file is no longer part of this field
	_filePaths.insert(ext, path);
	_paths.append(path);
	_replaced.insert(ext);

	return true;
}

QByteArray FieldLoose::readFile(const QString &ext) const
{
	if (!_filePaths.contains(ext)) {
		return QByteArray();
	}

	QFile f(_filePaths.value(ext));
	if (!f.open(QIODevice::ReadOnly)) {
		qWarning() << "FieldLoose::readFile" << f.fileName() << f.errorString();
		return QByteArray();
	}

	return f.readAll();
}

// The files a type is built from, the ones it cannot do without first
static QStringList requiredExtensions(Field::FileType fileType)
{
	switch (fileType) {
	case Field::Background:
		return {"map", "mim"};
	case Field::Jsm:
		return {"jsm"};
	case Field::CharaOne:
		return {"one"};
	default:
		return SINGLE_FILES.values().contains(fileType) ? QStringList(SINGLE_FILES.key(fileType)) : QStringList();
	}
}

static QStringList sourceExtensions(Field::FileType fileType)
{
	switch (fileType) {
	case Field::Jsm:
		return {"jsm", "sym"};
	case Field::CharaOne:
		return {"one", "pcb"};
	default:
		return requiredExtensions(fileType);
	}
}

bool FieldLoose::canBuild(FileType fileType) const
{
	const QStringList required = requiredExtensions(fileType);

	for (const QString &ext: required) {
		if (!_filePaths.contains(ext)) {
			return false;
		}
	}

	return !required.isEmpty();
}

// Not built yet (or unloaded since), or one of its files was replaced
bool FieldLoose::mustBuild(FileType fileType) const
{
	if (_getFile(fileType) == nullptr) {
		return true;
	}

	for (const QString &ext: sourceExtensions(fileType)) {
		if (_replaced.contains(ext)) {
			return true;
		}
	}

	return false;
}

void FieldLoose::buildFile(FileType fileType)
{
	// Field::openFile keeps the File it already has, so a file being rebuilt is dropped first
	deleteFile(fileType);

	switch (fileType) {
	case Background:
		openBackgroundFile(readFile("map"), readFile("mim"));
		break;
	case Jsm:
		openJsmFile(readFile("jsm"), readFile("sym"));
		break;
	case CharaOne:
		openCharaFile(readFile("one"), readFile("pcb"));
		break;
	default:
		openFile(fileType, readFile(requiredExtensions(fileType).first()));
		break;
	}

	for (const QString &ext: sourceExtensions(fileType)) {
		_replaced.remove(ext);
	}
}

// A file that can be built counts as there, like a file of FieldPC that is still in its archive
bool FieldLoose::hasFile(FileType fileType) const
{
	return _getFile(fileType) != nullptr || canBuild(fileType);
}

File *FieldLoose::getFile(FileType fileType)
{
	if (canBuild(fileType) && mustBuild(fileType)) {
		buildFile(fileType);
	}

	return _getFile(fileType);
}

void FieldLoose::buildFiles(bool withHeavyFiles)
{
	for (FileType fileType: fileTypes()) {
		if (withHeavyFiles || !isHeavy(fileType)) {
			getFile(fileType);
		}
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
