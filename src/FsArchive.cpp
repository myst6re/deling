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
#include "FsArchive.h"
#include "QLZ4.h"
#include "LZS.h"
#include "ArchiveObserver.h"

FsHeader::FsHeader()
    : _uncompressedSize(0), _position(quint32(-1)), _compression(quint32(CompressionNone))
{
}

FsHeader::FsHeader(const QString &path, quint32 uncompressedSize, quint32 position, quint32 compression)
    : _path(path), _uncompressedSize(uncompressedSize), _position(position), _compression(compression)
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
	if ((index=_path.lastIndexOf('\\'))!=-1)
		return QDir::cleanPath(_path.left(index));
	return QDir::cleanPath(_path);
}

QString FsHeader::fileName() const
{
	int index;
	if ((index=_path.lastIndexOf('\\'))!=-1)
		return _path.mid(index+1);
	return _path;
}

quint32 FsHeader::uncompressedSize() const
{
	return _uncompressedSize;
}

void FsHeader::setUncompressedSize(quint32 uncompressedSize)
{
	_uncompressedSize = uncompressedSize;
}

quint32 FsHeader::position() const
{
	return _position;
}

void FsHeader::setPosition(quint32 position)
{
	_position = position;
}

FiCompression FsHeader::compression() const
{
	switch (_compression) {
	case 0:
		return CompressionNone;
	case 1:
		return CompressionLzs;
	case 2:
		return CompressionLz4;
	}

	return CompressionUnknown;
}

void FsHeader::setCompression(FiCompression compression)
{
	_compression = quint32(compression);
}

bool FsHeader::compressedSize(QFile *fs, quint32 *lzsSize) const
{
	return isCompressed()
	        && fs->seek(_position)
	        && fs->read((char *)lzsSize, sizeof(quint32)) == sizeof(quint32);
}

