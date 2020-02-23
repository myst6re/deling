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

QList<QList<QImage> > Map::textureImages() const
{
	QList<QList<QImage> > ret;

	foreach (const TimFile &tim, _textures) {
		QList<QImage> images;
		for (int palID = 0; palID < tim.colorTableCount(); ++palID) {
			QImage img = tim.image(palID);
			QVector<QRgb> colorTable = img.colorTable();
			for (int i = 0 ; i < colorTable.size(); ++i) {
				if (colorTable.at(i) == qRgba(0, 0, 0, 0)) {
					colorTable[i] = qRgba(255, 0, 0, 127);
				}
			}
			img.setColorTable(colorTable);
			images.append(img);
		}
		ret.append(images);
	}

	return ret;
}
/*
QList<QImage> Map::seaTextureImage() const
{
	QList<QImage> images;
	for (int palID = 0; palID < tim.colorTableCount(); ++palID) {
		images.append(tim.image(palID));
	}
	ret.append(images);

	return specialTextureImage(Sea1, Sea5);
}

QList<QImage> Map::roadTextureImage() const
{
	return composeTextureImage(_roadTextures);
}

QList<QList<QImage> > Map::roadTextureImages() const
{
	QList<QList<QImage> > ret;

	foreach (const TimFile &tim, _textures) {
		QList<QImage> images;
		for (int palID = 0; palID < tim.colorTableCount(); ++palID) {
			images.append(tim.image(palID));
		}
		ret.append(images);
	}

	return ret;
} */

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

/* QImage Map::waterTextureImage() const
{
	QImage ret(QSize(256, 256), QImage::Format_RGB32);
	ret.fill(qRgb(35, 60, 75)); // Blue
	return ret;
} */
