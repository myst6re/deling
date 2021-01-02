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
#include "JsmOpcode.h"

JsmOpcode::JsmOpcode() :
	op(0)
{

}

JsmOpcode::JsmOpcode(quint32 op) :
	op(op)
{
}

JsmOpcode::JsmOpcode(unsigned int key, int param)
{
	setKey(key);
	setParam(param);
}

unsigned int JsmOpcode::key() const
{
	if(hasParam()) 	return op >> 24;
	else			return op;
}

int JsmOpcode::param() const
{
	if(!(op & 0x00800000))		return op & 0x00FFFFFF;
	else						return op | 0xFF000000;
}

quint32 JsmOpcode::opcode() const
{
	return op;
}

void JsmOpcode::setOpcode(quint32 op)
{
	this->op = op;
}

void JsmOpcode::setKey(unsigned int key)
{
	if(hasParam()) 	op = (key << 24) | (param() & 0x00FFFFFF);
	else			op = key;
}

void JsmOpcode::setParam(int param)
{
	op = (key() << 24) | (param & 0x00FFFFFF);
}

bool JsmOpcode::hasParam() const
{
	return (op & 0xFF000000) != 0;
}

int JsmOpcode::popCount() const
{
	const unsigned int k = key();
	return k < JSM_OPCODE_COUNT ? pops[k] : -1;
}

QString JsmOpcode::name() const
{
	const unsigned int k = key();
	return k < JSM_OPCODE_COUNT ? opcodes[k] : QString("UNKNOWN%1")
	                              .arg(k - JSM_OPCODE_COUNT + 1);
}

QString JsmOpcode::paramStr() const
{
	return QString::number(param());
}

QString JsmOpcode::desc() const
{
	return QString();
}

QString JsmOpcode::toString() const
{
	if(!hasParam()) {
		return name();
	} else {
		return QString("%1 %2")
		        .arg(name(), -11)
		        .arg(paramStr());
	}
}

JsmOpcodeCal::JsmOpcodeCal(const JsmOpcode &other) :
    JsmOpcode(other)
{
}

bool JsmOpcodeCal::hasParam() const
{
	return true;
}

int JsmOpcodeCal::popCount() const
{
	if(param() == 5 || param() == 15) {
		return 1;
	}
	if(param() < 15) {
		return 2;
	}
	return -1;
}

QString JsmOpcodeCal::paramStr() const
{
	const int p = param();
	return p < 16 ? cal_table[p] : JsmOpcode::paramStr();
}

const char *JsmOpcodeCal::cal_table[16] = {
	"ADD",
	"SUB",
	"MUL",
	"MOD",
	"DIV",
	"MIN",
	"EQ",
	"GT",
	"GE",
	"LS",
	"LE",
	"NT",
	"AND",
	"OR",
	"EOR",
    "NOT"
};

JsmOpcodePsh::JsmOpcodePsh(const JsmOpcode &other) :
    JsmOpcode(other)
{
}

bool JsmOpcodePsh::hasParam() const
{
	return true;
}

JsmOpcodePsh::PushType JsmOpcodePsh::pushType() const
{
	switch(key()) {
	case PSHN_L:
		return Int;
	case PSHI_L:
		return Temp;
	case PSHAC:
		return Model;
	default:
		return Var;
	}
}

QString JsmOpcodePsh::paramStr() const
{
	switch(pushType()) {
	case Int:
		return JsmOpcode::paramStr();
	case Temp:
		return QString("TEMP%1").arg(param());
	case Var:
		return QString("VAR%1").arg(param());
	case Model:
		return QString("MODEL%1").arg(param());
	}

	return QString();
}

JsmOpcodePop::JsmOpcodePop(const JsmOpcode &other) :
    JsmOpcode(other)
{
}

bool JsmOpcodePop::hasParam() const
{
	return true;
}

JsmOpcodePop::PopType JsmOpcodePop::popType() const
{
	if (key() == POPI_L) {
		return Temp;
	}
	return Var;
}

QString JsmOpcodePop::paramStr() const
{
	switch(popType()) {
	case Temp:
		return QString("TEMP%1").arg(param());
	case Var:
		return QString("VAR%1").arg(param());
	}

	return QString();
}

JsmOpcodeLabel::JsmOpcodeLabel(int label) :
    JsmOpcode()
{
	setParam(label);
}

JsmOpcodeLabel::JsmOpcodeLabel(const JsmOpcode &other) :
    JsmOpcode(other)
{
}

QString JsmOpcodeLabel::name() const
{
	return QString("LABEL%1").arg(param());
}

JsmOpcodeGoto::JsmOpcodeGoto(const JsmOpcode &other, int label) :
    JsmOpcode(other), _label(label)
{
}

JsmOpcodeGoto::JsmOpcodeGoto(const JsmOpcodeGoto &other) :
    JsmOpcode(other), _label(other._label)
{
}

QString JsmOpcodeGoto::paramStr() const
{
	return QString("LABEL%1").arg(_label);
}

