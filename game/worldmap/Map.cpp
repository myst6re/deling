#include "Map.h"

Map::Map()
{

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
					//colorTable[i] = qRgba(255, 0, 0, 100);
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
		pImg.drawImage(tim.imgPos() - maxImgRect.topLeft(), tim.image());
	}

	pImg.end();

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
