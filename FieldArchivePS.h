#ifndef FSARCHIVEPS_H
#define FSARCHIVEPS_H

#include "FieldArchive.h"
#include "FieldPS.h"
#include "FF8DiscArchive.h"

class FieldArchivePS : public FieldArchive
{
public:
	FieldArchivePS();
	~FieldArchivePS();
	QString archivePath() const;
	FF8DiscArchive *getFF8DiscArchive() const;
	bool open(const QString &, QProgressDialog *progress);
	bool openBG(Field *field, QByteArray &map_data, QByteArray &mim_data, QByteArray &tdw_data, QByteArray &chara_data) const;
private:
	FF8DiscArchive *iso;
};

#endif // FSARCHIVEPS_H
