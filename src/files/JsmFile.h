/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2024 Arzel Jérôme <myst6re@gmail.com>
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
#pragma once

#include "files/File.h"
#include <QtGui/QTextCursor>
#include "JsmScripts.h"

#define JUST_KEY		0
#define	KEY_AND_UPARAM	1
#define	KEY_AND_SPARAM	2

#define UNKNOWN_CHARACTER	254
#define DRAWPOINT_CHARACTER	255

struct FF8Window {
	unsigned int type;
	int x, y, u1, ask_first, ask_last, ask_first2;
	int groupID, methodID, opcodeID;
};

struct JsmHeader {
	quint8 countDoors;
	quint8 countLines;
	quint8 countBackground;
	quint8 countOther;
	quint16 offsetScriptPositionsSection;
	quint16 offsetScriptDataSection;
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

	bool open(const QString &path);
	// Ensure File::open is overloaded
	inline bool open(const QByteArray &jsm) override {
		return open(jsm, QByteArray());
	}
	bool open(const QByteArray &jsm, const QByteArray &sym_data, bool old_format = false);
	bool saveWithPath(const QString &path);
	bool save(QByteArray &jsm) const override;
	bool save(QByteArray &jsm, QByteArray &sym);
	bool openSym(const QByteArray &sym_data);
	bool toFileSym(const QString &path);
	QByteArray saveSym();
	inline QString filterText() const override {
		return QObject::tr("Field Script PC file (*.jsm)");
	}

	QString toString(int groupID, int methodID, bool moreDecompiled,
	                 int indent = 0, bool noCache = false);
	QString toString(bool moreDecompiled, bool noCache = false);
	int opcodePositionInText(int groupID, int methodID, int opcodeID, bool more) const;
	int fromString(int groupID, int methodID, const QString &text, QString &errorStr);

	inline const JsmScripts &getScripts() const {
		return scripts;
	}
	inline JsmScripts &getScripts() {
		return scripts;
	}

	bool search(SearchType type, quint64 value, int &groupID, int &methodID, int &opcodeID) const;
	bool search(SearchType type, const QList<quint64> &values, int &groupID, int &methodID, int &opcodeID) const;
	bool searchReverse(SearchType type, quint64 value, int &groupID, int &methodID, int &opcodeID) const;
	bool searchReverse(SearchType type, const QList<quint64> &values, int &groupID, int &methodID, int &opcodeID) const;
	QList<int> searchAllVars() const;
	QList<int> searchAllSpells(const QString &fieldName) const;
	QList<int> searchAllCards(const QString &fieldName) const;
	QList<int> searchAllCardPlayers(const QString &fieldName) const;
	QList<int> searchAllMoments() const;
	void searchAllOpcodeTypes(QMap<int, int> &ret) const;
	void searchDefaultBGStates(QMultiMap<quint8, quint8> &params) const;

	inline bool hasSym() const {
		return _hasSym;
	}
	inline int mapID() const {
		return _mapID;
	}
	inline bool oldFormat() const {
		return _oldFormat;
	}
	inline void setOldFormat(bool oldFormat) {
		_oldFormat = oldFormat;
	}

	inline int nbWindows(quint8 textID) const {
		return ff8Windows.count(textID);
	}
	inline QList<FF8Window> windows(quint8 textID) const {
		return ff8Windows.values(textID);
	}
	bool setWindow(quint8 textID, const FF8Window &value);

	void setDecompiledScript(int groupID, int methodID, const QString &text, bool moreDecompiled);
	void setCurrentOpcodeScroll(int groupID, int methodID, bool more, int scrollValue, const QTextCursor &textCursor);
	inline int currentGroupItem() const {
		return groupItem;
	}
	int currentMethodItem(int groupID) const;
	int currentOpcodeScroll(int groupID, int methodID, bool more) const;
	int currentTextCursor(int groupID, int methodID, bool more, int &anchor) const;

	static QStringList opcodeNames;
	static QStringList opcodeNamesCalc;
private:
	static JsmGroup createGroup(quint16 label, quint8 count, const JsmHeader &jsmHeader, int headerID);
	bool searchInOpcode(SearchType type, quint64 value, const JsmData &jsmData, int opcodeID) const;
	QString _toString(int position, int nbOpcode, int indent = 0) const;
	QString _toStringMore(int position, int nbOpcode, int indent = 0) const;

	void forceNames();
	void searchWindows();
	void searchGroupTypes();

	JsmScripts scripts;
	bool _hasSym;
	bool needUpdate, needUpdateMore;
	int _mapID;
	QMultiMap<quint8, FF8Window> ff8Windows;
	QMap<quint64, int> opcodeScroll, opcodeScrollMore;
	QMap<quint64, quint64> textCursors, textCursorsMore;
	QMap<int, int> methodItem;
	int groupItem;
	bool _oldFormat;
};
