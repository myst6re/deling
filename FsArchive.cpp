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
#include "FsArchive.h"

FsHeader::FsHeader()
	: _uncompressed_size(0), _position(-1), _isCompressed(false)
{
}

FsHeader::FsHeader(const QString &path, quint32 uncompressed_size, quint32 position, bool isCompressed)
	: _path(path), _uncompressed_size(uncompressed_size), _position(position), _isCompressed(isCompressed)
{
}

const QString &FsHeader::path() const
{
	return _path;
}

void FsHeader::setPath(const QString &path)
{
	_path = path;
}

QString FsHeader::dirName() const
{
	int index;
	if((index=_path.lastIndexOf('\\'))!=-1)
		return QDir::cleanPath(_path.left(index));
	return QDir::cleanPath(_path);
}

QString FsHeader::fileName() const
{
	int index;
	if((index=_path.lastIndexOf('\\'))!=-1)
		return _path.mid(index+1);
	return _path;
}

quint32 FsHeader::uncompressed_size() const
{
	return _uncompressed_size;
}

void FsHeader::setUncompressed_size(quint32 uncompressed_size)
{
	_uncompressed_size = uncompressed_size;
}

quint32 FsHeader::position() const
{
	return _position;
}

void FsHeader::setPosition(quint32 position)
{
	_position = position;
}

bool FsHeader::isCompressed() const
{
	return _isCompressed;
}

void FsHeader::setIsCompressed(bool isCompressed)
{
	_isCompressed = isCompressed;
}

bool FsHeader::compressedSize(QFile *fs, quint32 *lzsSize) const
{
	return _isCompressed && fs->seek(_position) &&
	        fs->read((char *)lzsSize, sizeof(quint32)) == sizeof(quint32);
}

bool FsHeader::compressedSize(const char *fs_data, int size, quint32 *lzsSize) const
{
	if(_isCompressed) {
		if(size <= sizeof(quint32))	return false;

		memcpy(&lzsSize, fs_data + _position, sizeof(quint32));

		return true;
	}
	return false;
}

bool FsHeader::compressedSize(const QByteArray &fs_data, quint32 *lzsSize) const
{
	return compressedSize(fs_data.constData(), fs_data.size(), lzsSize);
}

bool FsHeader::physicalSize(QFile *fs, quint32 *size) const
{
	if(_isCompressed) {
		if(!compressedSize(fs, size)) {
			return false;
		}
		*size += sizeof(quint32);
		return true;
	}
	*size = _uncompressed_size;
	return true;
}

bool FsHeader::physicalSize(const QByteArray &fs_data, quint32 *size) const
{
	if(_isCompressed) {
		if(!compressedSize(fs_data, size)) {
			return false;
		}
		*size += sizeof(quint32);
		return true;
	}
	return _uncompressed_size;
}

QByteArray FsHeader::data(const QByteArray &fs_data, bool uncompress, int maxUncompress) const
{
	if(_isCompressed)
	{
		quint32 size;

		if(!compressedSize(fs_data, &size))
			return QByteArray();

		// fucking size
		if(size > _uncompressed_size*2)
			return QByteArray();

		if(!uncompress)
			return fs_data.mid(_position, size+4);

		return LZS::decompress(fs_data.constData() + _position + 4, size, maxUncompress<=0 ? _uncompressed_size : maxUncompress);
	}

	return fs_data.mid(_position, _uncompressed_size);
}

QByteArray FsHeader::data(QFile *fs, bool uncompress, int maxUncompress) const
{
	if(!fs->seek(_position)) 	return QByteArray();

	if(_isCompressed)
	{
		quint32 size;

		if(!compressedSize(fs, &size))
			return QByteArray();

		// fucking size
		if(size > _uncompressed_size*2)
			return QByteArray();

		if(!uncompress) {
			fs->seek(_position);
			return fs->read(size+4);
		}
		char *buff = new char[size];

		if(fs->read(buff, size) != size)
			return QByteArray();

		const QByteArray &decData = LZS::decompress(buff, size, maxUncompress<=0 ? _uncompressed_size : maxUncompress);
		delete buff;

		return decData;
	}
	return fs->read(_uncompressed_size);
}

int FsHeader::setData(QByteArray &fs_data, const QByteArray &new_data)
{
	int diff, real_size;

	if(_isCompressed)
	{
		const char *fs_data_const = fs_data.constData();
		int size=0;

		memcpy(&size, &fs_data_const[_position], 4);

		_uncompressed_size = new_data.size();
		QByteArray new_data_lzsed = LZS::compress(new_data);
		real_size = new_data_lzsed.size();

		diff = real_size - size;

		fs_data.replace(_position, 4, (char *)&real_size);
		fs_data.replace(_position+4, size, new_data_lzsed);
	}
	else {
		fs_data.replace(_position, _uncompressed_size, new_data);
		real_size = new_data.size();
		diff = real_size - _uncompressed_size;
		_uncompressed_size = real_size;
	}

	return diff;
}

