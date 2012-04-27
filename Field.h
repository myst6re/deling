#ifndef FIELD_H
#define FIELD_H

#include <QtCore>
#include "MsdFile.h"
#include "JsmFile.h"
#include "WalkmeshFile.h"
#include "MiscFile.h"

class Field
{
public:
	Field(const QString &name);
	~Field();

	bool isOpen() const;
	void setOpen(bool open);
	void openMsdFile(const QByteArray &msd);
	void openJsmFile(const QByteArray &jsm, const QByteArray &sym=QByteArray());
	void openWalkmeshFile(const QByteArray &id, const QByteArray &ca=QByteArray());
	void openMiscFile(const QByteArray &inf, const QByteArray &rat, const QByteArray &mrt, const QByteArray &pmp, const QByteArray &pmd, const QByteArray &pvp);
	bool hasMsdFile() const;
	bool hasJsmFile() const;
	bool hasWalkmeshFile() const;
	bool hasMiscFile() const;
	virtual bool hasMapFiles() const=0;
	bool hasFiles() const;
	MsdFile *getMsdFile() const;
	JsmFile *getJsmFile() const;
	WalkmeshFile *getWalkmeshFile() const;
	MiscFile *getMiscFile() const;
	bool isModified() const;
	const QString &name() const;

protected:
	void deleteMsdFile();
	void deleteJsmFile();
	void deleteWalkmeshFile();
	void deleteMiscFile();

	bool _isOpen;
	QString _name;
	MsdFile *msdFile;
	JsmFile *jsmFile;
	WalkmeshFile *walkmeshFile;
	MiscFile *miscFile;
};

#endif // FIELD_H
