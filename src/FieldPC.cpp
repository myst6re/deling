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
#include "FieldPC.h"
#include "FsArchive.h"

QRegularExpression FieldPC::_pathReg = QRegularExpression("^" + QRegularExpression::escape("C:\\ff8\\Data\\") + "(\\w+)" + QRegularExpression::escape("\\FIELD\\mapdata\\") + "(\\w+)" + QRegularExpression::escape("\\"), QRegularExpression::CaseInsensitiveOption);
QRegularExpression FieldPC::_pathRegWithoutArchive = QRegularExpression("^" + QRegularExpression::escape("C:\\ff8\\Data\\") + "(\\w+)" + QRegularExpression::escape("\\FIELD\\mapdata\\") + "(\\w+)" + QRegularExpression::escape("\\") + "(\\w+)" + QRegularExpression::escape("\\"), QRegularExpression::CaseInsensitiveOption);

FieldPC::FieldPC(const QString &name, const QString &path, FsArchive *archive, const QString &gameLang)
    : Field(name), _path(path), _gameLang(gameLang), header(nullptr), _archive(archive)
{
	open(archive);
}

FieldPC::FieldPC(const QString &path, const QString &gameLang)
    : Field(QString()), _path(path), _gameLang(gameLang), header(nullptr), _archive(nullptr)
{
	open(path);
}

FieldPC::~FieldPC()
{
	if (header != nullptr) {
		delete header;
	}
}

bool FieldPC::hasFile(Field::FileType fileType) const
{
	if (header == nullptr) {
		return false;
	}

	QList<FileExt> exts = typeToExt(fileType);
	for (const FileExt &ext: exts) {
		if (!header->fileExists(filePath(ext))) {
			return false;
		}
	}

	return !exts.isEmpty();
}

File *FieldPC::getFile(Field::FileType fileType)
{
	File *f = _getFile(fileType);
	
	if (f != nullptr) {
		return f;
	}

	if (!openOptimized(QList<FileType>() << fileType)) {
		return newFile(fileType);
	}

	f = _getFile(fileType);

	if (f != nullptr) {
		return f;
	}

	return newFile(fileType);
}

bool FieldPC::openFull()
{
	if (!header) {
		return false;
	}

	return openOptimized(fileTypes());
}

bool FieldPC::open(const QString &path)
{
	setOpen(false);

	QString archivePath = path;
	archivePath.chop(1);

	if (header != nullptr) {
		delete header;
	}
	header = new FsArchive(archivePath);
	_archive = nullptr;
	if (!header->isOpen()) {
		qWarning() << "fieldData pas ouvert" << path;
		delete header;
		header = nullptr;
		return false;
	}

	FsHeader *infInfos = header->getFile("*.inf");

	if (!infInfos) {
		qWarning() << "fieldData not opened" << path;
		return false;
	}
	
	QRegularExpressionMatch match = _pathRegWithoutArchive.match(infInfos->path());
	if (!match.hasMatch()) {
		qWarning() << "fieldData not opened wrong path" << infInfos->path();
		return false;
	}

	_lang = match.captured(1);
	_subDir = match.captured(2);
	setName(match.captured(3));

	setOpen(true);
	return true;
}

bool FieldPC::openOptimizedWithoutArchive(const QList<FileType> &selectedFileTypes)
{
	if (selectedFileTypes.isEmpty()) {
		return false;
	}

	QMap<FileExt, QString> files;

	for (const FileType &type: selectedFileTypes) {
		QList<FileExt> exts = typeToExt(type);
		for (FileExt ext: exts) {
			QString path = filePath(ext);

			if (header->fileExists(path)) {
				files[ext] = path;
			}
		}
	}

	for (const FileType &type: selectedFileTypes) {
		QList<FileExt> exts = typeToExt(type);
		if (exts.isEmpty()) {
			continue;
		}
		const QString &path = files.value(exts.first());
		if (path.isEmpty()) {
			continue;
		}

		if (type == Jsm && files.contains(ExtSym)) {
			openJsmFile(header->fileData(path), header->fileData(files[ExtSym]));
		} else if (type == Background && files.contains(ExtMim)) {
			openBackgroundFile(header->fileData(path), header->fileData(files[ExtMim]));
		} else if (type == CharaOne) {
			openCharaFile(header->fileData(path), files.contains(ExtPcb) ? header->fileData(files[ExtPcb]) : QByteArray());
		} else {
			openFile(type, header->fileData(path));
		}
	}

	return true;
}

