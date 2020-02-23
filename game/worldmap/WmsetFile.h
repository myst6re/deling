#ifndef WMSETFILE_H
#define WMSETFILE_H

#include <QtCore>
#include "game/worldmap/Map.h"

#define OBJFILE_SECTION_COUNT      48
#define OBJFILE_SPECIAL_TEX_OFFSET 9
#define OBJFILE_SPECIAL_TEX_COUNT  36

class WmsetFile
{
public:
	explicit WmsetFile(QIODevice *io = Q_NULLPTR);
	inline void setDevice(QIODevice *device) {
		_io = device;
	}
	bool extract(quint32 id, const QString &fileName);
	bool extract(const QString &name, const QString &dirName);
	static bool build(const QString &dirName, const QString &fileName);
	bool readEncounters(Map &map);
	bool readEncounterRegions(Map &map);
	bool readSpecialTextures(Map &map);
	bool readRoadTextures(Map &map);
private:
	bool extract(quint32 offset, quint32 size, const QString &fileName);
	bool openToc();
	bool sectionInfos(quint32 id, quint32 &offset, quint32 &size);

	QIODevice *_io;
	QList<quint32> _toc;
};

#endif // WMSETFILE_H
