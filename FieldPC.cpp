/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2012 Arzel Jérôme <myst6re@gmail.com>
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

FieldPC::FieldPC(const QString &name, const QString &path, FsArchive *archive, const QString &gameLang)
    : Field(name), _path(path), _gameLang(gameLang), header(nullptr)
{
	open(archive);
}

FieldPC::FieldPC(const QString &path, const QString &gameLang)
    : Field(QString()), _path(path), _gameLang(gameLang), header(nullptr)
{
	open(path);
}

FieldPC::~FieldPC()
{
	if(header)		delete header;
}

bool FieldPC::isPc() const
{
	return true;
}

bool FieldPC::hasFiles2() const
{
	return header &&
	        ((header->fileExists(filePath(Mim)) && header->fileExists(filePath(Map)))
			|| header->fileExists(filePath(Tdw))
	        || header->fileExists(filePath(CharaOne)));
}

//void FieldPC::setArchiveHeader(FsArchive *header)
//{
//	this->header = header;
//}

FsArchive *FieldPC::getArchiveHeader() const
{
	return header;
}

const QString &FieldPC::path() const
{
	return _path;
}

bool FieldPC::open(const QString &path)
{
	setOpen(false);

	QString archivePath = path;
	archivePath.chop(1);

	if(header)	delete header;
	header = new FsArchive(archivePath);
	if(!header->isOpen()) {
		qWarning() << "fieldData pas ouvert" << path;
		delete header;
		header = nullptr;
		return false;
	}

	QRegExp pathReg("^" + QRegExp::escape("C:\\ff8\\Data\\") + "(\\w+)" + QRegExp::escape("\\FIELD\\mapdata\\") + "(\\w+)" + QRegExp::escape("\\") + "(\\w+)" + QRegExp::escape("\\"), Qt::CaseInsensitive);
	FsHeader *infInfos = header->getFile("*.inf");

	if(!infInfos || pathReg.indexIn(infInfos->path()) == -1) {
		if(infInfos) {
			qWarning() << "fieldData not opened wrong path" << infInfos->path();
		} else {
			qWarning() << "fieldData not opened" << path;
		}
		return false;
	}

	_lang = pathReg.capturedTexts().at(1);
	_subDir = pathReg.capturedTexts().at(2);
	setName(pathReg.capturedTexts().at(3));

	if(!openOptimized(openExts())) {
		return false;
	}

	setOpen(true);
	return true;
}

bool FieldPC::openOptimized(const QList<FileExt> &selectedExts)
{
	QMap<FileExt, QString> files;

	foreach(FileExt ext, selectedExts) {
		QString path = filePath(ext);

		if(header->fileExists(path)) {
			files[ext] = path;
		}
	}

	QMapIterator<FileExt, QString> it(files);

	while(it.hasNext()) {
		it.next();

		FileExt ext = it.key();
		FileType type = Field::Msd;

		if(ext != CharaOne) {
			bool ok;
			type = extToType(ext, &ok);

			if(!ok) {
				continue;
			}
		}

		const QString &path = it.value();

		if(ext == Jsm && files.contains(Sym)) {
			openJsmFile(header->fileData(path), header->fileData(files[Sym]));
		} else if(ext == Map && files.contains(Mim)) {
			openBackgroundFile(header->fileData(path), header->fileData(files[Mim]));
		} else if(ext == CharaOne) {
			openCharaFile(header->fileData(path));
		} else {
			openFile(type, header->fileData(path));
		}
	}

	return true;
}

