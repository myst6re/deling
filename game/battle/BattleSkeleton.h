#ifndef BATTLESKELETON_H
#define BATTLESKELETON_H

#include <QList>
#include "game/battle/BattleBone.h"

class BattleSkeleton : public QList<BattleBone>
{
public:
	BattleSkeleton();
	inline const QByteArray &unknownHeader() const {
		return _unknownHeader;
	}
	inline void setUnknownHeader(const QByteArray &unknownHeader) {
		_unknownHeader = unknownHeader;
	}

private:
	QByteArray _unknownHeader;
};

#endif // BATTLESKELETON_H
