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

	quint32 count, offset, size, modelID;
	const char *constData = one.constData();

	memcpy(&count, constData, 4);
	constData += 4;

	qDebug() << "count" << count;

	for(quint32 i=0 ; i<count ; ++i) {
		memcpy(&offset, constData, 4);
		qDebug() << "offset" << offset;
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
				QString name(one.mid(constData - one.constData(), 8));
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
			do {
				memcpy(&modelID, constData, 4);
				constData += 4;
			} while(modelID != 0xFFFFFFFF && constData - one.constData() < 0x800);

			if(modelID == 0xFFFFFFFF) {
//				memcpy(&modelID, constData, 4);
				constData += 4;
//				if(modelID == 0x4220) {
					QString name(one.mid(constData - one.constData(), 8));
					qDebug() << name;
					constData += 8;
					memcpy(&modelID, constData, 4);
					constData += 4;
					if(modelID != 0xEEEEEEEE) {
						qWarning() << "Unknown format (6)!" << QString::number(modelID, 16);
//						return false;
					}

					models.append(CharaModel(name, FF8Image::tim(one.mid(offset+4, size))));
//				} else {
//					qWarning() << "Unknown format (5)!" << QString::number(modelID, 16);
//					return false;
//				}
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
