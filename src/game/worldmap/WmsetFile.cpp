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
#include "WmsetFile.h"
#include "game/worldmap/WmEncounter.h"

WmsetFile::WmsetFile(QIODevice *io) :
    _io(io)
{
}

bool WmsetFile::extract(quint32 offset, quint32 size, const QString &fileName)
{
	if (!_io->seek(offset)) {
		return false;
	}

	QByteArray data = _io->read(size);
	if (data.size() != size) {
		return false;
	}

	QFile f(fileName);
	if (!f.open(QIODevice::WriteOnly)) {
		return false;
	}

	return f.write(data) == data.size();
}

bool WmsetFile::extract(quint32 id, const QString &fileName)
{
	quint32 offset, size;

	if (!sectionInfos(id, offset, size)) {
		return false;
	}

	return extract(offset, size, fileName);
}

bool WmsetFile::extract(const QString &fileName, const QString &dirName)
{
	if (_toc.isEmpty() && !openToc()) {
		return false;
	}

	QFileInfo info(fileName);
	QString name = info.completeBaseName(), ext = info.suffix();

	QDir dir(dirName);
	for (int i = 0; i < _toc.size() - 1; ++i) {
		quint32 offset = _toc.at(i);
		int size = _toc.at(i + 1) - offset;
		if (size < 0) {
			qWarning() << "WmsetFile::extract Invalid section size" << i;
		} else if (!extract(offset, size, dir.filePath(QString("%1.part%2.%3")
		                                         .arg(name)
		                                         .arg(i, 2, 10, QChar('0'))
		                                         .arg(ext)))) {
			qWarning() << "WmsetFile::extract Cannot extract file" << i;
		}
	}

	return true;
}

bool WmsetFile::build(const QString &dirName, const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		return false;
	}

	// Fake empty toc
	int headerSize = (OBJFILE_SECTION_COUNT + 1) * sizeof(quint32);
	if (file.write(QByteArray(headerSize, '\0')) != headerSize) {
		return false;
	}

	QDir dir(dirName);
	QStringList fileNames = dir.entryList(QStringList() << "*.part*.*",
	                                      QDir::Files, QDir::Name);
	quint32 toc[OBJFILE_SECTION_COUNT];
	QRegularExpression regExpPart("\\.part(\\d+)\\.");

	// Data
	int oldCurToc = 0;
	foreach (const QString &name, fileNames) {

		QRegularExpressionMatch match = regExpPart.match(name);
		bool ok;
		int number = match.captured(1).toInt(&ok);

		if (ok && number < OBJFILE_SECTION_COUNT) {
			if (oldCurToc + 1 < number) {
				// Not found files set to empty size
				for (int i = oldCurToc; i < number; ++i) {
					toc[i] = file.pos();
					qDebug() << i << QString::number(file.pos(), 16);
				}
			}
			toc[number] = file.pos();
			oldCurToc = number;

			QFile f(dir.filePath(name));
			if (!f.open(QIODevice::ReadOnly)) {
				qWarning() << "WmsetFile::build Cannot open file" << f.fileName();
			} else {
				file.write(f.readAll());
			}
		} else {
			qWarning() << "WmsetFile::build Cannot find partN in filename"
			           << dir.filePath(name);
		}
	}

	if (oldCurToc + 1 < OBJFILE_SECTION_COUNT) {
		// Not found files set to empty size
		for (int i = oldCurToc; i < OBJFILE_SECTION_COUNT; ++i) {
			toc[i] = file.pos();
		}
	}

	// Toc
	file.reset();
	headerSize -= sizeof(quint32);
	if (file.write((char *)toc, headerSize) != headerSize) {
		return false;
	}

	return true;
}

QByteArray WmsetFile::readSection(int id)
{
	if ((_toc.isEmpty() && !openToc()) || id >= _toc.size()) {
		return QByteArray();
	}

	const quint32 sizeSection = _toc.at(id + 1) - _toc.at(id);

	_io->seek(_toc.at(id));

	return _io->read(sizeSection);
}

QByteArray WmsetFile::tocData()
{
	if (_toc.isEmpty() && !openToc()) {
		return QByteArray();
	}

	QByteArray out;
	out.reserve(4 * _toc.size());
	
	for (qsizetype i = 0; i < _toc.size() - 1; ++i) {
		const quint32 entry = _toc.at(i);
		out.append((const char *)&entry, 4);
	}
	out.append(4, '\0');
	
	return out;
}

