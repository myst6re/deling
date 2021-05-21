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
//#define QT_NO_DEBUG 1
//#define QT_NO_DEBUG_OUTPUT 1
//#define QT_NO_WARNING_OUTPUT 1

#include <QApplication>
#include <QSurfaceFormat>
#include "Config.h"
#include "MainWindow.h"

// Only for static compilation
//Q_IMPORT_PLUGIN(qjpcodecs) // jp encoding

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setWindowIcon(QIcon(":/images/deling.png"));

	QSurfaceFormat glFormat;
#ifdef QT_DEBUG
	glFormat.setVersion(4, 3);
	glFormat.setOption(QSurfaceFormat::DebugContext);
#else
	glFormat.setVersion(3, 2);
#endif
	glFormat.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(glFormat);

	Config::set();

	QString lang = QLocale::system().name().toLower();
	lang = Config::value("lang", lang.left(lang.indexOf("_"))).toString();

	QTranslator qtTranslator;
	if (qtTranslator.load("qt_" + lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
		app.installTranslator(&qtTranslator);
	}

	QTranslator translator;
	if (translator.load("deling_" % lang, Config::programResourceDir())) {
		app.installTranslator(&translator);
		Config::setValue("lang", lang);
	} else{
		Config::setValue("lang", "fr");
	}

	if (!FF8Font::listFonts()) {
		QMessageBox::critical(nullptr, QObject::tr("Chargement des données"),
		                      QObject::tr("Les polices de caractères n'ont pas pu être chargées !"));
		return -1;
	}

	MainWindow *window = new MainWindow();
	window->show();
	if (argc > 1) {
		window->openFile(argv[1]);
	}

	return app.exec();
}
