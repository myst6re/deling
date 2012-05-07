#ifndef CHARAMODEL_H
#define CHARAMODEL_H

#include <QtCore>
#include <QPixmap>

class CharaModel
{
public:
	CharaModel(const QString &name, const QPixmap &texture);
	CharaModel(const QString &name);
	QString name;
	QPixmap texture;
};

#endif // CHARAMODEL_H
