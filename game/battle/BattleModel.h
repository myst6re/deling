#ifndef BATTLEMODEL_H
#define BATTLEMODEL_H

#include "game/battle/BattleSkeleton.h"
#include "game/battle/BattlePart.h"
#include "game/battle/BattleAnimation.h"
#include "files/TimFile.h"

class BattleModel
{
public:
	BattleModel();

	inline void setSkeleton(const BattleSkeleton &skeleton) {
		_skeleton = skeleton;
	}

	inline void setParts(const QList<BattlePart> &parts) {
		_parts = parts;
	}

	inline void setAnimations(const QList<BattleAnimation> &animations) {
		_animations = animations;
	}

	inline void setTextures(const QList<TimFile> &textures) {
		_textures = textures;
	}

private:
	BattleSkeleton _skeleton;
	QList<BattlePart> _parts;
	QList<BattleAnimation> _animations;
	QList<TimFile> _textures;
};

#endif // BATTLEMODEL_H
