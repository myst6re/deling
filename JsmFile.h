#ifndef JSMFILE_H
#define JSMFILE_H

#include <QtCore>
#include <QtGui/QTextCursor>
#include "Data.h"
#include "JsmScripts.h"

#define JUST_KEY		0
#define	KEY_AND_UPARAM	1
#define	KEY_AND_SPARAM	2

#define UNKNOWN_CHARACTER	254
#define DRAWPOINT_CHARACTER	255

typedef struct {
	unsigned int type;
	int x, y, u1, ask_first, ask_last, ask_first2, ask_last2;
	int script_pos;
} FF8Window;

typedef struct {
	quint8 count0;
	quint8 count1;
	quint8 count2;
	quint8 count3;
	quint16 section1;
	quint16 section2;
} JsmHeader;

class JsmFile
{
public:
    JsmFile();
	~JsmFile();

	QString printCount();

	bool open(QString);
	bool open(const QByteArray &jsm, const QByteArray &sym_data=QByteArray());
	bool save(const QString &);
	QByteArray save(QByteArray &sym);
	static QString lastError;

	bool compileAll(int &errorGroupID, int &errorMethodID, int &errorLine, QString &errorStr);
	QString toString(int groupID, int methodID);
	int opcodePositionInText(int groupID, int methodID, int opcodeID) const;
	int fromString(int groupID, int methodID, const QString &text, QString &errorStr);

	const JsmScripts &getScripts() const;

	bool search(int type, quint64 value, int &groupID, int &methodID, int &opcodeID) const;
	bool searchReverse(int type, quint64 value, int &groupID, int &methodID, int &opcodeID) const;
	QList<int> searchAllVars() const;
	QList<int> searchAllSpells(const QString &fieldName) const;
	QList<int> searchAllMoments() const;
	void searchAllOpcodeTypes(QMap<int, int> &ret) const;

	bool hasSym() const;
	bool isModified() const;
	int mapID() const;

	int nbWindows(quint8 textID) const;
	QList<FF8Window> windows(quint8 textID) const;
	void setWindow(quint8 textID, int winID, const FF8Window &value);

	void setDecompiledScript(int groupID, int methodID, const QString &text);
	void setCurrentOpcodeScroll(int groupID, int methodID, int scrollValue, const QTextCursor &textCursor);
	int currentGroupItem() const;
	int currentMethodItem(int groupID) const;
	int currentOpcodeScroll(int groupID, int methodID) const;
	int currentTextCursor(int groupID, int methodID, int &anchor) const;

	static QStringList opcodeName;
	static QStringList opcodeNameCalc;
private:
	bool search(int type, quint64 value, quint16 pos, int opcodeID) const;
	bool openSym(const QByteArray &sym_data);
	QString saveSym();
	QString _toString(int position, int nbOpcode) const;

	void searchWindows();
	QList<qint32> searchJumps(int groupID, int methodID) const;
	void searchMapID();
	void searchGroupTypes();

	JsmScripts scripts;
	bool _hasSym;
	bool modified;
	bool needUpdate;
	int section1_padding;
	int _mapID;
	QMultiMap<quint8, FF8Window> ff8Windows;
	QMap<quint64, int> opcodeScroll;
	QMap<quint64, quint64> textCursors;
	QMap<int, int> methodItem;
	int groupItem;
};

#endif // JSMFILE_H
