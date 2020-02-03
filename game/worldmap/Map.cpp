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
			images.append(tim.image(palID));
		}
		ret.append(images);
	}

	return ret;
}
