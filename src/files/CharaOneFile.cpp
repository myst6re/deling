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
#include "CharaOneFile.h"
#include "../LZS.h"
#include "MchFile.h"

CharaOneFile::CharaOneFile() :
	File(), _ps(false)
{
}

CharaOneFile::~CharaOneFile()
{
}

bool CharaOneFile::open(const QByteArray &one, bool ps)
{
	models.clear();

	if (one.size() < 0x800) {
		qWarning() << "CharaOneFile::open chara file too short" << one.size();
		return false;
	}

	quint32 count=0, offset, size, modelID;
	const char *constData = one.constData();
	const char * const startData = constData;

	if (!ps) {
		memcpy(&count, constData, 4);
		constData += 4;
//		qDebug() << "count" << count;
	}

	for (quint32 i=0 ; (ps || i<count) && constData - startData < 0x800 ; ++i) {
		memcpy(&offset, constData, 4);
		constData += 4;
//		qDebug() << i << "offset" << QString::number(offset, 16);
		if (offset == 0) {
			break;
		}
		memcpy(&size, constData, 4);
		constData += 4;
//		qDebug() << "size" << size;

		memcpy(&modelID, constData, 4);
		constData += 4;

		if (modelID == size || ps) {
			qDebug() << "size twice!";
			memcpy(&modelID, constData, 4);
			constData += 4;
		}

		quint32 animationOffset = 0, modelOffset = 0, timOffset = 0;
		QList<quint64> offsets;

		if ((modelID & 0xF0000000) == 0xD0000000) {
			qDebug() << "D modelID (HRC)" << (modelID & 0xFFFF) << QString::number(modelID, 16);
			memcpy(&animationOffset, constData, 4);
			constData += 4;
			offsets.append(quint64(0xD000000000) | quint64(animationOffset));
		} else if ((modelID & 0xF0000000) == 0xA0000000) {
			// Always 0xFFFFFFFF
			constData += 4;
			memcpy(&modelOffset, constData, 4);
			offsets.append(quint64(0xA000000000) | quint64(modelOffset));
			constData += 4;
		} else {
			timOffset = modelID;
			if (qint32(timOffset) >= 0) {
				offsets.append(timOffset & 0xFFFFFF);

				while (constData - startData < 0x800) {
					memcpy(&timOffset, constData, 4);
					constData += 4;

					if (qint32(timOffset) < 0) {
						break;
					}

					offsets.append(timOffset & 0xFFFFFF);
				}
			}

			memcpy(&modelOffset, constData, 4);
			offsets.append(quint64(0xA000000000) | quint64(modelOffset));
			constData += 4;
		}
//		qDebug() << "model data offset" << QString::number(modelOffset, 16);

		offsets.append(size);

		QString name;
		quint32 nextOffset, unknownFlags = 0x00808080, externalTextureModelLoaderId = 0xEEEEEEEE;

		memcpy(&nextOffset, constData, 4);

		if (nextOffset != 0 && nextOffset != offset + size) { // Not testno format
			QByteArray nameBA = one.mid(constData - startData, 4);
			name = QString::fromLatin1(nameBA, qstrnlen(nameBA, 4));
//			qDebug() << name;
			constData += 4;
			memcpy(&unknownFlags, constData, 4);
			constData += 4;
			memcpy(&externalTextureModelLoaderId, constData, 4);
			constData += 4;
		} else if ((modelID & 0xF0000000) == 0xD0000000) {
			name = QString("d%1").arg(modelID & 0xFFFF, 3, 10, QChar('0'));
		}

		if (!ps) {
			offset += 4;
		}

		QByteArray data = one.mid(offset, size);

		if (ps) {
			quint32 lzsSize = 0;
			memcpy(&lzsSize, data.constData(), 4);
			if ((quint32)data.size() < lzsSize + 4) {
				qWarning() << "CharaOneFile::open Compression error" << i << lzsSize << data.size();
				return false;
			}
			data = LZS::decompress(data.constData() + 4, lzsSize);
		}

		QList<TimFile> textures;
		CharaModel model;

		for (int i = 0; i < offsets.size() - 1; ++i) {
			quint32 pos = offsets.at(i) & 0xFFFFFFFF, sizePart = (offsets.at(i + 1) & 0xFFFFFFFF) - pos;
			quint8 flag = offsets.at(i) >> 32;
			if (pos + sizePart > data.size()) {
				qWarning() << "CharaModel::open invalid offsets" << name << flag << pos << sizePart << data.size();
				break;
			}
			if (flag == 0xD0) {
				model = CharaModel(name, (modelID >> 16) & 0xFF, MchFile::readAnimations(data.constData() + pos, sizePart));
			} else if (flag == 0xA0) {
				if (!MchFile::readFullModel(data.constData() + pos, sizePart, name, model)) {
					qWarning() << "CharaModel::open cannot read full model" << name;
				}
				model.setNoTextureFlag(modelID & 0xFFFFFF);
			} else {
				textures.append(TimFile(data.mid(pos, sizePart)));
				if (!textures.last().isValid()) {
					qWarning() << "CharaModel::open tim error: unknown format!" << name << i;
				}
			}
		}

		model.setUnknownFlags(unknownFlags);
		model.setTextures(textures);
		model.setExternalTextureModelLoaderId(externalTextureModelLoaderId);

		models.append(model);
	}

	_ps = ps;

	return true;
}

