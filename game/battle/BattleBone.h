#ifndef BATTLEBONE_H
#define BATTLEBONE_H

#include <QByteArray>

class BattleBone
{
public:
	BattleBone(quint16 parent, quint16 size, const QByteArray &unknown);
private:
	quint16 _parent, _size;
	QByteArray _unknown;
};

#endif // BATTLEBONE_H
