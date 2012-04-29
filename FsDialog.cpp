#include "FsDialog.h"

FsDialog::FsDialog(FsArchive *fsArchive, QWidget *parent) :
	QWidget(parent), fsArchive(fsArchive)
{
	setMinimumSize(800, 528);

	toolBar = new QToolBar(this);
	extractAction = toolBar->addAction(tr("Extraire"), this, SLOT(extract()));
	extractAction->setShortcut(QKeySequence(tr("Ctrl+E", "Extract")));
	replaceAction = toolBar->addAction(tr("Remplacer"), this, SLOT(replace()));
	replaceAction->setShortcut(QKeySequence(tr("Ctrl+R", "Replace")));
	_addAction = toolBar->addAction(tr("Ajouter"), this, SLOT(add()));
	_addAction->setShortcut(QKeySequence::New);
	removeAction = toolBar->addAction(tr("Supprimer"), this, SLOT(remove()));
	removeAction->setShortcut(QKeySequence::Delete);
	renameAction = toolBar->addAction(tr("Renommer"), this, SLOT(rename()));
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
	connect(list, SIGNAL(fileDropped(QStringList)), SLOT(add(QStringList)));
	connect(list, SIGNAL(itemSelectionChanged()), SLOT(setButtonsEnabled()));
	connect(list, SIGNAL(itemSelectionChanged()), SLOT(generatePreview()));
	connect(up, SIGNAL(released()), SLOT(parentDir()));
	connect(pathWidget, SIGNAL(returnPressed()), SLOT(openDir()));

	if(fsArchive!=NULL && fsArchive->dirExists("c:\\ff8\\data\\"))
		openDir("c:\\ff8\\data\\");
	else
		openDir(QString());
	setButtonsEnabled();
}

FsDialog::~FsDialog()
{
}

void FsDialog::setButtonsEnabled()
{
	QList<QTreeWidgetItem *> selectedItems = list->selectedItems();
	if(selectedItems.isEmpty())
	{
		extractAction->setEnabled(false);
		replaceAction->setEnabled(false);
		_addAction->setEnabled(true);
		removeAction->setEnabled(false);
		renameAction->setEnabled(false);
	}
	else if(selectedItems.size() == 1)
	{
		extractAction->setEnabled(true);
		replaceAction->setEnabled(true);
		_addAction->setEnabled(true);
		removeAction->setEnabled(true);
		renameAction->setEnabled(true);
	}
	else
	{
		extractAction->setEnabled(true);
		replaceAction->setEnabled(false);
		_addAction->setEnabled(true);
		removeAction->setEnabled(true);
		renameAction->setEnabled(false);
	}

	if(fsArchive && !fsArchive->isWritable()) {
		replaceAction->setEnabled(false);
		_addAction->setEnabled(false);
		removeAction->setEnabled(false);
		renameAction->setEnabled(false);
	}
}

void FsDialog::generatePreview()
{
	QList<QTreeWidgetItem *> items = list->selectedItems();
	QString fileName, fileType, filePath;

	if(items.isEmpty()) {
		preview->clearPreview();
		return;
	}

	fileName = items.first()->text(0);
	fileType = fileName.mid(fileName.lastIndexOf('.')+1).toLower();
	filePath = currentPath % fileName;
	QByteArray data = fsArchive->fileData(filePath);

	if(fileType == "lzs")
	{
		preview->imagePreview(FF8Image::lzs(data), fileName);
	}
	else if(fileType == "tex")
	{
		preview->imagePreview(FF8Image::tex(data), fileName);
	}
	else if(fileType == "tim")
	{
		preview->imagePreview(FF8Image::tim(data), fileName);
	}
	else if(fileType == "tdw")
	{
		preview->imagePreview(FF8Image::tdw(data), fileName);
	}
	else if(fileType == "map" || fileType == "mim")
	{
		QString filePathWithoutExt = filePath.left(filePath.size()-3);
		preview->imagePreview(FF8Image::background(fileType == "map" ? data : fsArchive->fileData(filePathWithoutExt+"map"),
											  fileType == "mim" ? data : fsArchive->fileData(filePathWithoutExt+"mim")), fileName);
	}
	else if(fileType == "cnf")
	{
		QTextCodec *codec = QTextCodec::codecForName("Shift-JIS");
		if(codec)
			preview->textPreview(codec->toUnicode(data));
		else
			preview->textPreview(QString(data));
	}
	else if(fileType == "h" || fileType == "c"
		 || fileType == "sym" || fileType.isEmpty()
		 || fileType == "bak" || fileType == "dir"
		 || fileType == "fl")
	{
		preview->textPreview(QString(data));
	}
	else
	{
		int index;
		if((index = FF8Image::findFirstTim(data)) != -1)
		{
			preview->imagePreview(FF8Image::tim(data.mid(index)), fileName);
		}
		else
		{
			preview->clearPreview();
		}
	}
}

