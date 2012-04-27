#ifndef FSARCHIVEPC_H
#define FSARCHIVEPC_H

#include "FieldArchive.h"
#include "FieldPC.h"
#include "FsArchive.h"

class FieldArchivePC : public FieldArchive
{
public:
	FieldArchivePC();
	~FieldArchivePC();
	QString archivePath() const;
	FsArchive *getFsArchive() const;
	bool open(const QString &, QProgressDialog *progress);
	bool save(QProgressDialog *progress, QString save_path=QString());
	bool openBG(Field *field, QByteArray &map_data, QByteArray &mim_data, QByteArray &tdw_data, QByteArray &chara_data) const;
	void restoreFieldHeaders(const QMap<Field *, QMap<QString, FsHeader> > &oldFields) const;
	bool optimiseArchive(QProgressDialog *progress);
private:
	FsArchive *archive;
};

#endif // FSARCHIVEPC_H