bool WmsetFile::readEncounters(Map &map)
{
	if ((_toc.isEmpty() && !openToc()) || _toc.size() < 7) {
		return false;
	}

	const quint32
	        offsetSection1 = _toc.at(0),
	        offsetSection2 = _toc.at(1),
	        offsetSection3 = _toc.at(2),
	        offsetSection4 = _toc.at(3),
	        offsetSection5 = _toc.at(4),
	        offsetSection6 = _toc.at(5),
	        offsetSection7 = _toc.at(6),
	        sizeSection1 = (offsetSection1 - offsetSection2) - 8,
	        sizeSection3 = offsetSection4 - offsetSection3,
	        sizeSection4 = offsetSection5 - offsetSection4,
	        sizeSection5 = offsetSection6 - offsetSection5,
	        sizeSection6 = offsetSection7 - offsetSection6;

	_io->reset();
	QByteArray data = _io->read(offsetSection7);
	const char *d = data.constData();

	QList<WmEncounter> encounters;

	for (int i = 0; i < int(sizeSection1); i += 4) {
		quint8 regionId = quint8(data.at(int(offsetSection1) + i)),
		       groundId = quint8(data.at(int(offsetSection1) + i + 1)),
		       esi = quint8(data.at(int(offsetSection1) + i + 2));

		if (quint32(esi) >= sizeSection3 || quint32(esi) * 16 >= sizeSection4) {
			qDebug() << "Warning: Bad ESI" << esi;
			continue;
		}

		quint8 flags = quint8(data.at(int(offsetSection3 + esi))),
		       flagsLunarCry = 0;
		QList<quint16> scenes, scenesLunarCry;

		for (int j = 0; j < 8; ++j) {
			quint16 sceneId;
			memcpy(&sceneId, d + offsetSection4, sizeof(quint16));
			scenes.append(sceneId);
		}

		if (regionId == 10) { // Esthar
			esi -= 80;

			if (esi >= sizeSection5 || quint32(esi) * 16 >= sizeSection6 || int(offsetSection5 + esi) < 0) {
				qDebug() << "Warning: Bad ESI lunar cry" << esi;
				continue;
			}

			flagsLunarCry = quint8(data.at(int(offsetSection5 + esi)));

			for (int j = 0; j < 8; ++j) {
				quint16 sceneId;
				memcpy(&sceneId, d + sizeSection6, sizeof(quint16));
				scenesLunarCry.append(sceneId);
			}

		}

		encounters.append(WmEncounter(
		    scenes, flags, regionId, groundId, scenesLunarCry, flagsLunarCry
		));
	}

	map.setEncounters(encounters);

	return true;
}

bool WmsetFile::readEncounterRegions(Map &map)
{
	if ((_toc.isEmpty() && !openToc()) || _toc.size() < 3) {
		return false;
	}

	const quint32 sizeSection2 = _toc.at(2) - _toc.at(1);

	if (sizeSection2 < 24 * 32) {
		qDebug() << "Section 2 size is not right" << sizeSection2;
		return false;
	}

	_io->seek(_toc.at(1));
	QByteArray data = _io->read(sizeSection2);

	QList<quint8> regions;

	for (int i = 0; i < 24 * 32; ++i) {
		regions.append(quint8(data.at(i)));
	}

	map.setEncounterRegions(regions);

	return true;
}

bool WmsetFile::readDrawPoints(Map &map)
{
	if ((_toc.isEmpty() && !openToc()) || _toc.size() < 35) {
		return false;
	}

	const quint32 sizeSection35 = _toc.at(35) - _toc.at(34);

	if (!_io->seek(_toc.at(34))) {
		return false;
	}
	QByteArray data = _io->read(sizeSection35);

	if (data.size() != int(sizeSection35)) {
		return false;
	}

	const char *constData = data.constData();

	QList<DrawPoint> drawPoints;
	const int entryCount = data.size() / 4;

	for (int i = 0; i < entryCount; ++i) {
		DrawPoint dp;
		dp.x = data.at(i * 4),
		dp.y = data.at(i * 4 + 1);
		dp.magicID = 0;
		memcpy(&dp.magicID, constData + i * 4 + 2, 2);
		drawPoints.append(dp);
	}

	map.setDrawPoints(drawPoints);

	return true;
}

