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
#pragma once

#include <QtCore>
#include "game/worldmap/MapSegment.h"
#include "game/worldmap/WmEncounter.h"
#include "files/TimFile.h"
#include "files/MsdFile.h"

struct DrawPoint {
	quint8 x, y;
	quint16 magicID;
};

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
	enum SegmentFiltering {
		NoFiltering = 0,
		NoEsthar = 1,
		TGUAlternative = 2,
		WithGGU = 4,
		WithBGU = 8,
		WithMissileBase = 16,
		TrabiaCraterAlternative = 32,
		WithDesertPrison = 64,
		NoHiddenSegments = 128
	};

	Map();

	inline const QList<MapSegment> &segments() const {
		return _segments;
	}
    
    QList<MapSegment> segments(SegmentFiltering filtering) const;

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

	inline const QList<DrawPoint> &drawPoints() const {
		return _drawPoints;
	}

	inline void setDrawPoints(const QList<DrawPoint> &drawPoints) {
		_drawPoints = drawPoints;
	}
	
	inline const MsdFile &texts() const {
		return _texts;
	}
	
	inline MsdFile *textsPtr() {
		return &_texts;
	}

	inline void setTexts(const MsdFile &texts) {
		_texts = texts;
	}

	QList<QList<QPair<QImage, bool> > > textureImages() const;
	QImage specialTextureImage(SpecialTextureName name) const;
	QImage seaTextureImage() const {
		return specialTextureImage(Sea1, Sea5);
	}
	QImage roadTextureImage() const {
		return composeTextureImage(_roadTextures);
	}
	QImage megaImage() const;
	void searchBlackPixels(const QList<QList<QImage> > &textures,
	                       const QImage &seaTexture, const QImage &roadTexture);

	QImage debugTextureCoords(int textureId) const;

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
	QList<DrawPoint> _drawPoints;
	MsdFile _texts;
};
