#ifndef DATFILE_H
#define DATFILE_H

#include "files/IO.h"
#include "files/TimFile.h"
#include "game/battle/BattleModel.h"

#define DAT_FILE_SECTION_COUNT 11

struct DatFileTriangle
{
	quint16 vertexIndexes[3]; // vertexIndexes[0] &= 0x7FFF
	TexCoord texCoords1, texCoords2;
	quint16 unknown1;
	TexCoord texCoords3;
	quint16 unknown2;
};

BattleTriangle toBattleTriangle(const DatFileTriangle &triangle,
                                const QList<BattleVertex> &vertices,
                                bool *ok);

struct DatFileQuad
{
	quint16 vertexIndexes[4]; // vertexIndexes[0] &= 0x7FFF
	TexCoord texCoords1;
	quint16 unknown1;
	TexCoord texCoords2;
	quint16 unknown2;
	TexCoord texCoords3;
	TexCoord texCoords4;
};

BattleQuad toBattleQuad(const DatFileQuad &quad,
                        const QList<BattleVertex> &vertices,
                        bool *ok);

class DatFile : public IO
{
public:
	enum Section {
		Skeleton = 0,
		Parts = 1,
		Animations = 2,
		// Unknown = 3..5,
		Stats = 6,
		Ai = 7,
		Sounds = 8,
		// Unknown = 9,
		Textures = 10
	};

	explicit DatFile(QIODevice *device);

	bool readModel(BattleModel &model);
	bool readSkeleton(BattleSkeleton &skeleton);
	bool readParts(QList<BattlePart> &parts);
	bool readAnimations(QList<BattleAnimation> &animations);
	bool readTextures(QList<TimFile> &textures);
private:
	inline bool hasPositions() const {
		return !_positions.isEmpty();
	}
	inline quint32 position(Section section) const {
		return _positions[int(section)];
	}
	inline quint32 size(Section section) const {
		return _positions[int(section) + 1] - position(section);
	}
	bool readPositions();
	bool readPositionsIfNotFilled() {
		return hasPositions() || readPositions();
	}

	QVector<quint32> _positions;
};

#endif // DATFILE_H