bool FieldPC::openOptimized(const QList<FileExt> &selectedExts, FsArchive *archive)
{
	if(!archive) {
		return openOptimized(selectedExts);
	}

	QMap<FileExt, FsHeader *> files;
	quint32 maxSize = 0;

	// Gather max decompression size and header infos
	foreach(FileExt ext, selectedExts) {
		FsHeader *infos = header->getFile(filePath(ext));

		if(infos != nullptr) {
			maxSize = qMax(maxSize, infos->position() + infos->uncompressedSize());
			files[ext] = infos;
		}
	}

	if(maxSize == 0) {
		qWarning() << "No files!" << name();
		return false;
	}

	// Get data and open files
	QByteArray fs_data = archive->fileData("*"%name()%".fs", true, int(maxSize));

	if(fs_data.isEmpty()) {
		qWarning() << "No data!" << name() << maxSize;
		return false;
	}

	QMapIterator<FileExt, FsHeader *> it(files);

	while(it.hasNext()) {
		it.next();

		FileExt ext = it.key();
		FileType type = Field::Msd;

		if(ext != CharaOne) {
			bool ok;
			type = extToType(ext, &ok);

			if(!ok) {
				continue;
			}
		}

		FsHeader *infos = it.value();

		if(ext == Jsm && files.contains(Sym)) {
			openJsmFile(infos->data(fs_data), files[Sym]->data(fs_data));
		} else if(ext == Map && files.contains(Mim)) {
			openBackgroundFile(infos->data(fs_data), files[Mim]->data(fs_data));
		} else if(ext == CharaOne) {
			openCharaFile(infos->data(fs_data));
		} else {
			openFile(type, infos->data(fs_data));
		}
	}

	return true;
}

bool FieldPC::open(FsArchive *archive)
{
	setOpen(false);

	if(header)	delete header;

	QRegExp pathReg("^" + QRegExp::escape("C:\\ff8\\Data\\") + "(\\w+)" + QRegExp::escape("\\FIELD\\mapdata\\") + "(\\w+)" + QRegExp::escape("\\"), Qt::CaseInsensitive);
	FsHeader *flInfos = archive->getFile("*"%name()%".fl");
	if(!flInfos || pathReg.indexIn(flInfos->path()) == -1) {
		if(flInfos) {
			qWarning() << "fieldData not opened" << name() << "wrong path" << flInfos->path();
		} else {
			qWarning() << "fieldData not opened" << name() << archive->path();
		}
		return false;
	}
	_lang = pathReg.capturedTexts().at(1);
	_subDir = pathReg.capturedTexts().at(2);

	header = new FsArchive(archive->fileData(flInfos->path()), archive->fileData("*"%name()%".fi"));
	if(!header->isOpen()) {
		qWarning() << "fieldData not opened" << name();
		delete header;
		header = nullptr;
		return false;
	}

	if(!openOptimized(openExts(), archive)) {
		delete header;
		header = nullptr;
		return false;
	}

	setOpen(true);
	return true;
}

bool FieldPC::open2(FsArchive *archive)
{
	return header && openOptimized(open2Exts(), archive);
}

