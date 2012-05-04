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
#include "FieldArchivePC.h"

FieldArchivePC::FieldArchivePC()
	: FieldArchive(), archive(NULL)
{
}

FieldArchivePC::~FieldArchivePC()
{
	if(archive != NULL)				delete archive;
}

QString FieldArchivePC::archivePath() const
{
	return archive->path();
}

FsArchive *FieldArchivePC::getFsArchive() const
{
	return archive;
}

int FieldArchivePC::open(const QString &path, QProgressDialog *progress)
{
	//qDebug() << QString("open(%1)").arg(path);
	QString archivePath = path;
	archivePath.chop(1);
	QStringList fsList;
	QString map, desc;
	int index, fieldID=0;

	if(archive != NULL)		delete archive;
	archive = new FsArchive(archivePath);
	if(!archive->isOpen())
		return 1;
	if(!archive->isWritable()) {
		readOnly = true;
	}

	clearFields();

	// Ouverture de la liste des écrans (facultatif)
	FsArchive mapData(archive->fileData("*mapdata.fl"), archive->fileData("*mapdata.fi"));
	if(mapData.isOpen()) {
		QByteArray mapdata_fs = archive->fileData("*mapdata.fs");
		setMapList(QString(mapData.fileData(QString("*maplist"), mapdata_fs)).split('\n'));
	} else {
		setMapList(QStringList());
	}

	int currentMap=0;
	// Ajout des écrans non-listés
	QStringList toc = archive->toc();
	foreach(const QString &entry, toc) {
		if(entry.endsWith(".fs", Qt::CaseInsensitive) && !entry.endsWith("mapdata.fs", Qt::CaseInsensitive)
			&& !entry.endsWith("main_chr.fs", Qt::CaseInsensitive)
			&& !entry.endsWith("ec.fs", Qt::CaseInsensitive) && !entry.endsWith("te.fs", Qt::CaseInsensitive))
		{
			if(!fsList.contains(entry, Qt::CaseInsensitive))
				fsList.append(entry);
		}
	}

	quint32 freq = fsList.size()>100 ? fsList.size()/100 : 1;

	progress->setRange(0, fsList.size());
	// Ouverture des écrans listés
	foreach(const QString &entry, fsList) {
		QCoreApplication::processEvents();

		if(progress->wasCanceled()) {
			clearFields();
			return 2;
		}

		if(currentMap%freq == 0) {
			progress->setValue(currentMap);
		}
		currentMap++;

		map = entry;
		map.chop(3);
		if((index = map.lastIndexOf('\\')) != -1)
			map = map.mid(index+1);

		if(!map.isEmpty())
		{
			Field *field = new FieldPC(map, entry, archive);
			if(field->isOpen() && field->hasFiles()) {
				if(field->hasJsmFile())
					desc = Data::locations().value(field->getJsmFile()->mapID());
				else
					desc = QString();

				index = mapList().indexOf(map);
				QString mapId = index==-1 ? "~" : QString("%1").arg(index, 3, 10, QChar('0'));

				fields.append(field);
				fieldsSortByName.insert(map, fieldID);
				fieldsSortByDesc.insert(desc, fieldID);
				fieldsSortByMapId.insert(mapId, fieldID);
				++fieldID;
			} else {
				qWarning() << "field pas ouvert" << map;
				delete field;
			}
//			break;
		}
	}

	if(fields.isEmpty()) {
		return 3;
	}

	Config::setValue("jp", false);

	return 0;
}

bool FieldArchivePC::openBG(Field *field, QByteArray &chara_data) const
{
	return ((FieldPC *)field)->open2(archive);
}

void FieldArchivePC::restoreFieldHeaders(const QMap<Field *, QMap<QString, FsHeader> > &oldFields) const
{
	QMapIterator<Field *, QMap<QString, FsHeader> > i(oldFields);
	while(i.hasNext()) {
		i.next();
		((FieldPC *)i.key())->getArchiveHeader()->setHeader(i.value());
	}
}

