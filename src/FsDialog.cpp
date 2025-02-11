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
#include "FsDialog.h"
#include "ProgressWidget.h"
#include "Config.h"
#include "FF8Image.h"
#include "files/BackgroundFile.h"
#include "files/TdwFile.h"
#include "files/TexFile.h"
#include "FsPreviewWidget.h"
#include "FsWidget.h"

FsDialog::FsDialog(FsArchive *fsArchive, QWidget *parent) :
    QWidget(parent), fsArchive(fsArchive), currentImage(0), currentPal(0)
{
	setMinimumSize(800, 528);

	toolBar = new QToolBar(this);
	extractAction = toolBar->addAction(tr("Extract"), this, SLOT(extract()));
	extractAction->setShortcut(QKeySequence(tr("Ctrl+E", "Extract")));
	replaceAction = toolBar->addAction(tr("Replace"), this, SLOT(replace()));
	replaceAction->setShortcut(QKeySequence(tr("Ctrl+R", "Replace")));
	_addFileAction = toolBar->addAction(tr("Add File"), this, SLOT(addFile()));
	_addFileAction->setShortcut(QKeySequence::New);
	_addDirAction = toolBar->addAction(tr("Add Directory"), this, SLOT(addDirectory()));
	removeAction = toolBar->addAction(tr("Remove"), this, SLOT(remove()));
	removeAction->setShortcut(QKeySequence::Delete);
	renameAction = toolBar->addAction(tr("Rename"), this, SLOT(rename()));
	renameAction->setShortcut(QKeySequence(tr("F2", "Rename")));
	toolBar->setWindowFlags(Qt::Widget);

	up = new QPushButton(QApplication::style()->standardIcon(QStyle::SP_FileDialogToParent), tr("Parent"), this);
	up->setAutoDefault(false);
	pathWidget = new QLineEdit(this);

	QHBoxLayout *pathLayout = new QHBoxLayout();
	pathLayout->addWidget(up);
	pathLayout->addWidget(pathWidget, 1);

	list = new FsWidget(this);
	preview = new FsPreviewWidget(this);

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(toolBar, 0, 0, 1, 2);
	layout->addLayout(pathLayout, 1, 0, 1, 2);
	layout->addWidget(list, 2, 0);
	layout->addWidget(preview, 2, 1);
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);

	connect(list, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), SLOT(doubleClicked(QTreeWidgetItem*)));
	connect(list, SIGNAL(itemChanged(QTreeWidgetItem*,int)), SLOT(renameOK(QTreeWidgetItem*,int)));
	connect(list, SIGNAL(fileDropped(QStringList)), SLOT(addFile(QStringList)));
	connect(list, SIGNAL(itemSelectionChanged()), SLOT(changePreview()));
	connect(up, SIGNAL(released()), SLOT(parentDir()));
	connect(pathWidget, SIGNAL(returnPressed()), SLOT(openDir()));
	connect(preview, SIGNAL(currentImageChanged(int)), SLOT(changeImageInPreview(int)));
	connect(preview, SIGNAL(currentPaletteChanged(int)), SLOT(changeImagePaletteInPreview(int)));
	connect(preview, SIGNAL(exportAllClicked()), SLOT(exportImages()));

	if (fsArchive != nullptr)
		openDir(fsArchive->mostCommonPrefixPath());
	else
		openDir(QString());
	setButtonsEnabled();
}

FsDialog::~FsDialog()
{
}

const QString &FsDialog::getCurrentPath() const
{
	return currentPath;
}

void FsDialog::setCurrentPath(const QString &path)
{
	openDir(path);
}

void FsDialog::setButtonsEnabled()
{
	QList<QTreeWidgetItem *> selectedItems = list->selectedItems();
	if (selectedItems.isEmpty())
	{
		extractAction->setEnabled(false);
		replaceAction->setEnabled(false);
		_addFileAction->setEnabled(true);
		_addDirAction->setEnabled(true);
		removeAction->setEnabled(false);
		renameAction->setEnabled(false);
	}
	else if (selectedItems.size() == 1)
	{
		extractAction->setEnabled(true);
		replaceAction->setEnabled(true);
		_addFileAction->setEnabled(true);
		_addDirAction->setEnabled(true);
		removeAction->setEnabled(true);
		renameAction->setEnabled(true);
	}
	else
	{
		extractAction->setEnabled(true);
		replaceAction->setEnabled(false);
		_addFileAction->setEnabled(true);
		_addDirAction->setEnabled(true);
		removeAction->setEnabled(true);
		renameAction->setEnabled(false);
	}

	if (fsArchive && !fsArchive->isWritable()) {
		replaceAction->setEnabled(false);
		_addFileAction->setEnabled(false);
		_addDirAction->setEnabled(false);
		removeAction->setEnabled(false);
		renameAction->setEnabled(false);
	}
}

