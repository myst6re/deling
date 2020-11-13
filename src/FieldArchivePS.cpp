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
#include "FieldArchivePS.h"

FieldArchivePS::FieldArchivePS()
	: FieldArchive(), iso(0)
{
	readOnly = true;
}

FieldArchivePS::~FieldArchivePS()
{
	if(iso) {
		if(iso->isDemo()) {
			FF8Font::deregisterFont("demo");
			Config::setValue("encoding", "00");
		}

		delete iso;
	}
}

QString FieldArchivePS::archivePath() const
{
	return iso ? iso->fileName() : QString();
}

FieldPS *FieldArchivePS::getField(int id) const
{
	return (FieldPS *)FieldArchive::getField(id);
}

FF8DiscArchive *FieldArchivePS::getFF8DiscArchive() const
{
	return iso;
}

int FieldArchivePS::open(const QString &path, ArchiveObserver *progress)
{
	FieldPS *field;
	QString desc;
	int i, currentMap=0, fieldID=0;

	if(iso)		delete iso;
	iso = new FF8DiscArchive(path);
	if(!iso->open(QIODevice::ReadOnly)) {
		errorMsg = QObject::tr("Impossible d'ouvrir le fichier image disque. (%1)").arg(iso->errorString());
		return 1;
	}

	if(!iso->findIMG()) {
		errorMsg = QObject::tr("Fichier FF8DISC?.IMG introuvable.");
		return 4;
	}

	const QList<FF8DiscFile> &fieldFiles = iso->fieldDirectory();
	int tocSize = fieldFiles.size();
	const int tocStart = iso->isDemo() ? 0 : 77;

	if(tocSize == 0) {
		errorMsg = QObject::tr("Impossible d'ouvrir le dossier field.");
		return 4;
	}

	clearFields();
	setMapList(QStringList());
	int indexOf;

	quint32 freq = ((tocSize - tocStart) / 3)/100;

	if(freq == 0) {
		freq = 1;
	}

	progress->setObserverMaximum((tocSize - tocStart) / 3);

	for(i=tocStart ; i<tocSize ; i += 3) {
		QCoreApplication::processEvents();
		if(progress->observerWasCanceled()) {
			clearFields();
			errorMsg = QObject::tr("Ouverture annulée.");
			return 2;
		}
		if(currentMap % freq == 0) {
			progress->setObserverValue(currentMap);
		}

		QByteArray fieldData = iso->fileLZS(fieldFiles.at(i));

		if(!fieldData.isEmpty()) {
			if (iso->isDemo()) {
				field = new FieldJpDemoPS(i);
			} else {
				field = new FieldPS(i);
			}
			if(field->open(fieldData)) {
				if(field->hasJsmFile())
					desc = Data::location(field->getJsmFile()->mapID());
				else
					desc = QString();

				indexOf = iso->isDemo() ? i : mapList().indexOf(field->name());
				QString mapId = indexOf==-1 ? "~" : QString("%1").arg(indexOf, 3, 10, QChar('0'));

				fields.append(field);
				fieldsSortByName.insert(field->name(), fieldID);
				fieldsSortByDesc.insert(desc, fieldID);
				fieldsSortByMapId.insert(mapId, fieldID);
				++fieldID;
			} else {
				delete field;
			}
		}
		++currentMap;
	}

	if(fields.isEmpty()) {
		errorMsg = QObject::tr("Aucun écran trouvé.");
		return 4;
	}

	openModels();

	if(iso->isDemo()) {
		QByteArray sysFntTdw = iso->file(iso->sysFntTdwFile());
		TdwFile *tdw = new TdwFile();
		tdw->open(sysFntTdw);
		FF8Font::registerFont("demo", new FF8Font(tdw, QByteArray()));
		Config::setValue("encoding", "demo");
	} else {
		if(iso->isJp() && Config::value("encoding", "00").toString() == "00") {
			Config::setValue("encoding", "01");
		} else if(!iso->isJp() && Config::value("encoding", "00").toString() == "01") {
			Config::setValue("encoding", "00");
		}
	}

	return 0;
}

bool FieldArchivePS::openModels()
{
	if(!iso)	return false;

	const QList<FF8DiscFile> &fieldFiles = iso->fieldDirectory();

	for(int i=0 ; i<77 && i<fieldFiles.size() ; ++i) {
		QByteArray fieldData = iso->file(fieldFiles.at(i));

		if(!fieldData.isEmpty()) {
			MchFile mch;
			if(mch.open(fieldData, QString("d%1").arg(i, 3, 10, QChar('0')))
					&& mch.hasModel()) {
				models.insert(i, new CharaModel(*mch.model()));
			}
		}
	}

	return !models.isEmpty();
}

bool FieldArchivePS::openBG(Field *field) const
{
	if(!iso)	return false;

	FieldPS *fieldPS = (FieldPS *)field;
	quint32 isoFieldID = fieldPS->isoFieldID();
	QByteArray dat, mim, lzk;

	if (iso->isDemo()) {
		if((int)isoFieldID+2 >= iso->fieldCount()) {
			qWarning() << "FieldArchivePS::openBG field ID out of range" << isoFieldID << iso->fieldCount();
			return false;
		}

		dat = iso->fileLZS(iso->fieldFile(isoFieldID));
		mim = iso->fileLZS(iso->fieldFile(isoFieldID+1));
		lzk = iso->fileLZS(iso->fieldFile(isoFieldID+2));
	} else {
		if(isoFieldID < 1 || (int)isoFieldID+1 >= iso->fieldCount()) {
			qWarning() << "FieldArchivePS::openBG field ID out of range" << isoFieldID << iso->fieldCount();
			return false;
		}

		mim = iso->fileLZS(iso->fieldFile(isoFieldID-1));
		dat = iso->fileLZS(iso->fieldFile(isoFieldID));
		lzk = iso->file(iso->fieldFile(isoFieldID+1));
	}

	return fieldPS->open2(dat, mim, lzk);
}
