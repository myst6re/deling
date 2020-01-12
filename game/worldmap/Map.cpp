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
