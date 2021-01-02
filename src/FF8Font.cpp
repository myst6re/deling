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
#include "FF8Font.h"
#include "parameters.h"
#include "Config.h"

FF8Font::FF8Font(TdwFile *tdw, const QByteArray &txtFileData) :
	_tdw(tdw), modified(false), readOnly(false)
{
	openTxt(QString::fromUtf8(txtFileData.data()));
}

TdwFile *FF8Font::tdw() const
{
	return _tdw;
}

const QList<QStringList> &FF8Font::tables() const
{
	return _tables;
}

void FF8Font::setTables(const QList<QStringList> &tables)
{
	_tables = tables;
	modified = true;
}

void FF8Font::setChar(int tableId, int charId, const QString &c)
{
	if(tableId < _tables.size() && charId < 224) {
		_tables[tableId][charId] = c;
		modified = true;
	}
}

bool FF8Font::isValid() const
{
	return !_tables.isEmpty() && !_tdw->isNull();
}

bool FF8Font::isModified() const
{
	return modified || _tdw->isModified();
}

void FF8Font::setModified(bool modified)
{
	this->modified = modified;
	_tdw->setModified(modified);
}

const QString &FF8Font::name() const
{
	return _name;
}

void FF8Font::setName(const QString &name)
{
	_name = name;
	modified = true;
}

void FF8Font::setPaths(const QString &txtPath, const QString &tdwPath)
{
	_txtPath = txtPath;
	_tdwPath = tdwPath;
	if(_name.isEmpty()) {
		_name = txtPath.mid(txtPath.lastIndexOf('/')+1);
		_name = _name.left(_name.size() - 4);
	}
}

const QString &FF8Font::txtPath() const
{
	return _txtPath;
}

const QString &FF8Font::tdwPath() const
{
	return _tdwPath;
}

void FF8Font::setReadOnly(bool ro)
{
	readOnly = ro;
}

bool FF8Font::isReadOnly() const
{
	return readOnly;
}

void FF8Font::openTxt(const QString &data)
{
	int tableCount = _tdw->tableCount();
	QStringList lines = data.split(QRegExp("\\s*(\\r\\n|\\r|\\n)\\s*"), QString::SkipEmptyParts);
	QRegExp nameRegExp("#NAME\\s+(\\S.*)"), letterRegExp("\\s*\"([^\"]*|\\\"*)\"\\s*,?\\s*");
	// nameRegExp:		#NAME blah blah blah
	// letterRegExp:	"Foo", "Foo","Foo"
	QStringList table;

	if(tableCount < 1) {
		qWarning() << "invalid tdwFile!";
		return;
	}

	foreach(const QString &line, lines) {
		if(line.startsWith("#")) {
			if(nameRegExp.indexIn(line) != -1) {
				QStringList capturedTexts = nameRegExp.capturedTexts();
				_name = capturedTexts.at(1).trimmed();
			}
		}
		else {
			int offset=0;
			while((offset = letterRegExp.indexIn(line, offset)) != -1) {
				QStringList capturedTexts = letterRegExp.capturedTexts();
				table.append(capturedTexts.at(1));
				offset += capturedTexts.first().size();

				if(table.size() == 224) {
					_tables.append(table);
					if(_tables.size() > tableCount) {
						//print();
						return;
					}
					table = QStringList();
				}
			}
		}
	}

	if(!table.isEmpty()) {
		if(table.size() < 224) {
			for(int i=table.size() ; i<224 ; ++i) {
				table.append(QString());
			}
		}

		_tables.append(table);
	}

	//print();
}

QString FF8Font::saveTxt()
{
	QString data;

	if(!_name.isEmpty()) {
		data.append("#NAME\t").append(_name).append("\n");
	}

	foreach(const QStringList &t, _tables) {
		for(int j=0 ; j<14 ; ++j) {
			for(int i=0 ; i<16 ; ++i) {
				data.append(QString("\"%1\"").arg(t[j*16 + i]));
				if(i<15) {
					data.append(",");
				}
			}
			if(j<13) {
				data.append(",");
			}
			data.append("\n");
		}
		data.append("\n");
	}

	return data;
}

void FF8Font::print()
{
	int tid=1;
	foreach(const QStringList &t, _tables) {
		qDebug() << QString("table %1").arg(tid++).toLatin1().data();
		for(int j=0 ; j<14 ; ++j) {
			QString buf;
			for(int i=0 ; i<16 ; ++i) {
				buf += QString("\"%1\",").arg(t[j*16 + i]);
			}
			qDebug() << buf.toLatin1().data();
		}
	}
}

QMap<QString, FF8Font *> FF8Font::fonts;
QString FF8Font::font_dirPath;

