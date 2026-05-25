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
	File(), _hasSym(false), needUpdate(true), needUpdateMore(true),
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

	if (headerID < jsmHeader.countLines) {
		type = JsmGroup::Location;
	} else if (headerID < jsmHeader.countLines + jsmHeader.countDoors) {
		type = JsmGroup::Door;
	} else if (headerID < jsmHeader.countLines + jsmHeader.countDoors + jsmHeader.countBackground) {
		type = JsmGroup::Background;
	} else {
		type = JsmGroup::No;
	}

	return JsmGroup(label, QList<JsmMethod>(int(count + 1), JsmMethod(JsmData())), type);
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
		groupListLabelPerScriptCount.insert(group.absMethodId(), group.methodCount());
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
		if (group.nativeType() == JsmGroup::No) {
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

	QList<quint16> methodOpcodeIDs;
	methodOpcodeIDs.reserve(scriptCount);

	for (int i = 0; i < scriptCount; ++i) {
		memcpy(&pos, &jsmData[jsmHeader.offsetScriptPositionsSection + i * 2], 2);
		if (i != 0 && pos == 0) {
			break;
		}
		pos &= 0x7FFF;
		if (jsmHeader.offsetScriptDataSection + pos * 4 > jsmDataSize || int(pos) < oldPos) {
			qWarning() << "JsmFile::open error (4)" << (jsmHeader.offsetScriptDataSection + pos * 4) << jsmDataSize;
			return false;
		}
		oldPos = pos;

		methodOpcodeIDs.append(pos);
	}

	methodOpcodeIDs.append((jsmDataSize - jsmHeader.offsetScriptDataSection) / 4);

	for (int groupID = 0; groupID < groupCount; ++groupID) {
		JsmGroup &jsmGroup = groupListByGroupID[groupID];

		for (int methodID = 0; methodID < jsmGroup.methodCount(); ++methodID) {
			int absMethodID = jsmGroup.absMethodId() + methodID;
			jsmGroup.method(methodID).setScriptData(JsmData(
				QByteArray(jsmData + jsmHeader.offsetScriptDataSection + methodOpcodeIDs.at(absMethodID) * 4, (methodOpcodeIDs.at(absMethodID + 1) - methodOpcodeIDs.at(absMethodID)) * 4),
				_oldFormat
			));
		}
	}

	scripts = JsmScripts(groupListByGroupID);

	_hasSym = openSym(symData);

	searchWindows();
	searchGroupTypes();
	forceNames();

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
	    nbMethods = scripts.countMethods();

	if (nbMethods <= 0 || nbLines != nbEntities + nbMethods) {
		qWarning() << "JsmFile::openSym error 1" << nbLines << nbEntities << nbMethods;
		return false;
	}

	QString line;
	QRegularExpression validName("^[\\w]+$");
	int fileOrderGroupID = -1, methodID = 1, index = -1, nbGroup = scripts.groups().size();
	QList<int> groupIDs = scripts.groupIDsSortedByAbsMethodId();

	for (int i = 0; i < nbMethods; ++i) {
		if (fileOrderGroupID >= nbGroup) {
			qWarning() << "JsmFile::openSym too much groups" << fileOrderGroupID;
			break;
		}

		line = symLines.at(nbEntities + i).simplified();
		index = line.indexOf("::");
		if (index == -1) {
			++fileOrderGroupID;

			if (validName.match(line).hasMatch()) {
				scripts.group(groupIDs.at(fileOrderGroupID)).setName(line);
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
			JsmGroup &jsmGroup = scripts.group(groupID);

			if (!line.startsWith(jsmGroup.name())) {
				qWarning() << "JsmFile::openSym method does not start with the group name" << line << scripts.group(groupID).name();
				break;
			}

			if (methodID < jsmGroup.methodCount()) {
				jsmGroup.method(methodID).setName(line.mid(index + 2));
			} else {
				qWarning() << "JsmFile::openSym error 2" << methodID << jsmGroup.methodCount();
				break;
			}
			++methodID;
		} else {
			qWarning() << "JsmFile::openSym error method name invalid" << line;
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
	quint16 toc;
	JsmHeader jsmHeader;
	// It is important to keep the ordering for doors and backgrounds
	QMap<int, quint16> doorGroups, backgroundGroups;
	QByteArray locationGroups, otherGroups;
	QSet<quint16> scriptsWithFlag;
	int groupID = 0;

	for (const JsmGroup &group: scripts.groups()) {
		if (group.methodCount() < 1) {
			qWarning() << "JsmFile::save" << "Cannot save script: methodCount cannot be < 1";
			return false;
		}

		if (_oldFormat) {
			toc = ((group.methodCount() - 1) << 8) | (group.absMethodId() & 0xFF);
		} else {
			toc = (group.absMethodId() << 7) | ((group.methodCount() - 1) & 0x7F);
		}

		int relativeId = scripts.calcGroupTypeRelativeId(groupID);

		bool isFlagged = true;
		switch (group.type()) {
		case JsmGroup::Door:
			if (doorGroups.contains(relativeId)) {
				qWarning() << "JsmFile::save" << "Same door id twice";
				return false;
			}
			doorGroups.insert(relativeId, toc);
			break;
		case JsmGroup::Location:
			locationGroups.append((const char *)&toc, 2);
			break;
		case JsmGroup::Background:
			if (backgroundGroups.contains(relativeId)) {
				qWarning() << "JsmFile::save" << "Same background id twice";
				return false;
			}
			backgroundGroups.insert(relativeId, toc);
			break;
		default:
			otherGroups.append((const char *)&toc, 2);
			isFlagged = false;
			break;
		}

		if (isFlagged) {
			for (int i = group.absMethodId(); i < group.absMethodId() + group.methodCount(); ++i) {
				scriptsWithFlag.insert(i);
			}
		}

		groupID += 1;
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

	JsmData jsmData;

	int scriptId = 0;
	for (const JsmMethod &jsmMethod: scripts.methods()) {
		toc = (int(scriptsWithFlag.contains(scriptId)) << 15) | (jsmData.nbOpcode() & 0x7FFF);
		offsetScriptPositionsSection.append((const char *)&toc, 2);
		jsmData.append(JsmOpcode(JsmOpcode::LBL, scriptId));
		jsmData.append(jsmMethod.scriptData());
		scriptId += 1;
	}
	toc = jsmData.nbOpcode() & 0x7FFF;
	offsetScriptPositionsSection.append((const char *)&toc, 2);
	jsmHeader.offsetScriptPositionsSection = 8 + section0.size();
	// Padding 4-bytes aligned
	if ((jsmHeader.offsetScriptPositionsSection + offsetScriptPositionsSection.size()) % 4 != 0) {
		offsetScriptPositionsSection.append(QByteArray(4 - (jsmHeader.offsetScriptPositionsSection + offsetScriptPositionsSection.size()) % 4, '\0'));
	}

	jsmHeader.offsetScriptDataSection = jsmHeader.offsetScriptPositionsSection + offsetScriptPositionsSection.size();

	jsm.append((const char *)&jsmHeader, 8);
	jsm.append(section0);
	jsm.append(offsetScriptPositionsSection);
	jsm.append(jsmData.constData()); // Section 2

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
	int nbGroups = scripts.groups().size();
	QList<int> groupIDs = scripts.groupIDsSortedByAbsMethodId();

	for (int fileOrderGroupID = 0; fileOrderGroupID < nbGroups; ++fileOrderGroupID) {
		const JsmGroup &group = scripts.group(groupIDs.at(fileOrderGroupID));

		if (group.name().isEmpty()) {
			qWarning() << "JsmFile::saveSym" << "Empty group name";
			return QByteArray();
		}

		if (group.nativeType() == JsmGroup::No) {
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
		const JsmGroup &jsmGroup = scripts.group(groupID);
		const int nbMethods = jsmGroup.methodCount();
		const QString &groupName = jsmGroup.name();
		for (int methodID = 0; methodID < nbMethods; ++methodID) {
			QString name;
			if (methodID != 0) {
				name = groupName + "::" + jsmGroup.method(methodID).name();
			} else {
				name = groupName; // constructor
			}
			if (name.isEmpty()) {
				qWarning() << "JsmFile::saveSym" << "Empty method name";
				return QByteArray();
			}

			ret.append(name.leftJustified(31));
			ret.append('\n');
		}
	}

	return ret.toLatin1();
}

void JsmFile::forceNames()
{
	int nbGroup = scripts.groups().size(),
	    directorCount = 1, squallCount = 1, zellCount = 1, irvineCount = 1, quistisCount = 1,
	    rinoaCount = 1, selphieCount = 1, seiferCount = 1, edeaCount = 1, lagunaCount = 1, kirosCount = 1,
	    wardCount = 1, modelCount = 1, drawPointCount = 1, eventLineCount = 1, doorCount = 1, bgCount = 1;
	QStringList groupNames;

	for (int groupID = 0; groupID < nbGroup; ++groupID) {
		const JsmGroup &grp = scripts.group(groupID);
		QString name = grp.name();
		JsmGroup::Type groupType = grp.type();

		if (name.isEmpty()) {
			switch (groupType) {
			case JsmGroup::Main:
				name = QString("Director%1").arg(directorCount);
				++directorCount;
				break;
			case JsmGroup::Model:
				switch (grp.character()) {
				case 0:
					name = QString("Squall%1").arg(squallCount);
					++squallCount;
					break;
				case 1:
					name = QString("Zell%1").arg(zellCount);
					zellCount++;
					break;
				case 2:
					name = QString("Irvine%1").arg(irvineCount);
					irvineCount++;
					break;
				case 3:
					name = QString("Quistis%1").arg(quistisCount);
					quistisCount++;
					break;
				case 4:
					name = QString("Rinoa%1").arg(rinoaCount);
					rinoaCount++;
					break;
				case 5:
					name = QString("Selphie%1").arg(selphieCount);
					selphieCount++;
					break;
				case 6:
					name = QString("Seifer%1").arg(seiferCount);
					seiferCount++;
					break;
				case 7:
					name = QString("Edea%1").arg(edeaCount);
					edeaCount++;
					break;
				case 8:
					name = QString("Laguna%1").arg(lagunaCount);
					lagunaCount++;
					break;
				case 9:
					name = QString("Kiros%1").arg(kirosCount);
					kirosCount++;
					break;
				case 10:
					name = QString("Ward%1").arg(wardCount);
					wardCount++;
					break;
				case -1:
					name = QString("Model%1").arg(modelCount);
					modelCount++;
					break;
				case DRAWPOINT_CHARACTER:
					name = QString("DrawPoint%1").arg(drawPointCount);
					drawPointCount++;
					break;
				}
				break;
			case JsmGroup::Location:
				name = QString("EventLine%1").arg(eventLineCount);
				eventLineCount++;
				break;
			case JsmGroup::Door:
				name = QString("Door%1").arg(doorCount);
				doorCount++;
				break;
			case JsmGroup::Background:
				name = QString("Background%1").arg(bgCount);
				bgCount++;
				break;
			case JsmGroup::No:
				break;
			}

			if (name.isEmpty()) {
				name = QString("Module%1").arg(groupID);
			}
		}

		// Force unique names
		if (groupNames.contains(name)) {
			name = QString("%1Dup%2").arg(name).arg(groupID);
		}

		JsmGroup &jsmGroup = scripts.group(groupID);
		jsmGroup.setName(name);
		groupNames.append(name);

		int nbMethods = jsmGroup.methodCount();
		QStringList methodNames;

		for (int methodID = 0; methodID < nbMethods; ++methodID) {
			JsmMethod &method = jsmGroup.method(methodID);
			name = method.name();

			if (name.isEmpty()) {
				if (methodID == 0) {
					name = "init";
				} else if (methodID == 1) {
					name = "default";
				} else {
					switch (groupType) {
					case JsmGroup::No:
						switch (methodID) {
						case 2: name = "talk"; break;
						case 3: name = "push"; break;
						}
						break;
					case JsmGroup::Location:
						switch (methodID) {
						case 2: name = "talk";     break;
						case 3: name = "push";     break;
						case 4: name = "across";   break;
						case 5: name = "touch";    break;
						case 6: name = "touchoff"; break;
						case 7: name = "touchon";  break;
						}
						break;
					case JsmGroup::Door:
						switch (methodID) {
						case 2: name = "open";  break;
						case 3: name = "close"; break;
						case 4: name = "on";    break;
						case 5: name = "off";   break;
						}
						break;
					case JsmGroup::Model:
					case JsmGroup::Main:
					case JsmGroup::Background:
						break;
					}
					if (name.isEmpty()) {
						name = QString("Method%1").arg(methodID);
					}
				}
			}

			if (methodID > 0) {
				// Force unique names
				if (methodNames.contains(name)) {
					name = QString("%1Dup%2").arg(name).arg(methodID);
				}

				methodNames.append(name);
			}

			method.setName(name);
		}
	}
}

void JsmFile::searchWindows()
{
//	qDebug() << "JsmFile::searchWindows";
	int nbGroup = scripts.groups().size();
	ff8Windows.clear();

	for (int groupID = 0; groupID < nbGroup; ++groupID) {
		const JsmGroup &jsmGroup = scripts.group(groupID);
		int methodCount = int(jsmGroup.methodCount());

		for (int methodID = 0; methodID < methodCount; ++methodID) {
			const JsmMethod &jsmMethod = jsmGroup.method(methodID);
			const JsmData &jsmData = jsmMethod.scriptData();
			int nbOpcode = jsmData.nbOpcode();

			for (int opcodeID = 4; opcodeID < nbOpcode; ++opcodeID) {
				JsmOpcode op = jsmData.opcode(opcodeID);
				quint32 key = op.key();

				switch (key) {
				case JsmOpcode::AMESW:
				case JsmOpcode::AMES:
				case JsmOpcode::RAMESW:
				{
					JsmOpcode _psh2 = jsmData.opcode(opcodeID - 3);
					if (_psh2.key() == JsmOpcode::PSHN_L) {
						JsmOpcode _psh1 = jsmData.opcode(opcodeID - 4),
						          _psh3 = jsmData.opcode(opcodeID - 2),
						          _psh4 = jsmData.opcode(opcodeID - 1);
						FF8Window window = FF8Window();
						window.type = key;
						window.x = _psh3.key() == JsmOpcode::PSHN_L ? _psh3.param() : -1;
						window.y = _psh4.key() == JsmOpcode::PSHN_L ? _psh4.param() : -1;
						window.u1 = _psh1.key() == JsmOpcode::PSHN_L ? _psh1.param() : -1;
						window.groupID = groupID;
						window.methodID = methodID;
						window.opcodeID = opcodeID;
						ff8Windows.insert(_psh2.param(), window);
					}
					break;
				}
				case JsmOpcode::AASK:
					if (opcodeID >= 8) {
						JsmOpcode _psh1 = jsmData.opcode(opcodeID - 8),
						          _psh2 = jsmData.opcode(opcodeID - 7),
						          _psh3 = jsmData.opcode(opcodeID - 6),
						          _psh4 = jsmData.opcode(opcodeID - 5),
						          _psh5 = jsmData.opcode(opcodeID - 4);
						// ask_mem
						JsmOpcode _psh7 = jsmData.opcode(opcodeID - 2),
						          _psh8 = jsmData.opcode(opcodeID - 1);
						if (opcodeID >= 8
								&& _psh1.key() == JsmOpcode::PSHN_L
								&& _psh2.key() == JsmOpcode::PSHN_L
								&& _psh3.key() == JsmOpcode::PSHN_L
								&& _psh4.key() == JsmOpcode::PSHN_L
								&& _psh5.key() == JsmOpcode::PSHN_L
								&& _psh7.key() == JsmOpcode::PSHN_L
								&& _psh8.key() == JsmOpcode::PSHN_L) {
							FF8Window window = FF8Window();
							window.type = key;
							window.x = _psh7.param();
							window.y = _psh8.param();
							window.ask_first = _psh3.param();
							window.ask_last = _psh4.param();
							window.ask_first2 = _psh5.param();
							window.u1 = _psh1.param();
							window.groupID = groupID;
							window.methodID = methodID;
							window.opcodeID = opcodeID;
							ff8Windows.insert(_psh2.param(), window);
						}
					}
					break;
				}
			}
		}
	}
}

void JsmFile::searchDefaultBGStates(QMultiMap<quint8, quint8> &params) const
{
	// qDebug() << "JsmFile::searchDefaultBGStates";
	int nbGroup = scripts.groups().size();

	for (int groupID = 0; groupID < nbGroup; ++groupID) {
		const JsmGroup &jsmGroup = scripts.group(groupID);

		if (jsmGroup.type() == JsmGroup::Background) {
			int methodCount = int(jsmGroup.methodCount()), bgParam = scripts.calcGroupTypeRelativeId(groupID);

			for (int methodID = 0; methodID < methodCount && methodID < 2; ++methodID) {
				const JsmData &jsmData = jsmGroup.method(methodID).scriptData();

				for (int opcodeID = 0; opcodeID < jsmData.nbOpcode(); ++opcodeID) {
					switch (jsmData.opcode(opcodeID).key()) {
					case JsmOpcode::BGDRAW:
						if (opcodeID > 0 && jsmData.opcode(opcodeID - 1).key() == JsmOpcode::PSHN_L) {
							params.insert(bgParam, jsmData.opcode(opcodeID - 1).param());
						}
						break;
					case JsmOpcode::RBGANIMELOOP:
						params.insert(bgParam, 0);
						break;
					case JsmOpcode::RBGSHADELOOP:
						params.insert(bgParam, 0);
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
	int nbGroup = scripts.groups().size();

	for (int groupID = 0; groupID < nbGroup; ++groupID) {
		const JsmGroup &jsmGroup = scripts.group(groupID);
		int methodCount = int(jsmGroup.methodCount());

		if (jsmGroup.type() == JsmGroup::Location && methodCount > 0) {
			const JsmData &jsmData = jsmGroup.method(0).scriptData();

			for (int opcodeID = 0; opcodeID < jsmData.nbOpcode(); ++opcodeID) {
				switch (jsmData.opcode(opcodeID).key()) {
				case JsmOpcode::SETLINE:
					//TODO
					break;
			}
		}
	}
}*/

void JsmFile::searchGroupTypes()
{
	int nbGroup = scripts.groups().size();

	_mapID = -1;

	for (int groupID = 0; groupID < nbGroup; ++groupID) {
		JsmGroup &jsmGroup = scripts.group(groupID);

		if (jsmGroup.type() == JsmGroup::Background || jsmGroup.type() == JsmGroup::Location || jsmGroup.type() == JsmGroup::Door) {
			continue;
		}

		int character = -1, modelId = -1;
		bool mainType = false;

		for (const JsmMethod &jsmMethod: jsmGroup.methods()) {
			const JsmData &jsmData = jsmMethod.scriptData();
			int nbOpcode = jsmData.nbOpcode();

			for (int opcodeID = 0; opcodeID < nbOpcode; ++opcodeID) {
				JsmOpcode op = jsmData.opcode(opcodeID);
				int param = op.param();

				switch (op.key()) {
				case JsmOpcode::SETMODEL:
					modelId = param;
					break;
				case JsmOpcode::SETPC:
					if (opcodeID > 0 && jsmData.opcode(opcodeID - 1).key() == JsmOpcode::PSHN_L) {
						param = jsmData.opcode(opcodeID - 1).param();
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
					if (opcodeID > 0 && jsmData.opcode(opcodeID - 1).key() == JsmOpcode::PSHN_L) {
						_mapID = jsmData.opcode(opcodeID - 1).param();
					}
					break;
				}
			}
		}

		if (character != -1) {
			jsmGroup.setCharacter(character);
		}
		if (modelId != -1) {
			jsmGroup.setType(JsmGroup::Model);
			jsmGroup.setModelId(modelId);
		} else if (mainType) {
			jsmGroup.setType(JsmGroup::Main);
		}
	}
}

bool JsmFile::setWindow(quint8 textID, const FF8Window &window)
{
	quint32 key;
	if (window.groupID >= scripts.groups().size()) {
		return false;
	}

	JsmGroup &jsmGroup = scripts.group(window.groupID);
	if (window.methodID >= int(jsmGroup.methodCount())) {
		return false;
	}

	JsmData &jsmData = jsmGroup.method(window.methodID).scriptData();
	if (window.opcodeID >= int(jsmData.nbOpcode())) {
		return false;
	}

	key = jsmData.opcode(window.opcodeID).key();
	if (window.type != key) {
		return false;
	}

	switch (window.type) {
	case JsmOpcode::AMESW:
	case JsmOpcode::AMES:
	case JsmOpcode::RAMESW:
		if (window.opcodeID <= 4) {
			return false;
		}
		jsmData.opcode(window.opcodeID - 4).setParam(window.u1);
		jsmData.opcode(window.opcodeID - 3).setParam(textID);
		jsmData.opcode(window.opcodeID - 2).setParam(window.x);
		jsmData.opcode(window.opcodeID - 1).setParam(window.y);
		break;
	case JsmOpcode::AASK:
		if (window.opcodeID <= 8) {
			return false;
		}
		jsmData.opcode(window.opcodeID - 8).setParam(window.u1);
		jsmData.opcode(window.opcodeID - 7).setParam(textID);
		jsmData.opcode(window.opcodeID - 6).setParam(window.ask_first);
		jsmData.opcode(window.opcodeID - 5).setParam(window.ask_last);
		jsmData.opcode(window.opcodeID - 4).setParam(window.ask_first2);
		// ask_mem
		jsmData.opcode(window.opcodeID - 2).setParam(window.x);
		jsmData.opcode(window.opcodeID - 1).setParam(window.y);
		break;
	default:
		return false;
	}

	needUpdate = true;
	modified = true;

	searchWindows();

	return true;
}

QString JsmFile::toString(int groupID, int methodID, bool moreDecompiled,
                          int indent, bool noCache)
{
	if (!noCache) {
		const QString &cache = scripts.group(groupID).method(methodID).decompiledScript(moreDecompiled);
		if (!cache.isEmpty() && !needUpdate && !(needUpdateMore && moreDecompiled)) {
			return cache;
		}
	}
	needUpdate = false;
	needUpdateMore = false;
	QString ret;
	if (moreDecompiled) {
		ret = _toStringMore(groupID, methodID, indent);
	} else {
		ret = _toString(groupID, methodID, indent);
	}

	if (indent == 0) {
		setDecompiledScript(groupID, methodID, ret, moreDecompiled);
	}

	return ret;
}

QString JsmFile::toString(bool moreDecompiled, bool noCache)
{
	QString ret;
	int nbGroups = scripts.groups().size();

	for (int groupID = 0; groupID < nbGroups; ++groupID) {
		const JsmGroup &jsmGroup = scripts.group(groupID);
		int nbMethods = jsmGroup.methodCount();
		for (int methodID = 0; methodID < nbMethods; ++methodID) {
			ret.append(QString("%1%2::%3\n{\n").arg(groupID == 0 && methodID == 0 ? "" : "\n", scripts.group(groupID).name(), scripts.group(groupID).method(methodID).name()));
			ret.append(toString(groupID, methodID, moreDecompiled, 1, noCache));
			ret.append(QString("\n}\n"));
		}
	}

	return ret;
}

QString JsmFile::_toString(int groupID, int methodID, int indent) const
{
	QString ret;
	QList<JsmOpcode *> opcodes = scripts.opcodesp(groupID, methodID, true);

	for (JsmOpcode *op: opcodes) {
		if (indent > 0) {
			ret.append(QString(indent, QChar('\t')));
		}
		ret.append(op->toString());
		ret.append("\n");
		delete op;
	}

	return ret;
}

QString JsmFile::_toStringMore(int groupID, int methodID, int indent) const
{
	QSet<void *> collectPointers;
	JsmProgram program = scripts.program(groupID, methodID, collectPointers);
	QStringList ret = program.toStringList(this, indent);

	qDeleteAll(collectPointers);

	return ret.join("\n");
}

int JsmFile::opcodePositionInText(int groupID, int methodID, int opcodeID, bool more) const
{
	if (more) {
		QSet<void *> collectPointers;
		JsmProgram program = scripts.program(groupID, methodID, collectPointers);
		QString ret = program.toStringList(this, 0, opcodeID).join("\n");

		qDeleteAll(collectPointers);

		int line = 1, index = ret.indexOf("[X]");
		if (index >= 0) {
			line = ret.left(index).count('\n');
		}

		return line;
	} else {
		QList<JsmOpcode *> opcodes = scripts.opcodesp(groupID, methodID, false);
		int line = opcodeID;
		QList<qint32> labels = JsmScripts::searchJumps(opcodes);

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

	scripts.group(groupID).method(methodID).setScriptData(res);

	modified = true;
	needUpdateMore = true;

	return 0;
}

bool JsmFile::search(SearchType type, quint64 value, int &groupID, int &methodID, int &opcodeID) const
{
	int groupListSize = scripts.groups().size(), methodCount;

	if (groupID < 0)   groupID = 0;
	if (methodID < 0)  methodID = 0;
	if (opcodeID < 0)  opcodeID = 0;

	while (groupID < groupListSize) {
		const JsmGroup &jsmGroup = scripts.group(groupID);
		methodCount = int(jsmGroup.methodCount());

		while (methodID < methodCount) {
			const JsmData &jsmData = jsmGroup.method(methodID).scriptData();
			int nbOpcode = jsmData.nbOpcode();

			while (opcodeID < nbOpcode) {
				if (searchInOpcode(type, value, jsmData, opcodeID)) {
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
	if (groupID >= scripts.groups().size()) {
		groupID = scripts.groups().size() - 1;
	}

	while (groupID >= 0) {
		const JsmGroup &jsmGroup = scripts.group(groupID);
		if (methodID >= int(jsmGroup.methodCount())) {
			methodID = int(jsmGroup.methodCount()) - 1;
		}

		while (methodID >= 0) {
			const JsmData &jsmData = jsmGroup.method(methodID).scriptData();
			int nbOpcode = jsmData.nbOpcode();

			if (opcodeID >= nbOpcode) {
				opcodeID = nbOpcode - 1;
			}

			while (opcodeID >= 0) {
				if (searchInOpcode(type, value, jsmData, opcodeID)) {
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
	int groupListSize = scripts.groups().size(), methodCount;

	if (groupID < 0)		groupID = 0;
	if (methodID < 0)	methodID = 0;
	if (opcodeID < 0)	opcodeID = 0;

	while (groupID < groupListSize) {
		const JsmGroup &jsmGroup = scripts.group(groupID);
		methodCount = int(jsmGroup.methodCount());

		while (methodID < methodCount) {
			const JsmData &jsmData = jsmGroup.method(methodID).scriptData();
			int nbOpcode = jsmData.nbOpcode();

			while (opcodeID < nbOpcode) {
				for (quint64 value: values) {
					if (searchInOpcode(type, value, jsmData, opcodeID)) {
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
	if (groupID >= scripts.groups().size()) {
		groupID = scripts.groups().size() - 1;
	}

	while (groupID >= 0) {
		const JsmGroup &jsmGroup = scripts.group(groupID);
		if (methodID >= int(jsmGroup.methodCount())) {
			methodID = int(jsmGroup.methodCount()) - 1;
		}

		while (methodID >= 0) {
			const JsmData &jsmData = jsmGroup.method(methodID).scriptData();
			int nbOpcode = jsmData.nbOpcode();

			if (opcodeID >= nbOpcode) {
				opcodeID = nbOpcode - 1;
			}

			while (opcodeID >= 0) {
				for (quint64 value: values) {
					if (searchInOpcode(type, value, jsmData, opcodeID)) {
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

bool JsmFile::searchInOpcode(SearchType type, quint64 value, const JsmData &jsmData, int opcodeID) const
{
	JsmOpcode op = jsmData.opcode(opcodeID);
	quint32 key = op.key();
	qint32 param = op.param();

	switch (type) {
	case SearchText:
		switch (key) {
		case JsmOpcode::AMESW:
		case JsmOpcode::AMES:
		case JsmOpcode::RAMESW:
			return opcodeID >= 3 && jsmData.opcode(opcodeID - 3).key() == JsmOpcode::PSHN_L
					&& value == quint32(jsmData.opcode(opcodeID - 3).param());
		case JsmOpcode::MES:
			return opcodeID >= 1 && jsmData.opcode(opcodeID - 1).key() == JsmOpcode::PSHN_L
					&& value == quint32(jsmData.opcode(opcodeID - 1).param());
		case JsmOpcode::AASK:
			return opcodeID >= 7 && jsmData.opcode(opcodeID - 7).key() == JsmOpcode::PSHN_L
					&& value == quint32(jsmData.opcode(opcodeID - 7).param());
		case JsmOpcode::ASK:
			return opcodeID >= 5 && jsmData.opcode(opcodeID - 5).key() == JsmOpcode::PSHN_L
					&& value == quint32(jsmData.opcode(opcodeID - 5).param());
		}
		return false;
	case SearchOpcode:
		return key == (value & 0xFFFF) && (int(value >> 16) == -1 || param == int(value >> 16));
	case SearchVar:
		return (((value & 0x80000000) && (key == JsmOpcode::POPM_B || key == JsmOpcode::POPM_W || key == JsmOpcode::POPM_L))
		        || ((value & 0x40000000) && (key >= 10 && key <= 18 && key != JsmOpcode::POPM_B && key != JsmOpcode::POPM_W && key != JsmOpcode::POPM_L))
		        || (!(value & 0xC0000000) && key >= 10 && key <= 18))
		        && (value & 0x3FFFFFFF) == quint32(param);
	case SearchExec:
		if (opcodeID < 1 || key < 20 || key > 22) {
			return false;
		}

		return ((value >> 16) & 0xFFFF) == quint32(jsmData.opcode(opcodeID - 1).param()) // label
				&& (value & 0xFFFF) == quint32(param); // group
	case SearchMapJump:
		switch (key) {
		case JsmOpcode::MAPJUMP:
		case JsmOpcode::MAPJUMP3:
		case JsmOpcode::MAPJUMPO:
			return opcodeID >= 1 && jsmData.opcode(opcodeID - 1).key() == JsmOpcode::PSHN_L
			        && value == quint32(jsmData.opcode(opcodeID - 1).param());
		}
		return false;
	}

	return false;
}

QList<int> JsmFile::searchAllVars() const
{
	quint32 key;
	QList<int> vars;

	for (const JsmGroup &group: scripts.groups()) {
		for (const JsmMethod &method: group.methods()) {
			int nbOpcodes = method.scriptData().nbOpcode();
			for (int i = 0; i < nbOpcodes; ++i) {
				JsmOpcode op = method.scriptData().opcode(i);
				key = op.key();

				if (key >= 10 && key <= 18 && !vars.contains(op.opcode())) {
					vars.append(op.opcode());
				}
			}
		}
	}

	return vars;
}

QList<int> JsmFile::searchAllSpells(const QString &fieldName) const
{
	QList<int> ret;
	bool setDrawPoint = false;
//	QString ret;

	for (const JsmGroup &group: scripts.groups()) {
		for (const JsmMethod &method: group.methods()) {
			int nbOpcodes = method.scriptData().nbOpcode();
			for (int i = 0; i < nbOpcodes; ++i) {
				JsmOpcode op = method.scriptData().opcode(i);
				unsigned int key = op.key();
				int param = op.param();

				if (key == JsmOpcode::SETDRAWPOINT) {
					setDrawPoint = true;
				}
				else if (key == JsmOpcode::DRAWPOINT && i > 0) {
					if (!setDrawPoint) {
						qDebug() << fieldName << "magie ?";
					} else if (method.scriptData().opcode(i - 1).key() == JsmOpcode::PSHN_L) {
						param = method.scriptData().opcode(i - 1).param();
						/*QStringList curList = ret.value(param, QStringList());
						curList.append(fieldName);
						ret.insert(param, curList);*/
						ret.append(param);
						// ret.append(" - ").append(QString::number(param)).append("\n");
					}
				}
			}
		}
	}

	return ret;
}

QList<int> JsmFile::searchAllCards(const QString &fieldName) const
{
	int param;
	unsigned int key;
	QList<int> ret;

	for (const JsmGroup &group: scripts.groups()) {
		for (const JsmMethod &method: group.methods()) {
			int nbOpcodes = method.scriptData().nbOpcode();
			for (int i = 0; i < nbOpcodes; ++i) {
				JsmOpcode op = method.scriptData().opcode(i);
				key = op.key();
				param = op.param();

				if (key == JsmOpcode::GETCARD && i > 0) {
					if (method.scriptData().opcode(i - 1).key() == JsmOpcode::PSHN_L) {
						param =method.scriptData().opcode(i - 1).param();
						ret.append(param);
					} else {
						qWarning() << fieldName << "GETCARD without PSHN_L!" << QString::number(method.scriptData().opcode(i - 1).key(), 16);
					}
				}
			}
		}
	}

	return ret;
}

QList<int> JsmFile::searchAllCardPlayers(const QString &fieldName) const
{
	int param;
	unsigned int key;
	QList<int> ret;

	for (const JsmGroup &group: scripts.groups()) {
		for (const JsmMethod &method: group.methods()) {
			int nbOpcodes = method.scriptData().nbOpcode();
			for (int i = 0; i < nbOpcodes; ++i) {
				JsmOpcode op = method.scriptData().opcode(i);
				key = op.key();
				param = op.param();

				if (key == JsmOpcode::CARDGAME && i > 6) {
					if (method.scriptData().opcode(i - 7).key() == JsmOpcode::PSHN_L) {
						param = method.scriptData().opcode(i - 7).param();
						ret.append(param);
					} else {
						qWarning() << fieldName << "CARDGAME without PSHN_L!" << QString::number(method.scriptData().opcode(i - 7).key(), 16);
					}
				}
			}
		}
	}

	return ret;
}

QList<int> JsmFile::searchAllMoments() const
{
	quint32 key;
	qint32 param;
	QList<int> ret;

	for (const JsmGroup &group: scripts.groups()) {
		for (const JsmMethod &method: group.methods()) {
			int nbOpcodes = method.scriptData().nbOpcode();
			for (int i = 0; i < nbOpcodes; ++i) {
				JsmOpcode op = method.scriptData().opcode(i);
				key = op.key();
				param = op.param();

				if ((key == JsmOpcode::POPM_B || key == JsmOpcode::POPM_W || key == JsmOpcode::POPM_L) && i > 0 && param == 256) {
					op = method.scriptData().opcode(i - 1);
					param = op.param();

					if (op.key() == JsmOpcode::PSHN_L && !ret.contains(param)) {
						ret.append(param);
					}
				}
			}
		}
	}

	return ret;
}

void JsmFile::searchAllOpcodeTypes(QMap<int, int> &ret/*, QMap<int, QString> &stackState*/) const
{
	quint32 key;
	qint32 param;
	QStack<QString> stack;

	for (const JsmGroup &group: scripts.groups()) {
		for (const JsmMethod &method: group.methods()) {
			int nbOpcodes = method.scriptData().nbOpcode();
			for (int i = 0; i < nbOpcodes; ++i) {
				JsmOpcode op = method.scriptData().opcode(i);
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
	}
}
/*
void JsmFile::searchAllSavePoints(QList<Vertex> &ret)
{
	int nbOpcode = scripts.data().nbOpcode();
	quint32 key;
	qint32 param;

	for (int i = 0; i < nbOpcode; ++i) {
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

	scripts.group(groupID).method(methodID).setDecompiledScript(text, moreDecompiled);
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
