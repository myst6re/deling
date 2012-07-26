#include "File.h"

File::File() :
	modified(false)
{
}

bool File::open(const QString &path)
{
	QFile f(path);
	if(f.open(QIODevice::ReadOnly)) {
		bool ok = open(f.readAll());
		f.close();
		return ok;
	}
	return false;
}

bool File::save(const QString &path)
{
	QFile f(path);
	if(f.open(QIODevice::WriteOnly)) {
		QByteArray data;
		bool ok;
		if((ok = save(data))) {
			f.write(data);
		}
		f.close();
		return ok;
	}
	return false;
}

bool File::open(const QByteArray &data)
{
	return false;
}

bool File::save(QByteArray &data)
{
	return false;
}

bool File::isModified() const
{
	return modified;
}
