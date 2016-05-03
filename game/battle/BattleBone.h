#ifndef BATTLEBONE_H
#define BATTLEBONE_H

#include <QByteArray>

class BattleBone
{
public:
	BattleBone(qint16 parent, qint16 size, const QByteArray &unknown);
	inline qint16 parent() const {
		return _parent;
	}
	inline qint16 size() const {
		return _size;
	}

private:
	qint16 _parent, _size;
	QByteArray _unknown;
};

#endif // BATTLEBONE_H
