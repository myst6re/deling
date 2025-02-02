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
#include "Map.h"
#include <QPainter>

Map::Map()
{

}

QList<MapSegment> Map::segments(SegmentFiltering filtering) const
{
	if (filtering == NoFiltering) {
		return _segments;
	}

	int endOfMap = 32 * 24;
	QList<MapSegment> ret = _segments.mid(0, endOfMap);

	// 32 * 24
	for (int i = NoEsthar; i <= WithDesertPrison; i <<= 1) {
		if (filtering & i) {
			switch (i) {
			case NoEsthar:
				for (int j = 0; j < 7; ++j) {
					for (int k = 0; k < 8; ++k) {
						ret.replace(373 + j * 32 + k, _segments.at(endOfMap + j * 8 + k));
					}
				}
				break;
			case TGUAlternative:
				ret.replace(149 + 0, _segments.at(endOfMap + 7 * 8 + 0));
				ret.replace(149 + 1, _segments.at(endOfMap + 7 * 8 + 1));
				break;
			case WithGGU:
				ret.replace(267, _segments.at(endOfMap + 7 * 8 + 2));
				break;
			case WithBGU:
				ret.replace(274 + 0, _segments.at(endOfMap + 7 * 8 + 2 + 1 + 0));
				ret.replace(275 + 1, _segments.at(endOfMap + 7 * 8 + 2 + 1 + 1));
				break;
			case WithMissileBase:
				ret.replace(327, _segments.at(endOfMap + 7 * 8 + 2 + 1 + 2));
				break;
			case TrabiaCraterAlternative:
				for (int j = 0; j < 2; ++j) {
					for (int k = 0; k < 2; ++k) {
						ret.replace(214 + j * 32 + k, _segments.at(endOfMap + 7 * 8 + 2 + 1 + 2 + 1 + j * 2 + k));
					}
				}
				break;
			case WithDesertPrison:
				ret.replace(361, _segments.at(endOfMap + 7 * 8 + 2 + 1 + 2 + 1 + 2 * 2));
				break;
			}
		}
	}
	
	return ret;
}

QList<WmEncounter> Map::encounters(quint8 region) const
{
	QList<WmEncounter> ret;
	
	foreach (const WmEncounter &enc, _encounters) {
		if (enc.region() == region) {
			ret.append(enc);
		}
	}
	
	return ret;
}

QList<QList<QPair<QImage, bool> > > Map::textureImages() const
{
	QList<QList<QPair<QImage, bool> > > ret;
	
	foreach (const TimFile &tim, _textures) {
		QList<QPair<QImage, bool> > images;
		for (int palID = 0; palID < tim.colorTableCount(); ++palID) {
			QImage img = tim.image(palID);
			QVector<QRgb> colorTable = img.colorTable();
			bool hasAlpha = false;
			for (int i = 0 ; i < colorTable.size(); ++i) {
				if (colorTable.at(i) == qRgba(0, 0, 0, 0)) {
					hasAlpha = true;
					break;
				}
			}
			img.setColorTable(colorTable);
			images.append(qMakePair(img, hasAlpha));
		}
		ret.append(images);
	}
	
	return ret;
}

QImage Map::specialTextureImage(SpecialTextureName name) const
{
	return _specialTextures.value(name).image();
}

QImage Map::composeTextureImage(const QList<TimFile> &tims)
{
	QRect maxImgRect;
	
	foreach (const TimFile &tim, tims) {
		maxImgRect |= tim.imgRect();
	}
	
	QImage retImg(maxImgRect.size(), QImage::Format_ARGB32);
	retImg.fill(qRgba(0, 0, 0, 0));
	
	QPainter pImg(&retImg);
	
	foreach (const TimFile &tim, tims) {
		pImg.drawImage(tim.imgPos() - maxImgRect.topLeft(), tim.colorTableCount() == 16 && tim.image().size() == QSize(256, 256) ? tim.gridImage(4, 4) : tim.image());
	}
	
	pImg.end();
	
	return retImg;
}

