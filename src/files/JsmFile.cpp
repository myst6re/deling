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
#include "files/JsmFile.h"
#include "Data.h"

QStringList JsmFile::opcodeNamesCalc;
QStringList JsmFile::opcodeNames;

JsmFile::JsmFile() :
	File(), _hasSym(false), needUpdate(true), needUpdateMore(true), _offsetScriptPositionsSectionPadding(0),
    groupItem(0), _oldFormat(false)
{
	if (opcodeNamesCalc.isEmpty()) {
		for (int i = 0; i < 18; ++i) {
			opcodeNamesCalc.append(JsmOpcodeCal::cal_table[i]);
		}

		for (int i = 0; i < JSM_OPCODE_COUNT; ++i) {
			opcodeNames.append(JsmOpcode::opcodes[i]);
		}
	}
}

JsmFile::~JsmFile()
{
}

bool JsmFile::open(const QString &path)
{
	QFile jsmFile(path);
	if (!jsmFile.open(QIODevice::ReadOnly)) {
		lastError = jsmFile.errorString();
		return false;
	}
	QString symPath = path;
	symPath.replace(QRegularExpression("\\..+$"), ".sym");
	if (QFile::exists(symPath)) {
		QFile symFile(symPath);
		if (!symFile.open(QIODevice::ReadOnly)) {
			lastError = symFile.errorString();
			return false;
		}
		return open(jsmFile.readAll(), symFile.readAll());
	}

	return open(jsmFile.readAll());
}

JsmGroup JsmFile::createGroup(quint16 label, quint8 count, const JsmHeader &jsmHeader, int headerID)
{
	JsmGroup::Type type = JsmGroup::No;
	int groupRelativeID = 0;

	if (headerID < jsmHeader.countLines) {
		type = JsmGroup::Location;
		groupRelativeID = headerID;
	} else if (headerID < jsmHeader.countLines + jsmHeader.countDoors) {
		type = JsmGroup::Door;
		groupRelativeID = headerID - jsmHeader.countLines;
	} else if (headerID < jsmHeader.countLines + jsmHeader.countDoors + jsmHeader.countBackground) {
		type = JsmGroup::Background;
		groupRelativeID = headerID - (jsmHeader.countLines + jsmHeader.countDoors);
	} else {
		type = JsmGroup::No;
		groupRelativeID = headerID - (jsmHeader.countLines + jsmHeader.countDoors + jsmHeader.countBackground);
	}

	return JsmGroup(label, count + 1, type, groupRelativeID);
}

bool JsmFile::open(const QByteArray &jsm, const QByteArray &symData, bool oldFormat)
{
	const char *jsmData = jsm.constData();
	int jsmDataSize = jsm.size(), oldPos = -1;
	quint16 group, label, pos;
	quint8 count;
	JsmHeader jsmHeader;

	if (jsmDataSize < 8) {
		qWarning() << "JsmFile::open error (1)" << jsmDataSize;
		return false;
	}

	memcpy(&jsmHeader, jsmData, 8);

	int groupCount = (jsmHeader.offsetScriptPositionsSection - 8) / 2,
	    scriptCount = (jsmHeader.offsetScriptDataSection - jsmHeader.offsetScriptPositionsSection) / 2;

	if (jsmDataSize <= jsmHeader.offsetScriptPositionsSection || jsmDataSize <= jsmHeader.offsetScriptDataSection) {
		qWarning() << "JsmFile::open error (2)" << jsmDataSize << jsmHeader.offsetScriptPositionsSection << jsmHeader.offsetScriptDataSection;
		return false;
	}

	if (groupCount != jsmHeader.countDoors + jsmHeader.countLines + jsmHeader.countBackground + jsmHeader.countOther) {
		if (jsmHeader.countDoors > 48) {
			jsmHeader.countDoors = 0;
		}
		if (jsmHeader.countLines > 48) {
			jsmHeader.countLines = 0;
		}
		if (jsmHeader.countBackground > 48) {
			jsmHeader.countBackground = 0;
		}
		if (jsmHeader.countOther > 48) {
			jsmHeader.countOther = 0;
		}
		if (groupCount != jsmHeader.countDoors + jsmHeader.countLines + jsmHeader.countBackground + jsmHeader.countOther) {
			qWarning() << "JsmFile::open invalid group count!" << groupCount << jsmHeader.countDoors << jsmHeader.countLines << jsmHeader.countBackground << jsmHeader.countOther;
			return false;
		}
	}

	_oldFormat = oldFormat;
	QList<JsmGroup> groupList;
	groupList.reserve(groupCount);

	if (!oldFormat) {
		for (int i = 0; i < groupCount; ++i) {
			memcpy(&group, &jsmData[8 + i * 2], 2);
			label = group >> 7;
			count = group & 0x7F;

			if (label + count + 1 >= scriptCount) {
				oldFormat = true; // Retry in old format mode
				break;
			}

			groupList.append(createGroup(label, count, jsmHeader, i));
		}
	}

	if (oldFormat) {
		for (int i = 0; i < groupCount; ++i) {
			memcpy(&group, &jsmData[8 + i * 2], 2);
			label = group & 0xFF;
			count = group >> 8;

			if (label + count + 1 >= scriptCount) {
				qWarning() << "JsmFile::open error (3)" << label << count << scriptCount;
				return false;
			}

			groupList.append(createGroup(label, count, jsmHeader, i));
		}
	}

	// verification no padding and no overlap between groups
	QMultiMap<quint16, quint16> groupListLabelPerScriptCount;
	for (const JsmGroup &group: groupList) {
		groupListLabelPerScriptCount.insert(group.label(), group.scriptCount());
	}
	QMultiMapIterator<quint16, quint16> it(groupListLabelPerScriptCount);
	while (it.hasNext()) {
		it.next();

		if (it.hasNext() && it.key() + it.value() != it.peekNext().key()) {
			qWarning() << "JsmFile::open incorrect group pos+size" << it.key() << it.value() << it.peekNext().key();
			return false;
		}
	}

	QList<JsmGroup> groupListByGroupID;
	groupListByGroupID.reserve(groupCount);
	for (const JsmGroup &group: groupList) {
		if (group.type() != JsmGroup::Background && group.type() != JsmGroup::Location && group.type() != JsmGroup::Door) {
			groupListByGroupID.append(group);
		}
	}
	for (const JsmGroup &group: groupList) {
		if (group.type() == JsmGroup::Location) {
			groupListByGroupID.append(group);
		}
	}
	for (const JsmGroup &group: groupList) {
		if (group.type() == JsmGroup::Background) {
			groupListByGroupID.append(group);
		}
	}
	for (const JsmGroup &group: groupList) {
		if (group.type() == JsmGroup::Door) {
			groupListByGroupID.append(group);
		}
	}

	QList<JsmScript> methodList;
	methodList.reserve(scriptCount);

	_offsetScriptPositionsSectionPadding = 0;
	for (int i = 0; i < scriptCount; ++i) {
		memcpy(&pos, &jsmData[jsmHeader.offsetScriptPositionsSection + i * 2], 2);
		if (i != 0 && pos == 0) {
			_offsetScriptPositionsSectionPadding = jsmHeader.offsetScriptDataSection - jsmHeader.offsetScriptPositionsSection - i * 2;
			break;
		}
		pos &= 0x7FFF;
		if (jsmHeader.offsetScriptDataSection + pos * 4 > jsmDataSize || int(pos) < oldPos) {
			qWarning() << "JsmFile::open error (4)" << (jsmHeader.offsetScriptDataSection + pos * 4) << jsmDataSize;
			return false;
		}
		oldPos = pos;

		methodList.append(JsmScript(pos));
	}

	scripts = JsmScripts(groupListByGroupID, methodList,
	                     JsmData(QByteArray((char *)&jsmData[jsmHeader.offsetScriptDataSection], jsmDataSize - jsmHeader.offsetScriptDataSection), _oldFormat));

	_hasSym = openSym(symData);

	forceNames();
	searchWindows();
	searchGroupTypes();

	modified = false;

	return true;
}