bool CharaOneFile::save(QByteArray &data) const
{
	QByteArray header;
	const qsizetype padding = 0x800;
	const qsizetype headerSize = padding;
	const qsizetype paddedHeaderSize = (headerSize / padding + int(headerSize % padding != 0)) * padding;
	data = QByteArray(paddedHeaderSize, '\0');

	if (!_ps) {
		quint32 count = quint32(models.size());
		header.append((const char *)&count, 4);
	}

	for (const CharaModel &model: models) {
		quint32 offset = data.size();
		header.append((const char *)&offset, 4);

		QByteArray modelHeader, mchData, modelData;

		if (model.isExternal()) {
			quint32 modelID = 0xD0000000 | (quint32(model.scale()) << 16) | model.id();
			modelHeader.append((const char *)&modelID, 4);
			quint32 animationOffset = mchData.size();
			modelHeader.append((const char *)&animationOffset, 4);
			mchData.append(MchFile::writeAnimations(model));
		} else if (model.textures().isEmpty()) {
			quint32 noTextureFlag = 0xA0000000 | model.noTextureFlag();
			modelHeader.append((const char *)&noTextureFlag, 4);
			modelHeader.append("\xFF\xFF\xFF\xFF", 4);
			quint32 modelOffset = mchData.size();
			modelHeader.append((const char *)&modelOffset, 4);
			mchData.append(MchFile::writeFullModel(model));
		} else {
			qsizetype textureCount = model.textures().size();
			for (const TimFile &texture: model.textures()) {
				QByteArray textureData;
				if (texture.save(textureData)) {
					textureCount -= 1;
					// `textureCount` is skipped by the game, we put it anyway to respect the file format
					// It is more important here to not transform `offset` to a negative value than having a correct `textureCount`
					// so we prevent textureCount to set the high bit of offset
					quint32 textureOffset = quint32(mchData.size()) | ((textureCount & 0x7) << 28);
					modelHeader.append((const char *)&textureOffset, 4);
					mchData.append(textureData);
				}
			}
			modelHeader.append("\xFF\xFF\xFF\xFF", 4);
			quint32 modelOffset = mchData.size();
			modelHeader.append((const char *)&modelOffset, 4);
			mchData.append(MchFile::writeFullModel(model));
		}

		// The data is padded, maybe related to how the file is loaded on PSX
		const qsizetype paddedDataSize = (mchData.size() / padding + int(mchData.size() % padding != 0)) * padding;
		mchData = mchData.leftJustified(paddedDataSize, '\0');

		quint32 size = mchData.size();
		header.append((const char *)&size, 4);
		// Redundant but mandatory
		header.append((const char *)&size, 4);
		header.append(modelHeader);
		header.append(model.name().toLatin1().leftJustified(4, '\0', true));
		quint32 unknownFlags = model.unknownFlags();
		header.append((const char *)&unknownFlags, 4);
		quint32 externalModelLoaderId = model.externalTextureModelLoaderId();
		header.append((const char *)&externalModelLoaderId, 4);

		if (_ps) {
			QByteArray compressedData = LZS::compress(mchData);
			quint32 compressedDataSize = compressedData.size();
			data.append((const char *)&compressedDataSize, 4);
			data.append(compressedData);
		} else {
			data.append("\0\0\0\0", 4);
			data.append(mchData.constData(), mchData.size() - 4);
		}
	}

	data.replace(0, paddedHeaderSize, header.leftJustified(paddedHeaderSize, '\0', true));

	if (!_ps) {
		data.append("\0\0\0\0", 4);
	}

	return true;
}

const CharaModel &CharaOneFile::model(int id) const
{
	return models.at(id);
}

void CharaOneFile::setModel(int id, const CharaModel &model)
{
	models.replace(id, model);
	modified = true;
}

int CharaOneFile::modelCount() const
{
	return models.size();
}
