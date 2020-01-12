#include "WmEncounter.h"

WmEncounter::WmEncounter(const QList<quint16> &sceneIds,
                         quint8 flags, quint8 region, quint8 ground,
                         const QList<quint16> &lunarCrySceneIds,
                         quint8 lunarCryFlags) :
    _sceneIds(sceneIds), _lunarCrySceneIds(lunarCrySceneIds),
    _flags(flags), _lunarCryFlags(lunarCryFlags),
    _region(region), _ground(ground)
{
}
