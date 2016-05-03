#include "BattleBone.h"

BattleBone::BattleBone(qint16 parent, qint16 size,
                       const QByteArray &unknown) :
    _parent(parent), _size(size),
    _unknown(unknown)
{
}
