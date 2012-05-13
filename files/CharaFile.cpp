#include "CharaFile.h"

CharaFile::CharaFile() :
	modified(false)
{
}

bool CharaFile::isModified() const
{
	return modified;
}

bool CharaFile::open(const QByteArray &one)
{
	models.clear();

	if(one.size() < 0x800) {
		qWarning() << "fichier trop petit";
		return false;
	}

	quint32 count, offset, size, modelID;
	const char *constData = one.constData();
	const char * const startData = constData;

	memcpy(&count, constData, 4);
	constData += 4;

	qDebug() << "count" << count;

	for(quint32 i=0 ; i<count && constData - startData < 0x800 ; ++i) {
		memcpy(&offset, constData, 4);
		qDebug() << i << "offset" << QString::number(offset, 16);
		constData += 4;
		memcpy(&size, constData, 4);
		constData += 4;
		qDebug() << "size" << size;

		memcpy(&modelID, constData, 4);
		constData += 4;

		if(modelID == size) {
			qDebug() << "size twice!";
			memcpy(&modelID, constData, 4);
			constData += 4;
		}

		if(modelID >> 16 == 0xd010) {
			qDebug() << "modelID" << (modelID & 0xFFFF);
			memcpy(&modelID, constData, 4);
			constData += 4;
			if(modelID == 0) {
				QString name(one.mid(constData - startData, 8));
				qDebug() << name;
				constData += 8;
				memcpy(&modelID, constData, 4);
				constData += 4;
				if(modelID != 0xEEEEEEEE) {
					qWarning() << "Unknown format (3)!" << QString::number(modelID, 16);
					return false;
				}

				models.append(CharaModel(name));
			} else {
				qWarning() << "Unknown format (2)!" << QString::number(modelID, 16);
				return false;
			}
		} else {
			qDebug() << "no modelID";
			qDebug() << "tim offset" << QString::number(modelID & 0xFFFFFF, 16) << QString::number((modelID >> 24) & 0xFF, 16);
			do {
				memcpy(&modelID, constData, 4);
				constData += 4;

				qDebug() << "tim offset" << QString::number(modelID & 0xFFFFFF, 16) << QString::number((modelID >> 24) & 0xFF, 16);
			} while(modelID != 0xFFFFFFFF && constData - startData < 0x800);

			if(modelID == 0xFFFFFFFF) {
				memcpy(&modelID, constData, 4);
				constData += 4;

				qDebug() << "model data offset" << QString::number(modelID, 16);

				QString name(one.mid(constData - startData, 8));
				qDebug() << name;
				constData += 8;
				memcpy(&modelID, constData, 4);
				constData += 4;
				if(modelID != 0xEEEEEEEE) {
					qWarning() << "Unknown format (6)!" << QString::number(modelID, 16);
//					return false;
				}

				models.append(CharaModel(name, FF8Image::tim(one.mid(offset+4, size))));
				qDebug() << "Tim ajouté" << name;
			} else {
				qWarning() << "Unknown format (5)!" << QString::number(modelID, 16);
				return false;
			}
		}
	}

	return true;
}

const CharaModel &CharaFile::model(int id) const
{
	return models.at(id);
}

void CharaFile::setModel(int id, const CharaModel &model)
{
	models.replace(id, model);
	modified = true;
}

int CharaFile::modelCount() const
{
	return models.size();
}