int FsHeader::setData(QFile *fs, QByteArray &new_data)
{
	int diff, real_size;

	if(_isCompressed)
	{
		if(!fs->seek(_position)) return 0;

		int size=0;

		fs->read((char *)&size, 4);

		_uncompressed_size = new_data.size();
		new_data = LZS::compress(new_data);
		real_size = new_data.size();
		new_data.prepend((char *)&real_size, 4);

		diff = real_size - size;
	}
	else {
		real_size = new_data.size();
		diff = real_size - _uncompressed_size;
		_uncompressed_size = real_size;
	}

	return diff;
}

//FsArchive::FsArchive()
//	: fromFile(false), _isOpen(false)
//{
//}

FsArchive::FsArchive(const QByteArray &fl_data, const QByteArray &fi_data)
	: fromFile(false), _isOpen(false)
{
	load(fl_data, fi_data);
}

FsArchive::FsArchive(const QString &path)
	: fromFile(true), _isOpen(false)
{
	load(path);
}

FsArchive::~FsArchive()
{
	if(fromFile) {
		fs.close();
		fl.close();
		fi.close();
		qDebug() << "close fs" << fs.fileName();
	}
    foreach(FsHeader *header, toc_access) 	delete header;
}

void FsArchive::addFile(const QString &path, quint32 uncompressed_size, quint32 position, bool isCompressed)
{
    if(toc_access.contains(path.toLower())) {
        qWarning() << "addFile error: file already exists" << path << uncompressed_size << position << isCompressed;
        return;
    }
    FsHeader *header = new FsHeader(path, uncompressed_size, position, isCompressed);
    sortedByPosition.insert(position, header);
    toc_access.insert(path.toLower(), header);
}

void FsArchive::addFile(const QString &path, bool isCompressed)
{
	if(toc_access.contains(path.toLower())) {
		qWarning() << "addFile error: file already exists" << path << isCompressed;
		return;
	}
	FsHeader *header = new FsHeader(path, 0, 0, isCompressed);
	sortedByPosition.insert(0, header);
	toc_access.insert(path.toLower(), header);
}

bool FsArchive::removeFile(QString path)
{
	path = path.toLower();
    FsHeader *header = toc_access.take(path);
    if(header != 0) {
        bool ok = sortedByPosition.remove(header->position(), header)==1;
        if(!ok) {
            qWarning() << "FsArchive::removeFile impossible" << path;
        }
        delete header;
        return ok;
    }
	return false;
}

FsHeader *FsArchive::getFile(const QString &path) const
{
	//	qDebug() << "getFile" << path;

	if(path.startsWith('*'))
	{
		QString path2 = path.mid(1);
        QMapIterator<QString, FsHeader *> i(toc_access);
		while(i.hasNext())
		{
			i.next();
			if(i.key().endsWith(path2, Qt::CaseInsensitive))
			{
                return i.value();
			}
		}
	}
	else {
        return toc_access.value(path.toLower(), NULL);
	}

	//	qDebug() << path << "non trouvé";

	return NULL;
}

bool FsArchive::fileExists(const QString &path) const
{
	return getFile(path)!=NULL;
}

bool FsArchive::dirExists(QString dir) const
{
	dir = cleanPath(dir);

	if(dir.isEmpty())
		return true;

    foreach(FsHeader *info, toc_access) {
		if(info->path().startsWith(dir, Qt::CaseInsensitive)) {
			return true;
		}
	}

	return false;
}

QString FsArchive::filePath(const QString &path) const
{
	FsHeader *header = getFile(path);
	return header!=NULL ? header->path() : QString();
}

bool FsArchive::setFilePath(QString path, const QString &newPath)
{
	path = path.toLower();

	if(toc_access.contains(path) && !toc_access.contains(newPath.toLower()))
	{
        FsHeader *header = toc_access.value(path);
        header->setPath(newPath);
		toc_access.remove(path);
        toc_access.insert(newPath.toLower(), header);

        return true;
	}

    return false;
}

quint32 FsArchive::uncompressedFileSize(const QString &path) const
{
	FsHeader *header = getFile(path);
	return header!=NULL ? header->uncompressed_size() : -1;
}

quint32 FsArchive::filePosition(const QString &path) const
{
	FsHeader *header = getFile(path);
	return header!=NULL ? header->position() : -1;
}

