#ifndef MAPPOLY_H
#define MAPPOLY_H

#include "Poly.h"

class MapPoly : public TrianglePoly
{
public:
	MapPoly(const QList<Vertex> &vertices, const QList<Vertex> &normals,
	        const QList<TexCoord> &texCoords,
	        quint8 texPage, quint8 clutId, quint8 groundType,
	        quint8 flags1, quint8 flags2);
	inline quint8 texPage() const {
		return _texPage;
	}
	inline void setTexPage(quint8 texPage) {
		_texPage = texPage;
	}
	inline quint8 clutId() const {
		return _clutId;
	}
	inline void setClutId(quint8 clutId) {
		_clutId = clutId;
	}
	inline quint8 groundType() const {
		return _groundType;
	}
	inline void setGroundType(quint8 groundType) {
		_groundType = groundType;
	}
	inline quint8 flags1() const {
		return _flags1;
	}
	inline bool isWaterTexture() const {
		return (_flags1 & 0x60) == 0x40;
	}
	inline bool isRoadTexture() const {
		return _flags1 & 0x20;
	}
	inline bool isTransparent() const {
		return _flags1 & 0x10;
	}
	inline bool isCity() const {
		return _flags1 & 0x8;
	}
	inline void setFlags1(quint8 flags) {
		_flags1 = flags;
	}
	inline quint8 flags2() const {
		return _flags2;
	}
	inline void setFlags2(quint8 flags) {
		_flags2 = flags;
	}
	inline bool hasBlackPixels() const {
		return _flags1 & 0x4;
	}
	inline void setHasBlackPixels(bool hasBlackPixels) {
		if (hasBlackPixels) {
			_flags1 |= 0x4;
		} else {
			_flags1 &= 0xFB;
		}
	}
private:
	quint8 _texPage, _clutId;
	quint8 _groundType, _flags1, _flags2;
};

#endif // MAPPOLY_H