bool FieldArchivePC::save(QProgressDialog *progress, QString save_path)
{
	QTime t;t.start();
	QStringList toc = archive->toc();
//	QByteArray fs_data;
	int pos, archiveSize;
//	quint32 freq;
	QString file, temp_path, path=archive->path();

	if(!archive->isWritable())	return false;

	save_path.chop(1);// remove s, i or l in extension
	path.chop(1);// remove s, i or l in extension

	if(save_path.isEmpty() || save_path.compare(path, Qt::CaseInsensitive)==0) {
		save_path = path;
		temp_path = save_path.left(save_path.lastIndexOf("/")+1) + "delingtemp.f";
	}
	else {
		temp_path = save_path;
	}

	QFile temp(temp_path%'s');
	if(!temp.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		temp.remove();
		return false;
	}

	archiveSize = archive->size();
	progress->setMaximum(archiveSize);

	/*QDir dd("debug");
	foreach(QString machin, dd.entryList(QStringList() << "*")) {
		QFile::remove(dd.absoluteFilePath(machin));
	}*/
	QMap<QString, FsHeader> oldValues = archive->getHeader();
	QMap<Field *, QMap<QString, FsHeader> > oldFields;

	foreach(Field *field, fields) {
		if(field->isModified()) {
			QCoreApplication::processEvents();
			if(progress->wasCanceled()) {
				temp.remove();
				restoreFieldHeaders(oldFields);
				archive->setHeader(oldValues);
				return false;
			}

			FsArchive *fieldHeader = ((FieldPC *)field)->getArchiveHeader();
			if(fieldHeader != NULL) {
				oldFields.insert(field, fieldHeader->getHeader());
			}

			/* Save Field data */
			file = ((FieldPC *)field)->path();
			QByteArray /*fs_savCmp = */fs_data = archive->fileData(file);
			QByteArray fl_data, fi_data/*, fl_savCmp, fi_savCmp*/;
			((FieldPC *)field)->save(fs_data, fl_data, fi_data);
//			fl_savCmp = fl_data;
//			fi_savCmp = fi_data;

//			if(fs_savCmp != fs_data) {
//				QFile savfs("debug/" + field->name() + ".fs.out");
//				savfs.open(QIODevice::WriteOnly);
//				savfs.write(fs_data);
//				savfs.close();
//				QFile savfs2("debug/" + field->name() + ".fs.expected");
//				savfs2.open(QIODevice::WriteOnly);
//				savfs2.write(fs_savCmp);
//				savfs2.close();
//			}

			/* FS */
			pos = temp.pos();
			archive->setFileData(file, fs_data);
//			qDebug() << "save" << pos << file;
			archive->setFilePosition(file, pos);
			temp.write(fs_data);
			toc.removeOne(file);

			/* FL */
			file.chop(1);
			file.append('l');
			pos = temp.pos();
			archive->setFileData(file, fl_data);
//			qDebug() << "save" << pos << file;
			archive->setFilePosition(file, pos);
			temp.write(fl_data);
			toc.removeOne(file);

			/* FI */
			file.chop(1);
			file.append('i');
			pos = temp.pos();
			archive->setFileData(file, fi_data);
//			qDebug() << "save" << pos << file;
			archive->setFilePosition(file, pos);
			temp.write(fi_data);
			toc.removeOne(file);

//			if(fl_savCmp != fl_data) {
//				QFile savfl("debug/" + field->name() + ".fl.out");
//				savfl.open(QIODevice::WriteOnly);
//				savfl.write(fl_data);
//				savfl.close();
//				QFile savfl2("debug/" + field->name() + ".fl.expected");
//				savfl2.open(QIODevice::WriteOnly);
//				savfl2.write(fl_savCmp);
//				savfl2.close();
//			}
//			if(fi_savCmp != fi_data) {
//				QFile savfi("debug/" + field->name() + ".fi.out");
//				savfi.open(QIODevice::WriteOnly);
//				savfi.write(fi_data);
//				savfi.close();
//				QFile savfi2("debug/" + field->name() + ".fi.expected");
//				savfi2.open(QIODevice::WriteOnly);
//				savfi2.write(fi_savCmp);
//				savfi2.close();
//			}

			progress->setValue(pos);
		}
	}

	foreach(const QString &entry, toc) {
		QCoreApplication::processEvents();
		if(progress->wasCanceled()) {
			temp.remove();
			restoreFieldHeaders(oldFields);
			archive->setHeader(oldValues);
			return false;
		}

		pos = temp.pos();
//		qDebug() << "laisse" << pos << entry;
		temp.write(archive->fileData(entry, false));
//		qDebug() << "laisse2";
		archive->setFilePosition(entry, pos);

		progress->setValue(pos);
	}
	temp.close();

	progress->setEnabled(false);
	if(progress->wasCanceled()) {
		temp.remove();
		restoreFieldHeaders(oldFields);
		archive->setHeader(oldValues);
		return false;
	}

	if(!archive->saveAs(temp_path)) {
		qWarning() << "Error save header!!!";
		QFile::remove(temp_path%"i");
		QFile::remove(temp_path%"l");
		temp.remove();
		restoreFieldHeaders(oldFields);
		archive->setHeader(oldValues);
		return false;
	}

	if(save_path.compare(path, Qt::CaseInsensitive)==0) {
		int replaceError = archive->replaceArchive(&temp);
		if(replaceError==1) {
			QFile::remove(temp_path%"i");
			QFile::remove(temp_path%"l");
			temp.remove();
			restoreFieldHeaders(oldFields);
			archive->setHeader(oldValues);
			return false;
		} else if(replaceError!=0) {
			return false;
		}
	}
	else if(!archive->setPath(save_path)) {
		return false;
	}

	archive->rebuildInfos();

	qDebug() << "save time" << t.elapsed();

	return true;
}