bool WmsetFile::readSpecialTextures(Map &map)
{
	if ((_toc.isEmpty() && !openToc()) || _toc.size() < 38) {
		return false;
	}

	const quint32 sizeSection38 = _toc.at(38) - _toc.at(37);

	if (!_io->seek(_toc.at(37))) {
		return false;
	}
	const int entryCount = OBJFILE_SPECIAL_TEX_COUNT;
	QByteArray data = _io->read(entryCount * 4);

	if (data.size() != entryCount * 4) {
		return false;
	}

	const quint32 *constData = (const quint32 *)data.constData();

	QList<quint32> toc;

	for (quint8 i = 0; i < entryCount; ++i) {
		toc.append(constData[i]);
	}
	toc.append(sizeSection38);

	QList<TimFile> textures;

	for (int i = 0; i < OBJFILE_SPECIAL_TEX_OFFSET; ++i) {
		_io->seek(_toc.at(37) + toc.at(i));
		QByteArray data = _io->read(toc.at(i + 1) - toc.at(i));
		textures.append(TimFile(data));
	}

	map.setLowResTextures(textures);

	QMap<Map::SpecialTextureName, TimFile> specialTextures;

	for (int i = OBJFILE_SPECIAL_TEX_OFFSET; i < toc.size() - 1; ++i) {
		_io->seek(_toc.at(37) + toc.at(i));
		QByteArray data = _io->read(toc.at(i + 1) - toc.at(i));
		specialTextures.insert(Map::SpecialTextureName(i - OBJFILE_SPECIAL_TEX_OFFSET), TimFile(data));
	}

	map.setSpecialTextures(specialTextures);

	return true;
}

bool WmsetFile::readRoadTextures(Map &map)
{
	if ((_toc.isEmpty() && !openToc()) || _toc.size() < 39) {
		return false;
	}

	const quint32 sizeSection39 = _toc.at(39) - _toc.at(38);

	if (!_io->seek(_toc.at(38))) {
		return false;
	}
	const int entryCount = 13;
	QByteArray data = _io->read(entryCount * 4);

	if (data.size() != entryCount * 4) {
		return false;
	}

	const quint32 *constData = (const quint32 *)data.constData();

	QList<quint32> toc;

	for (quint8 i = 0; i < entryCount; ++i) {
		toc.append(constData[i]);
	}
	toc.append(sizeSection39);

	QList<TimFile> textures;

	for (int i = 0; i < toc.size() - 1; ++i) {
		_io->seek(_toc.at(38) + toc.at(i));
		QByteArray data = _io->read(toc.at(i + 1) - toc.at(i));
		textures.append(TimFile(data));
	}

	map.setRoadTextures(textures);

	return true;
}

bool WmsetFile::readTexts(Map &map)
{
	if ((_toc.isEmpty() && !openToc()) || _toc.size() < 39) {
		return false;
	}
	
	const quint32 sizeSection14 = _toc.at(14) - _toc.at(13);
	
	if (!_io->seek(_toc.at(13))) {
		return false;
	}
	
	QByteArray data = _io->read(sizeSection14);
	
	if (data.size() != sizeSection14) {
		return false;
	}
	
	MsdFile msd(true);
	if (!msd.open(data)) {
		return false;
	}
	
	map.setTexts(msd);
	
	return true;
}

bool WmsetFile::openToc()
{
	_io->reset();
	const int r = OBJFILE_SECTION_COUNT * sizeof(quint32);
	QByteArray data = _io->read(r);
	if (data.size() != r) {
		qWarning() << "WmsetFile::openToc File too short";
		return false;
	}
	const quint32 *constData = (const quint32 *)data.constData();
	_toc.clear();

	for (int i = 0; i < OBJFILE_SECTION_COUNT; ++i) {
		_toc.append(constData[i]);
	}
	_toc.append(_io->size());

	return true;
}

bool WmsetFile::sectionInfos(quint32 id, quint32 &offset, quint32 &size)
{
	const int entrySize = sizeof(quint32);

	if (!_io->seek(id * entrySize)) {
		qWarning() << "WmsetFile::sectionInfos Cannot seek to" << id;
		return false;
	}

	if (_io->read((char *)&offset, entrySize) != entrySize) {
		qWarning() << "WmsetFile::sectionInfos File too short 1" << id;
		return false;
	}

	quint32 nextOffset;
	if (_io->read((char *)&nextOffset, entrySize) != entrySize) {
		qWarning() << "WmsetFile::sectionInfos File too short 2" << id;
		return false;
	}

	if (nextOffset < offset) {
		qWarning() << "WmsetFile::sectionInfos Wrong order" << id
		           << offset << nextOffset;
		return false;
	}

	size = nextOffset - offset;

	return true;
}
