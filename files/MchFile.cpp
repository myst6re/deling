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
	QList<quint32> timOffsets;
	const char *constData = mch.constData();
	const char * const startData = constData;

	do {
		memcpy(&timOffset, constData, 4);
		constData += 4;

		timOffsets.append(timOffset);

		qDebug() << "tim offset" << QString::number(timOffset & 0xFFFFFF, 16) << QString::number((timOffset >> 24) & 0xFF, 16);
	} while(timOffset != 0xFFFFFFFF && constData - startData < 0x100);

	if(timOffset == 0xFFFFFFFF) {
		memcpy(&modelOffset, constData, 4);
		constData += 4;

		qDebug() << "model data offset" << QString::number(modelOffset, 16);

		if(!timOffsets.isEmpty()) {
			timOffsets.append(modelOffset);
			timOffset = timOffsets.first() & 0xFFFFFF;
			_model = new CharaModel(name, TimFile(mch.mid(timOffset, timOffsets.at(1) - timOffset)));
		} else {
			qDebug() << "No tim";
		}
	} else {
		qWarning() << "Unknown format (5)!" << QString::number(timOffset, 16);
		return false;
	}

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