bool FsHeader::compressedSize(const char *fs_data, int size, quint32 *lzsSize) const
{
	if (isCompressed()) {
		if (size <= int(sizeof(quint32))) {
			return false;
		}

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
	if (isCompressed()) {
		if (!compressedSize(fs, size)) {
			return false;
		}
		*size += sizeof(quint32);
		return true;
	}
	*size = _uncompressedSize;
	return true;
}

bool FsHeader::physicalSize(const QByteArray &fs_data, quint32 *size) const
{
	if (isCompressed()) {
		if (!compressedSize(fs_data, size)) {
			return false;
		}
		*size += sizeof(quint32);
		return true;
	}
	return _uncompressedSize;
}

const QByteArray &FsHeader::decompress(const char *data, int size, int max) const
{
	if (compression() == CompressionLz4)
	{
		return QLZ4::decompress(data, size, max);
	}

	return LZS::decompress(data, size, max);
}

QByteArray FsHeader::compress(const QByteArray &data) const
{
	if (compression() == CompressionLz4)
	{
		return QLZ4::compress(data);
	}

	return LZS::compress(data);
}

QByteArray FsHeader::data(const QByteArray &fs_data, bool uncompress, int maxUncompress) const
{
	if (isCompressed())
	{
		quint32 size;

		if (!compressedSize(fs_data, &size))
			return QByteArray();

		// fucking size
		if (size > _uncompressedSize*2)
			return QByteArray();

		if (!uncompress)
			return fs_data.mid(_position, size+4);

		return decompress(fs_data.constData() + _position + 4, size, maxUncompress<=0 ? _uncompressedSize : maxUncompress);
	}

	return fs_data.mid(_position, _uncompressedSize);
}

QByteArray FsHeader::data(QFile *fs, bool uncompress, int maxUncompress) const
{
	if (!fs->seek(_position)) 	return QByteArray();

	if (isCompressed())
	{
		quint32 size;

		if (!compressedSize(fs, &size))
			return QByteArray();

		// fucking size
		if (size > _uncompressedSize*2)
			return QByteArray();

		if (!uncompress) {
			fs->seek(_position);
			return fs->read(size+4);
		}

		char *buff = new char[size];

		if (fs->read(buff, size) != size)
			return QByteArray();

		const QByteArray &decData = decompress(buff, size, maxUncompress<=0 ? _uncompressedSize : maxUncompress);
		delete[] buff;

		return decData;
	}

	return fs->read(_uncompressedSize);
}

int FsHeader::setData(QByteArray &fs_data, const QByteArray &new_data)
{
	int diff, real_size;

	if (isCompressed())
	{
		const char *fs_data_const = fs_data.constData();
		int size=0;

		memcpy(&size, &fs_data_const[_position], 4);

		_uncompressedSize = new_data.size();
		QByteArray new_data_lzsed = compress(new_data);
		real_size = new_data_lzsed.size();

		diff = real_size - size;

		fs_data.replace(_position, 4, (char *)&real_size);
		fs_data.replace(_position+4, size, new_data_lzsed);
	}
	else {
		fs_data.replace(_position, _uncompressedSize, new_data);
		real_size = new_data.size();
		diff = real_size - _uncompressedSize;
		_uncompressedSize = real_size;
	}

	return diff;
}

int FsHeader::setData(QFile *fs, QByteArray &new_data)
{
	int diff, real_size;

	if (isCompressed())
	{
		if (!fs->seek(_position)) return 0;

		int size=0;

		fs->read((char *)&size, 4);

		_uncompressedSize = new_data.size();
		new_data = compress(new_data);
		real_size = new_data.size();
		new_data.prepend((char *)&real_size, 4);

		diff = real_size - size;
	}
	else {
		real_size = new_data.size();
		diff = real_size - _uncompressedSize;
		_uncompressedSize = real_size;
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
    : fromFile(false), _isOpen(false)
{
	if (!path.isEmpty()) {
		open(path);
	}
}

FsArchive::~FsArchive()
{
	if (fromFile) {
		fs.close();
		fl.close();
		fi.close();
		qDebug() << "close fs" << fs.fileName();
	}
	for (FsHeader *header: toc_access) 	delete header;
}

void FsArchive::addFile(const QString &path, quint32 uncompressedSize, quint32 position, quint32 compression)
{
	if (toc_access.contains(path.toLower())) {
		qWarning() << "addFile error: file already exists" << path << uncompressedSize << position << compression;
		return;
	}
	FsHeader *header = new FsHeader(path, uncompressedSize, position, compression);
	sortedByPosition.insert(position, header);
	toc_access.insert(path.toLower(), header);
}

void FsArchive::addFile(const QString &path, FiCompression compression)
{
	if (toc_access.contains(path.toLower())) {
		qWarning() << "addFile error: file already exists" << path << quint32(compression);
		return;
	}
	FsHeader *header = new FsHeader(path, 0, 0, compression);
	sortedByPosition.insert(0, header);
	toc_access.insert(path.toLower(), header);
}

bool FsArchive::removeFile(QString path)
{
	path = path.toLower();
	FsHeader *header = toc_access.take(path);
	if (header != 0) {
		bool ok = sortedByPosition.remove(header->position(), header)==1;
		if (!ok) {
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

	if (path.startsWith('*'))
	{
		QString path2 = path.sliced(1);
		QMapIterator<QString, FsHeader *> i(toc_access);

		if (path2.contains('*') || path2.contains('?'))
		{
			QRegularExpression expr = QRegularExpression::fromWildcard(path2, Qt::CaseInsensitive, QRegularExpression::WildcardConversionOptions(QRegularExpression::UnanchoredWildcardConversion | QRegularExpression::NonPathWildcardConversion));
			expr.setPattern(expr.pattern().append("$"));
			
			while (i.hasNext())
			{
				i.next();
				if (i.key().contains(expr))
				{
					return i.value();
				}
			}
			
			return nullptr;
		}

		while (i.hasNext())
		{
			i.next();
			if (i.key().endsWith(path2, Qt::CaseInsensitive))
			{
				return i.value();
			}
		}
		
		return nullptr;
	}
	
	return toc_access.value(path.toLower(), nullptr);
}

bool FsArchive::fileExists(const QString &path) const
{
	return getFile(path)!=nullptr;
}

bool FsArchive::dirExists(QString dir) const
{
	dir = cleanPath(dir);

	if (dir.isEmpty())
		return true;

	for (FsHeader *info: toc_access) {
		if (info->path().startsWith(dir, Qt::CaseInsensitive)) {
			return true;
		}
	}

	return false;
}

QString FsArchive::filePath(const QString &path) const
{
	FsHeader *header = getFile(path);
	return header!=nullptr ? header->path() : QString();
}

bool FsArchive::setFilePath(QString path, const QString &newPath)
{
	path = path.toLower();

	if (toc_access.contains(path) && !toc_access.contains(newPath.toLower()))
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
	return header!=nullptr ? header->uncompressedSize() : quint32(-1);
}

quint32 FsArchive::filePosition(const QString &path) const
{
	FsHeader *header = getFile(path);
	return header!=nullptr ? header->position() : quint32(-1);
}

void FsArchive::setFilePosition(const QString &path, quint32 position)
{
	FsHeader *header = getFile(path);
	if (header!=nullptr) {
		header->setPosition(position);
	}
}

FiCompression FsArchive::fileCompression(const QString &path) const
{
	FsHeader *header = getFile(path);
	return header!=nullptr ? header->compression() : FiCompression::CompressionNone;
}

QByteArray FsArchive::fileData(const QString &path, const QByteArray &fs_data, bool uncompress, int uncompressedSize)
{
	FsHeader *header = getFile(path);
	//	qDebug() << "fileData1" << path << fs_data.size() << uncompressedSize;
	return header!=nullptr ? header->data(fs_data, uncompress, uncompressedSize) : QByteArray();
}

QByteArray FsArchive::fileData(const QString &path, bool uncompress, int uncompressedSize)
{
	FsHeader *header = getFile(path);
	//	qDebug() << "fileData2" << path << uncompressedSize << fromFile << _isOpen;
	return header!=nullptr && fromFile && _isOpen ? header->data(&fs, uncompress, uncompressedSize) : QByteArray();
}

void FsArchive::setFileData(const QString &path, QByteArray &fs_data, const QByteArray &new_data)
{
	FsHeader *header = getFile(path);
	if (header!=nullptr) {
		changePositions(header, header->setData(fs_data, new_data));
	}
}

void FsArchive::setFileData(const QString &path, QByteArray &new_data)
{
	FsHeader *header = getFile(path);
	if (header!=nullptr) {
		header->setData(&fs, new_data);
	}
}

void FsArchive::changePositions(FsHeader *start, int diff)
{
	if (diff==0)	return;

	FsHeader *header;

	// décalage des positions des fichiers qui suivent
	QMultiMap<quint32, FsHeader *>::iterator i = sortedByPosition.find(sortedByPosition.key(start), start);
	if (i != sortedByPosition.end())
	{
		++i;
		while (i != sortedByPosition.end())
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
	while (i.hasNext()) {
		i.next();
		header.insert(i.key(), *(i.value()));
	}
	return header;
}

void FsArchive::setHeader(const QMap<QString, FsHeader> &header)
{
	QMapIterator<QString, FsHeader> i(header);
	while (i.hasNext()) {
		i.next();
		FsHeader h = i.value(), *h2 = toc_access.value(i.key());
		if (h2) {
			h2->setPosition(h.position());
			h2->setUncompressedSize(h.uncompressedSize());
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
	for (FsHeader *header: sortedByPosition) {
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
	for (FsHeader *header: sortedByPosition) {
		dir = header->path();
		cleanPath(dir);
		lastIndex = dir.lastIndexOf('\\', -2);
		if (lastIndex != -1) {
			dir.truncate(lastIndex);
		} else {
			dir = "";
		}
		if (!dirs.contains(dir))
			dirs.append(dir);
	}

	return dirs;
}

bool FsArchive::extractFile(const QString &fileName, const QByteArray &fs_data, const QString &filePath, bool uncompress)
{
	if (!_isOpen)	return false;

	QFile fic(filePath);
	fic.open(QIODevice::WriteOnly);
	fic.write(fileData(fileName, fs_data, uncompress));
	fic.close();

	return true;
}

bool FsArchive::extractFile(const QString &fileName, const QString &filePath, bool uncompress)
{
	if (!fromFile || !_isOpen)	return false;

	QFile fic(filePath);
	fic.open(QIODevice::WriteOnly);
	fic.write(fileData(fileName, uncompress));
	fic.close();

	return true;
}

FsArchive::Error FsArchive::extractFiles(const QStringList &fileNames, const QString &baseFileName, const QString &fileDir, ArchiveObserver *progress, bool uncompress)
{
	if (!fromFile || !_isOpen)	return SourceCantBeOpened;

	// qDebug() << "extractFiles" << fileNames << fileDir << uncompress;

	if (progress != nullptr) {
		progress->setObserverMaximum(fileNames.size());
	}

	QDir dir(fileDir);
	int i=0, sizeOfBaseFileName = baseFileName.size();

	for (QString fileName: fileNames) {
		QCoreApplication::processEvents();

		if (progress != nullptr && progress->observerWasCanceled()) {
			return Canceled;
		}

		fileName = cleanPath(fileName);
		fileName.chop(1);
		int index = fileName.lastIndexOf('\\');
		if (index - sizeOfBaseFileName > 0) {
			dir.mkpath(QDir::cleanPath(fileName.mid(sizeOfBaseFileName, index-sizeOfBaseFileName)));
		}

		QFile fic(QDir::cleanPath(fileDir % QDir::separator() % fileName.mid(sizeOfBaseFileName).replace('\\', '/')));
		if (!fic.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
			return TargetCantBeOpened;
		}
		fic.write(fileData(fileName, uncompress));
		fic.close();

		if (progress != nullptr) {
			progress->setObserverValue(i++);
		}
	}

	return Ok;
}

FsArchive::Error FsArchive::replaceFile(const QString &source, const QString &destination, ArchiveObserver *progress)
{
	QElapsedTimer t;t.start();

	QStringList toc;
	int pos, i=0;
	QString save_path = path(), temp_path;
	QByteArray sourceData;

	if (!isWritable())	return NonWritable;

	save_path.chop(1);// remove s, i or l in extension
	temp_path = save_path.left(save_path.lastIndexOf("/")+1) + "delingtemp.f";

	QFile sourceFile(source);
	if (!sourceFile.open(QIODevice::ReadOnly))
		return SourceCantBeOpened;
	sourceData = sourceFile.readAll();
	sourceFile.close();

	QFile temp(FsArchive::fsPath(temp_path));
	if (!temp.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return TempCantBeOpened;

	toc = this->toc();

	progress->setObserverMaximum(toc.size());

	for (const QString &entry: toc) {
		QCoreApplication::processEvents();
		if (progress->observerWasCanceled()) {
			temp.remove();
			return Canceled;
		}

		pos = temp.pos();

		if (entry == destination) {
			setFileData(entry, sourceData);
			temp.write(sourceData);
		}
		else {
			temp.write(fileData(entry, false));
		}

		setFilePosition(entry, pos);
		progress->setObserverValue(i++);
	}

	if (!saveAs(temp_path)) {
		QFile::remove(FsArchive::fiPath(temp_path));
		QFile::remove(FsArchive::flPath(temp_path));
		temp.remove();
		return SaveHeaderError;
	}

	int replaceError = replaceArchive(&temp);
	if (replaceError==1) {
		QFile::remove(FsArchive::fiPath(temp_path));
		QFile::remove(FsArchive::flPath(temp_path));
		temp.remove();
		return ReplaceArchiveError;
	} else if (replaceError!=0) {
		return ReplaceArchiveError;
	}

	rebuildInfos();

	qDebug() << "save time" << t.elapsed();

	return Ok;
}

QList<FsArchive::Error> FsArchive::replaceDir(const QString &source, const QString &destination, FiCompression compression, ArchiveObserver *progress)
{
	// TODO progress
	Error err = remove(tocInDirectory(destination), progress);
	if (err != Ok) {
		return QList<FsArchive::Error>() << err;
	}

	// TODO progress
	return appendDir(source, destination, compression, progress);
}

QStringList FsArchive::listDirsRec(QDir *sourceDir)
{
	QStringList paths;

	for (const QFileInfo &fileInfo: sourceDir->entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Readable)) {
		if (fileInfo.isDir()) {
			sourceDir->cd(fileInfo.fileName());
			for (const QString &subPath: listDirsRec(sourceDir)) {
				paths << fileInfo.fileName() + "/" + subPath;
			}
			sourceDir->cdUp();
		} else {
			paths << fileInfo.fileName();
		}
	}

	return paths;
}

QList<FsArchive::Error> FsArchive::appendFiles(const QStringList &sources, const QStringList &destinations, FiCompression compression, ArchiveObserver *progress)
{
	QElapsedTimer t;t.start();

	QByteArray data, fl_data, fi_data;
	int i, nbFiles = sources.size();
	QList<Error> errors;

	if (!isWritable()) {
		errors.append(NonWritable);
		return errors;
	}

	fs.seek(fs.size());

	progress->setObserverMaximum(nbFiles);

	for (i = 0; i < nbFiles; ++i) {
		QCoreApplication::processEvents();
		if (progress->observerWasCanceled())				break;// Stop, don't canceling

		if (fileExists(destinations.at(i))) {
			//			qDebug() << destinations.at(i);
			errors.append(FileExists);
			continue;
		}

		//		qDebug() << sources.at(i);

		QFile file(sources.at(i));
		if (!file.open(QIODevice::ReadOnly)) {
			errors.append(SourceCantBeOpened);
			continue;
		}

		data = file.readAll();
		file.close();
		if (data.isEmpty()) {
			errors.append(EmptyFile);
			continue;
		}
		addFile(destinations.at(i), (quint32)data.size(), (quint32)fs.pos(), compression);

		if (compression == FiCompression::CompressionLzs) {
			data = LZS::compress(data);
			int size = data.size();
			data.prepend((char *)&size, 4);
		} else if (compression == FiCompression::CompressionLz4) {
			data = QLZ4::compress(data);
			int size = data.size();
			data.prepend((char *)&size, 4);
		}

		fs.write(data);
		progress->setObserverValue(i + 1);
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

QList<FsArchive::Error> FsArchive::appendDir(const QString &source, const QString &destination, FiCompression compression, ArchiveObserver *progress)
{
	QStringList sources, destinations;
	QDir sourceDir(source);

	for (const QString &relativePath: listDirsRec(&sourceDir)) {
		sources << sourceDir.absoluteFilePath(relativePath);
		destinations << destination + "\\" + QString(relativePath).replace('/', '\\');
	}

	return appendFiles(sources, destinations, compression, progress);
}

FsArchive::Error FsArchive::remove(QStringList destinations, ArchiveObserver *progress)
{
	QElapsedTimer t;t.start();

	QStringList toc = this->toc();
	//	QByteArray fl_data, fi_data;
	int pos, i=0;
	QString save_path = path(), temp_path;

	if (!isWritable())	return NonWritable;

	save_path.chop(1);// remove s, i or l in extension
	temp_path = save_path.left(save_path.lastIndexOf("/")+1) + "delingtemp.f";

	QFile temp(FsArchive::fsPath(temp_path));
	if (!temp.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return TempCantBeOpened;

	progress->setObserverMaximum(toc.size());

	for (const QString &entry: toc) {
		QCoreApplication::processEvents();
		if (progress->observerWasCanceled()) {
			temp.remove();
			return Canceled;
		}

		pos = temp.pos();

		if (destinations.contains(entry, Qt::CaseInsensitive)) {
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

	if (!saveAs(temp_path)) {
		QFile::remove(FsArchive::fiPath(temp_path));
		QFile::remove(FsArchive::flPath(temp_path));
		temp.remove();
		return SaveHeaderError;
	}

	int replaceError = replaceArchive(&temp);
	if (replaceError==1) {
		QFile::remove(FsArchive::fiPath(temp_path));
		QFile::remove(FsArchive::flPath(temp_path));
		temp.remove();
		return ReplaceArchiveError;
	} else if (replaceError!=0) {
		return ReplaceArchiveError;
	}

	qDebug() << "save time" << t.elapsed();

	return Ok;
}

FsArchive::Error FsArchive::rename(const QStringList &destinations, const QStringList &newDestinations)
{
	QElapsedTimer t;t.start();

	QByteArray fl_data, fi_data;
	QString destination, newDestination;
	int size, i;

	if (!isWritable())						return NonWritable;

	size = destinations.size();
	for (i = 0; i < size; ++i) {
		destination = destinations.at(i);
		newDestination = newDestinations.at(i);

		if (!fileExists(destination))			return FileDontExists;
		if (fileExists(newDestination))			return FileExists;

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

bool FsArchive::load(const QByteArray &fl_data, const QByteArray &fi_data)
{
	if (_isOpen || fl_data.isEmpty() || fi_data.isEmpty()) {
		qWarning() << "Error loading data" << fl_data.isEmpty() << fi_data.isEmpty();
		return false;
	}

	QStringList fl = QString(fl_data).split("\r\n", Qt::SkipEmptyParts);
	Fi_infos fi_infos;

	const char *fi_constData = fi_data.constData();

	if (fi_data.size()/12 != fl.size() || fi_data.size()%12 != 0) {
		_isOpen = false;
		qWarning() << "Invalid fl or fi" << (fi_data.size()/12.0) << (fl.size());
		return false;
	}

	for (const QString &line: fl) {
		memcpy(&fi_infos, fi_constData, 12);

		addFile(line, fi_infos.size, fi_infos.pos, fi_infos.compression);
		fi_constData += 12;
	}

	_isOpen = true;

	/* verify();
	if (repair()) {
		verify();
	} else {
		qDebug() << "cannot repair";
	} */

	return true;
}

bool FsArchive::open(const QString &path)
{
	if (_isOpen)	return false;

	fl.setFileName(FsArchive::flPath(path));
	fi.setFileName(FsArchive::fiPath(path));
	fs.setFileName(FsArchive::fsPath(path));
	qDebug() << fl.fileName() << fi.fileName() << fs.fileName();
	if (!fl.open(QIODevice::ReadWrite)) {
		if (!fl.open(QIODevice::ReadOnly))
			return false;
	}
	if (!fi.open(QIODevice::ReadWrite)) {
		if (!fi.open(QIODevice::ReadOnly))
			return false;
	}
	if (!fs.open(QIODevice::ReadWrite)) {
		if (!fs.open(QIODevice::ReadOnly))
			return false;
	}

	qDebug() << "open fs" << path;

	fromFile = true;

	return (_isOpen = load(fl.readAll(), fi.readAll()));
}

void FsArchive::save(QByteArray &fl_data, QByteArray &fi_data) const
{
	quint32 size, pos, compression;

	for (FsHeader *header: sortedByPosition) {
		fl_data.append(header->path().toLatin1());
		fl_data.append("\r\n", 2);
		size = header->uncompressedSize();
		pos = header->position();
		compression = header->compression();
		fi_data.append((char *)&size, 4);
		fi_data.append((char *)&pos, 4);
		fi_data.append((char *)&compression, 4);
	}
}

bool FsArchive::saveAs(const QString &path) const
{
	QFile fl(FsArchive::flPath(path)), fi(FsArchive::fiPath(path));
	if (!fl.open(QIODevice::WriteOnly | QIODevice::Truncate)
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
	if (!fromFile)	return 1;

	_isOpen = false;

	fs.close();

	if (!fs.remove()) {
		qWarning() << "Impossible to remove the fs" << fs.errorString();
		fs.open(QIODevice::ReadWrite);
		return 1;
	}

	fl.close();
	fi.close();

	qDebug() << "reclose fs" << fs.fileName();

	if (!fl.remove() || !fi.remove())	return 2;

	QString fileName = newFile->fileName();
	fileName.chop(1);

	if (!newFile->rename(fs.fileName()) || !QFile::rename(FsArchive::flPath(fileName), fl.fileName()) || !QFile::rename(FsArchive::fiPath(fileName), fi.fileName()))
		return 2;

	if (!fs.open(QIODevice::ReadWrite) || !fl.open(QIODevice::ReadWrite) || !fi.open(QIODevice::ReadWrite))
		return 2;

	qDebug() << "reopen fs" << fs.fileName();

	_isOpen = true;

	return 0;
}

QString FsArchive::fsPath(const QString &path)
{
	if (path.endsWith('F')) {
		return path % 'S';
	} else {
		return path % 's';
	}
}

QString FsArchive::flPath(const QString &path)
{
	if (path.endsWith('F')) {
		return path % 'L';
	} else {
		return path % 'l';
	}
}

QString FsArchive::fiPath(const QString &path)
{
	if (path.endsWith('F')) {
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

qint64 FsArchive::size() const
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

	if (header==nullptr)	return;

	QMultiMap<quint32, FsHeader *>::iterator i = sortedByPosition.find(header->position(), header);
	if (i == sortedByPosition.end())	return;
	++i;
	if (i == sortedByPosition.end())	return;

	qint64 size = i.value()->position() - header->position();// nextHeaderPos - headerPos = RealHeaderSize

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
	QMultiMap<quint32, FsHeader *> newInfos;
	QMap<QString, FsHeader *> newToc;
	for (FsHeader *info: sortedByPosition) {
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

	for (FsHeader *info: sortedByPosition) {
		filePath = info->path();

		if (filePath.compare(dir, Qt::CaseInsensitive)!=0
		        && (dir.isEmpty() || filePath.startsWith(dir, Qt::CaseInsensitive))) {
			if (!dir.isEmpty())
				filePath.remove(0, dir.size());

			if (filePath.count('\\')<=0) {
				list.insert(filePath.toLower(), info);
			}
			else {
				list.insert(filePath.toLower().section('\\', 0, 0), nullptr);
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

	for (FsHeader *info: sortedByPosition) {
		filePath = info->path();
		if (filePath.compare(dir, Qt::CaseInsensitive)!=0
		        && (dir.isEmpty() || filePath.startsWith(dir, Qt::CaseInsensitive))) {
			list.append(filePath);
		}
	}

	return list;
}

QString FsArchive::cleanPath(QString path)
{
	path = QDir::cleanPath(path);
	if (path.isEmpty() || path=="/") 	return path = QString();

	path.replace('/', '\\');
	if (path.endsWith('\\'))
		return path;
	path.append("\\");
	return path;
}

QString FsArchive::errorString(Error error, const QString &fileName)
{
	switch (error) {
	case Ok:
		return QObject::tr("Operation successfully completed!");
	case NonWritable:
		return QObject::tr("The Fs archive can not be modified, check your rights in writing.");
	case SourceCantBeOpened:
		return QObject::tr("The source file cannot be opened%1.").arg(fileName.isEmpty() ? "" : " ("+fileName+")");
	case TempCantBeOpened:
		return QObject::tr("The temps file cannont be opened%1.").arg(fileName.isEmpty() ? "" : " ("+fileName+")");
	case TargetCantBeOpened:
		return QObject::tr("The target file cannot be opened%1.").arg(fileName.isEmpty() ? "" : " ("+fileName+")");
	case Canceled:
		return QObject::tr("Operation canceled.");
	case SaveHeaderError:
		return QObject::tr("Error while creating files '*.fi' and '*.fl'.");
	case ReplaceArchiveError:
		return QObject::tr("Error while saving the archive Fs%1.").arg(fileName.isEmpty() ? "" : " ("+fileName+")");
	case FileExists:
		return QObject::tr("This file already exists in the archive%1.").arg(fileName.isEmpty() ? "" : " ("+fileName+")");
	case FileDontExists:
		return QObject::tr("This file does not exists in the archive%1.").arg(fileName.isEmpty() ? "" : " ("+fileName+")");
	case EmptyFile:
		return QObject::tr("This file is empty%1.").arg(fileName.isEmpty() ? "" : " ("+fileName+")");
	}
	return QObject::tr("Unknown error.");
}

/*QString FsArchive::toString()
{
 QString ret;
 FsHeader *header;

 if (_isOpen) {

  if (fromFile) {
   ret.append(QString("[FS] %1 | [FL] %2 | [FI] %3\n").arg(fs.fileName(), fl.fileName(), fi.fileName()));
  }

  for (int pos: toc_access) {
   header = sortedByPosition.value(pos);
   ret.append(QString("%1 = pos:%2 | uncompressedSize:%3 | isCompressed:%4 | isNull:%5\n")
		.arg(header->path()).arg(header->position()).arg(header->uncompressedSize()).arg(header->isCompressed()).arg(header->isNull()));
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
	for (FsHeader *info: sortedByPosition) {
		ss = qMax(info->uncompressedSize(), ss);
		quint32 size;
		if (!info->physicalSize(&fs, &size)) {
			qWarning() << "FsArchive::verify io error" << fs.errorString();
			return false;
		}

		if (size > 4478885) {
			qWarning() << "FsArchive::verify strange size" << size;
		}

		if (guessPos != info->position()) {
			qWarning() << "FsArchive::verify ko" << info->position() << guessPos << size << info->uncompressedSize() << quint32(info->compression());
		} else {
			qWarning() << "FsArchive::verify ok" << info->position() << guessPos << size << info->uncompressedSize() << quint32(info->compression());
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
	QMultiMapIterator<quint32, FsHeader *> it(sortedByPosition);
	fs.reset();
	forever {
		int decSize = 0;
		qint64 pos = fs.pos();
		if (pos >= fs.size()) {
			break;
		}
		fs.read((char *)&size, 4);
		bool compressed;
		QByteArray d;
		if (size <= 5000000) {
			QByteArray data = fs.read(size);
			d = LZS::decompress(data);
			decSize = d.size();
			qDebug() << "repair entry compressed" << pos << size << d.size();
			compressed = true;
		} else {
			fs.seek(pos);
			qDebug() << "uncompressed" << fs.peek(4).constData();
			if (QString(fs.peek(3)) == "C:\\") {
				forever {
					qint64 oldPos = fs.pos();
					QByteArray line = fs.readLine(150);
					if (line.isEmpty()) {
						fs.seek(oldPos);
						break;
					}
					QString modLine = line;
					if (modLine.size() < line.size()) {
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

		if (it.hasNext()) {
			it.next();
			it.value()->setPosition(pos);
			it.value()->setUncompressedSize(decSize);
			if (compressed && it.value()->compression() != CompressionLzs) {
				qWarning() << "repair compression change";
			}
			it.value()->setCompression(CompressionLzs);
			// Find path in correct archive
			quint32 posOther;
			if (searchData(otherHeaders, &(other->fs), d, posOther)) {
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
	if (it.hasNext()) {
		qWarning() << "FsArchive::repair not enough files in .fi 2";
	}
	return true;

	/*quint64 guessPos = 0;
	for (FsHeader *info: sortedByPosition) {
		quint32 size;
		if (!info->physicalSize(&fs, &size)) {
			qWarning() << "FsArchive::verify io error" << fs.errorString();
			return false;
		}
		if (guessPos != info->position()) {
			info->setPosition(guessPos);

			changePositions(info, guessPos - info->position());
		}
		guessPos += size;
	}
	rebuildInfos();
	return true;*/
}

bool FsArchive::searchData(const QMultiMap<quint32, FsHeader *> &headers,
                           QFile *fs, const QByteArray &data, quint32 &pos)
{
	for (const FsHeader *header: headers) {
		if (header->data(fs) == data) {
			pos = header->position();
			return true;
		}
	}
	return false;
}

QString FsArchive::mostCommonPrefixPath() const
{
	QString minPath;

	for (const FsHeader *header: sortedByPosition) {
		if (minPath.isNull() || minPath.size() < header->path().size()) {
			minPath = header->path();
		}
	}
	
	int minSize = minPath.size();
	
	for (const FsHeader *header: sortedByPosition) {
		const QString &path = header->path();
		
		int i = 0;
		while (i < minSize) {
			if (minPath.at(i).toLower() != path.at(i).toLower()) {
				break;
			}
			
			++i;
		}
		
		if (i < minSize) {
			minSize = i;
		}
	}
	
	QString ret = minPath.left(minSize);
	
	if (!ret.endsWith('\\')) {
		return ret.left(ret.lastIndexOf('\\') + 1);
	}
	
	return ret;
}
