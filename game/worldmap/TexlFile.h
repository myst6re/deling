#ifndef TEXLFILE_H
#define TEXLFILE_H

#include <QtCore>
#include "game/worldmap/Map.h"
#include "files/TimFile.h"

#define TEXLFILE_TEXTURE_SIZE 0x12800
#define TEXLFILE_TEXTURE_COUNT 20

class TexlFile
{
public:
	explicit TexlFile(QIODevice *io = Q_NULLPTR);
	inline void setDevice(QIODevice *device) {
		_io = device;
	}

	bool readTextures(Map &map);

private:
	bool seekTexture(quint8 id);
	bool readTexture(TimFile &tim);

	QIODevice *_io;
};

#endif // TEXLFILE_H