void FsDialog::changePreview()
{
	currentImage = 0;
	currentPal = 0;
	setButtonsEnabled();
	generatePreview();
}

void FsDialog::generatePreview()
{
	QList<QTreeWidgetItem *> items = list->selectedItems();
	QString fileName, fileType, filePath;

	if (items.isEmpty()) {
		preview->clearPreview();
		return;
	}

	fileName = items.first()->text(0);
	fileType = fileName.mid(fileName.lastIndexOf('.')+1).toLower();
	filePath = currentPath % fileName;
	QByteArray data = fsArchive->fileData(filePath);

	if (fileType == "lzs")
	{
		preview->imagePreview(FF8Image::lzs(data), fileName);
	}
	else if (fileType == "tex")
	{
		TexFile texFile(data);
		texFile.setCurrentColorTable(currentPal);
		preview->imagePreview(QPixmap::fromImage(texFile.image()), fileName, currentPal, texFile.colorTableCount());
	}
	else if (fileType == "tdw")
	{
		preview->imagePreview(QPixmap::fromImage(TdwFile::image(data, TdwFile::Color(currentPal))), fileName, currentPal, 8);
	}
	else if (fileType == "map")
	{
		QString filePathWithoutExt = filePath.left(filePath.size()-3);
		BackgroundFile backgroundFile;
		backgroundFile.open(data, fsArchive->fileData(filePathWithoutExt+"mim"));
		preview->imagePreview(QPixmap::fromImage(backgroundFile.background()), fileName);
	}
	else if (fileType == "mim")
	{
		QString filePathWithoutExt = filePath.left(filePath.size()-3);
		BackgroundFile backgroundFile;
		backgroundFile.open(fsArchive->fileData(filePathWithoutExt+"map"), data);
		preview->imagePreview(QPixmap::fromImage(BackgroundFile::mimToImage(BackgroundFile::DepthColor)), fileName);
	}
	else if (fileType == "cnf")
	{
		QStringDecoder decoder("Shift-JIS");
		if (decoder.isValid())
			preview->textPreview(decoder.decode(data));
		else
			preview->textPreview(QString(data));
	}
	else if (fileType == "h" || fileType == "c"
		 || fileType == "sym" || fileType.isEmpty()
		 || fileType == "bak" || fileType == "dir"
	     || fileType == "fl" || fileType == "txt")
	{
		preview->textPreview(QString(data));
	}
	else if (fileType == "png" || fileType == "jpg" || fileType == "jpeg")
	{
		preview->imagePreview(QPixmap::fromImage(QImage::fromData(data)), fileName);
	}
	else
	{
		QList<int> indexes = FF8Image::findTims(data);
		if (!indexes.isEmpty())
		{
			TimFile timFile(data.mid(indexes.value(currentImage, indexes.first())));
			timFile.setCurrentColorTable(currentPal);
			preview->imagePreview(QPixmap::fromImage(timFile.image()), fileName,
			                      currentPal, timFile.colorTableCount(),
			                      currentImage, indexes.size());
		}
		else
		{
			preview->clearPreview();
		}
	}
}

void FsDialog::exportImages()
{
	QList<QTreeWidgetItem *> items = list->selectedItems();

	if (items.isEmpty()) {
		return;
	}
	
	QString fileName = items.first()->text(0),
	    fileType = fileName.mid(fileName.lastIndexOf('.')+1).toLower(),
	    filePath = currentPath % fileName;
	QByteArray data = fsArchive->fileData(filePath);
	
	QString path = QFileDialog::getExistingDirectory(this, tr("Target directory"));
	
	if (path.isNull()) {
		return;
	}
	
	if (fileType == "tex")
	{
		TexFile texFile(data);
		for (int i = 0; i < texFile.colorTableCount(); ++i) {
			texFile.setCurrentColorTable(i);
			texFile.image().save(QString("%1/%2-%3.png").arg(path, fileName).arg(i));
		}
	}
	else if (fileType == "tdw")
	{
		for (int i = 0; i < 8; ++i) {
			TdwFile::image(data, TdwFile::Color(i)).save(QString("%1/%2-%3.png").arg(path, fileName).arg(i));
		}
	}
	else
	{
		QList<int> indexes = FF8Image::findTims(data);
		for (int j = 0; j < indexes.size(); ++j) {
			TimFile timFile(data.mid(indexes.at(j)));
			for (int i = 0; i < timFile.colorTableCount(); ++i) {
				timFile.setCurrentColorTable(i);
				timFile.image().save(QString("%1/%2-%3-%4.png").arg(path, fileName).arg(j).arg(i));
			}
		}
	}
	
}