/*
CAL: Un paramètre
JMP: Un paramètre (Signé ?)
JPF: Un paramètre (Signé ?)
LBL: Un paramètre
RET: Un paramètre
PSHN_L: PUSH_L XX: Un paramètre (Signé ?)
PSHI_L: PUSH_L tempXX: Un paramètre
POPI_L: POP_L tempXX: Un paramètre
PSHM_B: PUSH_B varXX: Un paramètre
POPM_B: POP_B varXX: Un paramètre
PSHM_W: PUSH_W varXX: Un paramètre
POPM_W: POP_W varXX: Un paramètre
PSHM_L: PUSH_L varXX: Un paramètre
POPM_L: POP_L varXX: Un paramètre
PSHSM_B: PUSH_SB varXX: Un paramètre
PSHSM_W: PUSH_SW varXX: Un paramètre
PSHSM_L: PUSH_SL varXX: Un paramètre
PSHAC: Un paramètre
REQ: Un paramètre
REQSW: Un paramètre
REQEW: Un paramètre
PREQ: Un paramètre
PREQSW: Un paramètre
PREQEW: Un paramètre
UNUSE: Un paramètre
HALT: Un paramètre
SET: Un paramètre
SET3: Un paramètre
IDLOCK: Un paramètre
IDUNLOCK: Un paramètre
EFFECTPLAY2: Un paramètre
FOOTSTEP: Un paramètre
JUMP: Un paramètre
JUMP3: Un paramètre
LADDERUP: Un paramètre
LADDERDOWN: Un paramètre
LADDERUP2: Un paramètre
LADDERDOWN2: Un paramètre
MAPJUMP: Un paramètre
MAPJUMP3: Un paramètre
SETMODEL: Un paramètre
BASEANIME: Un paramètre
ANIME: Un paramètre
ANIMEKEEP: Un paramètre
CANIME: Un paramètre
CANIMEKEEP: Un paramètre
RANIME: Un paramètre
RANIMEKEEP: Un paramètre
RCANIME: Un paramètre
RCANIMEKEEP: Un paramètre
RANIMELOOP: Un paramètre
RCANIMELOOP: Un paramètre
LADDERANIME: Un paramètre
DISCJUMP: Un paramètre
SETLINE: Aucun paramètre
LINEON: Aucun paramètre
LINEOFF: Aucun paramètre
WAIT: Aucun paramètre
MSPEED: Aucun paramètre
MOVE: Aucun paramètre
MOVEA: Aucun paramètre
PMOVEA: Aucun paramètre
CMOVE: Aucun paramètre
FMOVE: Aucun paramètre
PJUMPA: Aucun paramètre
ANIMESYNC: Aucun paramètre
ANIMESTOP: Aucun paramètre
MES: Aucun paramètre
MESSYNC: Aucun paramètre
MESVAR: Aucun paramètre
ASK: Aucun paramètre
WINSIZE: Aucun paramètre
WINCLOSE: Aucun paramètre
UCON: Aucun paramètre
UCOFF: Aucun paramètre
MOVIE: Aucun paramètre
MOVIESYNC: Aucun paramètre
SETPC: Aucun paramètre
DIR: Aucun paramètre
DIRP: Aucun paramètre
DIRA: Aucun paramètre
PDIRA: Aucun paramètre
SPUREADY: Aucun paramètre
TALKON: Aucun paramètre
TALKOFF: Aucun paramètre
PUSHON: Aucun paramètre
PUSHOFF: Aucun paramètre
ISTOUCH: Aucun paramètre
MAPJUMPO: Aucun paramètre
MAPJUMPON: Aucun paramètre
MAPJUMPOFF: Aucun paramètre
SETMESSPEED: Aucun paramètre
SHOW: Aucun paramètre
HIDE: Aucun paramètre
TALKRADIUS: Aucun paramètre
PUSHRADIUS: Aucun paramètre
AMESW: Aucun paramètre
AMES: Aucun paramètre
GETINFO: Aucun paramètre
THROUGHON: Aucun paramètre
THROUGHOFF: Aucun paramètre
BATTLE: Aucun paramètre
BATTLERESULT: Aucun paramètre
BATTLEON: Aucun paramètre
BATTLEOFF: Aucun paramètre
KEYSCAN: Aucun paramètre
KEYON: Aucun paramètre
AASK: Aucun paramètre
PGETINFO: Aucun paramètre
DSCROLL: Aucun paramètre
LSCROLL: Aucun paramètre
CSCROLL: Aucun paramètre
DSCROLLA: Aucun paramètre
LSCROLLA: Aucun paramètre
CSCROLLA: Aucun paramètre
SCROLLSYNC: Aucun paramètre
RMOVE: Aucun paramètre
RMOVEA: Aucun paramètre
RPMOVEA: Aucun paramètre
RCMOVE: Aucun paramètre
RFMOVE: Aucun paramètre
MOVESYNC: Aucun paramètre
CLEAR: Aucun paramètre
DSCROLLP: Aucun paramètre
LSCROLLP: Aucun paramètre
CSCROLLP: Aucun paramètre
LTURNR: Aucun paramètre
LTURNL: Aucun paramètre
CTURNR: Aucun paramètre
CTURNL: Aucun paramètre
ADDPARTY: Aucun paramètre
SUBPARTY: Aucun paramètre
CHANGEPARTY: Aucun paramètre
REFRESHPARTY: Aucun paramètre
SETPARTY: Aucun paramètre
ISPARTY: Aucun paramètre
ADDMEMBER: Aucun paramètre
SUBMEMBER: Aucun paramètre
LTURN: Aucun paramètre
CTURN: Aucun paramètre
PLTURN: Aucun paramètre
PCTURN: Aucun paramètre
JOIN: Aucun paramètre
BGANIME: Aucun paramètre
RBGANIME: Aucun paramètre
RBGANIMELOOP: Aucun paramètre
BGANIMESYNC: Aucun paramètre
BGDRAW: Aucun paramètre
BGOFF: Aucun paramètre
BGANIMESPEED: Aucun paramètre
SETTIMER: Aucun paramètre
DISPTIMER: Aucun paramètre
SETGETA: Aucun paramètre
SETROOTTRANS: Aucun paramètre
SETVIBRATE: Aucun paramètre
MOVIEREADY: Aucun paramètre
GETTIMER: Aucun paramètre
FADEIN: Aucun paramètre
FADEOUT: Aucun paramètre
FADESYNC: Aucun paramètre
SHAKE: Aucun paramètre
SHAKEOFF: Aucun paramètre
FADEBLACK: Aucun paramètre
FOLLOWOFF: Aucun paramètre
FOLLOWON: Aucun paramètre
GAMEOVER: Aucun paramètre
SHADELEVEL: Aucun paramètre
SHADEFORM: Aucun paramètre
FMOVEA: Aucun paramètre
FMOVEP: Aucun paramètre
SHADESET: Aucun paramètre
MUSICCHANGE: Aucun paramètre
MUSICLOAD: Aucun paramètre
FADENONE: Aucun paramètre
POLYCOLOR: Aucun paramètre
POLYCOLORALL: Aucun paramètre
KILLTIMER: Aucun paramètre
CROSSMUSIC: Aucun paramètre
DUALMUSIC: Aucun paramètre
EFFECTPLAY: Aucun paramètre
EFFECTLOAD: Aucun paramètre
LOADSYNC: Aucun paramètre
MUSICSTOP: Aucun paramètre
MUSICVOL: Aucun paramètre
MUSICVOLTRANS: Aucun paramètre
MUSICVOLFADE: Aucun paramètre
ALLSEVOL: Aucun paramètre
ALLSEVOLTRANS: Aucun paramètre
ALLSEPOSTRANS: Aucun paramètre
SEVOL: Aucun paramètre
SEVOLTRANS: Aucun paramètre
SEPOS: Aucun paramètre
SEPOSTRANS: Aucun paramètre
SETBATTLEMUSIC: Aucun paramètre
BATTLEMODE: Aucun paramètre
SESTOP: Aucun paramètre
INITSOUND: Aucun paramètre
BGSHADE: Aucun paramètre
BGSHADESTOP: Aucun paramètre
RBGSHADELOOP: Aucun paramètre
DSCROLL2: Aucun paramètre
LSCROLL2: Aucun paramètre
CSCROLL2: Aucun paramètre
DSCROLLA2: Aucun paramètre
CSCROLLA2: Aucun paramètre
SCROLLSYNC2: Aucun paramètre
SCROLLMODE2: Aucun paramètre
MENUENABLE: Aucun paramètre
MENUDISABLE: Aucun paramètre
FOOTSTEPON: Aucun paramètre
FOOTSTEPOFF: Aucun paramètre
FOOTSTEPOFFALL: Aucun paramètre
FOOTSTEPCUT: Aucun paramètre
PREMAPJUMP: Aucun paramètre
USE: Aucun paramètre
SPLIT: Aucun paramètre
ANIMESPEED: Aucun paramètre
RND: Aucun paramètre
DCOLADD: Aucun paramètre
DCOLSUB: Aucun paramètre
TCOLADD: Aucun paramètre
TCOLSUB: Aucun paramètre
FCOLADD: Aucun paramètre
FCOLSUB: Aucun paramètre
COLSYNC: Aucun paramètre
DOFFSET: Aucun paramètre
LOFFSETS: Aucun paramètre
COFFSETS: Aucun paramètre
LOFFSET: Aucun paramètre
COFFSET: Aucun paramètre
OFFSETSYNC: Aucun paramètre
RUNENABLE: Aucun paramètre
RUNDISABLE: Aucun paramètre
MAPFADEOFF: Aucun paramètre
MAPFADEON: Aucun paramètre
INITTRACE: Aucun paramètre
SETDRESS: Aucun paramètre
FACEDIR: Aucun paramètre
FACEDIRA: Aucun paramètre
FACEDIRP: Aucun paramètre
FACEDIRLIMIT: Aucun paramètre
FACEDIROFF: Aucun paramètre
SARALYOFF: Aucun paramètre
SARALYON: Aucun paramètre
SARALYDISPOFF: Aucun paramètre
SARALYDISPON: Aucun paramètre
MESMODE: Aucun paramètre
FACEDIRINIT: Aucun paramètre
FACEDIRI: Aucun paramètre
JUNCTION: Aucun paramètre
SETCAMERA: Aucun paramètre
BATTLECUT: Aucun paramètre
FOOTSTEPCOPY: Aucun paramètre
WORLDMAPJUMP: Aucun paramètre
RFACEDIR: Aucun paramètre
RFACEDIRA: Aucun paramètre
RFACEDIRP: Aucun paramètre
RFACEDIROFF: Aucun paramètre
FACEDIRSYNC: Aucun paramètre
COPYINFO: Aucun paramètre
RAMESW: Aucun paramètre
BGSHADEOFF: Aucun paramètre
AXIS: Aucun paramètre
MENUNORMAL: Aucun paramètre
MENUPHS: Aucun paramètre
BGCLEAR: Aucun paramètre
GETPARTY: Aucun paramètre
MENUSHOP: Aucun paramètre
DISC: Aucun paramètre
LSCROLL3: Aucun paramètre
CSCROLL3: Aucun paramètre
MACCEL: Aucun paramètre
MLIMIT: Aucun paramètre
ADDITEM: Aucun paramètre
SETWITCH: Aucun paramètre
SETODIN: Aucun paramètre
RESETGF: Aucun paramètre
MENUNAME: Aucun paramètre
REST: Aucun paramètre
MOVECANCEL: Aucun paramètre
ACTORMODE: Aucun paramètre
MENUSAVE: Aucun paramètre
SAVEENABLE: Aucun paramètre
PHSENABLE: Aucun paramètre
HOLD: Aucun paramètre
MOVIECUT: Aucun paramètre
SETPLACE: Aucun paramètre
SETDCAMERA: Aucun paramètre
CHOICEMUSIC: Aucun paramètre
GETCARD: Aucun paramètre
DRAWPOINT: Aucun paramètre
PHSPOWER: Aucun paramètre
KEY: Aucun paramètre
CARDGAME: Aucun paramètre
SETBAR: Aucun paramètre
DISPBAR: Aucun paramètre
KILLBAR: Aucun paramètre
SCROLLRATIO2: Aucun paramètre
WHOAMI: Aucun paramètre
MUSICSTATUS: Aucun paramètre
MUSICREPLAY: Aucun paramètre
DOORLINEOFF: Aucun paramètre
DOORLINEON: Aucun paramètre
MUSICSKIP: Aucun paramètre
DYING: Aucun paramètre
SETHP: Aucun paramètre
MOVEFLUSH: Aucun paramètre
MUSICVOLSYNC: Aucun paramètre
PUSHANIME: Aucun paramètre
POPANIME: Aucun paramètre
KEYSCAN2: Aucun paramètre
PARTICLEON: Aucun paramètre
PARTICLEOFF: Aucun paramètre
KEYSIGHNCHANGE: Aucun paramètre
ADDGIL: Aucun paramètre
ADDPASTGIL: Aucun paramètre
ADDSEEDLEVEL: Aucun paramètre
PARTICLESET: Aucun paramètre
SETDRAWPOINT: Aucun paramètre
MENUTIPS: Aucun paramètre
LASTIN: Aucun paramètre
LASTOUT: Aucun paramètre
SEALEDOFF: Aucun paramètre
MENUTUTO: Aucun paramètre
CLOSEEYES: Aucun paramètre
SETCARD: Aucun paramètre
HOWMANYCARD: Aucun paramètre
WHERECARD: Aucun paramètre
ADDMAGIC: Aucun paramètre
SWAP: Aucun paramètre
SPUSYNC: Aucun paramètre
BROKEN: Aucun paramètre
ANGELODISABLE: Aucun paramètre
Unknown2: Aucun paramètre
Unknown3: Aucun paramètre
Unknown4: Aucun paramètre
HASITEM: Aucun paramètre
Unknown6: Aucun paramètre
Unknown7: Aucun paramètre
Unknown8: Aucun paramètre
Unknown9: Aucun paramètre
Unknown10: Aucun paramètre
Unknown11: Aucun paramètre
Unknown12: Aucun paramètre
Unknown13: Aucun paramètre
Unknown14: Aucun paramètre
Unknown15: Aucun paramètre
Unknown16: Aucun paramètre
PREMAPJUMP2: Aucun paramètre
TUTO: Aucun paramètre
*/

