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
#include "ConfigDialog.h"
#include "Config.h"
#include "Data.h"
#include "TdwManagerDialog.h"

ConfigDialog::ConfigDialog(QWidget *parent)
	: QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint)
{
	setWindowTitle(tr("Configuration"));
	setSizeGripEnabled(true);

	QLabel *langLabel = new QLabel(tr("Language"), this);
	langComboBox = new QComboBox(this);

	QLabel *appPathLabel = new QLabel(tr("FF8 path"), this);
	useRegAppPath = new QCheckBox(tr("Auto"), this);
	appPathLine = new QLineEdit(this);
	QPushButton *appPathButton = new QPushButton(tr("Browse..."), this);
	QHBoxLayout *appPathLayout = new QHBoxLayout;
	appPathLayout->addWidget(useRegAppPath);
	appPathLayout->addWidget(appPathLine);
	appPathLayout->addWidget(appPathButton);

	QLabel *encodingLabel = new QLabel(tr("Text encoding"), this);
	encodingComboBox = new QComboBox(this);
	encodingComboBox->addItem(tr("Latin"), "00");
	encodingComboBox->addItem(tr("Japanese"), "01");
	QPushButton *encodingManage = new QPushButton(tr("Manage"), this);
	QHBoxLayout *encodingLayout = new QHBoxLayout();
	encodingLayout->addWidget(encodingComboBox, 1);
	encodingLayout->addWidget(encodingManage);
	encodingLayout->setContentsMargins(QMargins());

//	hideUnusedTexts = new QCheckBox(tr("Cacher les textes inutilisés"), this);

	QPushButton *okButton = new QPushButton(tr("Save"), this);
	okButton->setDefault(true);
	QPushButton *cancelButton = new QPushButton(tr("Cancel"), this);
	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(okButton);
	buttonsLayout->addWidget(cancelButton);
	buttonsLayout->setContentsMargins(QMargins());
	
	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(langLabel, 0, 0);
	layout->addWidget(langComboBox, 0, 1);
	layout->addWidget(appPathLabel, 1, 0);
	layout->addLayout(appPathLayout, 1, 1);
	layout->addWidget(encodingLabel, 2, 0);
	layout->addLayout(encodingLayout, 2, 1);
//	layout->addWidget(hideUnusedTexts, 3, 0, 1, 2);
	layout->addLayout(buttonsLayout, 4, 0, 1, 2, Qt::AlignRight);

	connect(useRegAppPath, SIGNAL(toggled(bool)), appPathLine, SLOT(setDisabled(bool)));
	connect(useRegAppPath, SIGNAL(toggled(bool)), appPathButton, SLOT(setDisabled(bool)));

	fillMenuLang();
	useRegAppPath->setChecked(!Config::value("dontUseRegAppPath").toBool());
	QString appPath = Config::value("appPath").toString();
	if (appPath.isEmpty()) {
		appPath = Data::AppPath();
	}
	QString ff8ExeName = Config::value("ff8ExeName", "FF8.exe").toString();
	appPathLine->setText(appPath + "/" + ff8ExeName);

	for (const QString &fontName: FF8Font::fontList()) {
		if (fontName == "00" || fontName == "01")	continue;
		FF8Font *font = FF8Font::font(fontName);
		if (font) {
			encodingComboBox->addItem(font->name(), fontName);
		}
	}

	int indexOfData;
	indexOfData = encodingComboBox->findData(Config::value("encoding", "00"));
	encodingComboBox->setCurrentIndex(indexOfData != -1 ? indexOfData : 0);
//	hideUnusedTexts->setChecked(Config::value("hideUnusedTexts").toBool());

	connect(encodingManage, SIGNAL(clicked()), SLOT(manageEncoding()));
	connect(appPathButton, SIGNAL(clicked()), SLOT(setAppPath()));
	connect(okButton, SIGNAL(clicked()), SLOT(saveConfig()));
	connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));
}

void ConfigDialog::fillMenuLang()
{
	QDir dir(Config::programLanguagesDir());
	QStringList stringList = dir.entryList(QStringList("Deling_*.qm"), QDir::Files, QDir::Name);
	langComboBox->addItem(QString::fromUtf8("English"));
	langComboBox->setItemData(0, "en");

	QTranslator translator;
	int i=1;
	for (const QString &str: stringList) {
		if (translator.load(dir.filePath(str))) {
			langComboBox->addItem(translator.translate("MainWindow", "English", "Your translation language"));
			langComboBox->setItemData(i++, str.right(5).left(2));
		}
	}

	for (i = 0; i < langComboBox->count(); ++i) {
		if (Config::value("lang")==langComboBox->itemData(i)) {
			langComboBox->setCurrentIndex(i);
			break;
		}
	}
}

void ConfigDialog::setAppPath()
{
	QString appPath = QFileDialog::getOpenFileName(this, tr("Executable path of Final Fantasy VIII PC"), appPathLine->text(), tr("FF8 exe (*.exe);;All files (*)"));
	if (!appPath.isNull()) {
		appPathLine->setText(appPath);
	}
}

void ConfigDialog::manageEncoding()
{
	TdwManagerDialog tdwManager(this);
	tdwManager.exec();
}

void ConfigDialog::restartNow()
{
	QString title, text;
	QTranslator translator;
	if (translator.load(QString("deling_%1")
	                   .arg(Config::value("lang").toString()),
	                   Config::programResourceDir())) {
		title = translator.translate("MainWindow", "Settings changed");
		text = translator.translate("MainWindow", "Restart the program for the settings to take effect.");
	} else {
		title = "Settings changed";
		text = "Restart the program for the settings to take effect.";
	}
	QMessageBox::information(this, title, text);
}

void ConfigDialog::saveConfig()
{
	QString oldLang = Config::value("lang").toString();

	Config::setValue("lang", langComboBox->itemData(langComboBox->currentIndex()));
	Config::setValue("dontUseRegAppPath", !useRegAppPath->isChecked());
	QFileInfo fullFF8ExePath(appPathLine->text());
	Config::setValue("appPath", fullFF8ExePath.absolutePath());
	Config::setValue("ff8ExeName", fullFF8ExePath.fileName());
	Config::setValue("encoding", encodingComboBox->itemData(encodingComboBox->currentIndex()));
//	Config::setValue("hideUnusedTexts", hideUnusedTexts->isChecked());

	if (oldLang != Config::value("lang").toString()) {
		restartNow();
	}

	accept();
}