void FsDialog::changeImageInPreview(int imageID)
{
	if (imageID < 0) {
		return;
	}

	currentImage = imageID;
	generatePreview();
}

void FsDialog::changeImagePaletteInPreview(int palID)
{
	if (palID < 0) {
		return;
	}

	currentPal = palID;
	generatePreview();
}

void FsDialog::openDir(const QString &name)
{
	if (fsArchive==nullptr)	return;

	QMap<QString, FsHeader *> files = fsArchive->fileList(name);
	QList<QTreeWidgetItem *> items;
	TreeWidgetItem *item;

//	if (files.isEmpty()) {
//		QMessageBox::warning(this, tr("Erreur"), tr("Emplacement inexistant."));
//		pathWidget->setText(currentPath);
//		return;
//	}

	list->clear();
	currentPath = FsArchive::cleanPath(name);
	pathWidget->setText(currentPath);
	up->setDisabled(currentPath.isEmpty());

	QMapIterator<QString, FsHeader *> i(files);
	while (i.hasNext()) {
		i.next();
		QString file = i.key();
		FsHeader *header = i.value();

		if (header == nullptr) {
			item = new TreeWidgetItem(QStringList() << file);
			item->setData(0, FILE_TYPE_ROLE, DIRECTORY);
			item->setData(0, FILE_NAME_ROLE, file);
			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			item->setIcon(0, FsWidget::getDirIcon());
			items.append(item);
		}
		else {
			QString compression;
			switch (header->compression()) {
			case FiCompression::CompressionLz4:
				compression = tr("LZ4");
				break;
			case FiCompression::CompressionLzs:
				compression = tr("LZS");
				break;
			case FiCompression::CompressionUnknown:
				compression = tr("Unknown");
				break;
			case FiCompression::CompressionNone:
				compression = tr("No");
				break;
			}
			item = new TreeWidgetItem(QStringList() << file << QString::number(header->uncompressedSize()) << compression);
			item->setData(0, FILE_TYPE_ROLE, FILE);
			item->setData(0, FILE_NAME_ROLE, file);
			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
			item->setIcon(0, FsWidget::getFileIcon());
			item->setTextAlignment(1, Qt::AlignRight | Qt::AlignVCenter);
			items.append(item);
		}
	}

	list->addTopLevelItems(items);
	QCompleter *completer = new QCompleter(fsArchive->dirs());
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	pathWidget->setCompleter(completer);

	for (int j = 0; j < list->topLevelItemCount(); ++j) {
		QCoreApplication::processEvents();
		QTreeWidgetItem *item = list->topLevelItem(j);
		if (item == nullptr)	break;

		if (item->data(0, FILE_TYPE_ROLE).toInt()==FILE) {
			item->setIcon(0, list->getFileIcon(item->text(0)));
		}
	}
}

void FsDialog::openDir()
{
	openDir(pathWidget->text());
}

void FsDialog::doubleClicked(QTreeWidgetItem *item)
{
	if (item->data(0, FILE_TYPE_ROLE).toInt() == DIRECTORY)
		openDir(currentPath + item->text(0));
	else
		extract();
}

void FsDialog::parentDir()
{
	int index = currentPath.lastIndexOf('\\', -2);
	if (index==-1)
		openDir(QString());
	else
		openDir(currentPath.left(index));
}

QStringList FsDialog::listFilesInDir(QString dirPath)
{
	dirPath = FsArchive::cleanPath(dirPath);
	QStringList filesInDir;
	QMap<QString, FsHeader *> files = fsArchive->fileList(dirPath);
	QMapIterator<QString, FsHeader *> i(files);
	while (i.hasNext()) {
		i.next();
		QString file = i.key();
		FsHeader *header = i.value();

		if (header == nullptr) {
			filesInDir.append(listFilesInDir(dirPath + file));
		}
		else {
			filesInDir.append(dirPath + file);
		}
	}
	return filesInDir;
}

