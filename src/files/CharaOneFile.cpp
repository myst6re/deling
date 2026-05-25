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

bool CharaOneFile::open(const QByteArray &one, const QByteArray &pcb, bool ps)
{
	_models.clear();

	if (one.size() < 0x800) {
		qWarning() << "CharaOneFile::open chara file too short" << one.size();
		return false;
	}

	quint32 count=0, offset, size, modelID;
	const char *constData = one.constData();
	const char * const startData = constData;
	PcbFile pcbFile;
	QMap<QRgb, quint16> calcDefaultLightColor;

	if (!pcb.isEmpty()) {
		pcbFile.open(pcb);
	}

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
			//qDebug() << "size twice!";
			memcpy(&modelID, constData, 4);
			constData += 4;
		}

		quint32 animationOffset = 0, modelOffset = 0, timOffset = 0;
		QList<quint64> offsets;

		if ((modelID & 0xF0000000) == 0xD0000000) {
			//qDebug() << "D modelID (HRC)" << (modelID & 0xFFFF) << QString::number(modelID, 16);
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
		quint32 nextOffset, externalTextureModelLoaderId = 0xEEEEEEEE;
		QRgb lightColor = qRgb(0x80, 0x80, 0x80);

		memcpy(&nextOffset, constData, 4);

		if (nextOffset != 0 && nextOffset != offset + size) { // Not testno format
			QByteArray nameBA = one.mid(constData - startData, 4);
			name = QString::fromLatin1(nameBA, -1);
//			qDebug() << name;
			constData += 4;
			lightColor = qRgb(constData[0], constData[1], constData[2]);
			constData += 4;
			memcpy(&externalTextureModelLoaderId, constData, 4);
			constData += 4;
		}

		// Override name with what's actually used by the game
		if ((modelID & 0xF0000000) == 0xD0000000) {
			name = QString("d%1").arg(modelID & 0xFFFF, 3, 10, QChar('0'));
		}

		if (!pcb.isEmpty()) {
			lightColor = pcbFile.modelLightColor(name);
		} else if (calcDefaultLightColor.contains(lightColor)) {
			calcDefaultLightColor[lightColor] += 1;
		} else {
			calcDefaultLightColor.insert(lightColor, 0);
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
			} else {
				textures.append(TimFile(data.mid(pos, sizePart)));
				if (!textures.last().isValid()) {
					qWarning() << "CharaModel::open tim error: unknown format!" << name << i;
				}
			}
		}

		model.setLightColor(lightColor);
		model.setTextures(textures);
		if ((modelID & 0xF0000000) == 0xA0000000) {
			model.setSharedTextureModelId((modelID >> 20) & 0xFF);
		}

		_models.append(model);
	}

	_ps = ps;
	if (!pcb.isEmpty()) {
		_defaultLightColor = pcbFile.defaultModelLightColor();
	} else {
		_defaultLightColor = qRgb(0x80, 0x80, 0x80);
		quint16 count = 0;
		QMapIterator<QRgb, quint16> it(calcDefaultLightColor);
		while (it.hasNext()) {
			it.next();
			if (it.value() > count) {
				count = it.value();
				_defaultLightColor = it.key();
			}
		}
	}

	return true;
}

bool CharaOneFile::save(QByteArray &one, QByteArray &pcb) const
{
	QByteArray header;
	const qsizetype padding = 0x800;
	const qsizetype headerSize = padding;
	const qsizetype paddedHeaderSize = (headerSize / padding + int(headerSize % padding != 0)) * padding;
	one = QByteArray(paddedHeaderSize, '\0');
	QList< QPair<QString, QRgb> > modelLightColors;

	if (!_ps) {
		quint32 count = quint32(_models.size());
		header.append((const char *)&count, 4);
	}

	for (const CharaModel &model: _models) {
		quint32 offset = one.size();
		header.append((const char *)&offset, 4);

		QByteArray modelHeader, mchData, modelData;

		if (model.loadingType() == CharaModel::External) {
			quint32 modelID = 0xD0000000 | (quint32(model.scale()) << 16) | model.id();
			modelHeader.append((const char *)&modelID, 4);
			quint32 animationOffset = mchData.size();
			modelHeader.append((const char *)&animationOffset, 4);
			mchData.append(MchFile::writeAnimations(model));
		} else if (model.loadingType() == CharaModel::LocalSharedTexture) {
			quint32 sharedTextureModelId = 0xA0000000 | (model.sharedTextureModelId() << 20) | ((offset - 0x800) & 0xFFFFF);
			modelHeader.append((const char *)&sharedTextureModelId, 4);
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
		if (_ps) {
			// FIXME: unknown value for PS version only
			header.append((const char *)&size, 4);
		} else {
			// Redundant but mandatory
			header.append((const char *)&size, 4);
		}
		header.append(modelHeader);
		header.append(model.name().toLatin1().leftJustified(4, '\0', true));
		QRgb lightColor = model.lightColor();
		header.append(char(qRed(lightColor)));
		header.append(char(qGreen(lightColor)));
		header.append(char(qBlue(lightColor)));
		header.append('\0');
		if (lightColor != _defaultLightColor) {
			modelLightColors.append(qMakePair(model.name(), lightColor));
		}
		quint32 externalModelLoaderId = model.loadingType() == CharaModel::LocalSharedTexture ? model.sharedTextureModelId() : 0xEEEEEEEE;
		header.append((const char *)&externalModelLoaderId, 4);

		if (_ps) {
			QByteArray compressedData = LZS::compress(mchData);
			quint32 compressedDataSize = compressedData.size();
			one.append((const char *)&compressedDataSize, 4);
			one.append(compressedData);
		} else {
			one.append("\0\0\0\0", 4);
			one.append(mchData.constData(), mchData.size() - 4);
		}
	}

	one.replace(0, paddedHeaderSize, header.leftJustified(paddedHeaderSize, '\0', true));

	if (!_ps) {
		one.append("\0\0\0\0", 4);
	}

	PcbFile pcbFile;
	pcbFile.setModelLightColors(modelLightColors);
	pcbFile.setDefaultModelLightColor(_defaultLightColor);
	pcbFile.save(pcb);

	return true;
}

void CharaOneFile::setModel(int id, const CharaModel &model)
{
	_models.replace(id, model);
	modified = true;
}

void CharaOneFile::insertModel(int id, const CharaModel &model)
{
	_models.insert(id, model);
	modified = true;
}

void CharaOneFile::removeModel(int id)
{
	_models.removeAt(id);
	modified = true;
}

bool CharaOneFile::upModel(int id)
{
	if (id <= 0 || id >= _models.size()) {
		return false;
	}

	_models.swapItemsAt(id, id - 1);
	modified = true;

	return true;
}

bool CharaOneFile::downModel(int id)
{
	if (id < 0 || id + 1 >= _models.size()) {
		return false;
	}

	_models.swapItemsAt(id, id + 1);
	modified = true;

	return true;
}

bool CharaOneFile::searchModel(const QString &name, CharaModel &modelOut, bool withStayWalkRunAnimations)
{
	for (const CharaModel &model: _models) {
		if (model.name() == name && (!withStayWalkRunAnimations || model.animations().size() >= 3)) {
			modelOut = model;
			return true;
		}
	}

	return false;
}

void CharaOneFile::setDefaultLightColor(QRgb defaultLightColor)
{
	for (int i = 0; i < _models.size(); ++i) {
		CharaModel &m = model(i);
		if (m.lightColor() == _defaultLightColor) {
			m.setLightColor(defaultLightColor);
		}
	}
	_defaultLightColor = defaultLightColor;
	modified = true;
}
