#ifndef FF8FONT_H
#define FF8FONT_H

#include "files/TdwFile.h"

class FF8Font
{
public:
	FF8Font(TdwFile *tdw, const QByteArray &txtFileData);
	TdwFile *tdw;
	QList<QStringList> tables;
	bool isValid() const;
	const QString &name() const;
private:
	void openTxt(const QString &data);
	void print();
	QString _name;
};

#endif // FF8FONT_H