void FsDialog::extract(QStringList sources)
{
	QString destination, source;

	if (sources.isEmpty())
	{
		QList<QTreeWidgetItem *> items = list->selectedItems();
		if (items.isEmpty())			return;

		for (QTreeWidgetItem *item: items) {
			source = currentPath % item->text(0);
			if (item->data(0, FILE_TYPE_ROLE).toInt() == FILE) {
				sources.append(source);
			} else {
				sources.append(listFilesInDir(source));
			}
		}

		if (sources.isEmpty())	return;
	}

	if (sources.size()==1)
	{
		destination = QDir::cleanPath(Config::value("extractPath").toString()) % "/" % sources.first().mid(currentPath.size());
		destination = QFileDialog::getSaveFileName(this, tr("Extract"), destination);
	}
	else
	{
		destination = QFileDialog::getExistingDirectory(this, tr("Extract"), QDir::cleanPath(Config::value("extractPath").toString()));
	}

	if (destination.isEmpty())	return;

	if (sources.size()==1)
	{
		if (!fsArchive->extractFile(sources.first(), destination))
			QMessageBox::warning(this, tr("Error"), tr("The file was not extracted!"));

		Config::setValue("extractPath", destination.left(destination.lastIndexOf('/')));
	}
	else
	{
		ProgressWidget progress(tr("Extract..."), ProgressWidget::Cancel, this);

		if (fsArchive->extractFiles(sources, currentPath, destination, &progress) != FsArchive::Ok)
			QMessageBox::warning(this, tr("Error"), tr("The files were not extracted!"));

		Config::setValue("extractPath", destination);
	}
}

void FsDialog::replace(QString source, QString destination)
{
	QTreeWidgetItem *item = list->currentItem();

	if (item == nullptr)	return;

	bool isDir = item->data(0, FILE_TYPE_ROLE).toInt() == DIRECTORY;

	if (destination.isEmpty()) {
		destination = currentPath % item->text(0);
	}

	if (source.isEmpty()) {
		source = QDir::cleanPath(Config::value("replacePath").toString()) % "/" % item->text(0);
		if (isDir) {
			if (!QFile::exists(source)) {
				source = QDir::cleanPath(Config::value("replacePath").toString());
			}
			source = QFileDialog::getExistingDirectory(this, tr("Replace"), source);
		} else {
			source = QFileDialog::getOpenFileName(this, tr("Replace"), source);
		}
		if (source.isEmpty())	return;
	}

	ProgressWidget progress(tr("Replace..."), ProgressWidget::Cancel, this);
	FsArchive::Error error;

	if (isDir) {
		QList<FsArchive::Error> errors = fsArchive->replaceDir(source, destination, FiCompression::CompressionLzs, &progress);
		if (errors.isEmpty()) {
			error = FsArchive::Ok;
		} else {
			error = errors.first(); // FIXME: other errors?
		}
	} else {
		error = fsArchive->replaceFile(source, destination, &progress);
	}

	if (error != FsArchive::Ok) {
		QMessageBox::warning(this, tr("Replacement error"), FsArchive::errorString(error));
	} else {
		openDir(currentPath);
	}

	Config::setValue("replacePath", source.left(source.lastIndexOf('/')));
}

void FsDialog::addFile(QStringList sources)
{
	if (sources.isEmpty()) {
		sources = QFileDialog::getOpenFileNames(this, tr("Add"), Config::value("addPath").toString());
		if (sources.isEmpty())	return;
	}

	add(sources);
}

void FsDialog::addDirectory(QString source)
{
	if (source.isEmpty()) {
		source = QFileDialog::getExistingDirectory(this, tr("Add"), Config::value("addPath").toString());
		if (source.isEmpty())	return;
	}

	add(QStringList(source), true);
}

