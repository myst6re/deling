#ifndef MAP_H
#define MAP_H

#include <QtCore>
#include "game/worldmap/MapSegment.h"
#include "game/worldmap/WmEncounter.h"
#include "files/TimFile.h"

class Map
{
public:
	enum SpecialTextureName {
		Moon = 0,
		Sky,
		MiniMap,
		Unknown1,
		Unknown2,
		Explosion,
		Unknown3,
		Sea1,
		Sea2,
		Sea3,
		Cascade,
		Sea4,
		Beach1,
		Beach2,
		Sea5,
		Unknown4,
		Unknown5,
		Unknown6,
		Unknown7,
		Unknown8,
		Unknown9,
		Unknown10,
		Unknown11,
		Unknown12,
		Unknown13,
		Unknown14,
		Unknown15,
		Unknown16
	};

	Map();

	inline const QList<MapSegment> &segments() const {
		return _segments;
	}

	inline void setSegments(const QList<MapSegment> &segments) {
		_segments = segments;
	}

	inline const QList<WmEncounter> &encounters() const {
		return _encounters;
	}

	QList<WmEncounter> encounters(quint8 region) const;

	inline void setEncounters(const QList<WmEncounter> &encounters) {
		_encounters = encounters;
	}

	inline const QList<quint8> &encounterRegions() const {
		return _encounterRegions;
	}

	inline void setEncounterRegions(const QList<quint8> &regions) {
		_encounterRegions = regions;
	}

	inline const QList<TimFile> &textures() const {
		return _textures;
	}

	inline void setTextures(const QList<TimFile> &textures) {
		_textures = textures;
	}

	inline const QList<TimFile> &lowResTextures() const {
		return _lowResTextures;
	}

	inline void setLowResTextures(const QList<TimFile> &textures) {
		_lowResTextures = textures;
	}

	inline const TimFile specialTexture(SpecialTextureName name) const {
		return _specialTextures[name];
	}

	inline void setSpecialTextures(const QMap<SpecialTextureName, TimFile> &textures) {
		_specialTextures = textures;
	}

	inline const QList<TimFile> &roadTextures() const {
		return _roadTextures;
	}

	inline void setRoadTextures(const QList<TimFile> &textures) {
		_roadTextures = textures;
	}

	QList<QList<QPair<QImage, bool> > > textureImages() const;
	QImage specialTextureImage(SpecialTextureName name) const;
	QImage seaTextureImage() const {
		return specialTextureImage(Sea1, Sea5);
	}
	QImage roadTextureImage() const {
		return composeTextureImage(_roadTextures);
	}
	void searchBlackPixels(const QList<QList<QImage> > &textures,
	                       const QImage &seaTexture, const QImage &roadTexture);

private:
	QImage specialTextureImage(SpecialTextureName min, SpecialTextureName max) const;
	static QImage composeTextureImage(const QList<TimFile> &tims);
	static bool searchBlackPixelsTexture(const QImage &texture,
	                                     const QList<TexCoord> &tc);

	QList<MapSegment> _segments;
	QList<WmEncounter> _encounters;
	QList<quint8> _encounterRegions;
	QList<TimFile> _textures, _lowResTextures;
	QMap<SpecialTextureName, TimFile> _specialTextures;
	QList<TimFile> _roadTextures;
};

#endif // MAP_H