bool JsmFile::openSym(const QByteArray &symData)
{
	QStringList symLines = QString(symData).split('\n', Qt::SkipEmptyParts);
	int nbLines = symLines.size();

	if (nbLines <= 0) {
		return false;
	}

	quint8 countDoors, countLines, countBackgrounds, countOthers;
	scripts.countTypes(countDoors, countLines, countBackgrounds, countOthers);

	int nbEntities = countLines + countBackgrounds + countOthers, // No doors
	    nbScripts = scripts.scripts().size() - 1;

	if (nbScripts <= 0 || nbLines != nbEntities + nbScripts) {
		qWarning() << "JsmFile::openSym error 1" << nbLines << nbEntities << nbScripts;
		return false;
	}

	QString line;
	QRegularExpression validName("^[\\w]+$");
	int fileOrderGroupID = -1, methodID = 1, index = -1, nbGroup = scripts.nbGroup();
	QList<int> groupIDs = scripts.groupIDsSortedByLabel();

	for (int i = 0; i < nbScripts; ++i) {
		if (fileOrderGroupID >= nbGroup) {
			qWarning() << "JsmFile::openSym too much groups" << fileOrderGroupID;
			break;
		}

		line = symLines.at(nbEntities + i).simplified();
		index = line.indexOf("::");
		if (index == -1) {
			++fileOrderGroupID;

			if (validName.match(line).hasMatch()) {
				scripts.setGroupName(groupIDs.at(fileOrderGroupID), line);
			} else {
				qWarning() << "JsmFile::openSym error invalid group name" << line;
			}
			methodID = 1;
		} else if (validName.match(line.mid(index + 2)).hasMatch()) {
			if (fileOrderGroupID == -1) {
				qWarning() << "JsmFile::openSym undefined constructor" << line;
				break;
			}
			int groupID = groupIDs.at(fileOrderGroupID);

			if (!line.startsWith(scripts.group(groupID).name())) {
				qWarning() << "JsmFile::openSym method does not start with the group name" << line << scripts.group(groupID).name();
				break;
			}

			if (methodID < scripts.nbScript(groupID)) {
				scripts.setScriptName(groupID, methodID, line.mid(index + 2));
			} else {
				qWarning() << "JsmFile::openSym error 2" << methodID << (scripts.nbScript(groupID));
				break;
			}
			++methodID;
		} else {
			qWarning() << "JsmFile::openSym error script name invalid" << line;
			++methodID;
		}
	}

	return true;
}

bool JsmFile::saveWithPath(const QString &path)
{
	QFile jsmFile(path);
	if (!jsmFile.open(QIODevice::WriteOnly)) {
		lastError = jsmFile.errorString();
		return false;
	}
	QByteArray jsm, sym;
	save(jsm, sym);
	jsmFile.write(jsm);
	jsmFile.close();

	if (!sym.isEmpty()) {
		QFile symFile(path.left(path.lastIndexOf('.')) + ".sym");
		if (!symFile.open(QIODevice::WriteOnly)) {
			lastError = symFile.errorString();
			return false;
		}
		symFile.write(sym);
		symFile.close();
	}

	return true;
}

