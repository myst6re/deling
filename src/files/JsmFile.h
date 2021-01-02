/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2012 Arzel Jérôme <myst6re@gmail.com>
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#ifndef JSMFILE_H
#define JSMFILE_H

#include "files/File.h"
#include <QtGui/QTextCursor>
#include "Data.h"
#include "JsmScripts.h"

#define JUST_KEY		0
#define	KEY_AND_UPARAM	1
#define	KEY_AND_SPARAM	2

#define UNKNOWN_CHARACTER	254
#define DRAWPOINT_CHARACTER	255

struct FF8Window {
	unsigned int type;
	int x, y, u1, ask_first, ask_last, ask_first2;
	int script_pos;
};

struct JsmHeader {
	quint8 count0;
	quint8 count1;
	quint8 count2;
	quint8 count3;
	quint16 section1;
	quint16 section2;
};

class JsmFile : public File
{
public:
	enum SearchType {
		SearchText,
		SearchOpcode,
		SearchVar,
		SearchExec,
		SearchMapJump
	};

	JsmFile();
	virtual ~JsmFile();

	QString printCount();

	bool open(const QString &path);
	// Ensure File::open is overloaded
	inline bool open(const QByteArray &jsm) {
		return open(jsm, QByteArray());
	}
	bool open(const QByteArray &jsm, const QByteArray &sym_data, bool old_format = false);
	bool save(const QString &path);
	bool save(QByteArray &jsm);
	bool save(QByteArray &jsm, QByteArray &sym);
	bool openSym(const QByteArray &sym_data);
	bool toFileSym(const QString &path);
	QString saveSym();
	inline QString filterText() const {
		return QObject::tr("Fichier scripts écran PC (*.jsm)");
    }

	bool compileAll(int &errorGroupID, int &errorMethodID, int &errorLine, QString &errorStr);
	QString toString(int groupID, int methodID, bool moreDecompiled,
	                 const Field *field, int indent = 0, bool noCache = false);
	int opcodePositionInText(int groupID, int methodID, int opcodeID) const;
	int fromString(int groupID, int methodID, const QString &text, QString &errorStr);

	const JsmScripts &getScripts() const;

	bool search(SearchType type, quint64 value, int &groupID, int &methodID, int &opcodeID) const;
	bool searchReverse(SearchType type, quint64 value, int &groupID, int &methodID, int &opcodeID) const;
	QList<int> searchAllVars() const;
	QList<int> searchAllSpells(const QString &fieldName) const;
	QList<int> searchAllCards(const QString &fieldName) const;
	QList<int> searchAllCardPlayers(const QString &fieldName) const;
	QList<int> searchAllMoments() const;
	void searchAllOpcodeTypes(QMap<int, int> &ret) const;
	void searchDefaultBGStates(QMultiMap<quint8, quint8> &params) const;

	bool hasSym() const;
	int mapID() const;
	inline bool oldFormat() const {
		return _oldFormat;
	}
	inline void setOldFormat(bool oldFormat) {
		_oldFormat = oldFormat;
	}

	int nbWindows(quint8 textID) const;
	QList<FF8Window> windows(quint8 textID) const;
	void setWindow(quint8 textID, int winID, const FF8Window &value);

	void setDecompiledScript(int groupID, int methodID, const QString &text, bool moreDecompiled);
	void setCurrentOpcodeScroll(int groupID, int methodID, int scrollValue, const QTextCursor &textCursor);
	int currentGroupItem() const;
	int currentMethodItem(int groupID) const;
	int currentOpcodeScroll(int groupID, int methodID) const;
	int currentTextCursor(int groupID, int methodID, int &anchor) const;

	static QStringList opcodeName;
	static QStringList opcodeNameCalc;
private:
	bool search(SearchType type, quint64 value, quint16 pos, int opcodeID) const;
	QString _toString(int position, int nbOpcode, int indent = 0) const;
	QString _toStringMore(int position, int nbOpcode, const Field *field, int indent = 0) const;

	void searchWindows();
	void searchGroupTypes();

	JsmScripts scripts;
	bool _hasSym;
	bool needUpdate, needUpdateMore;
	int section1_padding;
	int _mapID;
	QMultiMap<quint8, FF8Window> ff8Windows;
	QMap<quint64, int> opcodeScroll;
	QMap<quint64, quint64> textCursors;
	QMap<int, int> methodItem;
	int groupItem;
	bool _oldFormat;
};

#endif // JSMFILE_H
