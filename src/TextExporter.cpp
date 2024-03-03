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
#include "TextExporter.h"
#include "FieldArchive.h"
#include "FieldArchivePC.h"
#include "Field.h"
#include "ArchiveObserver.h"
#include "files/MsdFile.h"
#include "Config.h"

TextExporter::TextExporter(FieldArchive *archive) :
      _archive(archive)
{
}

bool TextExporter::toCsv(const QString &fileName, const QStringList &langs, QChar fieldSeparator, QChar quoteCharacter, CsvFile::CsvEncoding encoding, ArchiveObserver *observer)
{
	if (!_archive) {
		return false;
	}
	
	FieldArchiveIterator it = _archive->iterator();
	
	if (observer) {
		observer->setObserverMaximum(quint32(_archive->nbFields()));
	}
	
	QFile io(fileName);
	
	if (!io.open(QIODevice::WriteOnly)) {
		_lastErrorString = io.errorString();
		return false;
	}
	
	CsvFile csv(&io, fieldSeparator, quoteCharacter, encoding);
	csv.writeLine(langs.empty() ? QStringList() << "Text" << "Comment" : QStringList() << langs << "Comment");
	const QStringList &mapList = _archive->mapList();
	
	QVariant oldEncoding = Config::value("encoding");
	
	int i = 0;
	
	while (it.hasNext()) {
		Field *f = it.next();
		
		if (observer) {
			if (observer->observerWasCanceled()) {
				return false;
			}
			observer->setObserverValue(i++);
		}
		
		if (f && f->isOpen() && f->hasMsdFile()) {
			QString fieldName = f->name();
			if (fieldName.isEmpty()) {
				continue;
			}
			qsizetype index = mapList.indexOf(fieldName);
			if (index < 0) {
				continue;
			}
			
			csv.writeLine(QStringList() << QString("#%1").arg(fieldName) << QString::number(index));
			
			MsdFile *msd = f->getMsdFile();
			if (msd != nullptr) {
				QList<QList<QByteArray>> textsByLang;

				if (langs.size() > 1 && f->isPc()) {
					int langId = 0;
					for (const QString &lang: langs) {
						((FieldPC *)f)->changeGameLang(lang, ((FieldArchivePC *)_archive)->getFsArchive());
						MsdFile *msd2 = f->getMsdFile();
						if (msd2 != nullptr) {
							const QList<QByteArray> &texts = msd2->getTexts();
							int textId = 0;
							for (const QByteArray &text: texts) {
								if (textId >= textsByLang.size()) {
									QList<QByteArray> textByLang(langs.size());
									textByLang[langId] = text;
									textsByLang.append(textByLang);
								} else {
									textsByLang[textId][langId] = text;
								}
								textId += 1;
							}
						}
						langId += 1;
					}
					((FieldPC *)f)->changeGameLang(Config::value("gameLang", "en").toString(), ((FieldArchivePC *)_archive)->getFsArchive());
					
					for (const QList<QByteArray> &texts: textsByLang) {
						QStringList line;
						
						langId = 0;
						for (const QByteArray &text: texts) {
							Config::setValue("encoding", langs.at(langId).compare("jp", Qt::CaseInsensitive) == 0 ? "01" : "00");
							line << FF8Text(text);
							langId += 1;
						}
						
						csv.writeLine(line << "");
					}
				} else {
					for (const QByteArray &text: msd->getTexts()) {
						csv.writeLine(QStringList() << FF8Text(text) << "");
					}
				}
			}
		}
	}
	
	Config::setValue("encoding", oldEncoding);
	
	return true;
}

bool TextExporter::fromCsv(const QString &fileName, quint8 column, ArchiveObserver *observer)
{
	if (!_archive) {
		return false;
	}
	
	if (observer) {
		observer->setObserverMaximum(quint32(_archive->nbFields()));
	}
	
	QFile io(fileName);
	
	if (!io.open(QIODevice::ReadOnly)) {
		_lastErrorString = io.errorString();
		return false;
	}
	
	CsvFile csv(&io);
	const QStringList &mapList = _archive->mapList();
	QStringList line;
	MsdFile *msd = nullptr;
	QList<QByteArray> texts;
	int currentLine = 0, i = 0;
	
	while (csv.readLine(line)) {
		if (observer && observer->observerWasCanceled()) {
			return false;
		}
		currentLine += 1;
		if (line.size() >= 2 && line.first().startsWith("#")) {
			qsizetype index = mapList.indexOf(line.first().mid(1));
			bool ok = false;
			int mapId = line.at(1).toInt(&ok);
			if (ok && index >= 0 && mapId == index) {
				if (msd != nullptr) {
					msd->setTexts(texts);
					texts.clear();
				}
				
				Field *f = _archive->getFieldFromMapId(mapId);
				if (f == nullptr) {
					qWarning() << "TextExporter::fromCsv field not found" << line << "at line" << currentLine;
					msd = nullptr;
					continue;
				}
				msd = f->getMsdFile();
				if (msd == nullptr) {
					qWarning() << "TextExporter::fromCsv field invalid" << line << "at line" << currentLine;
				}
				
				if (observer) {
					observer->setObserverValue(i++);
				}
				
				continue;
			}
		}
		
		if (msd != nullptr) {
			if (column < line.size()) {
				texts.append(FF8Text(line.at(column)).toFF8());
			} else {
				qWarning() << "TextExporter::fromCsv invalid CSV" << line << "at line" << currentLine;
			}
		}
	}
	
	if (msd != nullptr) {
		msd->setTexts(texts);
	}
	
	return true;
}
