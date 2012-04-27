#ifndef FIELDPC_H
#define FIELDPC_H

#include "Field.h"
#include "FsArchive.h"

class FieldPC : public Field
{
public:
	FieldPC(const QString &name, const QString &path, FsArchive *archive);
	~FieldPC();

	bool hasMapFiles() const;
//	void setArchiveHeader(FsArchive *header);
	FsArchive *getArchiveHeader() const;
	const QString &path() const;
	bool open(FsArchive *archive);
	void save(QByteArray &fs_data, QByteArray &fl_data, QByteArray &fi_data);
	void optimize(QByteArray &fs_data, QByteArray &fl_data, QByteArray &fi_data);

private:
	QString _path;
	FsArchive *header;
};

#endif // FIELDPC_H