bool JsmFile::save(QByteArray &jsm) const
{
	QByteArray section0, offsetScriptPositionsSection;
	quint16 data;
	JsmHeader jsmHeader;
	// It is important to keep the ordering for doors and backgrounds
	QMap<int, quint16> doorGroups, backgroundGroups;
	QByteArray locationGroups, otherGroups;
	QSet<quint16> scriptsWithFlag;

	for (const JsmGroup &group: scripts.groups()) {
		if (group.scriptCount() < 1) {
			qWarning() << "JsmFile::save" << "Cannot save script: scriptCount cannot be < 1";
			return false;
		}

		if (_oldFormat) {
			data = ((group.scriptCount() - 1) << 8) | (group.label() & 0xFF);
		} else {
			data = (group.label() << 7) | ((group.scriptCount() - 1) & 0x7F);
		}

		bool isFlagged = true;
		switch (group.type()) {
		case JsmGroup::Door:
			if (doorGroups.contains(group.groupTypeRelativeId())) {
				qWarning() << "JsmFile::save" << "Same door id twice";
				return false;
			}
			doorGroups.insert(group.groupTypeRelativeId(), data);
			break;
		case JsmGroup::Location:
			locationGroups.append((const char *)&data, 2);
			break;
		case JsmGroup::Background:
			if (backgroundGroups.contains(group.groupTypeRelativeId())) {
				qWarning() << "JsmFile::save" << "Same background id twice";
				return false;
			}
			backgroundGroups.insert(group.groupTypeRelativeId(), data);
			break;
		default:
			otherGroups.append((const char *)&data, 2);
			isFlagged = false;
			break;
		}

		if (isFlagged) {
			for (int i = group.label(); i < group.label() + group.scriptCount(); ++i) {
				scriptsWithFlag.insert(i);
			}
		}
	}

	section0.append(locationGroups);
	for (quint16 p: doorGroups) {
		section0.append((const char *)&p, 2);
	}
	for (quint16 p: backgroundGroups) {
		section0.append((const char *)&p, 2);
	}
	section0.append(otherGroups);
	jsmHeader.countDoors = doorGroups.size();
	jsmHeader.countLines = locationGroups.size() / 2;
	jsmHeader.countBackground = backgroundGroups.size();
	jsmHeader.countOther = otherGroups.size() / 2;

	int scriptId = 0;
	for (const JsmScript &script: scripts.scripts()) {
		data = (int(scriptsWithFlag.contains(scriptId)) << 15) | (script.opcodeIDStart() & 0x7FFF);
		offsetScriptPositionsSection.append((char *)&data, 2);
		scriptId += 1;
	}
	offsetScriptPositionsSection.append(QByteArray(_offsetScriptPositionsSectionPadding, '\x00'));

	jsmHeader.offsetScriptPositionsSection = 8 + section0.size();
	jsmHeader.offsetScriptDataSection = jsmHeader.offsetScriptPositionsSection + offsetScriptPositionsSection.size();

	jsm.append((char *)&jsmHeader, 8);
	jsm.append(section0);
	jsm.append(offsetScriptPositionsSection);
	jsm.append(scripts.data().constData());// Section 2

	return true;
}

bool JsmFile::save(QByteArray &jsm, QByteArray &sym)
{
	save(jsm);
	sym = saveSym();

	return true;
}

bool JsmFile::toFileSym(const QString &path)
{
	QFile f(path);
	if (f.open(QIODevice::WriteOnly)) {
		f.write(saveSym());
		f.close();
		return true;
	}
	lastError = f.errorString();
	return false;
}

QByteArray JsmFile::saveSym()
{
	QString ret;
	int nbGroups = scripts.nbGroup();
	QList<int> groupIDs = scripts.groupIDsSortedByLabel();

	for (int fileOrderGroupID = 0; fileOrderGroupID < nbGroups; ++fileOrderGroupID) {
		const JsmGroup &group = scripts.group(groupIDs.at(fileOrderGroupID));

		if (group.name().isEmpty()) {
			qWarning() << "JsmFile::saveSym" << "Empty group name";
			return QByteArray();
		}

		if (group.type() != JsmGroup::Door && group.type() != JsmGroup::Background && group.type() != JsmGroup::Location) {
			ret.append(group.name().leftJustified(31));
			ret.append('\n');
		}
	}

	for (int fileOrderGroupID = 0; fileOrderGroupID < nbGroups; ++fileOrderGroupID) {
		const JsmGroup &group = scripts.group(groupIDs.at(fileOrderGroupID));

		if (group.type() == JsmGroup::Location) {
			ret.append(group.name().leftJustified(31));
			ret.append('\n');
		}
	}

	for (int fileOrderGroupID = 0; fileOrderGroupID < nbGroups; ++fileOrderGroupID) {
		const JsmGroup &group = scripts.group(groupIDs.at(fileOrderGroupID));

		if (group.type() == JsmGroup::Background) {
			ret.append(group.name().leftJustified(31));
			ret.append('\n');
		}
	}

	// Door type is not present in the header list

	if (ret.isEmpty()) {
		qWarning() << "JsmFile::saveSym" << "Missing names";
		return QByteArray();
	}

	for (int fileOrderGroupID = 0; fileOrderGroupID < nbGroups; ++fileOrderGroupID) {
		const int groupID = groupIDs.at(fileOrderGroupID);
		const int nbScripts = scripts.nbScript(groupID);
		const QString &groupName = scripts.group(groupID).name();
		for (int scriptID = 0; scriptID < nbScripts; ++scriptID) {
			QString name;
			if (scriptID != 0) {
				name = groupName + "::" + scripts.script(groupID, scriptID).name();
			} else {
				name = groupName; // constructor
			}
			if (name.isEmpty()) {
				qWarning() << "JsmFile::saveSym" << "Empty script name";
				return QByteArray();
			}

			ret.append(name.leftJustified(31));
			ret.append('\n');
		}
	}

	return ret.toLatin1();
}

void JsmFile::searchWindows()
{
//	qDebug() << "JsmFile::searchWindows";
	int nbOpcode = scripts.data().nbOpcode();
	quint32 key;
	FF8Window window;
	window.ask_last = window.ask_first = window.ask_first2 = 0;
	ff8Windows.clear();

	for (int i=4 ; i<nbOpcode ; ++i) {
		JsmOpcode op = scripts.opcode(i);
		key = op.key();

		switch (key) {
		case JsmOpcode::AMESW:
		case JsmOpcode::AMES:
		case JsmOpcode::RAMESW:
		{
			JsmOpcode _psh2=scripts.opcode(i-3);
			if (_psh2.key() == JsmOpcode::PSHN_L) {
				JsmOpcode _psh1=scripts.opcode(i-4);
				JsmOpcode _psh3=scripts.opcode(i-2);
				JsmOpcode _psh4=scripts.opcode(i-1);
				window.type = key;
				window.x = _psh3.key() == JsmOpcode::PSHN_L ? _psh3.param() : -1;
				window.y = _psh4.key() == JsmOpcode::PSHN_L ? _psh4.param() : -1;
				window.u1 = _psh1.key() == JsmOpcode::PSHN_L ? _psh1.param() : -1;
				window.script_pos = i;
				ff8Windows.insert(_psh2.param(), window);
			}
			break;
		}
		case JsmOpcode::AASK:
			if (i>=8) {
				JsmOpcode _psh1=scripts.opcode(i-8);
				JsmOpcode _psh2=scripts.opcode(i-7);
				JsmOpcode _psh3=scripts.opcode(i-6);
				JsmOpcode _psh4=scripts.opcode(i-5);
				JsmOpcode _psh5=scripts.opcode(i-4);
				// ask_mem
				JsmOpcode _psh7=scripts.opcode(i-2);
				JsmOpcode _psh8=scripts.opcode(i-1);
				if (i>=8
						&& _psh1.key() == JsmOpcode::PSHN_L
						&& _psh2.key() == JsmOpcode::PSHN_L
						&& _psh3.key() == JsmOpcode::PSHN_L
						&& _psh4.key() == JsmOpcode::PSHN_L
						&& _psh5.key() == JsmOpcode::PSHN_L
						&& _psh7.key() == JsmOpcode::PSHN_L
						&& _psh8.key() == JsmOpcode::PSHN_L) {
					window.type = key;
					window.x = _psh7.param();
					window.y = _psh8.param();
					window.ask_first = _psh3.param();
					window.ask_last = _psh4.param();
					window.ask_first2 = _psh5.param();
					window.u1 = _psh1.param();
					window.script_pos = i;
					ff8Windows.insert(_psh2.param(), window);
				}
			}
			break;
		}
	}
}