void FsArchive::setFilePosition(const QString &path, quint32 position)
{
	FsHeader *header = getFile(path);
	if(header!=NULL) {
		header->setPosition(position);
	}
}

bool FsArchive::fileIsCompressed(const QString &path) const
{
	FsHeader *header = getFile(path);
	return header!=NULL ? header->isCompressed() : false;
}

QByteArray FsArchive::fileData(const QString &path, const QByteArray &fs_data, bool uncompress, int uncompressed_size)
{
	FsHeader *header = getFile(path);
	//	qDebug() << "fileData1" << path << fs_data.size() << uncompressed_size;
	return header!=NULL ? header->data(fs_data, uncompress, uncompressed_size) : QByteArray();
}

QByteArray FsArchive::fileData(const QString &path, bool uncompress, int uncompressed_size)
{
	FsHeader *header = getFile(path);
	//	qDebug() << "fileData2" << path << uncompressed_size << fromFile << _isOpen;
	return header!=NULL && fromFile && _isOpen ? header->data(&fs, uncompress, uncompressed_size) : QByteArray();
}

void FsArchive::setFileData(const QString &path, QByteArray &fs_data, const QByteArray &new_data)
{
	FsHeader *header = getFile(path);
	if(header!=NULL) {
		changePositions(header, header->setData(fs_data, new_data));
	}
}

void FsArchive::setFileData(const QString &path, QByteArray &new_data)
{
	FsHeader *header = getFile(path);
	if(header!=NULL) {
		header->setData(&fs, new_data);
	}
}

void FsArchive::changePositions(FsHeader *start, int diff)
{
	if(diff==0)	return;

	FsHeader *header;

	// décalage des positions des fichiers qui suivent
    QMap<quint32, FsHeader *>::iterator i = sortedByPosition.find(sortedByPosition.key(start), start);
    if(i != sortedByPosition.end())
	{
        ++i;
        while(i != sortedByPosition.end())
		{
			header = i.value();
            header->setPosition(header->position() + diff);
            // toc_access.insert(header->path().toLower(), newPos);
            ++i;
        }
    } else {
        qWarning() << "FsArchive::changePositions already at the end";
    }
}

QMap<QString, FsHeader> FsArchive::getHeader() const
{
	QMap<QString, FsHeader> header;
    QMapIterator<QString, FsHeader *> i(toc_access);
	while(i.hasNext()) {
		i.next();
        header.insert(i.key(), *(i.value()));
    }
	return header;
}

void FsArchive::setHeader(const QMap<QString, FsHeader> &header)
{
	QMapIterator<QString, FsHeader> i(header);
	while(i.hasNext()) {
		i.next();
        FsHeader h = i.value(), *h2 = toc_access.value(i.key());
        if(h2) {
            h2->setPosition(h.position());
            h2->setUncompressed_size(h.uncompressed_size());
        } else {
            qWarning() << "FsArchive::setHeader error:" << i.key() << "doesn't exists!";
        }
	}
	rebuildInfos();
}

QStringList FsArchive::toc() const
{
	QStringList toc;

	// Files sorted by positions
    foreach(FsHeader *header, sortedByPosition) {
		toc.append(header->path());
	}

	return toc;
}

QStringList FsArchive::dirs() const
{
	QStringList dirs;
	QString dir;
	int lastIndex;

	// Files sorted by positions
    foreach(FsHeader *header, sortedByPosition) {
		dir = header->path();
		cleanPath(dir);
		lastIndex = dir.lastIndexOf('\\', -2);
		if(lastIndex != -1) {
			dir.truncate(lastIndex);
		} else {
			dir = "";
		}
		if(!dirs.contains(dir))
			dirs.append(dir);
	}

	return dirs;
}

bool FsArchive::extractFile(const QString &fileName, const QByteArray &fs_data, const QString &filePath, bool uncompress)
{
	if(!_isOpen)	return false;

	QFile fic(filePath);
	fic.open(QIODevice::WriteOnly);
	fic.write(fileData(fileName, fs_data, uncompress));
	fic.close();

	return true;
}

bool FsArchive::extractFile(const QString &fileName, const QString &filePath, bool uncompress)
{
	if(!fromFile || !_isOpen)	return false;

	QFile fic(filePath);
	fic.open(QIODevice::WriteOnly);
	fic.write(fileData(fileName, uncompress));
	fic.close();

	return true;
}