bool FieldPC::save(const QString &path)
{
	QString archivePath = path;
	archivePath.chop(1);

	QFile fs(FsArchive::fsPath(archivePath));
	if(fs.open(QIODevice::ReadWrite)) {
		QFile fl(FsArchive::flPath(archivePath));
		if(fl.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
			QFile fi(FsArchive::fiPath(archivePath));
			if(fi.open(QIODevice::WriteOnly | QIODevice::Truncate)) {

				QByteArray fs_data=fs.readAll(), fl_data, fi_data;

				/* fs_data must be filled by the file contents
				 * fl_data and fi_data are rebuilt
				 */
				save(fs_data, fl_data, fi_data);

				fs.resize(0);
				fs.reset();
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
	if(!header)	return;

	if(hasMsdFile() && getMsdFile()->isModified()) {
		QByteArray msd;
		if(getMsdFile()->save(msd)) {
			header->setFileData(filePath(Msd), fs_data, msd);
		}
	}
	if(hasJsmFile() && getJsmFile()->isModified()) {
		QByteArray jsm, sym;
		if(getJsmFile()->save(jsm, sym)) {
			header->setFileData(filePath(Jsm), fs_data, jsm);
			if(!sym.isEmpty()) {
				header->setFileData(filePath(Sym), fs_data, sym);
			}
		}
	}
	if(hasRatFile() && getRatFile()->isModified()) {
		QByteArray rat;
		if(getRatFile()->save(rat)) {
			header->setFileData(filePath(Rat), fs_data, rat);
		}
	}
	if(hasMrtFile() && getMrtFile()->isModified()) {
		QByteArray mrt;
		if(getMrtFile()->save(mrt)) {
			header->setFileData(filePath(Mrt), fs_data, mrt);
		}
	}
	if(hasInfFile() && getInfFile()->isModified()) {
		QByteArray inf;
		if(getInfFile()->save(inf)) {
			header->setFileData(filePath(Inf), fs_data, inf);
		}
	}
	if(hasPmpFile() && getPmpFile()->isModified()) {
		QByteArray pmp;
		if(getPmpFile()->save(pmp)) {
			header->setFileData(filePath(Pmp), fs_data, pmp);
		}
	}
	if(hasPmdFile() && getPmdFile()->isModified()) {
		QByteArray pmd;
		if(getPmdFile()->save(pmd)) {
			header->setFileData(filePath(Pmd), fs_data, pmd);
		}
	}
	if(hasPvpFile() && getPvpFile()->isModified()) {
		QByteArray pvp;
		if(getPvpFile()->save(pvp)) {
			header->setFileData(filePath(Pvp), fs_data, pvp);
		}
	}
	if(hasIdFile() && getIdFile()->isModified()) {
		QByteArray id;
		if(getIdFile()->save(id)) {
			header->setFileData(filePath(Id), fs_data, id);
		}
	}
	if(hasCaFile() && getCaFile()->isModified()) {
		QByteArray ca;
		if(getCaFile()->save(ca)) {
			header->setFileData(filePath(Ca), fs_data, ca);
		}
	}
	if(hasMskFile() && getMskFile()->isModified()) {
		QByteArray msk;
		if(getMskFile()->save(msk)) {
			header->setFileData(filePath(Msk), fs_data, msk);
		}
	}
	if(hasTdwFile() && getTdwFile()->isModified()) {
		QByteArray tdw;
		if(getTdwFile()->save(tdw)) {
			header->setFileData(filePath(Tdw), fs_data, tdw);
		}
	}
	if(hasSfxFile() && getSfxFile()->isModified()) {
		QByteArray sfx;
		if(getSfxFile()->save(sfx)) {
			header->setFileData(filePath(Sfx), fs_data, sfx);
		}
	}
	if(hasBackgroundFile() && getBackgroundFile()->isModified()) {
		QByteArray map;
		if(getBackgroundFile()->save(map)) {
			header->setFileData(filePath(Map), fs_data, map);
		}
	}
	header->save(fl_data, fi_data);
}

void FieldPC::optimize(QByteArray &fs_data, QByteArray &fl_data, QByteArray &fi_data)
{
	if(!header)	return;

	foreach(FileExt ext, open2Exts()) {
		header->fileToTheEnd(filePath(ext), fs_data);
	}

	header->save(fl_data, fi_data);
}

void FieldPC::setFile(FileType fileType)
{
	QString path = filePath(typeToExt(fileType, nullptr));
	if(!path.isEmpty() && !header->fileExists(path)) {
		header->addFile(path, FiCompression::CompressionNone);
		Field::setFile(fileType);
	}
}

bool FieldPC::isMultiLanguage() const
{
	return _lang.compare("x", Qt::CaseInsensitive) == 0;
}

QStringList FieldPC::languages() const
{
	QStringList files = header->toc();
	QRegExp pathReg("_([a-z]+)\\.[a-z]+$", Qt::CaseInsensitive);
	QStringList langs;

	foreach(const QString &file, files) {
		if (pathReg.indexIn(file) != -1) {
			const QString &lang = pathReg.capturedTexts().at(1);

			if(!langs.contains(lang, Qt::CaseInsensitive)) {
				langs.append(lang.toLower());
			}
		}
	}

	return langs;
}

QString FieldPC::fileName(FileExt fileExt, bool useGameLang) const
{
	QString lang;

	if(useGameLang) {
		lang.append("_" % _gameLang);
	}

	switch(fileExt) {
	case Msd:
		return name() + lang + ".msd";
	case Jsm:
		return name() + lang + ".jsm";
	case Id:
		return name() + lang + ".id";
	case Ca:
		return name() + lang + ".ca";
	case Rat:
		return name() + lang + ".rat";
	case Mrt:
		return name() + lang + ".mrt";
	case Inf:
		return name() + lang + ".inf";
	case Pmp:
		return name() + lang + ".pmp";
	case Pmd:
		return name() + lang + ".pmd";
	case Pvp:
		return name() + lang + ".pvp";
	case Map:
		return name() + lang + ".map";
	case Tdw:
		return name() + lang + ".tdw";
	case Msk:
		return name() + lang + ".msk";
	case Sfx:
		return name() + lang + ".sfx";
	case CharaOne:
		return "chara" + lang + ".one";
	case Mim:
		return name() + lang + ".mim";
	case Sym:
		return name() + lang + ".sym";
	}
	return QString();
}

QString FieldPC::filePath(FileExt fileExt) const
{
	QString path = filePath(fileExt, false);

	if(!header->fileExists(path)) {
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

Field::FileType FieldPC::extToType(FieldPC::FileExt fileExt, bool *ok)
{
	if (ok) {
		*ok = true;
	}

	switch(fileExt) {
	case FieldPC::Msd:
		return Field::Msd;
	case FieldPC::Jsm:
		return Field::Jsm;
	case FieldPC::Id:
		return Field::Id;
	case FieldPC::Ca:
		return Field::Ca;
	case FieldPC::Rat:
		return Field::Rat;
	case FieldPC::Mrt:
		return Field::Mrt;
	case FieldPC::Inf:
		return Field::Inf;
	case FieldPC::Pmp:
		return Field::Pmp;
	case FieldPC::Pmd:
		return Field::Pmd;
	case FieldPC::Pvp:
		return Field::Pvp;
	case FieldPC::Map:
		return Field::Background;
	case FieldPC::Tdw:
		return Field::Tdw;
	case FieldPC::Msk:
		return Field::Msk;
	case FieldPC::Sfx:
		return Field::Sfx;
	case FieldPC::CharaOne:
	case FieldPC::Mim:
	case FieldPC::Sym:
		break;
	}

	if (ok) {
		*ok = false;
	}

	return Field::Msd;
}

FieldPC::FileExt FieldPC::typeToExt(Field::FileType fileType, bool *ok)
{
	if (ok) {
		*ok = true;
	}

	switch(fileType) {
	case Field::Msd:
		return FieldPC::Msd;
	case Field::Jsm:
		return FieldPC::Jsm;
	case Field::Id:
		return FieldPC::Id;
	case Field::Ca:
		return FieldPC::Ca;
	case Field::Rat:
		return FieldPC::Rat;
	case Field::Mrt:
		return FieldPC::Mrt;
	case Field::Inf:
		return FieldPC::Inf;
	case Field::Pmp:
		return FieldPC::Pmp;
	case Field::Pmd:
		return FieldPC::Pmd;
	case Field::Pvp:
		return FieldPC::Pvp;
	case Field::Background:
		return FieldPC::Map;
	case Field::Tdw:
		return FieldPC::Tdw;
	case Field::Msk:
		return FieldPC::Msk;
	case Field::Sfx:
		return FieldPC::Sfx;
	case Field::AkaoList:
		break;
	}

	if (ok) {
		*ok = false;
	}

	return FieldPC::Msd;
}
