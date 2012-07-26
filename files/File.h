#ifndef FILE_H
#define FILE_H

#include <QtCore>

class File
{
public:
	File();
	bool open(const QString &path);
	bool save(const QString &path);
	virtual bool open(const QByteArray &data);
	virtual bool save(QByteArray &data);
	bool isModified() const;
protected:
	bool modified;
};

#endif // FILE_H
