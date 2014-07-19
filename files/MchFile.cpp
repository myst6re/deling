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
#include "MchFile.h"

MchFile::MchFile() :
	File(), _model(0)
{
}

bool MchFile::open(const QByteArray &mch, const QString &name)
{
	_model = 0;

	if(mch.size() < 0x100) {
		if(mch.size() != 33) { // "This is dummy file. Kazuo Suzuki{0a}"
			qWarning() << "MchFile::open mch file too short" << name << mch.size();
		}
		return false;
	}

	quint32 timOffset, modelOffset;
	QList<quint32> toc;
	const char *constData = mch.constData();
	const char * const startData = constData;

	do {
		memcpy(&timOffset, constData, 4);
		constData += 4;

		if(timOffset != 0xFFFFFFFF) {
			toc.append(timOffset);
		}

//		qDebug() << "tim offset" << QString::number(timOffset & 0xFFFFFF, 16) << QString::number((timOffset >> 24) & 0xFF, 16);
	} while(timOffset != 0xFFFFFFFF && constData - startData < 0x100);

	if(timOffset == 0xFFFFFFFF) {
		memcpy(&modelOffset, constData, 4);

//		qDebug() << "model data offset" << QString::number(modelOffset, 16);

		if(!toc.isEmpty()) {
			toc.append(modelOffset);
			toc.append(mch.size());

			_model = new CharaModel(name, toc, mch);
		}
//		else {
//			qDebug() << "No tim";
//		}
	} else {
		qWarning() << "MchFile::open Unknown format (5)!" << QString::number(timOffset, 16);
		return false;
	}

//	qDebug() << "MchFile ouvert" << name;

	return true;
}

bool MchFile::hasModel() const
{
	return _model;
}

CharaModel *MchFile::model() const
{
	return _model;
}

void MchFile::setModel(CharaModel *model)
{
	_model = model;
}