void FsDialog::add(QStringList sources, bool fromDir)
{
	QStringList destinations;
	QString source, destination;
	FiCompression compress;
	int i, nbFiles;

	nbFiles = sources.size();
	for (i = 0; i < nbFiles; ++i) {
		source = QDir::cleanPath(sources.at(i));
		sources.replace(i, source);
		destinations.append(currentPath % source.mid(source.lastIndexOf('/')+1));
	}

	compress = compressionMessage(this);
	if (compress == CompressionUnknown) {
		return;
	}

	ProgressWidget progress(tr("Add..."), ProgressWidget::Stop, this);
	QList<FsArchive::Error> errors;

	if (fromDir) {
		errors = fsArchive->appendDir(sources.first(), destinations.first(), compress, &progress);
	} else {
		errors = fsArchive->appendFiles(sources, destinations, compress, &progress);
	}

	if (errors.contains(FsArchive::NonWritable)) {
		QMessageBox::warning(this, tr("Add error"), FsArchive::errorString(FsArchive::NonWritable));
	}
	QStringList errorOut;
	for (i = 0; i < errors.size() && errorOut.size() < 20; ++i) {
		if (errors.at(i) == FsArchive::FileExists) {
			if (QMessageBox::question(this, tr("The file already exists"), tr("The file already exists, do you want to replace it?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
				replace(sources.at(i), destinations.at(i));
				continue;
			}
		}
		if (errors.at(i) != FsArchive::Ok)
			errorOut.append(FsArchive::errorString(errors.at(i), sources.at(i)));
	}
	if (!errorOut.isEmpty()) {
		QMessageBox::warning(this, tr("Add error"), tr("There was a problem for one or more files to add:\n - %1").arg(errorOut.join("\n - ")));
	}

	if (errors.contains(FsArchive::Ok)) {
		openDir(currentPath);
	}

	destination = destinations.first();
	Config::setValue("addPath", destination.left(destination.lastIndexOf('/')));
}

void FsDialog::remove(QStringList destinations)
{
	if (destinations.isEmpty()) {
		QList<QTreeWidgetItem *> items = list->selectedItems();
		if (items.isEmpty())			return;

		for (QTreeWidgetItem *item: items) {
			if (item->data(0, FILE_TYPE_ROLE).toInt() == FILE) {
				destinations.append(currentPath % item->text(0));
			}
			else {
				destinations.append(fsArchive->tocInDirectory(currentPath % item->text(0)));
			}
		}

		if (destinations.isEmpty())	return;
	}

	if (QMessageBox::question(this, tr("Remove"), tr("Do you want to delete the selected items?"), QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
		return;

	ProgressWidget progress(tr("Deleting..."), ProgressWidget::Cancel, this);

	FsArchive::Error error = fsArchive->remove(destinations, &progress);

	if (error != FsArchive::Ok) {
		QMessageBox::warning(this, tr("Deleting error"), FsArchive::errorString(error));
	} else {
		openDir(currentPath);
	}
}

void FsDialog::rename()
{
	QTreeWidgetItem *item = list->currentItem();
	if (item==nullptr)	return;

	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEditable);
	list->editItem(item, 0);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
}

void FsDialog::renameOK(QTreeWidgetItem *item, int column)
{
	if (column != 0)	return;

	list->blockSignals(true);

	QString oldName = item->data(0, FILE_NAME_ROLE).toString(), newName = item->text(0);
	QString destination = currentPath % oldName, newDestination = currentPath % newName;
	QStringList destinations, newDestinations;

	if (destination.compare(newDestination, Qt::CaseInsensitive) == 0) {
		list->blockSignals(false);
		return;
	}

	if (newName.contains('\\') || newName.contains('/') || newName.contains('\n') || newName.contains('\r')) {
		QMessageBox::warning(this, tr("Rename error"), tr("Illegal characters used (eg '\\' or '/')"));
		item->setText(0, oldName);
		list->blockSignals(false);
		return;
	}

//	qDebug() << destination << newDestination;

	if (item->data(0, FILE_TYPE_ROLE).toInt() == FILE) {
		item->setIcon(0, list->getFileIcon(item->text(0)));
		destinations.append(destination);
		newDestinations.append(newDestination);
	}
	else {
		destinations.append(fsArchive->tocInDirectory(destination));
		for (const QString &path: destinations) {
			newDestinations.append(newDestination % path.mid(destination.size()));
		}
	}
	item->setData(0, FILE_NAME_ROLE, newName);

	FsArchive::Error error = fsArchive->rename(destinations, newDestinations);

	if (error != FsArchive::Ok) {
		QMessageBox::warning(this, tr("Rename error"), FsArchive::errorString(error));
	} else {
		list->scrollToItem(item);
	}
	list->blockSignals(false);
}

FiCompression FsDialog::compressionMessage(QWidget *parent)
{
	QMessageBox msgBox(QMessageBox::Information,
	                   tr("Compression"),
	                   tr("Would you compress file(s)?"),
	                   QMessageBox::NoButton, parent);
	QPushButton *lzs = msgBox.addButton(tr("LZS"), QMessageBox::YesRole);
	QPushButton *lz4 = msgBox.addButton(tr("LZ4 (FF8 Remaster)"), QMessageBox::YesRole);
	QPushButton *no = msgBox.addButton(tr("No"), QMessageBox::NoRole);
	QPushButton *cancel = msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);

	msgBox.setDefaultButton(lzs);
	msgBox.setEscapeButton(cancel);

	if (msgBox.exec() == -1) {
		return CompressionUnknown;
	}
	if (msgBox.clickedButton() == lzs) {
		return CompressionLzs;
	}
	if (msgBox.clickedButton() == lz4) {
		return CompressionLz4;
	}
	if (msgBox.clickedButton() == no) {
		return CompressionNone;
	}

	return CompressionUnknown;
}
