#include "CharaModel.h"

CharaModel::CharaModel(const QString &name, const QPixmap &texture) :
	name(name), texture(texture)
{
}

CharaModel::CharaModel(const QString &name) :
	name(name)
{
}