void JsmFile::searchDefaultBGStates(QMultiMap<quint8, quint8> &params) const
{
	// qDebug() << "JsmFile::searchDefaultBGStates";
	int nbGroup = scripts.nbGroup(), nbOpcode;

	for (int groupID=0 ; groupID < nbGroup ; ++groupID) {
		const JsmGroup &jsmGroup = scripts.group(groupID);
		int methodCount = (int)jsmGroup.scriptCount();

		if (jsmGroup.type() == JsmGroup::Background) {
			for (int methodID=0 ; methodID < methodCount && methodID < 2 ; ++methodID) {
				int pos = scripts.opcodeIDStartScript(groupID, methodID, &nbOpcode);

				for (int opcodeID=0 ; opcodeID < nbOpcode ; ++opcodeID) {
					// qDebug() << groupID << methodID << jsmGroup.groupTypeRelativeId() << scripts.key(pos + opcodeID);
					switch (scripts.key(pos + opcodeID)) {
					case JsmOpcode::BGDRAW:
						if (scripts.key(pos + opcodeID - 1) == JsmOpcode::PSHN_L)
							params.insert(jsmGroup.groupTypeRelativeId(), scripts.param(pos + opcodeID - 1));
						break;
					case JsmOpcode::RBGANIMELOOP:
						params.insert(jsmGroup.groupTypeRelativeId(), 0);
						break;
					case JsmOpcode::RBGSHADELOOP:
						params.insert(jsmGroup.groupTypeRelativeId(), 0);
						break;
					}
				}
			}
		}
	}
}

/*void JsmFile::searchLines(QMap<quint8, Vertex[2]> &lines) const
{
//	qDebug() << "JsmFile::searchDefaultBGStates";
	int nbGroup = scripts.nbGroup(), nbOpcode;

	for (int groupID = 0; groupID < nbGroup; ++groupID) {
		const JsmGroup &jsmGroup = scripts.group(groupID);
		int methodCount = (int)jsmGroup.scriptCount();

		if (jsmGroup.type() == JsmGroup::Location && methodCount > 0) {
			int pos = scripts.opcodeIDStartScript(groupID, 0, &nbOpcode);

			for (int opcodeID = 0; opcodeID < nbOpcode; ++opcodeID) {
				switch (scripts.key(pos + opcodeID)) {
				case JsmOpcode::SETLINE:
					//TODO
					break;
			}
		}
	}
}*/

void JsmFile::searchGroupTypes()
{
	int nbGroup = scripts.nbGroup(), character = -1, modelId = -1;
	bool mainType = false;

	_mapID = -1;

	for (int groupID = 0; groupID < nbGroup; ++groupID) {
		const JsmGroup &jsmGroup = scripts.group(groupID);

		if (jsmGroup.type() != JsmGroup::No) {
			continue;
		}

		int methodCount = int(jsmGroup.scriptCount());
		mainType = false;
		character = -1;
		modelId = -1;

		for(int methodID = 0; methodID < methodCount; ++methodID) {
			int nbOpcode = 0;
			// First script only
			int pos = scripts.opcodeIDStartScript(groupID, methodID, &nbOpcode);

			for (int opcodeID = 0; opcodeID < nbOpcode; ++opcodeID) {
				JsmOpcode op = scripts.opcode(pos + opcodeID);
				int param = op.param();

				switch (op.key()) {
				case JsmOpcode::SETMODEL:
					modelId = param;
					break;
				case JsmOpcode::SETPC:
					if (opcodeID > 0 && scripts.key(pos + opcodeID - 1) == JsmOpcode::PSHN_L) {
						param = scripts.param(pos + opcodeID - 1);
						// if setpc has multiple definition
						if (character != -1 && param != character) {
							character = UNKNOWN_CHARACTER;
						} else {
							character = param;
						}
					}
					break;
				case JsmOpcode::SETDRAWPOINT:
					character = DRAWPOINT_CHARACTER;
					break;
				case JsmOpcode::FADEIN:
					mainType = true;
					break;
				case JsmOpcode::SETPLACE:
					mainType = true;
					if (scripts.key(pos + opcodeID - 1) == JsmOpcode::PSHN_L) {
						_mapID = scripts.param(pos + opcodeID - 1);
					}
					break;
				}
			}
		}

		if (character != -1) {
			scripts.setGroupCharacter(groupID, character);
		}
		if (modelId != -1) {
			scripts.setGroupType(groupID, JsmGroup::Model);
			scripts.setGroupModelId(groupID, modelId);
		} else if (mainType) {
			scripts.setGroupType(groupID, JsmGroup::Main);
		}
	}
}

int JsmFile::mapID() const
{
	return _mapID;
}

int JsmFile::nbWindows(quint8 textID) const
{
	return ff8Windows.count(textID);
}

QList<FF8Window> JsmFile::windows(quint8 textID) const
{
	return ff8Windows.values(textID);
}

