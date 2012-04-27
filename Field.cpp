#include "Field.h"

Field::Field(const QString &name)
	: _isOpen(false), _name(name), msdFile(NULL), jsmFile(NULL), walkmeshFile(NULL), miscFile(NULL)
{
}

Field::~Field()
{
	if(msdFile!=NULL)		delete msdFile;
	if(jsmFile!=NULL)		delete jsmFile;
	if(walkmeshFile!=NULL)	delete walkmeshFile;
	if(miscFile!=NULL)		delete miscFile;
}

bool Field::isOpen() const
{
	return _isOpen;
}

void Field::setOpen(bool open)
{
	_isOpen = open;
}

void Field::openMsdFile(const QByteArray &msd)
{
	if(msdFile!=NULL) 	deleteMsdFile();
	msdFile = new MsdFile();

	if(!msdFile->open(msd)) {
		qWarning() << "Field::openMsdFile error" << _name;
		deleteMsdFile();
	}
}

void Field::openJsmFile(const QByteArray &jsm, const QByteArray &sym)
{
	if(jsmFile!=NULL) 	deleteJsmFile();
	jsmFile = new JsmFile();

	if(!jsmFile->open(jsm, sym)) {
		qWarning() << "Field::openJsmFile error" << _name;
		deleteJsmFile();
	}
}

void Field::openWalkmeshFile(const QByteArray &id, const QByteArray &ca)
{
	if(walkmeshFile!=NULL) 	deleteWalkmeshFile();
	walkmeshFile = new WalkmeshFile();

	if(!walkmeshFile->open(id, ca)) {
		qWarning() << "Field::openWalkmeshFile error" << _name;
		deleteWalkmeshFile();
	}
}

void Field::openMiscFile(const QByteArray &inf, const QByteArray &rat, const QByteArray &mrt, const QByteArray &pmp, const QByteArray &pmd, const QByteArray &pvp)
{
	if(miscFile!=NULL) 	deleteMiscFile();
	miscFile = new MiscFile();

	if(!miscFile->open(inf, rat, mrt, pmp, pmd, pvp)) {
		qWarning() << "Field::openMiscFile error" << _name;
		deleteMiscFile();
	}
}

void Field::deleteMsdFile()
{
	if(msdFile!=NULL) {
		delete msdFile;
		msdFile = NULL;
	}
}

void Field::deleteJsmFile()
{
	if(jsmFile!=NULL) {
		delete jsmFile;
		jsmFile = NULL;
	}
}

void Field::deleteWalkmeshFile()
{
	if(walkmeshFile!=NULL) {
		delete walkmeshFile;
		walkmeshFile = NULL;
	}
}

void Field::deleteMiscFile()
{
	if(miscFile!=NULL) {
		delete miscFile;
		miscFile = NULL;
	}
}

bool Field::hasMsdFile() const
{
	return msdFile != NULL;
}

bool Field::hasJsmFile() const
{
	return jsmFile != NULL;
}

bool Field::hasWalkmeshFile() const
{
	return walkmeshFile != NULL;
}

bool Field::hasMiscFile() const
{
	return miscFile != NULL;
}

bool Field::hasFiles() const
{
	return hasMsdFile() || hasJsmFile() || hasMapFiles() || hasWalkmeshFile() || hasMiscFile();
}

MsdFile *Field::getMsdFile() const
{
	return msdFile;
}

JsmFile *Field::getJsmFile() const
{
	return jsmFile;
}

WalkmeshFile *Field::getWalkmeshFile() const
{
	return walkmeshFile;
}

MiscFile *Field::getMiscFile() const
{
	return miscFile;
}

bool Field::isModified() const
{
	return (msdFile != NULL && msdFile->isModified())
			|| (jsmFile != NULL && jsmFile->isModified())
			|| (miscFile != NULL && miscFile->isModified())
			|| (walkmeshFile != NULL && walkmeshFile->isModified());
}

const QString &Field::name() const
{
	return _name;
}
