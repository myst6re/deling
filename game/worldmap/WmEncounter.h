#ifndef WMENCOUNTER_H
#define WMENCOUNTER_H

#include <QtCore>

class WmEncounter
{
public:
	WmEncounter(const QList<quint16> &sceneIds,
	            quint8 flags, quint8 region, quint8 ground,
	            const QList<quint16> &lunarCrySceneIds = QList<quint16>(),
	            quint8 lunarCryFlags = 0);

	inline const QList<quint16> &sceneIds() const {
		return _sceneIds;
	}

	inline void setSceneIds(const QList<quint16> &sceneIds) {
		_sceneIds = sceneIds;
	}

	inline const QList<quint16> &lunarCrySceneIds() const {
		return _lunarCrySceneIds;
	}

	inline void setLunarCrySceneIds(const QList<quint16> &sceneIds) {
		_lunarCrySceneIds = sceneIds;
	}

	inline quint8 flags() const {
		return _flags;
	}

	inline void setFlags(quint8 flags) {
		_flags = flags;
	}

	inline quint8 lunarCryFlags() const {
		return _lunarCryFlags;
	}

	inline void setLunarCryFlags(quint8 flags) {
		_lunarCryFlags = flags;
	}

	inline quint8 region() const {
		return _region;
	}

	inline void setRegion(quint8 region) {
		_region = region;
	}

	inline quint8 ground() const {
		return _ground;
	}

	inline void setGround(quint8 ground) {
		_ground = ground;
	}

private:
	QList<quint16> _sceneIds, _lunarCrySceneIds;
	quint8 _flags, _lunarCryFlags, _region, _ground;
};

#endif // WMENCOUNTER_H
