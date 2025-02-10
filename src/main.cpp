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
//#define QT_NO_DEBUG 1
//#define QT_NO_DEBUG_OUTPUT 1
//#define QT_NO_WARNING_OUTPUT 1

#ifdef DELING_CONSOLE
#include <QCoreApplication>
#include "CLI.h"
#else
#include <QApplication>
#include <QSurfaceFormat>
#include "MainWindow.h"
#endif
#include "Config.h"
#include "FF8Font.h"

// Only for static compilation
//Q_IMPORT_PLUGIN(qjpcodecs) // jp encoding

int main(int argc, char *argv[])
{
#ifdef DELING_CONSOLE
	QCoreApplication app(argc, argv);
	QCoreApplication::setApplicationName(DELING_NAME);
	QCoreApplication::setApplicationVersion(DELING_VERSION);
#ifdef Q_OS_WIN
	// QTextCodec::setCodecForLocale(QTextCodec::codecForName("IBM 850"));
#endif
	Config::set();
	
	if (!FF8Font::listFonts()) {
		qWarning() << "Font could not be loaded!";
		return -1;
	}
	
	CLI::exec();
	
	QTimer::singleShot(0, &app, &QCoreApplication::quit);
#else
	QApplication app(argc, argv);
	app.setWindowIcon(QIcon(":/images/deling.png"));
	
	QSurfaceFormat format;
	format.setOption(QSurfaceFormat::DeprecatedFunctions, false);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setVersion(4, 1);
#ifdef QT_DEBUG
	format.setOption(QSurfaceFormat::DebugContext);
#endif
	QSurfaceFormat::setDefaultFormat(format);

	Config::set();

	QString lang = QLocale::system().name().toLower();
	lang = Config::value("lang", lang.left(lang.indexOf("_"))).toString();

	QTranslator qtTranslator;
	if (qtTranslator.load("qt_" + lang, QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
		app.installTranslator(&qtTranslator);
	}

	QTranslator translator;
	if (translator.load("Deling_" % lang, Config::programLanguagesDir()) || translator.load("Deling_" % lang)) {
		app.installTranslator(&translator);
		Config::setValue("lang", lang);
	} else {
		Config::setValue("lang", "en");
	}

	if (!FF8Font::listFonts()) {
		QMessageBox::critical(nullptr, QObject::tr("Loading data"),
		                      QObject::tr("Fonts could not be loaded!"));
		return -1;
	}

	MainWindow *window = new MainWindow();
	window->show();
	if (argc > 1) {
		window->openFile(argv[1]);
	}
#endif

	return app.exec();
}