bool FieldPC::openOptimized(const QList<FileType> &selectedFileTypes)
{
	if (selectedFileTypes.isEmpty()) {
		return false;
	}

	if (_archive == nullptr) {
		return openOptimizedWithoutArchive(selectedFileTypes);
	}

	QMap<FileExt, FsHeader *> files;
	quint32 maxSize = 0;

	// Gather max decompression size and header infos
	for (const FileType &type: selectedFileTypes) {
		QList<FileExt> exts = typeToExt(type);
		for (const FileExt &ext: exts) {
			FsHeader *infos = header->getFile(filePath(ext));

			if (infos != nullptr) {
				maxSize = qMax(maxSize, infos->position() + infos->uncompressedSize());
				files[ext] = infos;
			}
		}
	}

	if (maxSize == 0) {
		qWarning() << "No files!" << name();
		return false;
	}

	// Get data and open files
	QByteArray fs_data = _archive->fileData("*" % name() % ".fs", true, int(maxSize));

	if (fs_data.isEmpty()) {
		qWarning() << "No data!" << name() << maxSize;
		return false;
	}

	for (const FileType &type: selectedFileTypes) {
		QList<FileExt> exts = typeToExt(type);
		if (exts.isEmpty()) {
			continue;
		}
		FsHeader *infos = files.value(exts.first());
		if (infos == nullptr) {
			continue;
		}

		if (type == Jsm && files.contains(ExtSym)) {
			openJsmFile(infos->data(fs_data), files[ExtSym]->data(fs_data));
		} else if (type == Background && files.contains(ExtMim)) {
			openBackgroundFile(infos->data(fs_data), files[ExtMim]->data(fs_data));
		} else if (type == CharaOne) {
			openCharaFile(infos->data(fs_data), files.contains(ExtPcb) ? files[ExtPcb]->data(fs_data) : QByteArray());
		} else {
			openFile(type, infos->data(fs_data));
		}
	}

	return true;
}

bool FieldPC::open(FsArchive *archive)
{
	setOpen(false);

	if (header)	delete header;
	_archive = archive;

	FsHeader *flInfos = archive->getFile("*" % name() % ".fl");
	if (!flInfos) {
		qWarning() << "fieldData not opened" << name() << archive->path();
		return false;
	}
	QRegularExpressionMatch match = _pathReg.match(flInfos->path());
	if (!match.hasMatch()) {
		qWarning() << "fieldData not opened" << name() << "wrong path" << flInfos->path();
		return false;
	}
	_lang = match.captured(1);
	_subDir = match.captured(2);

	header = new FsArchive(archive->fileData(flInfos->path()), archive->fileData("*" % name() % ".fi"));
	if (!header->isOpen()) {
		qWarning() << "fieldData not opened" << name();
		delete header;
		header = nullptr;
		return false;
	}

	setOpen(true);
	return true;
}

