#ifndef MAPPOLY_H
#define MAPPOLY_H

#include "Poly.h"

class MapPoly : public TrianglePoly
{
public:
	MapPoly(const QList<Vertex> &vertices, const QList<Vertex> &normals,
	        const QList<TexCoord> &texCoords,
	        quint8 texPage, quint8 clutId, quint8 groundType,
	        quint8 u1, quint8 u2);
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
	inline quint8 u1() const {
		return _u1;
	}
	inline void setU1(quint8 u1) {
		_u1 = u1;
	}
	inline quint8 u2() const {
		return _u2;
	}
	inline void setU2(quint8 u2) {
		_u2 = u2;
	}
private:
	quint8 _texPage, _clutId;
	quint8 _groundType, _u1, _u2;
};

#endif // MAPPOLY_H
