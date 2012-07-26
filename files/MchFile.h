#ifndef MCHFILE_H
#define MCHFILE_H

#include "files/File.h"
#include "files/TimFile.h"
#include "CharaModel.h"

class MchFile : public File
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