bool FieldPC::save(const QString &path)
{
	QString oldArchivePath = _path;
	oldArchivePath.chop(1);
	
	QString archivePath = path;
	archivePath.chop(1);
	
	bool saveAs = QFileInfo(FsArchive::fsPath(oldArchivePath)) != QFileInfo(FsArchive::fsPath(archivePath));

	QFile fs(FsArchive::fsPath(archivePath));
	if (fs.open(saveAs ? (QIODevice::WriteOnly | QIODevice::Truncate) : QIODevice::ReadWrite)) {
		QFile fl(FsArchive::flPath(archivePath));
		if (fl.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
			QFile fi(FsArchive::fiPath(archivePath));
			if (fi.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
				QByteArray fs_data, fl_data, fi_data;

				if (saveAs) {
					QFile oldFs(FsArchive::fsPath(oldArchivePath));
					if (!oldFs.open(QIODevice::ReadOnly)) {
						return false;
					}
					fs_data = oldFs.readAll();
				} else {
					fs_data = fs.readAll();
					fs.resize(0);
					fs.reset();
				}

				/* fs_data must be filled by the file contents
				 * fl_data and fi_data are rebuilt
				 */
				save(fs_data, fl_data, fi_data);

				fs.write(fs_data);
				fl.write(fl_data);
				fi.write(fi_data);

				fi.close();
			} else {
				return false;
			}
			fl.close();
		} else {
			return false;
		}
		fs.close();
	} else {
		return false;
	}

	return true;
}

void FieldPC::save(QByteArray &fs_data, QByteArray &fl_data, QByteArray &fi_data)
{
	if (!header)	return;

	if (hasMsdFile() && getMsdFile()->isModified()) {
		QByteArray msd;
		if (getMsdFile()->save(msd)) {
			header->setFileData(filePath(ExtMsd), fs_data, msd);
		}
	}
	if (hasJsmFile() && getJsmFile()->isModified()) {
		QByteArray jsm, sym;
		if (getJsmFile()->save(jsm, sym)) {
			header->setFileData(filePath(ExtJsm), fs_data, jsm);
			if (!sym.isEmpty()) {
				header->setFileData(filePath(ExtSym), fs_data, sym);
			}
		}
	}
	if (hasRatFile() && getRatFile()->isModified()) {
		QByteArray rat;
		if (getRatFile()->save(rat)) {
			header->setFileData(filePath(ExtRat), fs_data, rat);
		}
	}
	if (hasMrtFile() && getMrtFile()->isModified()) {
		QByteArray mrt;
		if (getMrtFile()->save(mrt)) {
			header->setFileData(filePath(ExtMrt), fs_data, mrt);
		}
	}
	if (hasInfFile() && getInfFile()->isModified()) {
		QByteArray inf;
		if (getInfFile()->save(inf)) {
			header->setFileData(filePath(ExtInf), fs_data, inf);
		}
	}
	if (hasPmpFile() && getPmpFile()->isModified()) {
		QByteArray pmp;
		if (getPmpFile()->save(pmp)) {
			header->setFileData(filePath(ExtPmp), fs_data, pmp);
		}
	}
	if (hasPmdFile() && getPmdFile()->isModified()) {
		QByteArray pmd;
		if (getPmdFile()->save(pmd)) {
			header->setFileData(filePath(ExtPmd), fs_data, pmd);
		}
	}
	if (hasPvpFile() && getPvpFile()->isModified()) {
		QByteArray pvp;
		if (getPvpFile()->save(pvp)) {
			header->setFileData(filePath(ExtPvp), fs_data, pvp);
		}
	}
	if (hasIdFile() && getIdFile()->isModified()) {
		QByteArray id;
		if (getIdFile()->save(id)) {
			header->setFileData(filePath(ExtId), fs_data, id);
		}
	}
	if (hasCaFile() && getCaFile()->isModified()) {
		QByteArray ca;
		if (getCaFile()->save(ca)) {
			header->setFileData(filePath(ExtCa), fs_data, ca);
		}
	}
	if (hasMskFile() && getMskFile()->isModified()) {
		QByteArray msk;
		if (getMskFile()->save(msk)) {
			header->setFileData(filePath(ExtMsk), fs_data, msk);
		}
	}
	if (hasTdwFile() && getTdwFile()->isModified()) {
		QByteArray tdw;
		if (getTdwFile()->save(tdw)) {
			header->setFileData(filePath(ExtTdw), fs_data, tdw);
		}
	}
	if (hasSfxFile() && getSfxFile()->isModified()) {
		QByteArray sfx;
		if (getSfxFile()->save(sfx)) {
			header->setFileData(filePath(ExtSfx), fs_data, sfx);
		}
	}
	if (hasCharaFile() && getCharaFile()->isModified()) {
		QByteArray charaOne, pcb;
		if (getCharaFile()->save(charaOne, pcb)) {
			header->setFileData(filePath(ExtCharaOne), fs_data, charaOne);
			if (!pcb.isEmpty()) {
				header->setFileData(filePath(ExtPcb), fs_data, pcb);
			}
		}
	}
	if (hasBackgroundFile() && getBackgroundFile()->isModified()) {
		QByteArray map;
		if (getBackgroundFile()->save(map)) {
			header->setFileData(filePath(ExtMap), fs_data, map);
		}
	}
	header->save(fl_data, fi_data);
}