FsArchive::Error FsArchive::extractFiles(const QStringList &fileNames, const QString &baseFileName, const QString &fileDir, ArchiveObserver *progress, bool uncompress)
{
	if(!fromFile || !_isOpen)	return SourceCantBeOpened;

	// qDebug() << "extractFiles" << fileNames << fileDir << uncompress;

	progress->setObserverMaximum(fileNames.size());

	QDir dir(fileDir);
	int i=0, sizeOfBaseFileName = baseFileName.size();

	foreach(QString fileName, fileNames) {
		QCoreApplication::processEvents();

		if(progress->observerWasCanceled()) {
			return Canceled;
		}

		fileName = cleanPath(fileName);
		fileName.chop(1);
		int index = fileName.lastIndexOf('\\');
		if(index - sizeOfBaseFileName > 0) {
			dir.mkpath(QDir::cleanPath(fileName.mid(sizeOfBaseFileName, index-sizeOfBaseFileName)));
		}

        QFile fic(QDir::cleanPath(fileDir % QDir::separator() % fileName.mid(sizeOfBaseFileName).replace('\\', '/')));
		if(!fic.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
			return TargetCantBeOpened;
		}
		fic.write(fileData(fileName, uncompress));
		fic.close();

		progress->setObserverValue(i++);
	}

	return Ok;
}

FsArchive::Error FsArchive::replaceFile(const QString &source, const QString &destination, ArchiveObserver *progress)
{
	QTime t;t.start();

	QStringList toc;
	int pos, i=0;
	QString save_path = path(), temp_path;
	QByteArray sourceData;

	if(!isWritable())	return NonWritable;

	save_path.chop(1);// remove s, i or l in extension
	temp_path = save_path.left(save_path.lastIndexOf("/")+1) + "delingtemp.f";

	QFile sourceFile(source);
	if(!sourceFile.open(QIODevice::ReadOnly))
		return SourceCantBeOpened;
	sourceData = sourceFile.readAll();
	sourceFile.close();

    QFile temp(FsArchive::fsPath(temp_path));
	if(!temp.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return TempCantBeOpened;

	toc = this->toc();

	progress->setObserverMaximum(toc.size());

	foreach(const QString &entry, toc) {
		QCoreApplication::processEvents();
		if(progress->observerWasCanceled()) {
			temp.remove();
			return Canceled;
		}

		pos = temp.pos();

		if(entry == destination) {
			setFileData(entry, sourceData);
			temp.write(sourceData);
		}
		else {
			temp.write(fileData(entry, false));
		}

		setFilePosition(entry, pos);
		progress->setObserverValue(i++);
	}

	if(!saveAs(temp_path)) {
        QFile::remove(FsArchive::fiPath(temp_path));
        QFile::remove(FsArchive::flPath(temp_path));
		temp.remove();
		return SaveHeaderError;
	}

	int replaceError = replaceArchive(&temp);
	if(replaceError==1) {
        QFile::remove(FsArchive::fiPath(temp_path));
        QFile::remove(FsArchive::flPath(temp_path));
		temp.remove();
		return ReplaceArchiveError;
	} else if(replaceError!=0) {
		return ReplaceArchiveError;
	}

	rebuildInfos();

	qDebug() << "save time" << t.elapsed();

	return Ok;
}

QList<FsArchive::Error> FsArchive::replaceDir(const QString &source, const QString &destination, bool compress, ArchiveObserver *progress)
{
	// TODO progress
	Error err = remove(tocInDirectory(destination), progress);
	if (err != Ok) {
		return QList<FsArchive::Error>() << err;
	}

	// TODO progress
	return appendDir(source, destination, compress, progress);
}

QStringList FsArchive::listDirsRec(QDir *sourceDir)
{
	QStringList paths;

	foreach (const QFileInfo &fileInfo, sourceDir->entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Readable)) {
		if (fileInfo.isDir()) {
			sourceDir->cd(fileInfo.fileName());
			foreach (const QString &subPath, listDirsRec(sourceDir)) {
				paths << fileInfo.fileName() + "/" + subPath;
			}
			sourceDir->cdUp();
		} else {
			paths << fileInfo.fileName();
		}
	}

	return paths;
}

QList<FsArchive::Error> FsArchive::appendFiles(const QStringList &sources, const QStringList &destinations, bool compress, ArchiveObserver *progress)
{
	QTime t;t.start();

	QByteArray data, fl_data, fi_data;
	int i, nbFiles = sources.size();
	QList<Error> errors;

	if(!isWritable()) {
		errors.append(NonWritable);
		return errors;
	}

	fs.seek(fs.size());

	progress->setObserverMaximum(nbFiles);

	for(i=0 ; i<nbFiles ; ++i) {
		QCoreApplication::processEvents();
		if(progress->observerWasCanceled())				break;// Stop, don't canceling

		if(fileExists(destinations.at(i))) {
//			qDebug() << destinations.at(i);
			errors.append(FileExists);
			continue;
		}

//		qDebug() << sources.at(i);

		QFile file(sources.at(i));
		if(!file.open(QIODevice::ReadOnly)) {
			errors.append(SourceCantBeOpened);
			continue;
		}

		data = file.readAll();
		file.close();
		if(data.isEmpty()) {
			errors.append(EmptyFile);
			continue;
		}
		addFile(destinations.at(i), (quint32)data.size(), (quint32)fs.pos(), compress);

		if(compress) {
			data = LZS::compress(data);
			int size = data.size();
			data.prepend((char *)&size, 4);
		}

		fs.write(data);
		progress->setObserverValue(i);
		errors.append(Ok);
	}

	save(fl_data, fi_data);

	fi.resize(0);
	fi.write(fi_data);
	fi.flush();

	fl.resize(0);
	fl.write(fl_data);
	fl.flush();

	qDebug() << "save time" << t.elapsed();

	return errors;
}

