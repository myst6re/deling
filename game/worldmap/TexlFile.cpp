#include "TexlFile.h"

TexlFile::TexlFile(QIODevice *io) :
    _io(io)
{
}

bool TexlFile::seekTexture(quint8 id)
{
	return _io->seek(id * TEXLFILE_TEXTURE_SIZE);
}

bool TexlFile::readTexture(TimFile &tim)
{
	QByteArray data = _io->read(TEXLFILE_TEXTURE_SIZE);

	return tim.open(data);
}

bool TexlFile::readTextures(Map &map)
{
	QList<TimFile> textures;

	_io->reset();

	for (int i = 0; i < TEXLFILE_TEXTURE_COUNT; ++i) {
		TimFile tim;
		if (!readTexture(tim)) {
			qDebug() << "TexlFile::readTextures cannot read texture";
			return false;
		}

		textures.append(tim);
	}

	map.setTextures(textures);

	return true;
}