void JsmFile::setWindow(quint8 textID, int winID, const FF8Window &window)
{
//	qDebug() << "JsmFile::setWindow" << window.x << window.y;
	quint32 key;

	if (window.script_pos >= scripts.data().nbOpcode()) {
		qWarning() << "JsmFile::setWindow error 1" << window.script_pos << scripts.data().nbOpcode() << textID << winID;
		return;
	}

	key = scripts.key(window.script_pos);
	if (window.type != key) {
		qWarning() << "JsmFile::setWindow error 2" << window.type << key << textID << winID;
		return;
	}

	switch (window.type) {
	case JsmOpcode::AMESW:
	case JsmOpcode::AMES:
	case JsmOpcode::RAMESW:
		scripts.setParam(window.script_pos-4, window.u1);
		scripts.setParam(window.script_pos-3, textID);
		scripts.setParam(window.script_pos-2, window.x);
		scripts.setParam(window.script_pos-1, window.y);
		break;
	case JsmOpcode::AASK:
		scripts.setParam(window.script_pos-8, window.u1);
		scripts.setParam(window.script_pos-7, textID);
		scripts.setParam(window.script_pos-6, window.ask_first);
		scripts.setParam(window.script_pos-5, window.ask_last);
		scripts.setParam(window.script_pos-4, window.ask_first2);
		// ask_mem
		scripts.setParam(window.script_pos-2, window.x);
		scripts.setParam(window.script_pos-1, window.y);
		break;
	default:
		qWarning() << "JsmFile::setWindow error 3" << window.type << textID << winID;
		return;
	}

	needUpdate = modified = true;

	searchWindows();
}

QString JsmFile::toString(int groupID, int methodID, bool moreDecompiled,
                          const Field *field, int indent, bool noCache)
{
	if (!noCache) {
		const QString &cache = scripts.script(groupID, methodID).decompiledScript(moreDecompiled);
		if (!cache.isEmpty() && !needUpdate && !(needUpdateMore && moreDecompiled)) {
			return cache;
		}
	}
	needUpdate = false;
	needUpdateMore = false;
	QString ret;
	if (moreDecompiled) {
		ret = _toStringMore(groupID, methodID, field, indent);
	} else {
		ret = _toString(groupID, methodID, indent);
	}

	setDecompiledScript(groupID, methodID, ret, moreDecompiled);

	return ret;
}

QString JsmFile::_toString(int groupID, int methodID, int indent) const
{
	QString ret;
	QList<JsmOpcode *> opcodes = scripts.opcodesp(groupID, methodID, true);

	for (JsmOpcode *op: opcodes) {
		if (ret.isEmpty() && op->key() == JsmOpcode::LBL) { // LBL is auto-generated
			delete op;
			continue;
		}
		if (indent > 0) {
			ret.append(QString(indent, QChar('\t')));
		}
		ret.append(op->toString());
		ret.append("\n");
		delete op;
	}

	return ret;
}

QString JsmFile::_toStringMore(int groupID, int methodID, const Field *field, int indent) const
{
	QSet<void *> collectPointers;
	JsmProgram program = scripts.program(groupID, methodID, collectPointers);
	QStringList ret = program.toStringList(field, indent);

	qDeleteAll(collectPointers);

	return ret.join("\n");
}

int JsmFile::opcodePositionInText(int groupID, int methodID, int opcodeID, bool more, const Field *field) const
{
	if (more) {
		QSet<void *> collectPointers;
		if (scripts.data().nbOpcode() > 0 && scripts.opcode(groupID, methodID, 0).key() == JsmOpcode::LBL) {
			opcodeID -= 1;
		}
		JsmProgram program = scripts.program(groupID, methodID, collectPointers);
		QString ret = program.toStringList(field, 0, opcodeID).join("\n");

		qDeleteAll(collectPointers);

		int line = 1, index = ret.indexOf("[X]");
		if (index >= 0) {
			line = ret.left(index).count('\n');
		}

		return line;
	} else {
		QList<JsmOpcode *> opcodes = scripts.opcodesp(groupID, methodID, false);
		int line = opcodeID;
		QList<qint32> labels = scripts.searchJumps(opcodes);
		if (!opcodes.isEmpty() && opcodes.first()->key() == JsmOpcode::LBL) {
			--line;
		}

		for (int i = 0; i <= opcodeID; ++i) {
			if (labels.contains(i)) {
				++line;
			}
		}

		qDeleteAll(opcodes);

		return line;
	}
}