const char *JsmOpcode::opcodes[JSM_OPCODE_COUNT] = {
    "NOP",
    "CAL",
    "JMP",
    "JPF",
    "GJMP",
    "LBL",
    "RET",
    "PSHN_L",
    "PSHI_L",
    "POPI_L",
    "PSHM_B",
    "POPM_B",
    "PSHM_W",
    "POPM_W",
    "PSHM_L",
    "POPM_L",
    "PSHSM_B",
    "PSHSM_W",
    "PSHSM_L",
    "PSHAC",
    "REQ",
    "REQSW",
    "REQEW",
    "PREQ",
    "PREQSW",
    "PREQEW",
    "UNUSE",
    "DEBUG",
    "HALT",
    "SET",
    "SET3",
    "IDLOCK",
    "IDUNLOCK",
    "EFFECTPLAY2",
    "FOOTSTEP",
    "JUMP",
    "JUMP3",
    "LADDERUP",
    "LADDERDOWN",
    "LADDERUP2",
    "LADDERDOWN2",
    "MAPJUMP",
    "MAPJUMP3",
    "SETMODEL",
    "BASEANIME",
    "ANIME",
    "ANIMEKEEP",
    "CANIME",
    "CANIMEKEEP",
    "RANIME",
    "RANIMEKEEP",
    "RCANIME",
    "RCANIMEKEEP",
    "RANIMELOOP",
    "RCANIMELOOP",
    "LADDERANIME",
    "DISCJUMP",
    "SETLINE",
    "LINEON",
    "LINEOFF",
    "WAIT",
    "MSPEED",
    "MOVE",
    "MOVEA",
    "PMOVEA",
    "CMOVE",
    "FMOVE",
    "PJUMPA",
    "ANIMESYNC",
    "ANIMESTOP",
    "MESW",
    "MES",
    "MESSYNC",
    "MESVAR",
    "ASK",
    "WINSIZE",
    "WINCLOSE",
    "UCON",
    "UCOFF",
    "MOVIE",
    "MOVIESYNC",
    "SETPC",
    "DIR",
    "DIRP",
    "DIRA",
    "PDIRA",
    "SPUREADY",
    "TALKON",
    "TALKOFF",
    "PUSHON",
    "PUSHOFF",
    "ISTOUCH",
    "MAPJUMPO",
    "MAPJUMPON",
    "MAPJUMPOFF",
    "SETMESSPEED",
    "SHOW",
    "HIDE",
    "TALKRADIUS",
    "PUSHRADIUS",
    "AMESW",
    "AMES",
    "GETINFO",
    "THROUGHON",
    "THROUGHOFF",
    "BATTLE",
    "BATTLERESULT",
    "BATTLEON",
    "BATTLEOFF",
    "KEYSCAN",
    "KEYON",
    "AASK",
    "PGETINFO",
    "DSCROLL",
    "LSCROLL",
    "CSCROLL",
    "DSCROLLA",
    "LSCROLLA",
    "CSCROLLA",
    "SCROLLSYNC",
    "RMOVE",
    "RMOVEA",
    "RPMOVEA",
    "RCMOVE",
    "RFMOVE",
    "MOVESYNC",
    "CLEAR",
    "DSCROLLP",
    "LSCROLLP",
    "CSCROLLP",
    "LTURNR",
    "LTURNL",
    "CTURNR",
    "CTURNL",
    "ADDPARTY",
    "SUBPARTY",
    "CHANGEPARTY",
    "REFRESHPARTY",
    "SETPARTY",
    "ISPARTY",
    "ADDMEMBER",
    "SUBMEMBER",
    "ISMEMBER",
    "LTURN",
    "CTURN",
    "PLTURN",
    "PCTURN",
    "JOIN",
    "MESFORCUS",
    "BGANIME",
    "RBGANIME",
    "RBGANIMELOOP",
    "BGANIMESYNC",
    "BGDRAW",
    "BGOFF",
    "BGANIMESPEED",
    "SETTIMER",
    "DISPTIMER",
    "SHADETIMER",
    "SETGETA",
    "SETROOTTRANS",
    "SETVIBRATE",
    "STOPVIBRATE",
    "MOVIEREADY",
    "GETTIMER",
    "FADEIN",
    "FADEOUT",
    "FADESYNC",
    "SHAKE",
    "SHAKEOFF",
    "FADEBLACK",
    "FOLLOWOFF",
    "FOLLOWON",
    "GAMEOVER",
    "ENDING",
    "SHADELEVEL",
    "SHADEFORM",
    "FMOVEA",
    "FMOVEP",
    "SHADESET",
    "MUSICCHANGE",
    "MUSICLOAD",
    "FADENONE",
    "POLYCOLOR",
    "POLYCOLORALL",
    "KILLTIMER",
    "CROSSMUSIC",
    "DUALMUSIC",
    "EFFECTPLAY",
    "EFFECTLOAD",
    "LOADSYNC",
    "MUSICSTOP",
    "MUSICVOL",
    "MUSICVOLTRANS",
    "MUSICVOLFADE",
    "ALLSEVOL",
    "ALLSEVOLTRANS",
    "ALLSEPOS",
    "ALLSEPOSTRANS",
    "SEVOL",
    "SEVOLTRANS",
    "SEPOS",
    "SEPOSTRANS",
    "SETBATTLEMUSIC",
    "BATTLEMODE",
    "SESTOP",
    "BGANIMEFLAG",
    "INITSOUND",
    "BGSHADE",
    "BGSHADESTOP",
    "RBGSHADELOOP",
    "DSCROLL2",
    "LSCROLL2",
    "CSCROLL2",
    "DSCROLLA2",
    "LSCROLLA2",
    "CSCROLLA2",
    "DSCROLLP2",
    "LSCROLLP2",
    "CSCROLLP2",
    "SCROLLSYNC2",
    "SCROLLMODE2",
    "MENUENABLE",
    "MENUDISABLE",
    "FOOTSTEPON",
    "FOOTSTEPOFF",
    "FOOTSTEPOFFALL",
    "FOOTSTEPCUT",
    "PREMAPJUMP",
    "USE",
    "SPLIT",
    "ANIMESPEED",
    "RND",
    "DCOLADD",
    "DCOLSUB",
    "TCOLADD",
    "TCOLSUB",
    "FCOLADD",
    "FCOLSUB",
    "COLSYNC",
    "DOFFSET",
    "LOFFSETS",
    "COFFSETS",
    "LOFFSET",
    "COFFSET",
    "OFFSETSYNC",
    "RUNENABLE",
    "RUNDISABLE",
    "MAPFADEOFF",
    "MAPFADEON",
    "INITTRACE",
    "SETDRESS",
    "GETDRESS",
    "FACEDIR",
    "FACEDIRA",
    "FACEDIRP",
    "FACEDIRLIMIT",
    "FACEDIROFF",
    "SARALYOFF",
    "SARALYON",
    "SARALYDISPOFF",
    "SARALYDISPON",
    "MESMODE",
    "FACEDIRINIT",
    "FACEDIRI",
    "JUNCTION",
    "SETCAMERA",
    "BATTLECUT",
    "FOOTSTEPCOPY",
    "WORLDMAPJUMP",
    "RFACEDIRI",
    "RFACEDIR",
    "RFACEDIRA",
    "RFACEDIRP",
    "RFACEDIROFF",
    "FACEDIRSYNC",
    "COPYINFO",
    "PCOPYINFO",
    "RAMESW",
    "BGSHADEOFF",
    "AXIS",
    "AXISSYNC",
    "MENUNORMAL",
    "MENUPHS",
    "BGCLEAR",
    "GETPARTY",
    "MENUSHOP",
    "DISC",
    "DSCROLL3",
    "LSCROLL3",
    "CSCROLL3",
    "MACCEL",
    "MLIMIT",
    "ADDITEM",
    "SETWITCH",
    "SETODIN",
    "RESETGF",
    "MENUNAME",
    "REST",
    "MOVECANCEL",
    "PMOVECANCEL",
    "ACTORMODE",
    "MENUSAVE",
    "SAVEENABLE",
    "PHSENABLE",
    "HOLD",
    "MOVIECUT",
    "SETPLACE",
    "SETDCAMERA",
    "CHOICEMUSIC",
    "GETCARD",
    "DRAWPOINT",
    "PHSPOWER",
    "KEY",
    "CARDGAME",
    "SETBAR",
    "DISPBAR",
    "KILLBAR",
    "SCROLLRATIO2",
    "WHOAMI",
    "MUSICSTATUS",
    "MUSICREPLAY",
    "DOORLINEOFF",
    "DOORLINEON",
    "MUSICSKIP",
    "DYING",
    "SETHP",
    "GETHP",
    "MOVEFLUSH",
    "MUSICVOLSYNC",
    "PUSHANIME",
    "POPANIME",
    "KEYSCAN2",
    "KEYON2",
    "PARTICLEON",
    "PARTICLEOFF",
    "KEYSIGHNCHANGE",
    "ADDGIL",
    "ADDPASTGIL",
    "ADDSEEDLEVEL",
    "PARTICLESET",
    "SETDRAWPOINT",
    "MENUTIPS",
    "LASTIN",
    "LASTOUT",
    "SEALEDOFF",
    "MENUTUTO",
    "OPENEYES",
    "CLOSEEYES",
    "BLINKEYES",
    "SETCARD",
    "HOWMANYCARD",
    "WHERECARD",
    "ADDMAGIC",
    "SWAP",
    "SETPARTY2",
    "SPUSYNC",
    "BROKEN",
    "ANGELODISABLE",
    "UNKNOWN2",
    "UNKNOWN3",
    "UNKNOWN4",
    "HASITEM",
    "UNKNOWN6",
    "UNKNOWN7",
    "UNKNOWN8",
    "UNKNOWN9",
    "UNKNOWN10",
    "UNKNOWN11",
    "UNKNOWN12",
    "UNKNOWN13",
    "UNKNOWN14",
    "UNKNOWN15",
    "UNKNOWN16",
    "PREMAPJUMP2",
    "TUTO"
};