void FsDialog::openDir(const QString &name)
{
	if(fsArchive==NULL)	return;

	QMap<QString, FsHeader *> files = fsArchive->fileList(name);
	QList<QTreeWidgetItem *> items;
	TreeWidgetItem *item;

//	if(files.isEmpty()) {
//		QMessageBox::warning(this, tr("Erreur"), tr("Emplacement inexistant."));
//		pathWidget->setText(currentPath);
//		return;
//	}

	list->clear();
	currentPath = FsArchive::cleanPath(name);
	pathWidget->setText(currentPath);
	up->setDisabled(currentPath.isEmpty());

	QMapIterator<QString, FsHeader *> i(files);
	while(i.hasNext()) {
		i.next();
		QString file = i.key();
		FsHeader *header = i.value();

		if(header == NULL) {
			item = new TreeWidgetItem(QStringList() << file);
			item->setData(0, FILE_TYPE_ROLE, DIR);
			item->setData(0, FILE_NAME_ROLE, file);
			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			item->setIcon(0, FsWidget::getDirIcon());
			items.append(item);
		}
		else {
			item = new TreeWidgetItem(QStringList() << file << QString::number(header->uncompressed_size()) << (header->isCompressed() ? tr("Oui") : tr("Non")));
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

	for(int j=0 ; j<list->topLevelItemCount() ; ++j) {
		QCoreApplication::processEvents();
		QTreeWidgetItem *item = list->topLevelItem(j);
		if(item == NULL)	break;

		if(item->data(0, FILE_TYPE_ROLE).toInt()==FILE) {
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
	if(item->data(0, FILE_TYPE_ROLE).toInt() == DIR)
		openDir(currentPath + item->text(0));
	else
		extract();
}

void FsDialog::parentDir()
{
	int index = currentPath.lastIndexOf('\\', -2);
	if(index==-1)
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
	while(i.hasNext()) {
		i.next();
		QString file = i.key();
		FsHeader *header = i.value();

		if(header == NULL) {
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

	if(sources.isEmpty())
	{
		QList<QTreeWidgetItem *> items = list->selectedItems();
		if(items.isEmpty())			return;

		foreach(QTreeWidgetItem *item, items) {
			source = currentPath % item->text(0);
			if(item->data(0, FILE_TYPE_ROLE).toInt() == FILE) {
				sources.append(source);
			} else {
				sources.append(listFilesInDir(source));
			}
		}

		if(sources.isEmpty())	return;
	}

	if(sources.size()==1)
	{
		destination = QDir::cleanPath(Config::value("extractPath").toString()) % "/" % sources.first().mid(currentPath.size());
		destination = QFileDialog::getSaveFileName(this, tr("Extraire"), destination);
	}
	else
	{
		destination = QFileDialog::getExistingDirectory(this, tr("Extraire"), QDir::cleanPath(Config::value("extractPath").toString()));
	}

	if(destination.isEmpty())	return;

	if(sources.size()==1)
	{
		if(!fsArchive->extractFile(sources.first(), destination))
			QMessageBox::warning(this, tr("Erreur"), tr("Le fichier n'a pas été extrait !"));

		Config::setValue("extractPath", destination.left(destination.lastIndexOf('/')));
	}
	else
	{
		QProgressDialog progress(tr("Extraction..."), tr("Annuler"), 0, 0, this, Qt::Dialog | Qt::WindowCloseButtonHint);
		progress.setWindowModality(Qt::WindowModal);
		progress.show();

		if(fsArchive->extractFiles(sources, currentPath, destination, &progress) != FsArchive::Ok)
			QMessageBox::warning(this, tr("Erreur"), tr("Les fichiers n'ont pas été extraits !"));

		Config::setValue("extractPath", destination);
	}
}

void FsDialog::replace(QString source, QString destination)
{
//	bool compress;
	QTreeWidgetItem *item = list->currentItem();

	if(item==NULL || item->data(0, FILE_TYPE_ROLE).toInt() == DIR)	return;

	if(destination.isEmpty())
		destination = currentPath % item->text(0);

	if(source.isEmpty()) {
		source = QDir::cleanPath(Config::value("replacePath").toString()) % "/" % item->text(0);
		source = QFileDialog::getOpenFileName(this, tr("Remplacer"), source);
		if(source.isEmpty())	return;
	}

	QProgressDialog progress(tr("Remplacement..."), tr("Annuler"), 0, 0, this, Qt::Dialog | Qt::WindowCloseButtonHint);
	progress.setWindowModality(Qt::WindowModal);
	progress.show();

//	compress = fsArchive->fileIsCompressed(destination) && QMessageBox::question(this, tr("Compression"), tr("Voulez-vous compresser le fichier ?"), tr("Oui"), tr("Non")) == 0;

	FsArchive::Error error = fsArchive->replace(source, destination, &progress);

	if(error != FsArchive::Ok) {
		QMessageBox::warning(this, tr("Erreur de remplacement"), FsArchive::errorString(error));
	} else {
		openDir(currentPath);
	}

	Config::setValue("replacePath", source.left(source.lastIndexOf('/')));
}

void FsDialog::add(QStringList sources)
{
	QStringList destinations;
	QString source, destination;
	bool compress;
	int i, nbFiles;

	if(sources.isEmpty()) {
		sources = QFileDialog::getOpenFileNames(this, tr("Ajouter"), Config::value("addPath").toString());
		if(sources.isEmpty())	return;
	}

	nbFiles = sources.size();
	for(i=0 ; i<nbFiles ; ++i) {
		source = QDir::cleanPath(sources.at(i));
		sources.replace(i, source);
		destinations.append(currentPath % source.mid(source.lastIndexOf('/')+1));
	}

	compress = QMessageBox::question(this, tr("Compression"), tr("Voulez-vous compresser le fichier ?"), tr("Oui"), tr("Non")) == 0;

	QProgressDialog progress(tr("Ajout..."), tr("Arrêter"), 0, 0, this, Qt::Dialog | Qt::WindowCloseButtonHint);
	progress.setWindowModality(Qt::WindowModal);
	progress.show();

	QList<FsArchive::Error> errors = fsArchive->append(sources, destinations, compress, &progress);

	if(errors.contains(FsArchive::NonWritable)) {
		QMessageBox::warning(this, tr("Erreur d'ajout"), FsArchive::errorString(FsArchive::NonWritable));
	}
	QStringList errorOut;
	for(i=0 ; i<errors.size() && errorOut.size() < 20 ; ++i) {
		if(errors.at(i) == FsArchive::FileExists) {
			if(QMessageBox::question(this, tr("Le fichier existe déjà"), tr("Le fichier existe déjà, voulez-vous le remplacer ?"), tr("Oui"), tr("Non")) == 0) {
				replace(sources.at(i), destinations.at(i));
				continue;
			}
		}
		if(errors.at(i) != FsArchive::Ok)
			errorOut.append(FsArchive::errorString(errors.at(i), sources.at(i)));
	}
	if(!errorOut.isEmpty()) {
		QMessageBox::warning(this, tr("Erreur d'ajout"), tr("Un problème est survenu pour un ou plusieurs des fichiers à ajouter :\n - %1").arg(errorOut.join("\n - ")));
	}

	if(errors.contains(FsArchive::Ok)) {
		openDir(currentPath);
	}

	destination = destinations.first();
	Config::setValue("addPath", destination.left(destination.lastIndexOf('/')));
}

void FsDialog::remove(QStringList destinations)
{
	if(destinations.isEmpty()) {
		QList<QTreeWidgetItem *> items = list->selectedItems();
		if(items.isEmpty())			return;

		foreach(QTreeWidgetItem *item, items) {
			if(item->data(0, FILE_TYPE_ROLE).toInt() == FILE) {
				destinations.append(currentPath % item->text(0));
			}
			else {
				destinations.append(fsArchive->tocInDirectory(currentPath % item->text(0)));
			}
		}

		if(destinations.isEmpty())	return;
	}

	if(QMessageBox::question(this, tr("Supprimer"), tr("Voulez-vous supprimer les éléments sélectionnés ?"), tr("Oui"), tr("Non")) != 0)
		return;

	QProgressDialog progress(tr("Suppression..."), tr("Annuler"), 0, 0, this, Qt::Dialog | Qt::WindowCloseButtonHint);
	progress.setWindowModality(Qt::WindowModal);
	progress.show();

	FsArchive::Error error = fsArchive->remove(destinations, &progress);

	if(error != FsArchive::Ok) {
		QMessageBox::warning(this, tr("Erreur de suppression"), FsArchive::errorString(error));
	} else {
		openDir(currentPath);
	}
}

void FsDialog::rename()
{
	QTreeWidgetItem *item = list->currentItem();
	if(item==NULL)	return;

	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEditable);
	list->editItem(item, 0);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
}

void FsDialog::renameOK(QTreeWidgetItem *item, int column)
{
	if(column != 0)	return;

	list->blockSignals(true);

	QString oldName = item->data(0, FILE_NAME_ROLE).toString(), newName = item->text(0);
	QString destination = currentPath % oldName, newDestination = currentPath % newName;
	QStringList destinations, newDestinations;

	if(destination.compare(newDestination, Qt::CaseInsensitive) == 0) {
		list->blockSignals(false);
		return;
	}

	if(newName.contains('\\') || newName.contains('/') || newName.contains('\n') || newName.contains('\r')) {
		QMessageBox::warning(this, tr("Erreur de renommage"), tr("Caractères interdits utilisés (par exemple : '\\' ou '/')"));
		item->setText(0, oldName);
		list->blockSignals(false);
		return;
	}

//	qDebug() << destination << newDestination;

	if(item->data(0, FILE_TYPE_ROLE).toInt() == FILE) {
		item->setIcon(0, list->getFileIcon(item->text(0)));
		destinations.append(destination);
		newDestinations.append(newDestination);
	}
	else {
		destinations.append(fsArchive->tocInDirectory(destination));
		foreach(const QString &path, destinations) {
			newDestinations.append(newDestination % path.mid(destination.size()));
		}
	}
	item->setData(0, FILE_NAME_ROLE, newName);

	FsArchive::Error error = fsArchive->rename(destinations, newDestinations);

	if(error != FsArchive::Ok) {
		QMessageBox::warning(this, tr("Erreur de renommage"), FsArchive::errorString(error));
	} else {
		list->scrollToItem(item);
	}
	list->blockSignals(false);
}
