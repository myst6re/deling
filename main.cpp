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
#include "Config.h"
#include "MainWindow.h"

#include "game/worldmap/WmsetFile.h"
#include "FsArchive.h"
#include "ProgressWidget.h"
#include "widgets/WorldmapWidget.h"
#include "game/worldmap/WmxFile.h"
#include "game/worldmap/TexlFile.h"
#include "QLZ4.h"

// Only for static compilation
//Q_IMPORT_PLUGIN(qjpcodecs) // jp encoding

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setWindowIcon(QIcon(":/images/deling.png"));
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
#endif
	/* QFile objFile("E:/Documents/Deling-build-desktop/wm/game/wmsetfr.obj");
	WmsetFile obj(&objFile);
	if (objFile.open(QIODevice::ReadOnly)) {
		obj.extract(objFile.fileName(), "E:/Documents/Deling-build-desktop/wm/wmsetfr");
	} */
	/* WmsetFile::build("E:/Documents/Deling-build-desktop/wm/wmsetfr",
					 "E:/Documents/Deling-build-desktop/wm/wmsetfr.modified.obj"); */

	Config::set();

	QString lang = QLocale::system().name().toLower();
	lang = Config::value("lang", lang.left(lang.indexOf("_"))).toString();

	QTranslator qtTranslator;
	if(qtTranslator.load("qt_" + lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
		app.installTranslator(&qtTranslator);

	QTranslator translator;
	if(translator.load("deling_" % lang, Config::programResourceDir())){
		app.installTranslator(&translator);
		Config::setValue("lang", lang);
	} else{
		Config::setValue("lang", "fr");
	}

	if(!FF8Font::listFonts()) {
		QMessageBox::critical(nullptr, QObject::tr("Chargement des données"), QObject::tr("Les polices de caractères n'ont pas pu être chargées !"));
		return -1;
	}

	MainWindow *window = new MainWindow();
	window->show();
	if(argc>1)
		window->openFile(argv[1]);

	/* QFile f1("E:/Documents/hyne/autres/ff8slot00_switch_version.dat");
	f1.open(QIODevice::ReadOnly);
	QFile f2("E:/Documents/hyne/autres/ff8slot00_switch_version.dat.dec");
	f2.open(QIODevice::WriteOnly);
	f2.write(QLZ4::decompressAll(f1.readAll())); */

	Map map1, map2;
	WmxFile wmx;
	WmsetFile wmset;
	TexlFile texl;
	QFile file("E:/Documents/Deling-build-desktop/wm/game/wmx.obj");
	file.open(QIODevice::ReadOnly);
	QFile file2("E:/Documents/Deling-build-desktop/wm/game/wmsetfr.obj");
	file2.open(QIODevice::ReadOnly);
	QFile file3("E:/Documents/Deling-build-desktop/wm/game/texl.obj");
	file3.open(QIODevice::ReadOnly);
	wmx.setDevice(&file);
	wmset.setDevice(&file2);
	texl.setDevice(&file3);
	if(!wmx.readSegments(map1, 835)) {
		qWarning() << "Cannot read segments 1";
	} else {
		if (!wmset.readEncounterRegions(map1)) {
			qWarning() << "Cannot read encounter regions";
		}
		if (!wmset.readEncounters(map1)) {
			qWarning() << "Cannot read encounters";
		}
		if (!wmset.readSpecialTextures(map1)) {
			qWarning() << "Cannot read special textures";
		}
		if (!wmset.readRoadTextures(map1)) {
			qWarning() << "Cannot read special textures";
		}
		if (!texl.readTextures(map1)) {
			qWarning() << "Cannot read textures";
		}
		WorldmapWidget *wmWidget = new WorldmapWidget();
		wmWidget->resize(640, 480);
		wmWidget->show();
		wmWidget->setMap(&map1);

		// wmWidget->scene()->setLimits(QRect(10, 0, 1, 1));

		// wmWidget->scene()->setZTrans(-0.714249f);

		/* forever {
			QImage pm = wmWidget->scene()->renderPixmap(4096, 4096).toImage();
			if (qAlpha(pm.pixel(0, 0)) == 0 ||
					qAlpha(pm.pixel(pm.width() - 1, pm.height() - 1)) == 0) {
				if(wmWidget->scene()->zTrans() > 0.0 || wmWidget->scene()->zTrans() < -1.0) {
					break;
				}
				wmWidget->scene()->setZTrans(wmWidget->scene()->zTrans() + 0.0000001f);
				continue;
			}
			break;
		}
		qDebug() << wmWidget->scene()->zTrans();

		wmWidget->scene()->renderPixmap(4096, 4096).save("E:/Documents/Deling-build-desktop/wm/render.png");
		*/
	/*
		int scale = 128;
		QPixmap pixmap(32 * scale,
		               (map1.segments().size() / 32 + (map1.segments().size() % 32 && true)) * scale);
		qDebug() << pixmap.size();
		QPainter p(&pixmap);
		int xs = 0, ys = 0;
		foreach (const MapSegment &segment, map1.segments()) {
			Q_UNUSED(segment)
			wmWidget->scene()->setLimits(QRect(xs, ys, 1, 1));
			QPixmap pix = wmWidget->scene()->renderPixmap(scale, scale);
			p.drawPixmap(xs * scale, pixmap.height() - ys * scale, pix);
			xs += 1;
			if (xs >= 32) {
				xs = 0;
				ys += 1;
			}
		}
		p.end();
		pixmap.toImage().mirrored().save("E:/Documents/Deling-build-desktop/wm/render.png");

		// wmWidget->scene()->renderPixmap(scale, scale)
		*/
	}


	/* if(!wmx.readSegments(map2)) {
		qWarning() << "Cannot read segments 2";
	}
	QFile file2("E:/Documents/Deling-build-desktop/wm/wmx.custom.obj");
	file2.open(QIODevice::WriteOnly | QIODevice::Truncate);
	wmx.setDevice(&file2);
	if(!wmx.writeSegments(map1)) {
		qWarning() << "Cannot write segments 1";
	}
	if(!wmx.writeSegments(map2)) {
		qWarning() << "Cannot write segments 2";
	} */

	/* FsArchive fsArchiveOk("E:/Downloads/field-patched-ok/field.f"),
	          fsArchiveBroken("E:/Downloads/field.f");
	fsArchiveBroken.repair(&fsArchiveOk); */



	// ProgressWidget pw("toto", ProgressWidget::Stop, window);
	// FsArchive fsArchive("E:/Programs (x86)/Steam/steamapps/common/FINAL FANTASY VIII/Data/lang-fr/world.f");
	/* fsArchive.replaceFile("E:/Documents/Deling-build-desktop/wm/wmsetfr.modified.obj",
	                      "C:\\ff8\\Data\\fre\\world\\dat\\wmsetfr.obj", &pw);
	*/
	/* fsArchive.replaceFile("E:/Documents/Deling-build-desktop/wm/wmx.custom.obj",
	                      "C:\\ff8\\Data\\fre\\world\\dat\\wmx.obj", &pw);
	*/

	return app.exec();
}