int JsmFile::fromString(int groupID, int methodID, const QString &text, QString &errorStr)
{
	QRegularExpression endLine("(\\r\\n|\\n|\\r)");
	QRegularExpression spaces("[\\t ]+");
	QStringList lines = text.split(endLine);
	JsmData res;
	int key, param, posLbl, lbl;
	bool ok;
	QMap<int, int> labelsPos;
	QMultiMap<int, int> gotosPos;

	errorStr = "";

	int l = 1;
	for (const QString &line: lines) {
		QStringList rows = line.split(spaces, Qt::SkipEmptyParts);
		int rowsSize = rows.size();
		if (rowsSize < 1) {
			++l;
			continue;
		}

		if (rowsSize > 2) {
			errorStr = QObject::tr("Too many arguments");
			return l;
		}

		const QString &first = rows.first();

		// Automatic LBL
		if (res.constData().isEmpty() && !first.startsWith("LBL")) {
			res.append(JsmOpcode(JsmOpcode::LBL, scripts.firstMethodID(groupID) + methodID));
		}

		if (first.startsWith("LABEL", Qt::CaseInsensitive)) {
			lbl = first.mid(5).toInt(&ok);
			if (!ok) {
				errorStr = QObject::tr("Unable to convert to integer after 'LABEL': %1").arg(first.mid(5));
				return l;
			}
			if (labelsPos.contains(lbl)) {
				errorStr = QObject::tr("'LABEL %1' already declared.").arg(lbl);
				return l;
			}
			labelsPos.insert(lbl, res.nbOpcode());
			++l;
			continue;
		}

		if ((key = opcodeNames.indexOf(first.toUpper())) == -1) {
			errorStr = QObject::tr("Unknown opcode: %1").arg(first);
			return l;
		}

		if (rowsSize == 2) {
			const QString &second = rows.at(1);

			if (key > 0x38) {
				errorStr = QObject::tr("This opcode can not have parameters: %1").arg(first);
				return l;
			}

			if (key == JsmOpcode::CAL) {
				param = opcodeNamesCalc.indexOf(second.toUpper());
				if (param == -1) {
					param = second.toInt(&ok);
					if (!ok) {
						errorStr = QObject::tr("Unknown operation: %1")
						           .arg(second);
						return l;
					}
				}
			} else if (key >= JsmOpcode::JMP && key <= JsmOpcode::GJMP) {
				if (second.startsWith("LABEL", Qt::CaseInsensitive)) {
					lbl = second.mid(5).toInt(&ok);
					if (!ok) {
						errorStr = QObject::tr("Unable to convert to integer after 'LABEL': %1").arg(second.mid(5));
						return l;
					}
					if ((posLbl = labelsPos.value(lbl, -1)) != -1) {
						param = posLbl - res.nbOpcode();
					} else {
						param = -1;
						gotosPos.insert(lbl, res.nbOpcode());
					}
				} else {
					param = second.toInt(&ok);
					if (!ok) {
						errorStr = QObject::tr("Unable to convert to integer: %1").arg(second);
						return l;
					}
				}
			} else if (key == JsmOpcode::PSHM_B ||
			          key == JsmOpcode::PSHM_W ||
			          key == JsmOpcode::PSHM_L ||
			          key == JsmOpcode::PSHSM_B ||
			          key == JsmOpcode::PSHSM_W ||
			          key == JsmOpcode::PSHSM_L ||
			          key == JsmOpcode::POPM_B ||
			          key == JsmOpcode::POPM_W ||
			          key == JsmOpcode::POPM_L) {
				if (second.startsWith("VAR", Qt::CaseInsensitive)) {
					param = second.mid(3).toInt(&ok);
					if (!ok) {
						errorStr = QObject::tr("Unable to convert to integer after 'VAR': %1")
						           .arg(second.mid(3));
						return l;
					}
				} else {
					param = second.toInt(&ok);
					if (!ok) {
						errorStr = QObject::tr("Unable to convert to integer: %1").arg(second);
						return l;
					}
				}
			} else if (key == JsmOpcode::PSHI_L || key == JsmOpcode::POPI_L) {
				if (second.startsWith("TEMP", Qt::CaseInsensitive)) {
					param = second.mid(4).toInt(&ok);
					if (!ok) {
						errorStr = QObject::tr("Unable to convert to integer after 'TEMP': %1")
						           .arg(second.mid(4));
						return l;
					}
				} else {
					param = second.toInt(&ok);
					if (!ok) {
						errorStr = QObject::tr("Unable to convert to integer: %1").arg(second);
						return l;
					}
				}
			} else if (key == JsmOpcode::PSHAC) {
				if (second.startsWith("MODEL", Qt::CaseInsensitive)) {
					param = second.mid(5).toInt(&ok);
					if (!ok) {
						errorStr = QObject::tr("Unable to convert to integer after 'MODEL': %1")
						           .arg(second.mid(5));
						return l;
					}
				} else {
					param = second.toInt(&ok);
					if (!ok) {
						errorStr = QObject::tr("Unable to convert to integer: %1").arg(second);
						return l;
					}
				}
			} else {
				param = second.toInt(&ok);
				if (!ok) {
					errorStr = QObject::tr("Unable to convert to integer: %1").arg(second);
					return l;
				}
			}

			res.append(JsmOpcode(key, param));
		} else {
			res.append(JsmOpcode(key));
		}

		++l;
	}

	QMultiMapIterator<int, int> it(gotosPos);
	while (it.hasNext()) {
		it.next();
		lbl = it.key();
		if ((posLbl = labelsPos.value(lbl, -1)) != -1) {
			param = posLbl - it.value();
			JsmOpcode op = res.opcode(it.value());
			op.setParam(param);
			res.setOpcode(it.value(), op);
		} else {
			errorStr = QObject::tr("'LABEL %1' undefined.").arg(lbl);
			return -1;
		}
	}

	scripts.replaceScript(groupID, methodID, res);

	modified = true;
	needUpdateMore = true;

	return 0;
}

bool JsmFile::search(SearchType type, quint64 value, int &groupID, int &methodID, int &opcodeID) const
{
	int groupListSize = scripts.nbGroup(), nbOpcode, methodCount;

	if (groupID < 0)   groupID = 0;
	if (methodID < 0)  methodID = 0;
	if (opcodeID < 0)  opcodeID = 0;

	while (groupID < groupListSize) {
		methodCount = scripts.nbScript(groupID);

		while (methodID < methodCount) {
			const int pos = scripts.opcodeIDStartScript(groupID, methodID, &nbOpcode);

			while (opcodeID < nbOpcode) {
				if (searchInOpcode(type, value, quint16(pos), opcodeID)) {
					return true;
				}

				++opcodeID;
			}
			++methodID;
			opcodeID = 0;
		}
		++groupID;
		methodID = 0;
	}

	return false;
}

bool JsmFile::searchReverse(SearchType type, quint64 value, int &groupID, int &methodID, int &opcodeID) const
{
	if (groupID >= scripts.nbGroup()) {
		groupID = scripts.nbGroup() - 1;
	}

	while (groupID >= 0) {
		if (methodID >= scripts.nbScript(groupID)) {
			methodID = scripts.nbScript(groupID) - 1;
		}

		while (methodID >= 0) {
			int nbOpcode;
			const int pos = scripts.opcodeIDStartScript(groupID, methodID, &nbOpcode);

			if (opcodeID >= nbOpcode) {
				opcodeID = nbOpcode - 1;
			}

			while (opcodeID >= 0) {
				if (searchInOpcode(type, value, quint16(pos), opcodeID)) {
					return true;
				}

				--opcodeID;
			}
			--methodID;
			opcodeID = 2147483647;
		}
		--groupID;
		methodID = 2147483647;
	}

	return false;
}

