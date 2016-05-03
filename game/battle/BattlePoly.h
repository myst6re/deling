#ifndef BATTLEPOLY_H
#define BATTLEPOLY_H

#include <QVector>
#include "game/battle/BattleVertex.h"

struct TexCoord {
	quint8 x, y;
};

class BattlePoly
{
public:
	BattlePoly();
	BattlePoly(const QVector<BattleVertex> &vertices,
	           const QVector<TexCoord> &texCoords,
	           quint16 unknown1,
	           quint16 unknown2,
	           quint8 flags);
	virtual ~BattlePoly() {}
	inline bool isEmpty() const {
		return _vertices.isEmpty();
	}
	void setVertices(const QVector<BattleVertex> &vertices,
	                 const QVector<TexCoord> &texCoords);
	inline int count() const {
		return _vertices.size();
	}
	inline const BattleVertex &vertex(quint8 id) const {
		return _vertices.at(id);
	}
	inline const TexCoord &texCoord(quint8 id) const {
		return _texCoords.at(id);
	}
	inline void setTexCoord(quint8 id, const TexCoord &texCoord) {
		_texCoords.replace(id, texCoord);
	}
	inline quint16 unknown1() const {
		return _unknown1;
	}
	inline quint16 unknown2() const {
		return _unknown2;
	}

protected:
	QVector<BattleVertex> _vertices;
	QVector<TexCoord> _texCoords;
	quint16 _unknown1, _unknown2;
	quint8 _flags;
};

#endif // BATTLEPOLY_H
