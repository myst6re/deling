#ifndef FF8FONT_H
#define FF8FONT_H

#include "files/TdwFile.h"
#include "parameters.h"

class FF8Font
{
public:
	FF8Font(TdwFile *tdw, const QByteArray &txtFileData);
	TdwFile *tdw() const;
	const QList<QStringList> &tables() const;
	void setTables(const QList<QStringList> &tables);
	void setChar(int tableId, int charId, const QString &c);
	bool isValid() const;
	bool isModified() const;
	const QString &name() const;
	void setName(const QString &name);
	void setPaths(const QString &txtPath, const QString &tdwPath);
	const QString &txtPath() const;
	const QString &tdwPath() const;
	void setReadOnly(bool ro);
	bool isReadOnly() const;
	QString saveTxt();

	static bool listFonts();
	static QStringList fontList();
	static FF8Font *font(QString name);
	static bool saveFonts();
	static bool copyFont(const QString &name, const QString &from, const QString &name2);
	static bool removeFont(const QString &name);
	static const QString &fontDirPath();
private:
	void openTxt(const QString &data);
	void print();
	TdwFile *_tdw;
	QString _name;
	QString _txtPath, _tdwPath;
	bool modified, readOnly;
	QList<QStringList> _tables;

	static FF8Font *openFont(const QString &tdwPath, const QString &txtPath);
	static QString font_dirPath;
	static QMap<QString, FF8Font *> fonts;
};

#endif // FF8FONT_H