int JsmOpcode::pops[JSM_OPCODE_COUNT] = {
	0,  // NOP
	-1, // CAL (1 or 2)
	0,  // JMP
	1,  // JPF
	-1, // GJMP
	0,  // LBL
	0,  // RET
	0,  // PSHN_L
	0,  // PSHI_L
	1,  // POPI_L
	0,  // PSHM_B
	1,  // POPM_B
	0,  // PSHM_W
	1,  // POPM_W
	0,  // PSHM_L
	1,  // POPM_L
	0,  // PSHSM_B
	0,  // PSHSM_W
	0,  // PSHSM_L
	0,  // PSHAC
	2,  // REQ
	2,  // REQSW
	2,  // REQEW
	2,  // PREQ
	2,  // PREQSW
	2,  // PREQEW
	0,  // UNUSE
	-1, // DEBUG
	0,  // HALT
	2,  // SET
	3,  // SET3
	0,  // IDLOCK
	0,  // IDUNLOCK
	3,  // EFFECTPLAY2
	1,  // FOOTSTEP
	3,  // JUMP
	4,  // JUMP3
	4,  // LADDERUP
	4,  // LADDERDOWN
	9,  // LADDERUP2
	9,  // LADDERDOWN2
	4,  // MAPJUMP
	5,  // MAPJUMP3
	0,  // SETMODEL
	2,  // BASEANIME
	0,  // ANIME
	0,  // ANIMEKEEP
	2,  // CANIME
	2,  // CANIMEKEEP
	0,  // RANIME
	0,  // RANIMEKEEP
	2,  // RCANIME
	2,  // RCANIMEKEEP
	0,  // RANIMELOOP
	2,  // RCANIMELOOP
	2,  // LADDERANIME
	5,  // DISCJUMP
	6,  // SETLINE
	0,  // LINEON
	0,  // LINEOFF
	1,  // WAIT
	1,  // MSPEED
	4,  // MOVE
	2,  // MOVEA
	2,  // PMOVEA
	4,  // CMOVE
	4,  // FMOVE
	2,  // PJUMPA
	0,  // ANIMESYNC
	0,  // ANIMESTOP
	-1, // MESW
	2,  // MES
	1,  // MESSYNC
	2,  // MESVAR
	6,  // ASK
	5,  // WINSIZE
	1,  // WINCLOSE
	0,  // UCON
	0,  // UCOFF
	0,  // MOVIE
	0,  // MOVIESYNC
	1,  // SETPC
	1,  // DIR
	3,  // DIRP
	1,  // DIRA
	1,  // PDIRA
	1,  // SPUREADY
	0,  // TALKON
	0,  // TALKOFF
	0,  // PUSHON
	0,  // PUSHOFF
	1,  // ISTOUCH
	2,  // MAPJUMPO
	0,  // MAPJUMPON
	0,  // MAPJUMPOFF
	2,  // SETMESSPEED
	0,  // SHOW
	0,  // HIDE
	1,  // TALKRADIUS
	1,  // PUSHRADIUS
	4,  // AMESW
	-1, // AMES
	-1, // GETINFO
	-1, // THROUGHON
	-1, // THROUGHOFF
	-1, // BATTLE
	-1, // BATTLERESULT
	-1, // BATTLEON
	-1, // BATTLEOFF
	-1, // KEYSCAN
	-1, // KEYON
	-1, // AASK
	-1, // PGETINFO
	-1, // DSCROLL
	-1, // LSCROLL
	-1, // CSCROLL
	-1, // DSCROLLA
	-1, // LSCROLLA
	-1, // CSCROLLA
	-1, // SCROLLSYNC
	-1, // RMOVE
	-1, // RMOVEA
	-1, // RPMOVEA
	-1, // RCMOVE
	-1, // RFMOVE
	-1, // MOVESYNC
	-1, // CLEAR
	-1, // DSCROLLP
	-1, // LSCROLLP
	-1, // CSCROLLP
	-1, // LTURNR
	-1, // LTURNL
	-1, // CTURNR
	-1, // CTURNL
	-1, // ADDPARTY
	-1, // SUBPARTY
	-1, // CHANGEPARTY
	-1, // REFRESHPARTY
	-1, // SETPARTY
	-1, // ISPARTY
	-1, // ADDMEMBER
	-1, // SUBMEMBER
	-1, // ISMEMBER
	-1, // LTURN
	-1, // CTURN
	-1, // PLTURN
	-1, // PCTURN
	-1, // JOIN
	-1, // MESFORCUS
	-1, // BGANIME
	-1, // RBGANIME
	-1, // RBGANIMELOOP
	-1, // BGANIMESYNC
	-1, // BGDRAW
	-1, // BGOFF
	-1, // BGANIMESPEED
	-1, // SETTIMER
	-1, // DISPTIMER
	-1, // SHADETIMER
	-1, // SETGETA
	-1, // SETROOTTRANS
	-1, // SETVIBRATE
	-1, // STOPVIBRATE
	-1, // MOVIEREADY
	-1, // GETTIMER
	-1, // FADEIN
	-1, // FADEOUT
	-1, // FADESYNC
	-1, // SHAKE
	-1, // SHAKEOFF
	-1, // FADEBLACK
	-1, // FOLLOWOFF
	-1, // FOLLOWON
	-1, // GAMEOVER
	-1, // ENDING
	-1, // SHADELEVEL
	-1, // SHADEFORM
	-1, // FMOVEA
	-1, // FMOVEP
	-1, // SHADESET
	-1, // MUSICCHANGE
	-1, // MUSICLOAD
	-1, // FADENONE
	-1, // POLYCOLOR
	-1, // POLYCOLORALL
	-1, // KILLTIMER
	-1, // CROSSMUSIC
	-1, // DUALMUSIC
	-1, // EFFECTPLAY
	-1, // EFFECTLOAD
	-1, // LOADSYNC
	-1, // MUSICSTOP
	-1, // MUSICVOL
	-1, // MUSICVOLTRANS
	-1, // MUSICVOLFADE
	-1, // ALLSEVOL
	-1, // ALLSEVOLTRANS
	-1, // ALLSEPOS
	-1, // ALLSEPOSTRANS
	-1, // SEVOL
	-1, // SEVOLTRANS
	-1, // SEPOS
	-1, // SEPOSTRANS
	-1, // SETBATTLEMUSIC
	-1, // BATTLEMODE
	-1, // SESTOP
	-1, // BGANIMEFLAG
	-1, // INITSOUND
	-1, // BGSHADE
	-1, // BGSHADESTOP
	-1, // RBGSHADELOOP
	-1, // DSCROLL2
	-1, // LSCROLL2
	-1, // CSCROLL2
	-1, // DSCROLLA2
	-1, // LSCROLLA2
	-1, // CSCROLLA2
	-1, // DSCROLLP2
	-1, // LSCROLLP2
	-1, // CSCROLLP2
	-1, // SCROLLSYNC2
	-1, // SCROLLMODE2
	-1, // MENUENABLE
	-1, // MENUDISABLE
	-1, // FOOTSTEPON
	-1, // FOOTSTEPOFF
	-1, // FOOTSTEPOFFALL
	-1, // FOOTSTEPCUT
	-1, // PREMAPJUMP
	-1, // USE
	-1, // SPLIT
	-1, // ANIMESPEED
	-1, // RND
	-1, // DCOLADD
	-1, // DCOLSUB
	-1, // TCOLADD
	-1, // TCOLSUB
	-1, // FCOLADD
	-1, // FCOLSUB
	-1, // COLSYNC
	-1, // DOFFSET
	-1, // LOFFSETS
	-1, // COFFSETS
	-1, // LOFFSET
	-1, // COFFSET
	-1, // OFFSETSYNC
	-1, // RUNENABLE
	-1, // RUNDISABLE
	-1, // MAPFADEOFF
	-1, // MAPFADEON
	-1, // INITTRACE
	-1, // SETDRESS
	-1, // GETDRESS
	-1, // FACEDIR
	-1, // FACEDIRA
	-1, // FACEDIRP
	-1, // FACEDIRLIMIT
	-1, // FACEDIROFF
	-1, // SARALYOFF
	-1, // SARALYON
	-1, // SARALYDISPOFF
	-1, // SARALYDISPON
	-1, // MESMODE
	-1, // FACEDIRINIT
	-1, // FACEDIRI
	-1, // JUNCTION
	-1, // SETCAMERA
	-1, // BATTLECUT
	-1, // FOOTSTEPCOPY
	-1, // WORLDMAPJUMP
	-1, // RFACEDIRI
	-1, // RFACEDIR
	-1, // RFACEDIRA
	-1, // RFACEDIRP
	-1, // RFACEDIROFF
	-1, // FACEDIRSYNC
	-1, // COPYINFO
	-1, // PCOPYINFO
	-1, // RAMESW
	-1, // BGSHADEOFF
	-1, // AXIS
	-1, // AXISSYNC
	-1, // MENUNORMAL
	-1, // MENUPHS
	-1, // BGCLEAR
	-1, // GETPARTY
	-1, // MENUSHOP
	-1, // DISC
	-1, // DSCROLL3
	-1, // LSCROLL3
	-1, // CSCROLL3
	-1, // MACCEL
	-1, // MLIMIT
	-1, // ADDITEM
	-1, // SETWITCH
	-1, // SETODIN
	-1, // RESETGF
	-1, // MENUNAME
	-1, // REST
	-1, // MOVECANCEL
	-1, // PMOVECANCEL
	-1, // ACTORMODE
	-1, // MENUSAVE
	-1, // SAVEENABLE
	-1, // PHSENABLE
	-1, // HOLD
	-1, // MOVIECUT
	-1, // SETPLACE
	-1, // SETDCAMERA
	-1, // CHOICEMUSIC
	-1, // GETCARD
	-1, // DRAWPOINT
	-1, // PHSPOWER
	-1, // KEY
	-1, // CARDGAME
	-1, // SETBAR
	-1, // DISPBAR
	-1, // KILLBAR
	-1, // SCROLLRATIO2
	-1, // WHOAMI
	-1, // MUSICSTATUS
	-1, // MUSICREPLAY
	-1, // DOORLINEOFF
	-1, // DOORLINEON
	-1, // MUSICSKIP
	-1, // DYING
	-1, // SETHP
	-1, // GETHP
	-1, // MOVEFLUSH
	-1, // MUSICVOLSYNC
	-1, // PUSHANIME
	-1, // POPANIME
	-1, // KEYSCAN2
	-1, // KEYON2
	-1, // PARTICLEON
	-1, // PARTICLEOFF
	-1, // KEYSIGHNCHANGE
	-1, // ADDGIL
	-1, // ADDPASTGIL
	-1, // ADDSEEDLEVEL
	-1, // PARTICLESET
	-1, // SETDRAWPOINT
	-1, // MENUTIPS
	-1, // LASTIN
	-1, // LASTOUT
	-1, // SEALEDOFF
	-1, // MENUTUTO
	-1, // OPENEYES
	-1, // CLOSEEYES
	-1, // BLINKEYES
	-1, // SETCARD
	-1, // HOWMANYCARD
	-1, // WHERECARD
	-1, // ADDMAGIC
	-1, // SWAP
	-1, // SETPARTY2
	-1, // SPUSYNC
	-1, // BROKEN
    -1, // ANGELODISABLE
	-1, // UNKNOWN2
	-1, // UNKNOWN3
	-1, // UNKNOWN4
    -1, // HASITEM
	-1, // UNKNOWN6
	-1, // UNKNOWN7
	-1, // UNKNOWN8
	-1, // UNKNOWN9
	-1, // UNKNOWN10
	-1, // UNKNOWN11
	-1, // UNKNOWN12
	-1, // UNKNOWN13
	-1, // UNKNOWN14
	-1, // UNKNOWN15
	-1, // UNKNOWN16
	1,  // PREMAPJUMP2
	1,  // TUTO
};
