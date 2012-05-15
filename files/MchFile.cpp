#include "MchFile.h"

MchFile::MchFile() :
	_model(0)
{
}

bool MchFile::open(const QByteArray &mch, const QString &name)
{
	_model = 0;

	if(mch.size() < 0x100) {
		qWarning() << "fichier trop petit" << mch.size();
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

		qDebug() << "tim offset" << QString::number(timOffset & 0xFFFFFF, 16) << QString::number((timOffset >> 24) & 0xFF, 16);
	} while(timOffset != 0xFFFFFFFF && constData - startData < 0x100);

	if(timOffset == 0xFFFFFFFF) {
		memcpy(&modelOffset, constData, 4);
		constData += 4;

		qDebug() << "model data offset" << QString::number(modelOffset, 16);

		if(!toc.isEmpty()) {
			toc.append(modelOffset);
			toc.append(mch.size());

			_model = new CharaModel(name, toc, mch);
		} else {
			qDebug() << "No tim";
		}
	} else {
		qWarning() << "Unknown format (5)!" << QString::number(timOffset, 16);
		return false;
	}

	qDebug() << "MchFile ouvert" << name;

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