bool FieldArchivePC::optimiseArchive(QProgressDialog *progress)
{
	QTime t;t.start();
	QStringList toc = archive->toc();
	QByteArray fs_data;
	int pos;
	QString file, temp_path, save_path=archive->path();

	if(!archive->isWritable())	return false;

	save_path.chop(1);// remove s, i or l in extension
	temp_path = save_path.left(save_path.lastIndexOf("/")+1) + "delingtemp.f";

	QFile temp(temp_path%"s");
	if(!temp.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return false;

	progress->setMaximum(archive->size());
	QMap<QString, FsHeader> oldValues = archive->getHeader();
	QMap<Field *, QMap<QString, FsHeader> > oldFields;

	foreach(Field *field, fields) {
		QCoreApplication::processEvents();

		if(progress->wasCanceled()) {
			temp.remove();
			restoreFieldHeaders(oldFields);
			archive->setHeader(oldValues);
			return false;
		}

//		if(!field->isOpen()) {
//			openField(field);
//		}

		FsArchive *fieldHeader = ((FieldPC *)field)->getArchiveHeader();
		if(fieldHeader != NULL) {
			oldFields.insert(field, fieldHeader->getHeader());
		}

		/* Save Field data */
		file = ((FieldPC *)field)->path();
		fs_data = archive->fileData(file);
		QByteArray fl_data, fi_data;
		((FieldPC *)field)->optimize(fs_data, fl_data, fi_data);

		/* FS */
		pos = temp.pos();
		archive->setFileData(file, fs_data);
//		qDebug() << "save" << pos << file;
		archive->setFilePosition(file, pos);
		temp.write(fs_data);
		toc.removeOne(file);

		/* FL */
		file.chop(1);
		file.append('l');
		pos = temp.pos();
		archive->setFileData(file, fl_data);
//		qDebug() << "save" << pos << file;
		archive->setFilePosition(file, pos);
		temp.write(fl_data);
		toc.removeOne(file);

		/* FI */
		file.chop(1);
		file.append('i');
		pos = temp.pos();
		archive->setFileData(file, fi_data);
//		qDebug() << "save" << pos << file;
		archive->setFilePosition(file, pos);
		temp.write(fi_data);
		toc.removeOne(file);

		progress->setValue(pos);
	}

	foreach(const QString &entry, toc) {
		QCoreApplication::processEvents();

		if(progress->wasCanceled()) {
			temp.remove();
			restoreFieldHeaders(oldFields);
			archive->setHeader(oldValues);
			return false;
		}

		pos = temp.pos();
		temp.write(archive->fileData(entry, false));
		archive->setFilePosition(entry, pos);

		progress->setValue(pos);
	}

	progress->setEnabled(false);
	if(progress->wasCanceled()) {
		temp.remove();
		restoreFieldHeaders(oldFields);
		archive->setHeader(oldValues);
		return false;
	}

	if(!archive->saveAs(temp_path)) {
		qWarning() << "Error save header!!!";
		QFile::remove(temp_path%"i");
		QFile::remove(temp_path%"l");
		temp.remove();
		restoreFieldHeaders(oldFields);
		archive->setHeader(oldValues);
		return false;
	}

	int replaceError = archive->replaceArchive(&temp);
	if(replaceError==1) {
		QFile::remove(temp_path%"i");
		QFile::remove(temp_path%"l");
		temp.remove();
		restoreFieldHeaders(oldFields);
		archive->setHeader(oldValues);
		return false;
	} else if(replaceError!=0) {
		return false;
	}

	archive->rebuildInfos();

	qDebug() << "save time" << t.elapsed();

	return true;
}