QList<FsArchive::Error> FsArchive::appendDir(const QString &source, const QString &destination, bool compress, ArchiveObserver *progress)
{
	QStringList sources, destinations;
	QDir sourceDir(source);

	foreach (const QString &relativePath, listDirsRec(&sourceDir)) {
		sources << sourceDir.absoluteFilePath(relativePath);
		destinations << destination + "\\" + relativePath;
	}

	return appendFiles(sources, destinations, compress, progress);
}

FsArchive::Error FsArchive::remove(QStringList destinations, ArchiveObserver *progress)
{
	QTime t;t.start();

	QStringList toc = this->toc();
//	QByteArray fl_data, fi_data;
	int pos, i=0;
	QString save_path = path(), temp_path;

	if(!isWritable())	return NonWritable;

	save_path.chop(1);// remove s, i or l in extension
	temp_path = save_path.left(save_path.lastIndexOf("/")+1) + "delingtemp.f";

    QFile temp(FsArchive::fsPath(temp_path));
	if(!temp.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return TempCantBeOpened;

	progress->setObserverMaximum(toc.size());

	foreach(const QString &entry, toc) {
		QCoreApplication::processEvents();
		if(progress->observerWasCanceled()) {
			temp.remove();
			return Canceled;
		}

		pos = temp.pos();

		if(destinations.contains(entry, Qt::CaseInsensitive)) {
            removeFile(entry);
			destinations.removeOne(entry);
		}
		else {
			temp.write(fileData(entry, false));
		}

		setFilePosition(entry, pos);
		progress->setObserverValue(i++);
	}

	rebuildInfos();

	if(!saveAs(temp_path)) {
        QFile::remove(FsArchive::fiPath(temp_path));
        QFile::remove(FsArchive::flPath(temp_path));
		temp.remove();
		return SaveHeaderError;
	}

	int replaceError = replaceArchive(&temp);
	if(replaceError==1) {
        QFile::remove(FsArchive::fiPath(temp_path));
        QFile::remove(FsArchive::flPath(temp_path));
		temp.remove();
		return ReplaceArchiveError;
	} else if(replaceError!=0) {
		return ReplaceArchiveError;
	}

	qDebug() << "save time" << t.elapsed();

	return Ok;
}

FsArchive::Error FsArchive::rename(const QStringList &destinations, const QStringList &newDestinations)
{
	QTime t;t.start();

	QByteArray fl_data, fi_data;
	QString destination, newDestination;
	int size, i;

	if(!isWritable())						return NonWritable;

	size = destinations.size();
	for(i=0 ; i<size ; ++i) {
		destination = destinations.at(i);
		newDestination = newDestinations.at(i);

		if(!fileExists(destination))			return FileDontExists;
		if(fileExists(newDestination))			return FileExists;

		setFilePath(destination, newDestination);
	}

	save(fl_data, fi_data);

	fi.resize(0);
	fi.write(fi_data);
	fi.flush();

	fl.resize(0);
	fl.write(fl_data);
	fl.flush();

	qDebug() << "save time" << t.elapsed();

	return Ok;
}

bool FsArchive::load(const QString &fl_data, const QByteArray &fi_data)
{
	if(_isOpen || fl_data.isEmpty() || fi_data.isEmpty()) {
		qWarning() << "Error loading data" << fl_data.isEmpty() << fi_data.isEmpty();
		return false;
	}

	QStringList fl = fl_data.split("\r\n", QString::SkipEmptyParts);
	Fi_infos fi_infos;

	const char *fi_constData = fi_data.constData();

	if(fi_data.size()/12.0 != (float)fl.size()) {
		qWarning() << "Invalid fl or fi" << (fi_data.size()/12.0) << (fl.size());
		return false;
	}

	foreach(const QString &line, fl) {
		memcpy(&fi_infos, fi_constData, 12);

		addFile(line, fi_infos.size, fi_infos.pos, (fi_infos.isCompressed & 1));
		fi_constData += 12;
	}

	_isOpen = true;

	/* verify();
	if(repair()) {
		verify();
	} else {
		qDebug() << "cannot repair";
	} */

	return true;
}

bool FsArchive::load(const QString &path)
{
	if(_isOpen)	return false;

    fl.setFileName(FsArchive::flPath(path));
    fi.setFileName(FsArchive::fiPath(path));
    fs.setFileName(FsArchive::fsPath(path));
	if(!fl.open(QIODevice::ReadWrite)) {
		if(!fl.open(QIODevice::ReadOnly))
			return false;
	}
	if(!fi.open(QIODevice::ReadWrite)) {
		if(!fi.open(QIODevice::ReadOnly))
			return false;
	}
	if(!fs.open(QIODevice::ReadWrite)) {
		if(!fs.open(QIODevice::ReadOnly))
			return false;
	}

	qDebug() << "open fs" << path;

	fromFile = true;

	return (_isOpen = load(QString(fl.readAll()), fi.readAll()));
}

void FsArchive::save(QByteArray &fl_data, QByteArray &fi_data) const
{
	int size, pos, isCompressed;

    foreach(FsHeader *header, sortedByPosition) {
		fl_data.append(header->path().toLatin1());
		fl_data.append("\r\n", 2);
		size = header->uncompressed_size();
		pos = header->position();
		isCompressed = (int)header->isCompressed();
		fi_data.append((char *)&size, 4);
		fi_data.append((char *)&pos, 4);
		fi_data.append((char *)&isCompressed, 4);
	}
}

bool FsArchive::saveAs(const QString &path) const
{
    QFile fl(FsArchive::flPath(path)), fi(FsArchive::fiPath(path));
	if(!fl.open(QIODevice::WriteOnly | QIODevice::Truncate)
			|| !fi.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return false;

	QByteArray fl_data, fi_data;

	save(fl_data, fi_data);

	fl.write(fl_data);
	fl.close();

	fi.write(fi_data);
	fi.close();

	return true;
}

int FsArchive::replaceArchive(QFile *newFile)
{
	if(!fromFile)	return 1;

	_isOpen = false;

	fs.close();

	if(!fs.remove()) {
        qWarning() << "Impossible to remove the fs" << fs.errorString();
		fs.open(QIODevice::ReadWrite);		
		return 1;
	}

	fl.close();
	fi.close();

	qDebug() << "reclose fs" << fs.fileName();

	if(!fl.remove() || !fi.remove())	return 2;

	QString fileName = newFile->fileName();
	fileName.chop(1);

    if(!newFile->rename(fs.fileName()) || !QFile::rename(FsArchive::flPath(fileName), fl.fileName()) || !QFile::rename(FsArchive::fiPath(fileName), fi.fileName()))
		return 2;

	if(!fs.open(QIODevice::ReadWrite) || !fl.open(QIODevice::ReadWrite) || !fi.open(QIODevice::ReadWrite))
		return 2;

	qDebug() << "reopen fs" << fs.fileName();

	_isOpen = true;

	return 0;
}

QString FsArchive::fsPath(const QString &path)
{
    if(path.endsWith('F')) {
        return path % 'S';
    } else {
        return path % 's';
    }
}

QString FsArchive::flPath(const QString &path)
{
    if(path.endsWith('F')) {
        return path % 'L';
    } else {
        return path % 'l';
    }
}

QString FsArchive::fiPath(const QString &path)
{
    if(path.endsWith('F')) {
        return path % 'I';
    } else {
        return path % 'i';
    }
}

QString FsArchive::path() const
{
	return fs.fileName();
}

bool FsArchive::setPath(const QString &path)
{
	fl.close();
	fi.close();
	fs.close();
	qDebug() << "reclose fs" << fs.fileName();
    fl.setFileName(FsArchive::flPath(path));
    fi.setFileName(FsArchive::fiPath(path));
    fs.setFileName(FsArchive::fsPath(path));
	qDebug() << "reopen fs in new path" << path;
	return fl.open(QIODevice::ReadWrite)
			&& fi.open(QIODevice::ReadWrite)
			&& fs.open(QIODevice::ReadWrite);
}

int FsArchive::size() const
{
	return fs.size();
}

bool FsArchive::isOpen() const
{
	return _isOpen;
}

bool FsArchive::isWritable() const
{
	return fromFile && _isOpen && fs.isWritable() && fl.isWritable() && fi.isWritable()
			&& fs.permissions().testFlag(QFile::WriteUser) && fl.permissions().testFlag(QFile::WriteUser) && fi.permissions().testFlag(QFile::WriteUser);
}

void FsArchive::fileToTheEnd(const QString &path, QByteArray &fs_data)
{
	FsHeader *header = getFile(path);

	if(header==NULL)	return;

    QMap<quint32, FsHeader *>::iterator i = sortedByPosition.find(header->position(), header);
    if(i == sortedByPosition.end())	return;
	++i;
    if(i == sortedByPosition.end())	return;

	int size = i.value()->position() - header->position();// nextHeaderPos - headerPos = RealHeaderSize

	QByteArray data = header->data(fs_data, false);
	changePositions(header, -size);

	fs_data.remove(header->position(), size);
	header->setPosition(fs_data.size());
	// toc_access.insert(header->path().toLower(), fs_data.size());
	fs_data.append(data);

	rebuildInfos();
}

void FsArchive::rebuildInfos()
{
	// Rebuild structure and order indication
	QMap<quint32, FsHeader *> newInfos;
    QMap<QString, FsHeader *> newToc;
    foreach(FsHeader *info, sortedByPosition) {
		newInfos.insert(info->position(), info);
        newToc.insert(info->path().toLower(), info);
	}

    sortedByPosition = newInfos;
	toc_access = newToc;
}

QMap<QString, FsHeader *> FsArchive::fileList(QString dir) const
{
	QString filePath;
	QMap<QString, FsHeader *> list;

	dir = cleanPath(dir);

	// qDebug() << "FsArchive::fileList(QString dir)" << dir;

    foreach(FsHeader *info, sortedByPosition) {
		filePath = info->path();

		if(filePath.compare(dir, Qt::CaseInsensitive)!=0
				&& (dir.isEmpty() || filePath.startsWith(dir, Qt::CaseInsensitive))) {
			if(!dir.isEmpty())
				filePath.remove(0, dir.size());

			if(filePath.count('\\')<=0) {
				list.insert(filePath.toLower(), info);
			}
			else {
				list.insert(filePath.toLower().section('\\', 0, 0), NULL);
			}
		}
	}

	return list;
}

QStringList FsArchive::tocInDirectory(QString dir) const
{
	QString filePath;
	QStringList list;

	dir = cleanPath(dir);

    foreach(FsHeader *info, sortedByPosition) {
		filePath = info->path();
		if(filePath.compare(dir, Qt::CaseInsensitive)!=0
				&& (dir.isEmpty() || filePath.startsWith(dir, Qt::CaseInsensitive))) {
			list.append(filePath);
		}
	}

	return list;
}

QString FsArchive::cleanPath(QString path)
{
	path = QDir::cleanPath(path);
	if(path.isEmpty() || path=="/") 	return path = QString();

	path.replace('/', '\\');
	if(path.endsWith('\\'))
		return path;
	path.append("\\");
	return path;
}

QString FsArchive::errorString(Error error, const QString &fileName)
{
	switch(error) {
	case Ok:
		return QObject::tr("Opération réalisée avec succès !");
	case NonWritable:
		return QObject::tr("L'archive Fs ne peut pas être modifiée, vérifiez vos droits en écriture.");
	case SourceCantBeOpened:
		return QObject::tr("Le fichier source ne peut pas être ouvert%1.").arg(fileName.isEmpty() ? "" : " ("+fileName+")");
	case TempCantBeOpened:
		return QObject::tr("Le fichier temp ne peut pas être ouvert%1.").arg(fileName.isEmpty() ? "" : " ("+fileName+")");
	case TargetCantBeOpened:
		return QObject::tr("Le fichier cible ne peut pas être ouvert%1.").arg(fileName.isEmpty() ? "" : " ("+fileName+")");
	case Canceled:
		return QObject::tr("Opération annulée.");
	case SaveHeaderError:
		return QObject::tr("Erreur lors de la création des fichiers '*.fi' et '*.fl'.");
	case ReplaceArchiveError:
		return QObject::tr("Erreur lors de la sauvegarde de l'archive Fs%1.").arg(fileName.isEmpty() ? "" : " ("+fileName+")");
	case FileExists:
		return QObject::tr("Ce fichier existe déjà dans l'archive%1.").arg(fileName.isEmpty() ? "" : " ("+fileName+")");
	case FileDontExists:
		return QObject::tr("Ce fichier n'existe pas dans l'archive%1.").arg(fileName.isEmpty() ? "" : " ("+fileName+")");
	case EmptyFile:
		return QObject::tr("Ce fichier est vide%1.").arg(fileName.isEmpty() ? "" : " ("+fileName+")");
	}
	return QObject::tr("Erreur inconnue.");
}

/*QString FsArchive::toString()
{
 QString ret;
 FsHeader *header;

 if(_isOpen) {

  if(fromFile) {
   ret.append(QString("[FS] %1 | [FL] %2 | [FI] %3\n").arg(fs.fileName(), fl.fileName(), fi.fileName()));
  }

  foreach(int pos, toc_access) {
   header = sortedByPosition.value(pos);
   ret.append(QString("%1 = pos:%2 | uncompressed_size:%3 | isCompressed:%4 | isNull:%5\n")
		.arg(header->path()).arg(header->position()).arg(header->uncompressed_size()).arg(header->isCompressed()).arg(header->isNull()));
  }
 }
 else {
  ret.append("Erreur : archive non ouverte");
 }

 return ret;
}*/

bool FsArchive::verify()
{
	quint64 guessPos = 0;
	quint32 ss = 0;
	foreach(FsHeader *info, sortedByPosition) {
		ss = qMax(info->uncompressed_size(), ss);
		quint32 size;
		if(!info->physicalSize(&fs, &size)) {
			qWarning() << "FsArchive::verify io error" << fs.errorString();
			return false;
		}

		if(size > 4478885) {
			qWarning() << "FsArchive::verify strange size" << size;
		}

		if(guessPos != info->position()) {
			qWarning() << "FsArchive::verify ko" << info->position() << guessPos << size << info->uncompressed_size() << info->isCompressed();
		} else {
			qWarning() << "FsArchive::verify ok" << info->position() << guessPos << size << info->uncompressed_size() << info->isCompressed();
		}
		guessPos += size;
	}

	return true;
}

bool FsArchive::repair(FsArchive *other)
{
	quint32 size;
	 // Copy
	QMultiMap<quint32, FsHeader *> otherHeaders = other->sortedByPosition;
	QMapIterator<quint32, FsHeader *> it(sortedByPosition);
	fs.reset();
	forever {
		int decSize = 0;
		qint64 pos = fs.pos();
		if(pos >= fs.size()) {
			break;
		}
		fs.read((char *)&size, 4);
		bool compressed;
		QByteArray d;
		if(size <= 5000000) {
			QByteArray data = fs.read(size);
			d = LZS::decompressAll(data);
			decSize = d.size();
			qDebug() << "repair entry compressed" << pos << size << d.size();
			compressed = true;
		} else {
			fs.seek(pos);
			qDebug() << "uncompressed" << fs.peek(4).constData();
			if(QString(fs.peek(3)) == "C:\\") {
				forever {
					qint64 oldPos = fs.pos();
					QByteArray line = fs.readLine(150);
					if(line.isEmpty()) {
						fs.seek(oldPos);
						break;
					}
					QString modLine = line;
					if(modLine.size() < line.size()) {
						fs.seek(oldPos);
						break;
					}
					qDebug() << "repair entry read line" << line.toHex() << modLine << modLine.size();
					decSize += modLine.size();
				}

				qDebug() << "repair entry uncompressed" << pos << decSize;
				compressed = false;
				d = fs.read(decSize);
			} else {
				qWarning() << "FsArchive::repair not an uncompressed .fl";
				return false;
			}
		}

		if(it.hasNext()) {
			it.next();
			it.value()->setPosition(pos);
			it.value()->setUncompressed_size(decSize);
			if(it.value()->isCompressed() != compressed) {
				qWarning() << "repair compression change";
			}
			it.value()->setIsCompressed(compressed);
			// Find path in correct archive
			quint32 posOther;
			if(searchData(otherHeaders, &(other->fs), d, posOther)) {
				it.value()->setPath(otherHeaders.value(posOther)->path());
				otherHeaders.remove(posOther);
			} else {
				qDebug() << "FsArchive::repair path diff" << it.value()->path();
			}
		} else {
			qWarning() << "FsArchive::repair not enough files in .fi";
			return false;
		}
	}
	if(it.hasNext()) {
		qWarning() << "FsArchive::repair not enough files in .fi 2";
	}
	return true;

	quint64 guessPos = 0;
	foreach(FsHeader *info, sortedByPosition) {
		quint32 size;
		if(!info->physicalSize(&fs, &size)) {
			qWarning() << "FsArchive::verify io error" << fs.errorString();
			return false;
		}
		if(guessPos != info->position()) {
			info->setPosition(guessPos);

			changePositions(info, guessPos - info->position());
		}
		guessPos += size;
	}
	rebuildInfos();
	return true;
}

bool FsArchive::searchData(const QMultiMap<quint32, FsHeader *> &headers,
                           QFile *fs, const QByteArray &data, quint32 &pos)
{
	foreach (const FsHeader *header, headers) {
		if (header->data(fs) == data) {
			pos = header->position();
			return true;
		}
	}
	return false;
}
