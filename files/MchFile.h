#ifndef MCHFILE_H
#define MCHFILE_H

#include <QtCore>
#include "files/TimFile.h"
#include "CharaModel.h"

class MchFile
{
public:
	MchFile();
	bool open(const QByteArray &mch, const QString &name);
	bool hasModel() const;
	CharaModel *model() const;
	void setModel(CharaModel *model);
private:
	CharaModel *_model;
};

#endif // MCHFILE_H
