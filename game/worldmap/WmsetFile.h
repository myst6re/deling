#ifndef WMSETFILE_H
#define WMSETFILE_H

#include <QtCore>

#define OBJFILE_SECTION_COUNT 48

class WmsetFile
{
public:
	explicit WmsetFile(QIODevice *io = Q_NULLPTR);
	inline void setDevice(QIODevice *device) {
		_io = device;
	}
	bool extract(quint32 id, const QString &fileName);
	bool extract(const QString &name, const QString &dirName);
	static bool build(const QString &dirName, const QString &fileName);
private:
	bool extract(quint32 offset, quint32 size, const QString &fileName);
	QList<quint32> openToc();
	bool sectionInfos(quint32 id, quint32 &offset, quint32 &size);

	QIODevice *_io;
};

#endif // WMSETFILE_H