QImage Map::debugTextureCoords(int textureId) const
{
	const TimFile &tim = _textures.at(textureId);
	
	QImage retImg(tim.imgSize(), QImage::Format_ARGB32);
	retImg.fill(qRgba(0, 0, 0, 0));
	
	QPainter p(&retImg);
	p.setBrush(Qt::white);
	//p.drawImage(QPoint(0, 0), tim.image());
	
	for (const MapSegment &segment: _segments) {
		for (const MapBlock &block: segment.blocks()) {
			for (const MapPoly &polygon: block.polygons()) {
				if (polygon.texPage() == textureId) {
					p.drawLine(
						QPoint(polygon.texCoord(0).x, polygon.texCoord(0).y),
						QPoint(polygon.texCoord(1).x, polygon.texCoord(1).y)
						);
				}
			}
			
		}
	}
	
	p.end();
	
	return retImg;
}

QImage Map::specialTextureImage(SpecialTextureName min, SpecialTextureName max) const
{
	QList<TimFile> tims;
	
	for (int i = int(min); i <= int(max); ++i) {
		tims.append(_specialTextures[SpecialTextureName(i)]);
	}
	
	return composeTextureImage(tims);
}

QImage Map::megaImage() const
{
	int row = 0, col = 0;

	QImage retImg(QSize(5, 5) * 256, QImage::Format_ARGB32);
	retImg.fill(qRgba(0, 0, 0, 0));
	QPainter pImg(&retImg);
	
	for (const TimFile &tim: _textures) {
		pImg.drawImage(QPoint(col, row) * 256, tim.gridImage(4, 4));
		
		row += 1;
		
		if (row >= 5) {
			row = 0;
			col += 1;
		}
	}
	
	pImg.drawImage(QPoint(col, row) * 256, seaTextureImage());
	row += 1;
	if (row >= 5) {
		row = 0;
		col += 1;
	}
	
	pImg.drawImage(QPoint(col, row) * 256, roadTextureImage());
	pImg.end();

	return retImg;
}

static double triangleArea(const TexCoord &a, const TexCoord &b, const TexCoord &c)
{
	return abs((a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y)) / 2.0);
}

static double triangleArea(const QList<TexCoord> &tc)
{
	return triangleArea(tc.at(0), tc.at(1), tc.at(2));
}

bool Map::searchBlackPixelsTexture(const QImage &texture,
								   const QList<TexCoord> &tc)
{
	double area = triangleArea(tc);
	TexCoord point;
	quint8 maxY = texture.height() - 1,
		maxX = texture.width() - 1;
	
	for(int y = 0; y <= maxY; y++) {
		for(int x = 0; x <= maxX; x++) {
			point.x = x;
			point.y = y;
			double area1 = triangleArea(point, tc.at(1), tc.at(2)),
				area2 = triangleArea(tc.at(0), point, tc.at(2)),
				area3 = triangleArea(tc.at(0), tc.at(1), point);
			
			if (area == area1 + area2 + area3
				&& texture.color(point.y * texture.width() + point.x) == qRgba(0, 0, 0, 0)) {
				return true;
			}
		}
	}
	
	return false;
}

void Map::searchBlackPixels(const QList<QList<QImage> > &textures,
							const QImage &seaTexture, const QImage &roadTexture)
{
	QSet<quint64> visited;
	int segmentId = 0;
	foreach (MapSegment segment, segments()) {
		int blockId = 0;
		foreach (MapBlock block, segment.blocks()) {
			int polyId = 0;
			foreach (MapPoly poly, block.polygons()) {
				const QList<TexCoord> &tc = poly.texCoords();
				quint64 coordHash = tc.at(0).x | (quint64(tc.at(0).y) << 8)
									| (quint64(tc.at(1).x) << 16) | (quint64(tc.at(1).y) << 24)
									| (quint64(tc.at(2).x) << 32) | (quint64(tc.at(2).y) << 40);
				
				if (visited.contains(coordHash)) {
					polyId++;
					continue;
				}
				
				QImage texture;
				
				if (poly.isRoadTexture()) {
					texture = roadTexture;
				} else if (poly.isWaterTexture()) {
					texture = seaTexture;
				} else {
					texture = textures.at(poly.texPage()).at(poly.clutId());
				}
				
				poly.setHasBlackPixels(searchBlackPixelsTexture(texture, tc));
				visited.insert(coordHash);
				block.setPolygon(polyId++, poly);
			}
			
			segment.setBlock(blockId++, block);
		}
		
		_segments[segmentId++] = segment;
	}
}
