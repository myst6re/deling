#ifndef CHARAFILE_H
#define CHARAFILE_H

#include <QtCore>
#include "FF8Image.h"
#include "CharaModel.h"

class CharaFile
{
public:
	CharaFile();
	bool open(const QByteArray &one);
	bool isModified() const;
	const CharaModel &model(int id) const;
	void setModel(int id, const CharaModel &model);
	int modelCount() const;
private:
	QList<CharaModel> models;
	bool modified;
};

#endif // CHARAFILE_H
