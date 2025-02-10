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

#include <QCoreApplication>
#include "CLI.h"
#include <QXmlStreamReader>

// Only for static compilation
//Q_IMPORT_PLUGIN(qjpcodecs) // jp encoding

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	QCoreApplication::setApplicationName(DELING_NAME);
	QCoreApplication::setApplicationVersion(DELING_VERSION);

	QFile f("translations/Deling_en_fr.ts");
	f.open(QIODevice::ReadOnly);
	QXmlStreamReader reader(&f);
	bool messageStart = false, sourceStart = false, translationStart = false, nameStart = false, commentStart = false;
	QString source, translation, contextName, comment, lastName;
	QMap<QString, QString> frToEn;
	
	while (!reader.atEnd()) {
		QXmlStreamReader::TokenType tokenType = reader.readNext();
		
		if (tokenType == QXmlStreamReader::StartElement) {
			lastName = reader.name().toString();
			if (reader.name() == "name") {
				nameStart = true;
			} else if (reader.name() == "comment") {
				commentStart = true;
			} else if (reader.name() == "message") {
				messageStart = true;
				source = QString();
				translation = QString();
			} else if (messageStart && reader.name() == "source") {
				sourceStart = true;
			} else if (messageStart && reader.name() == "translation") {
				translationStart = true;
			}
		} else if (tokenType == QXmlStreamReader::EndElement) {
			if (reader.name() == "message") {
				if (!source.isEmpty()) {
					qDebug() << "read" << contextName << comment << source << translation;
					
					if (translation.isEmpty()) {
						translation = source;
					}
					QString key = contextName + "|" + comment + "|" + source;

					if (frToEn.contains(key) && frToEn.value(key) != translation) {
						qWarning() << "Error: already referenced text" << contextName << source << translation;
					}
					
					frToEn.insert(key, translation);
				}

				messageStart = false;
				source = QString();
				translation = QString();
				comment = QString();
			}
			sourceStart = false;
			translationStart = false;
			nameStart = false;
			commentStart = false;
			if (lastName == "context") {
				contextName = QString();
			}
		} else if (tokenType == QXmlStreamReader::Characters) {
			if (sourceStart) {
				source = reader.text().toString();
			}
			if (translationStart) {
				translation = reader.text().toString();
			}
			if (nameStart) {
				contextName = reader.text().toString();
			}
			if (commentStart) {
				comment = reader.text().toString();
			}
		}
	}
	
	QFile f2("translations/Deling_es_fr.ts");
	f2.open(QIODevice::ReadOnly);
	QXmlStreamReader reader2(&f2);
	
	QFile f3("translations/Deling_es_en.ts");
	f3.open(QIODevice::WriteOnly | QIODevice::Truncate);
	QXmlStreamWriter writer(&f3);
	writer.setAutoFormatting(true);
	writer.writeStartDocument();
	messageStart = false;
	sourceStart = false;
	translationStart = false;
	nameStart = false;
	commentStart = false;
	lastName = QString();
	contextName = QString();
	comment = QString();
	
	while (!reader2.atEnd()) {
		QXmlStreamReader::TokenType tokenType = reader2.readNext();
		
		if (tokenType == QXmlStreamReader::StartElement) {
			lastName = reader2.name().toString();
			if (reader2.name() == "name") {
				nameStart = true;
			} else if (reader2.name() == "comment") {
				commentStart = true;
			} else if (reader2.name() == "message") {
				messageStart = true;
				source = QString();
				translation = QString();
			} else if (messageStart && reader2.name() == "source") {
				sourceStart = true;
			} else if (messageStart && reader2.name() == "translation") {
				translationStart = true;
			}
			if (reader2.name() != "source") {
				writer.writeStartElement(reader2.name());
				writer.writeAttributes(reader2.attributes());
			}
		} else if (tokenType == QXmlStreamReader::EndElement) {
			if (reader2.name() == "message") {
				if (!source.isEmpty()) {
					qDebug() << "write" << contextName << comment << source << translation;
					
					if (translation.isEmpty()) {
						translation = source;
					}
					QString key = contextName + "|" + comment + "|" + source;

					if (!frToEn.contains(key)) {
						qWarning() << "Error: not found text" << contextName << source << translation;
					} else {
						writer.writeStartElement("source");
						writer.writeCharacters(frToEn.value(key));
						writer.writeEndElement();
					}
				}

				messageStart = false;
				source = QString();
				translation = QString();
				comment = QString();
			}
			sourceStart = false;
			translationStart = false;
			nameStart = false;
			commentStart = false;			
			if (lastName != "source") {
				writer.writeEndElement();
			}
			if (lastName == "context") {
				contextName = QString();
			}
		} else if (tokenType == QXmlStreamReader::Characters) {
			if (sourceStart) {
				source = reader2.text().toString();
			}
			if (translationStart) {
				translation = reader2.text().toString();
			}
			if (nameStart) {
				contextName = reader2.text().toString();
			}
			if (commentStart) {
				comment = reader2.text().toString();
			}
			
			if (lastName != "source") {
				writer.writeCharacters(reader2.text());
			}
		}
	}
	
	QTimer::singleShot(0, &app, &QCoreApplication::quit);

	/* QMap<QString, QStringList> filesCache;
	QDir dir("translations/");
	QFile f("translations/Deling_en.ts");
	f.open(QIODevice::ReadOnly);
	QXmlStreamReader reader(&f);
	bool messageStart = false, sourceStart = false, translationStart = false;
	QString source, translation;
	QStringList paths;
	QList<int> lines;
	while (!reader.atEnd()) {
		QXmlStreamReader::TokenType tokenType = reader.readNext();
		
		if (tokenType == QXmlStreamReader::StartElement) {
			if (reader.name() == "message") {
				messageStart = true;
				paths.clear();
				lines.clear();
			} else if (messageStart && reader.name() == "location" && !reader.attributes().first().value().toString().contains(".en.")) {
				QString path = reader.attributes().first().value().toString();
				int line = reader.attributes().last().value().toInt();
				if (line != 0) {
					paths.append(path);
					lines.append(line);
				}
			} else if (messageStart && reader.name() == "source") {
				sourceStart = true;
			} else if (messageStart && reader.name() == "translation") {
				translationStart = true;
			}
		} else if (tokenType == QXmlStreamReader::EndElement) {
			if (reader.name() == "message") {
				if (!paths.isEmpty() && !translation.isEmpty()) {
					qDebug() << paths << lines;
					int i = 0;
					for (const QString &path: paths) {
						int line = lines.at(i);
						if (!filesCache.contains(path)) {
							QFile fi(dir.absoluteFilePath(path));
							fi.open(QIODevice::ReadOnly);
							QByteArray data = fi.readAll();
							fi.close();
							
							filesCache.insert(path, QString::fromUtf8(data).split("\r\n"));
						}
						
						QString trSource = QString("\"%1\"").arg(source.replace("\\", "\\\\").replace("\n", "\\n").replace("\"", "\\\"")), trTranslation = QString("\"%1\"").arg(translation.replace("\\", "\\\\").replace("\n", "\\n").replace("\"", "\\\""));
						if (!filesCache[path][line - 1].contains(trSource)) {
							qWarning() << "not found" << trSource << trTranslation << path << line;
						} else {
							filesCache[path][line - 1].replace(trSource, trTranslation);
						}
						
						++i;
					}
				}
				messageStart = false;
				paths.clear();
				lines.clear();
				source = QString();
				translation = QString();
			}
			sourceStart = false;
			translationStart = false;
		} else if (tokenType == QXmlStreamReader::Characters) {
			if (sourceStart) {
				source = reader.text().toString();
			}
			if (translationStart) {
				translation = reader.text().toString();
			}
		}
	}
	
	QMapIterator<QString, QStringList> it(filesCache);
	while (it.hasNext()) {
		it.next();
		QFile fi(dir.absoluteFilePath(it.key()));
		fi.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
		fi.write(it.value().join("\n").toUtf8());
		fi.close();
	} */
}