bool FF8Font::listFonts()
{
	fonts.clear();

#ifdef Q_OS_WIN
	font_dirPath = qApp->applicationDirPath();
//	font_dirPath = QDir::cleanPath(QSettings(QSettings::IniFormat, QSettings::UserScope, PROG_NAME, PROG_NAME).fileName());
//	font_dirPath = font_dirPath.left(font_dirPath.lastIndexOf('/'));
#else
	font_dirPath = QDir::cleanPath(QSettings(PROG_NAME, PROG_NAME).fileName());
	font_dirPath = font_dirPath.left(font_dirPath.lastIndexOf('/'));
#endif

	QDir dir(font_dirPath);
	dir.mkpath(font_dirPath);
	QStringList stringList = dir.entryList(QStringList("*.tdw"), QDir::Files, QDir::Name);

	FF8Font *latinFont = openFont(":/fonts/sysfnt.tdw", ":/fonts/sysfnt.txt");
	FF8Font *jpFont = openFont(":/fonts/sysfnt_jp.tdw", ":/fonts/sysfnt_jp.txt");

	if(!latinFont || !jpFont)	return false;

	latinFont->setReadOnly(true);
	jpFont->setReadOnly(true);

	fonts.insert("00", latinFont);
	fonts.insert("01", jpFont);

	foreach(const QString &str, stringList) {
		int index = str.lastIndexOf('.');
		fonts.insert(str.left(index), NULL);
	}

	return true;
}

QStringList FF8Font::fontList()
{
	return fonts.keys();
}

FF8Font *FF8Font::openFont(const QString &tdwPath, const QString &txtPath)
{
	FF8Font *ff8Font = NULL;
	TdwFile *tdw = NULL;
	QFile f(tdwPath);
	if(f.open(QIODevice::ReadOnly)) {
		tdw = new TdwFile();
		if(!tdw->open(f.readAll())) {
			qWarning() << "Cannot open tdw file!" << f.fileName();
			delete tdw;
			tdw = NULL;
		}
		f.close();
	}

	if(!tdw) {
		return NULL;
	} else {
		QFile f2(txtPath);
		if(f2.open(QIODevice::ReadOnly)) {
			ff8Font = new FF8Font(tdw, f2.readAll());
			f2.close();
		} else {
			ff8Font = new FF8Font(tdw, QByteArray());
		}
		ff8Font->setPaths(txtPath, tdwPath);
		return ff8Font;
	}
}

void FF8Font::registerFont(const QString &name, FF8Font *font)
{
	fonts.insert(name, font);
}

void FF8Font::deregisterFont(const QString &name)
{
	if(fonts.contains(name)) {
		delete fonts.take(name);
	}
}

FF8Font *FF8Font::font(QString name)
{
	if(name.isEmpty()) {
		name = "00";
	}

	if(fonts.contains(name)) {
		FF8Font *ff8Font = fonts.value(name);
		if(!ff8Font) {
			ff8Font = openFont(font_dirPath + "/" + name + ".tdw", font_dirPath + "/" + name + ".txt");
			if(!ff8Font) {
				fonts.remove(name);// Bad font, we can remove it
			} else {
				fonts.insert(name, ff8Font);
			}
		}
		return ff8Font;
	}

	return NULL;
}

FF8Font *FF8Font::getCurrentConfigFont()
{
	QString fnt = Config::value("encoding", "00").toString();

	QStringList fontL = fontList();
	if(fontL.contains(fnt)) {
		return font(fnt);
	}
	return font(fontL.first());
}

bool FF8Font::saveFonts()
{
	bool ok = true;

	foreach(FF8Font *font, fonts) {
		if(font && !font->isReadOnly() && font->isModified()) {
			QFile f1(font->txtPath());
			if(f1.open(QIODevice::WriteOnly)) {
				f1.write(font->saveTxt().toUtf8());
				f1.close();
			} else {
				ok = false;
			}
			QFile f2(font->tdwPath());
			if(f2.open(QIODevice::WriteOnly)) {
				QByteArray tdwData;
				if(font->tdw()->save(tdwData)) {
					f2.write(tdwData);
				} else {
					ok = false;
				}
				f2.close();
			} else {
				ok = false;
			}
			if(ok) {
				font->setModified(false);
			}
		}
	}

	return ok;
}

bool FF8Font::copyFont(const QString &name, const QString &from, const QString &name2)
{
	if(fonts.contains(name) || !fonts.contains(from)) {
		return false;
	}

	FF8Font *ff8Font = font(from);
	if(!ff8Font) {
		return false;
	}

	QFile ftxt(font_dirPath + "/" + name + ".txt");
	if(!ftxt.open(QIODevice::WriteOnly)) {
		return false;
	}

	QFile ftxt2(ff8Font->txtPath());
	if(!ftxt2.open(QIODevice::ReadOnly)) {
		return false;
	}

	ftxt.write(ftxt2.readAll());

	ftxt.close();
	ftxt2.close();

	QFile ftdw(font_dirPath + "/" + name + ".tdw");
	if(!ftdw.open(QIODevice::WriteOnly)) {
		return false;
	}

	QFile ftdw2(ff8Font->tdwPath());
	if(!ftdw2.open(QIODevice::ReadOnly)) {
		return false;
	}

	ftdw.write(ftdw2.readAll());

	ftdw.close();
	ftdw2.close();

	fonts.insert(name, NULL);

	ff8Font = font(name);
	if(!ff8Font) {
		return false;
	}

	ff8Font->setName(name2);
	ff8Font->setReadOnly(false);

	return true;
}

bool FF8Font::removeFont(const QString &name)
{

	FF8Font *ff8Font = font(name);

	if(!ff8Font || ff8Font->isReadOnly()) {
		return false;
	}

	if(!QFile::remove(ff8Font->txtPath())
	|| !QFile::remove(ff8Font->tdwPath())) {
		return false;
	}

	delete fonts.take(name);

	return true;
}

const QString &FF8Font::fontDirPath()
{
	return font_dirPath;
}
