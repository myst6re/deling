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
#include "parameters.h"
#include "Config.h"
#include "MainWindow.h"
#include "IsoArchive.h"

// Only for static compilation
//Q_IMPORT_PLUGIN(qjpcodecs) // jp encoding

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setWindowIcon(QIcon(":/images/deling.png"));

	Config::set();

	QString lang = QLocale::system().name().toLower();
	lang = Config::value("lang", lang.left(lang.indexOf("_"))).toString();

	QTranslator qtTranslator;
	if(qtTranslator.load("qt_" + lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
		app.installTranslator(&qtTranslator);

	QTranslator translator;
	if(translator.load(qApp->applicationDirPath() % "/deling_" % lang))
		app.installTranslator(&translator);
	else
		Config::setValue("lang", "fr");
	
	//Data::load();

	MainWindow *window = new MainWindow();
	window->show();
	if(argc>1)
		window->openFile(argv[1]);

//	QFile debug("iso_footers.txt");
//	debug.open(QIODevice::WriteOnly | QIODevice::Truncate);

//	IsoArchive f("C:/Program Files/ePSXe/games/ff8-cd1-PAL.bin");
//	IsoArchive f("C:/Program Files/ePSXe/games/Final Fantasy VII-PSX-PAL-FR-CD1.bin");
//	IsoArchive f("C:/Program Files/ePSXe/games/FF7_CD1J.iso");
//	IsoArchive f("C:/Program Files/Square Soft, Inc/Final Fantasy VII/custom/ff7iso/FF7_CD1J.iso");
//	if(f.open(QIODevice::ReadOnly)) {
//		f.extract("FF8DISC1.IMG", "FF8DISC1.IMG.test");
//		f.close();
//	}
	
//	debug.close();
	/*QImage font1("C:/Users/vista/Documents/Deling-build-desktop/font-jp-14.png");
	QImage font2("C:/Users/vista/Documents/Deling-build-desktop/font-jp-15.png");
	QPixmap result(12 * 21, 12 * 11 * 4);// 224 carrés * 4
	result.fill(Qt::black);
	QPainter p(&result);

	int x=0, y=0, x2=0, y2=0;

	for(int j=0 ; j<4 ; ++j) {
		for(int i=0 ; i<112 ; ++i) {
			p.drawImage(x2, y2, font1.copy(x, y, 12, 12));

			x2 += 12;
			if(x2 >= 252) {
				x2 = 0;
				y2 += 12;
			}

			p.drawImage(x2, y2, font2.copy(x, y, 12, 12));

			x2 += 12;
			if(x2 >= 252) {
				x2 = 0;
				y2 += 12;
			}

			x += 12;
			if(x >= 252) {
				x = 0;
				y += 12;
			}
		}
		x2 = 0;
		y2 += 12;
	}

	p.end();

	result.save("new_font-jp.png");*/

	/********* WORLDMAP investigations *************/

//	quint32 posSection1, posSection1b, posSection, i=0;
//	QList<quint32> posSections, posSections2, newPosSections;
//	QFile f("C:/wamp/www/ff8/fils/fre/world/dat/wmsetfr.obj.bak");
//	f.open(QIODevice::ReadOnly);
//	QFile f2("C:/wamp/www/ff8/fils/fre/world/dat/wmsetfr.obj");
//	f2.open(QIODevice::WriteOnly);
//	f.read((char *)&posSection1, 4);
//	posSections.append(posSection1);
//	do {
//		f.read((char *)&posSection, 4);
//		if(posSection == 0)	break;
//		posSections.append(posSection);
//		++i;
//	} while(i<posSection/4);

//	posSections.append(f.size());

//	qDebug() << posSections;

//	/*for(i=0 ; i<posSections.size()-1 ; ++i) {
//		QFile t(QString("section%1.hex").arg(i));
//		t.open(QIODevice::WriteOnly);
//		f.seek(posSections.at(i));
//		t.write(f.read(posSections.at(i+1) - posSections.at(i)));
//		t.close();
//	}*/

//	f.seek(posSections.at(16));
//	f.read((char *)&posSection1b, 4);
//	posSections2.append(posSection1b);
//	i=0;
//	do {
//		f.read((char *)&posSection, 4);
//		if(posSection == 0)	break;
//		posSections2.append(posSection);
//		++i;
//	} while(i<posSection/4);

//	posSections2.append(posSections.at(17)-posSections.at(16));
//	qDebug() << posSections2;

//	for(i=0 ; i<posSections2.size()-1 ; ++i) {
//		QFile t(QString("section16-%1.hex").arg(i));
//		t.open(QIODevice::WriteOnly);
//		f.seek(posSections.at(16) + posSections2.at(i));
//		t.write(f.read(posSections2.at(i+1) - posSections2.at(i)));
//		t.close();
//	}

//	f2.write(f.read(posSection1));

//	for(i=0 ; i<posSections.size()-1 ; ++i) {
//		newPosSections.append(f2.pos());
//		f.seek(posSections.at(i));
//		if(i>=1 || i==15 || i==7 || i==8 || i==9 || i==13 || /*i==31 ||*/ i==32 || i==36 || i==37 || i==38 || i==41 || i==16 || i>=42) {
//			f2.write(f.read(posSections.at(i+1) - posSections.at(i)));
//		} else {
//			f2.write("\x00\x00\x00\x00", 4);
//		}
//	}

//	f2.reset();

//	for(i=0 ; i<newPosSections.size()-1 ; ++i) {
//		posSection = newPosSections.at(i);
//		f2.write((char *)&posSection, 4);
//	}

//	f2.close();
//	f.close();

	/*for(int i=1 ; i<256 ; ++i) {
		for(int j=1 ; j<256 ; ++j) {
			for(int k=1 ; k<256 ; ++k) {
				qDebug() << i << j << k;
				for(int l=1 ; l<256 ; ++l) {
					QByteArray base = QByteArray().append((char)i).append((char)j).append((char)k).append((char)l);
					QString text = FF8Text::fromFF8(base);
					if(base != FF8Text::toFF8(text)) {
						qDebug() << "Erreur" << base.toHex() << FF8Text::toFF8(text).toHex() << text;
						return app.exec();
					}
				}
			}
		}
	}*/

//	QFile f("C:/Users/vista/Documents/Makou_Reactor/data/WINDOW.BIN/WINDOW-jp.3.tim");
//	f.open(QIODevice::ReadOnly);
//	QByteArray data = f.readAll();

//	for(int i=0 ; i<16 ; ++i)
//		FF8Image::tim(data, i).save(QString("C:/Users/vista/Documents/Makou_Reactor/data/WINDOW.BIN/font-jp2-%1.png").arg(i, 2, 10, QChar('0')));

    return app.exec();
}
/*
  Section 0 -> battle counter ?
  Section 3 -> battle formations
  Section 6 -> rail texturing
  Section 7 -> wm2field
  Section 8 -> field2wm
  Section 9 -> pos related
  Section 10 -> pos lunatic pandora ???
  Section 13 -> texts
  Section 14 -> 00000000
  Section 15 -> ??? !important²
  Section 16 -> !important
  Section 18 -> map name
  Section 21 -> 00000000
  Section 22 -> 00000000
  Section 23 -> 00000000
  Section 24 -> 00000000
  Section 25 -> 00000000
  Section 26 -> 00000000
  Section 27 -> 00000000
  Section 29 -> 00000000
  Section 31 -> town names
  Section 32 -> light
  Section 34 -> drawPoints
  Section 36 -> ???? !important²
  Section 37 -> main worldmap textures
  Section 38 -> town & rail textures
  Section 39 -> ??? tex
  Section 41 -> vehicles textures !important
  Sections 42-47 -> AKAOs
*/
