#ifndef OBJFILE_H
#define OBJFILE_H

#include <QtCore>

#define OBJFILE_SECTION_COUNT 48

class ObjFile : public QFile
{
public:
	ObjFile(const QString &fileName);
	bool extract(quint32 id, const QString &fileName);
	bool extract(const QString &dirName);
	static bool build(const QString &dirName, const QString &fileName);
private:
	bool extract(quint32 offset, quint32 size, const QString &fileName);
	QList<quint32> openToc();
	bool sectionInfos(quint32 id, quint32 &offset, quint32 &size);
};

#endif // OBJFILE_H