void FieldPC::optimize(QByteArray &fs_data, QByteArray &fl_data, QByteArray &fi_data)
{
	if (!header)	return;

	QList<FileExt> exts = QList<FileExt>() << ExtInf << ExtMrt << ExtMsk << ExtRat
	                      << ExtPcb << ExtPmp << ExtPmd << ExtPvp << ExtSfx
	                      << ExtId << ExtCa << ExtTdw << ExtMap << ExtMim << ExtCharaOne;

	for (FileExt ext: openExts()) {
		header->fileToTheEnd(filePath(ext), fs_data);
	}

	header->save(fl_data, fi_data);
}

void FieldPC::setFile(FileType fileType)
{
	QList<FileExt> exts = typeToExt(fileType);
	for (const FileExt &ext: exts) {
		QString path = filePath(ext);
		if (!path.isEmpty() && !header->fileExists(path)) {
			header->addFile(path, FiCompression::CompressionNone);
			Field::setFile(fileType);
		}
	}
}

bool FieldPC::isMultiLanguage() const
{
	return _lang.compare("x", Qt::CaseInsensitive) == 0;
}

QStringList FieldPC::languages() const
{
	if (!isMultiLanguage()) {
		return QStringList(_lang);
	}

	QStringList files = header->toc();
	QRegularExpression pathReg("_([a-z]+)\\.[a-z]+$", QRegularExpression::CaseInsensitiveOption);
	QStringList langs;

	for (const QString &file: files) {
		QRegularExpressionMatch match = pathReg.match(file);
		if (match.hasMatch()) {
			const QString &lang = match.captured(1);

			if (!langs.contains(lang, Qt::CaseInsensitive)) {
				langs.append(lang.toLower());
			}
		}
	}

	return langs;
}

bool FieldPC::changeGameLang(const QString &gameLang)
{
	if (_gameLang == gameLang) {
		return true;
	}
	
	QList<FileType> fts;

	for (FileType t: fileTypes()) {
		File *f = getFile(t);
		if (f == nullptr) {
			continue;
		}
		
		for (const FileExt &ext: typeToExt(t)) {
			if (filePath(ext).contains(QString("_%1.").arg(_gameLang))) {
				fts.append(t);
				break;
			}
		}
	}
	
	_gameLang = gameLang;
	
	if (!fts.empty()) {
		return openOptimized(fts);
	}
	
	return true;
}

QString FieldPC::fileName(FileExt fileExt, bool useGameLang) const
{
	QString lang;

	if (useGameLang) {
		lang.append("_" % _gameLang);
	}

	switch (fileExt) {
	case ExtMsd:
		return name() + lang + ".msd";
	case ExtJsm:
		return name() + lang + ".jsm";
	case ExtId:
		return name() + lang + ".id";
	case ExtCa:
		return name() + lang + ".ca";
	case ExtRat:
		return name() + lang + ".rat";
	case ExtMrt:
		return name() + lang + ".mrt";
	case ExtInf:
		return name() + lang + ".inf";
	case ExtPcb:
		return name() + lang + ".pcb";
	case ExtPmp:
		return name() + lang + ".pmp";
	case ExtPmd:
		return name() + lang + ".pmd";
	case ExtPvp:
		return name() + lang + ".pvp";
	case ExtMap:
		return name() + lang + ".map";
	case ExtTdw:
		return name() + lang + ".tdw";
	case ExtMsk:
		return name() + lang + ".msk";
	case ExtSfx:
		return name() + lang + ".sfx";
	case ExtCharaOne:
		return "chara" + lang + ".one";
	case ExtMim:
		return name() + lang + ".mim";
	case ExtSym:
		return name() + lang + ".sym";
	}
	return QString();
}

