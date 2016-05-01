#include "BattleBone.h"

BattleBone::BattleBone(quint16 parent, quint16 size,
                       const QByteArray &unknown) :
    _parent(parent), _size(size),
    _unknown(unknown)
{
}