bool JsmFile::search(SearchType type, const QList<quint64> &values, int &groupID, int &methodID, int &opcodeID) const
{
	int groupListSize = scripts.nbGroup(), nbOpcode, methodCount;

	if (groupID < 0)		groupID = 0;
	if (methodID < 0)	methodID = 0;
	if (opcodeID < 0)	opcodeID = 0;

	while (groupID < groupListSize) {
		methodCount = scripts.nbScript(groupID);

		while (methodID < methodCount) {
			const int pos = scripts.opcodeIDStartScript(groupID, methodID, &nbOpcode);

			while (opcodeID < nbOpcode) {
				for (quint64 value: values) {
					if (searchInOpcode(type, value, quint16(pos), opcodeID)) {
						return true;
					}
				}

				++opcodeID;
			}
			++methodID;
			opcodeID = 0;
		}
		++groupID;
		methodID = 0;
	}

	return false;
}

bool JsmFile::searchReverse(SearchType type, const QList<quint64> &values, int &groupID, int &methodID, int &opcodeID) const
{
	if (groupID >= scripts.nbGroup()) {
		groupID = scripts.nbGroup() - 1;
	}

	while (groupID >= 0) {
		if (methodID >= scripts.nbScript(groupID)) {
			methodID = scripts.nbScript(groupID) - 1;
		}

		while (methodID >= 0) {
			int nbOpcode;
			const int pos = scripts.opcodeIDStartScript(groupID, methodID, &nbOpcode);

			if (opcodeID >= nbOpcode) {
				opcodeID = nbOpcode - 1;
			}

			while (opcodeID >= 0) {
				for (quint64 value: values) {
					if (searchInOpcode(type, value, quint16(pos), opcodeID)) {
						return true;
					}
				}

				--opcodeID;
			}
			--methodID;
			opcodeID = 2147483647;
		}
		--groupID;
		methodID = 2147483647;
	}

	return false;
}

bool JsmFile::searchInOpcode(SearchType type, quint64 value, quint16 pos, int opcodeID) const
{
	JsmOpcode op = scripts.opcode(pos + opcodeID);
	quint32 key = op.key();
	qint32 param = op.param();

	switch (type) {
	case SearchText:
		switch (key) {
		case JsmOpcode::AMESW:
		case JsmOpcode::AMES:
		case JsmOpcode::RAMESW:
			return opcodeID >= 3 && scripts.key(pos + opcodeID - 3) == JsmOpcode::PSHN_L
					&& value == (quint32)scripts.param(pos + opcodeID - 3);
		case JsmOpcode::MES:
			return opcodeID >= 1 && scripts.key(pos + opcodeID - 1) == JsmOpcode::PSHN_L
					&& value == (quint32)scripts.param(pos + opcodeID - 1);
		case JsmOpcode::AASK:
			return opcodeID >= 7 && scripts.key(pos + opcodeID - 7) == JsmOpcode::PSHN_L
					&& value == (quint32)scripts.param(pos + opcodeID - 7);
		case JsmOpcode::ASK:
			return opcodeID >= 5 && scripts.key(pos + opcodeID - 5) == JsmOpcode::PSHN_L
					&& value == (quint32)scripts.param(pos + opcodeID - 5);
		}
		return false;
	case SearchOpcode:
		return key == (value & 0xFFFF) && ((int)(value >> 16) == -1 || param == (int)(value >> 16));
	case SearchVar:
		return (((value & 0x80000000) && (key == JsmOpcode::POPM_B || key == JsmOpcode::POPM_W || key == JsmOpcode::POPM_L))
		        || ((value & 0x40000000) && (key >= 10 && key <= 18 && key != JsmOpcode::POPM_B && key != JsmOpcode::POPM_W && key != JsmOpcode::POPM_L))
		        || (!(value & 0xC0000000) && key >= 10 && key <= 18))
		        && (value & 0x3FFFFFFF) == (quint32)param;
	case SearchExec:
		if (opcodeID < 1 || key < 20 || key > 22) {
			return false;
		}

		return ((value >> 16) & 0xFFFF) == (quint32)scripts.param(pos + opcodeID - 1) // label
				&& (value & 0xFFFF) == (quint32)param; // group
	case SearchMapJump:
		switch (key) {
		case JsmOpcode::MAPJUMP:
		case JsmOpcode::MAPJUMP3:
		case JsmOpcode::MAPJUMPO:
			return opcodeID >= 1 && scripts.key(pos + opcodeID - 1) == JsmOpcode::PSHN_L
			        && value == (quint32)scripts.param(pos + opcodeID - 1);
		}
		return false;
	}

	return false;
}

QList<int> JsmFile::searchAllVars() const
{
	int nbOpcode = scripts.data().nbOpcode();
	quint32 key;
	QList<int> vars;

	for (int i = 0; i < nbOpcode; ++i) {
		JsmOpcode op = scripts.opcode(i);
		key = op.key();

		if (key >= 10 && key <= 18 && !vars.contains(op.opcode())) {
			vars.append(op.opcode());
		}
	}

	return vars;
}

QList<int> JsmFile::searchAllSpells(const QString &fieldName) const
{
	int nbOpcode = scripts.data().nbOpcode(), param;
	unsigned int key;
	QList<int> ret;
	bool setDrawPoint = false;
//	QString ret;

	for (int i = 0; i < nbOpcode; ++i) {
		JsmOpcode op = scripts.opcode(i);
		key = op.key();
		param = op.param();

		if (key == JsmOpcode::SETDRAWPOINT) {
			setDrawPoint = true;
		}
		else if (key == JsmOpcode::DRAWPOINT && i>0) {
			if (!setDrawPoint) {
				qDebug() << fieldName << "magie ?";
			} else if (scripts.key(i-1) == JsmOpcode::PSHN_L) {
				param = scripts.param(i-1);
				/*QStringList curList = ret.value(param, QStringList());
				curList.append(fieldName);
				ret.insert(param, curList);*/
				ret.append(param);
//				ret.append(" - ").append(QString::number(param)).append("\n");
			}
		}
	}

	return ret;
}