QString FieldPC::filePath(FileExt fileExt) const
{
	QString path = filePath(fileExt, false);

	if (!header->fileExists(path)) {
		path = filePath(fileExt, true);
	}

	return path;
}

QString FieldPC::filePath(FileExt fileExt, bool useGameLang) const
{
	return filePath(fileName(fileExt, useGameLang));
}

QString FieldPC::filePath(const QString &fileName) const
{
	return QString("C:\\ff8\\Data\\%1\\FIELD\\mapdata\\%2\\%3\\%4")
	        .arg(_lang, _subDir, name(), fileName);
}

Field::FileType FieldPC::extToType(FieldPC::FileExt fileExt)
{
	switch (fileExt) {
	case FieldPC::ExtMsd:
	case FieldPC::ExtSym:
		return Field::Msd;
	case FieldPC::ExtJsm:
		return Field::Jsm;
	case FieldPC::ExtId:
		return Field::Id;
	case FieldPC::ExtCa:
		return Field::Ca;
	case FieldPC::ExtRat:
		return Field::Rat;
	case FieldPC::ExtMrt:
		return Field::Mrt;
	case FieldPC::ExtInf:
		return Field::Inf;
	case FieldPC::ExtPmp:
		return Field::Pmp;
	case FieldPC::ExtPmd:
		return Field::Pmd;
	case FieldPC::ExtPvp:
		return Field::Pvp;
	case FieldPC::ExtMap:
	case FieldPC::ExtMim:
		return Field::Background;
	case FieldPC::ExtTdw:
		return Field::Tdw;
	case FieldPC::ExtMsk:
		return Field::Msk;
	case FieldPC::ExtSfx:
		return Field::Sfx;
	case FieldPC::ExtCharaOne:
	case FieldPC::ExtPcb:
		return Field::CharaOne;
	}

	return Field::Msd;
}

QList<FieldPC::FileExt> FieldPC::typeToExt(Field::FileType fileType)
{
	switch (fileType) {
	case Field::Msd:
		return QList<FileExt>() << FieldPC::ExtMsd;
	case Field::Jsm:
		return QList<FileExt>() << FieldPC::ExtJsm << FieldPC::ExtSym;
	case Field::Id:
		return QList<FileExt>() << FieldPC::ExtId;
	case Field::Ca:
		return QList<FileExt>() << FieldPC::ExtCa;
	case Field::Rat:
		return QList<FileExt>() << FieldPC::ExtRat;
	case Field::Mrt:
		return QList<FileExt>() << FieldPC::ExtMrt;
	case Field::Inf:
		return QList<FileExt>() << FieldPC::ExtInf;
	case Field::Pmp:
		return QList<FileExt>() << FieldPC::ExtPmp;
	case Field::Pmd:
		return QList<FileExt>() << FieldPC::ExtPmd;
	case Field::Pvp:
		return QList<FileExt>() << FieldPC::ExtPvp;
	case Field::Background:
		return QList<FileExt>() << FieldPC::ExtMap << FieldPC::ExtMim;
	case Field::Tdw:
		return QList<FileExt>() << FieldPC::ExtTdw;
	case Field::Msk:
		return QList<FileExt>() << FieldPC::ExtMsk;
	case Field::Sfx:
		return QList<FileExt>() << FieldPC::ExtSfx;
	case Field::CharaOne:
		return QList<FileExt>() << FieldPC::ExtCharaOne << FieldPC::ExtPcb;
	case Field::AkaoList:
		break;
	}

	return QList<FileExt>();
}
