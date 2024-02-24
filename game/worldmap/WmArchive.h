#ifndef WMARCHIVE_H
#define WMARCHIVE_H

#include "game/worldmap/Map.h"
#include "ArchiveObserver.h"
#include "FsArchive.h"

class WmArchive
{
public:
	WmArchive();
	bool open(const QString &filename, Map &map, ArchiveObserver *progress);
	inline const QString &errorString() const {
		return _errorString;
	}
private:
	bool _isOpen;
	QString _errorString;
	FsArchive _fsArchive;
};

#endif // WMARCHIVE_H