QList<int> JsmFile::searchAllCards(const QString &fieldName) const
{
	int nbOpcode = scripts.data().nbOpcode(), param;
	unsigned int key;
	QList<int> ret;

	for (int i = 0; i < nbOpcode; ++i) {
		JsmOpcode op = scripts.opcode(i);
		key = op.key();
		param = op.param();

		if (key == JsmOpcode::GETCARD && i > 0) {
			if (scripts.key(i - 1) == JsmOpcode::PSHN_L) {
				param = scripts.param(i - 1);
				ret.append(param);
			} else {
				qWarning() << fieldName << "GETCARD without PSHN_L!" << QString::number(scripts.key(i - 1), 16);
			}
		}
	}

	return ret;
}

QList<int> JsmFile::searchAllCardPlayers(const QString &fieldName) const
{
	int nbOpcode = scripts.data().nbOpcode(), param;
	unsigned int key;
	QList<int> ret;

	for (int i = 0; i < nbOpcode; ++i) {
		JsmOpcode op = scripts.opcode(i);
		key = op.key();
		param = op.param();

		if (key == JsmOpcode::CARDGAME && i > 6) {
			if (scripts.key(i - 7) == JsmOpcode::PSHN_L) {
				param = scripts.param(i - 7);
				ret.append(param);
			} else {
				qWarning() << fieldName << "CARDGAME without PSHN_L!" << QString::number(scripts.key(i - 7), 16);
			}
		}
	}

	return ret;
}

QList<int> JsmFile::searchAllMoments() const
{
	int nbOpcode = scripts.data().nbOpcode();
	quint32 key;
	qint32 param;
	QList<int> ret;

	for (int i = 0; i < nbOpcode; ++i) {
		JsmOpcode op = scripts.opcode(i);
		key = op.key();
		param = op.param();

		if ((key == JsmOpcode::POPM_B || key == JsmOpcode::POPM_W || key == JsmOpcode::POPM_L) && i > 0 && param == 256) {
			op = scripts.opcode(i-1);
			param = op.param();

			if (op.key() == JsmOpcode::PSHN_L && !ret.contains(param)) {
				ret.append(param);
			}
		}
	}

	return ret;
}

void JsmFile::searchAllOpcodeTypes(QMap<int, int> &ret/*, QMap<int, QString> &stackState*/) const
{
	int nbOpcode = scripts.data().nbOpcode();
	quint32 key;
	qint32 param;
	QStack<QString> stack;

	for (int i = 0; i < nbOpcode; ++i) {
		JsmOpcode op = scripts.opcode(i);
		key = op.key();
		param = op.param();
		int type = !op.hasParam() ? JUST_KEY : (param < 0 ? KEY_AND_SPARAM : KEY_AND_UPARAM);

		switch (key) {
		case JsmOpcode::PSHN_L:
			stack.push("numerical value");
			break;
		case JsmOpcode::PSHI_L:
			stack.push("temp value");
			break;
		case JsmOpcode::PSHM_B:
			stack.push("var value (byte)");
			break;
		case JsmOpcode::PSHM_W:
			stack.push("var value (word)");
			break;
		case JsmOpcode::PSHM_L:
			stack.push("var value (long)");
			break;
		case JsmOpcode::PSHSM_B:
			stack.push("var value (signed byte)");
			break;
		case JsmOpcode::PSHSM_W:
			stack.push("var value (signed word)");
			break;
		case JsmOpcode::PSHSM_L:
			stack.push("var value (signed long)");
			break;
		case JsmOpcode::PSHAC:
			stack.push("AC value");
			break;
		case JsmOpcode::POPI_L:
		case JsmOpcode::POPM_B:
		case JsmOpcode::POPM_W:
		case JsmOpcode::POPM_L:
			stack.pop();
			break;
		default:
			stack.clear();
			break;
		}

//		if (ret.contains(key)) {
//			if (ret.value(key) != type) {
//				if (ret.value(key) == JUST_KEY || type == JUST_KEY) {
//					qWarning() << "[Type !=]" << "key" << QString::number(key,16) << "referencedType" << ret.value(key) << "currentType" << type;
//				}
//				else if (type == KEY_AND_SPARAM) {
//					ret.insert(key, KEY_AND_SPARAM);
//				}
//			}
//		} else {
			ret.insert(key, type);
//			stackState.insert(key,
//		}
	}
}
/*
void JsmFile::searchAllSavePoints(QList<Vertex> &ret)
{
	int nbOpcode = scripts.data().nbOpcode();
	quint32 key;
	qint32 param;

	for (int i=0 ; i<nbOpcode ; ++i) {
		JsmOpcode op = scripts.opcode(i);
		key = op.key();
		param = op.param();

		switch (key) {
		case JsmOpcode::SETLINE:
			// TODO
			break;
		case JsmOpcode::SAVEENABLE:
			// TODO
			break;
		}
	}
}*/

void JsmFile::setDecompiledScript(int groupID, int methodID, const QString &text, bool moreDecompiled)
{
	if (groupID == -1 || methodID == -1) {
		return;
	}

	scripts.setDecompiledScript(groupID, methodID, text, moreDecompiled);
}

void JsmFile::setCurrentOpcodeScroll(int groupID, int methodID, bool more, int scrollValue, const QTextCursor &textCursor)
{
	if (groupID == -1) {
		return;
	}
	groupItem = groupID;
	if (methodID == -1) {
		return;
	}
	methodItem.insert(groupID, methodID);
	if (scrollValue == -1) {
		return;
	}
	quint64 key = ((qint64)groupID << 32) | (qint64)methodID;
	(more ? opcodeScrollMore : opcodeScroll).insert(key, scrollValue);
	(more ? textCursorsMore : textCursors).insert(key, ((qint64)textCursor.position() << 32) | (qint64)textCursor.anchor());
}

int JsmFile::currentMethodItem(int groupID) const
{
	return methodItem.value(groupID, -1);
}

int JsmFile::currentOpcodeScroll(int groupID, int methodID, bool more) const
{
	return (more ? opcodeScrollMore : opcodeScroll).value(((qint64)groupID << 32) | (qint64)methodID, -1);
}

int JsmFile::currentTextCursor(int groupID, int methodID, bool more, int &anchor) const
{
	quint64 val = (more ? textCursorsMore : textCursors).value(((qint64)groupID << 32) | (qint64)methodID, -1);
	anchor = int(val & 0xFFFFFFFF);
	return val >> 32;
}
