#include "ObjFile.h"

ObjFile::ObjFile(const QString &fileName) :
    QFile(fileName)
{

}

bool ObjFile::extract(quint32 offset, quint32 size, const QString &fileName)
{
	if (!seek(offset)) {
		return false;
	}

	QByteArray data = read(size);
	if (data.size() != size) {
		return false;
	}

	QFile f(fileName);
	if (!f.open(QIODevice::WriteOnly)) {
		return false;
	}

	return f.write(data) == data.size();
}

bool ObjFile::extract(quint32 id, const QString &fileName)
{
	quint32 offset, size;

	if (!sectionInfos(id, offset, size)) {
		return false;
	}

	return extract(offset, size, fileName);
}

bool ObjFile::extract(const QString &dirName)
{
	QList<quint32> toc = openToc();
	if (toc.isEmpty()) {
		return false;
	}
	qDebug() << toc;
	QFileInfo info(fileName());
	QString name = info.completeBaseName(), ext = info.suffix();

	QDir dir(dirName);
	for (int i = 0; i < toc.size() - 1; ++i) {
		quint32 offset = toc.at(i);
		int size = toc.at(i + 1) - offset;
		if (size < 0) {
			qWarning() << "ObjFile::extract Invalid section size" << i;
		} else if (!extract(offset, size, dir.filePath(QString("%1.part%2.%3")
		                                         .arg(name)
		                                         .arg(i, 2, 10, QChar('0'))
		                                         .arg(ext)))) {
			qWarning() << "ObjFile::extract Cannot extract file" << i;
		}
	}

	return true;
}

bool ObjFile::build(const QString &dirName, const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		return false;
	}

	// Fake empty toc
	int headerSize = (OBJFILE_SECTION_COUNT + 1) * sizeof(quint32);
	if (file.write(QByteArray(headerSize, '\0')) != headerSize) {
		return false;
	}

	QDir dir(dirName);
	QStringList fileNames = dir.entryList(QStringList() << "*.part*.*",
	                                      QDir::Files, QDir::Name);
	quint32 toc[OBJFILE_SECTION_COUNT];
	QRegularExpression regExpPart("\\.part(\\d+)\\.");

	// Data
	int oldCurToc = 0;
	foreach (const QString &name, fileNames) {

		QRegularExpressionMatch match = regExpPart.match(name);
		bool ok;
		int number = match.captured(1).toInt(&ok);

		if (ok && number < OBJFILE_SECTION_COUNT) {
			if (oldCurToc + 1 < number) {
				// Not found files set to empty size
				for (int i = oldCurToc; i < number; ++i) {
					toc[i] = file.pos();
					qDebug() << i << QString::number(file.pos(), 16);
				}
			}
			toc[number] = file.pos();
			oldCurToc = number;

			QFile f(dir.filePath(name));
			if (!f.open(QIODevice::ReadOnly)) {
				qWarning() << "ObjFile::build Cannot open file" << f.fileName();
			} else {
				file.write(f.readAll());
			}
		} else {
			qWarning() << "ObjFile::build Cannot find partN in filename"
			           << dir.filePath(name);
		}
	}

	if (oldCurToc + 1 < OBJFILE_SECTION_COUNT) {
		// Not found files set to empty size
		for (int i = oldCurToc; i < OBJFILE_SECTION_COUNT; ++i) {
			toc[i] = file.pos();
		}
	}

	// Toc
	file.reset();
	headerSize -= sizeof(quint32);
	if (file.write((char *)toc, headerSize) != headerSize) {
		return false;
	}

	return true;
}

QList<quint32> ObjFile::openToc()
{
	reset();
	const int r = OBJFILE_SECTION_COUNT * sizeof(quint32);
	QByteArray data = read(r);
	if (data.size() != r) {
		qWarning() << "ObjFile::openToc File too short";
		return QList<quint32>();
	}
	const quint32 *constData = (const quint32 *)data.constData();
	QList<quint32> ret;

	for (int i = 0; i < OBJFILE_SECTION_COUNT; ++i) {
		ret.append(constData[i]);
	}
	ret.append(size());

	return ret;
}

bool ObjFile::sectionInfos(quint32 id, quint32 &offset, quint32 &size)
{
	const int entrySize = sizeof(quint32);

	if (!seek(id * entrySize)) {
		qWarning() << "ObjFile::sectionInfos Cannot seek to" << id;
		return false;
	}

	if (read((char *)&offset, entrySize) != entrySize) {
		qWarning() << "ObjFile::sectionInfos File too short 1" << id;
		return false;
	}

	quint32 nextOffset;
	if (read((char *)&nextOffset, entrySize) != entrySize) {
		qWarning() << "ObjFile::sectionInfos File too short 2" << id;
		return false;
	}

	if (nextOffset < offset) {
		qWarning() << "ObjFile::sectionInfos Wrong order" << id
		           << offset << nextOffset;
		return false;
	}

	size = nextOffset - offset;

	return true;
}
