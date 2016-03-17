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
	return -1;
}

int JsmOpcode::pushCount() const
{
	return -1;
}

QString JsmOpcode::name() const
{
	unsigned int k = key();
	return k<358 ? opcodes[k] : QString("UNKNOWN%1").arg(k-358+1);
}

QString JsmOpcode::paramStr() const
{
	return QString::number(param());
}

QString JsmOpcode::desc() const
{
	return QString();
}

JsmOpcodeCal::JsmOpcodeCal(quint32 op) : JsmOpcode(op) {}
JsmOpcodeCal::JsmOpcodeCal(unsigned int key, int param) : JsmOpcode(key, param) {}
bool JsmOpcodeCal::hasParam() const {	return true; }
int JsmOpcodeCal::popCount() const {	return 2; }
int JsmOpcodeCal::pushCount() const {	return 1; }
QString JsmOpcodeCal::paramStr() const {
	int p = param();
	return p<15 ? cal_table[p] : JsmOpcode::paramStr();
}

const char *JsmOpcodeCal::cal_table[15] = {
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
};

JsmOpcodePsh::JsmOpcodePsh(quint32 op) : JsmOpcode(op) {}
JsmOpcodePsh::JsmOpcodePsh(unsigned int key, int param) : JsmOpcode(key, param) {}
bool JsmOpcodePsh::hasParam() const {	return true; }
int JsmOpcodePsh::popCount() const {	return 0; }
int JsmOpcodePsh::pushCount() const {	return 1; }

JsmOpcodePop::JsmOpcodePop(quint32 op) : JsmOpcode(op) {}
JsmOpcodePop::JsmOpcodePop(unsigned int key, int param) : JsmOpcode(key, param) {}
bool JsmOpcodePop::hasParam() const {	return true; }
int JsmOpcodePop::popCount() const {	return 1; }
int JsmOpcodePop::pushCount() const {	return 0; }

/*
CAL: Un paramètre
JMP: Un paramètre (Signé ?)
JPF: Un paramètre (Signé ?)
LBL: Un paramètre
RET: Un paramètre
PSHN_L: Un paramètre (Signé ?)
PSHI_L: Un paramètre
POPI_L: Un paramètre
PSHM_B: Un paramètre
POPM_B: Un paramètre
PSHM_W: Un paramètre
POPM_W: Un paramètre
PSHM_L: Un paramètre
POPM_L: Un paramètre
PSHSM_B: Un paramètre
PSHSM_W: Un paramètre
PSHSM_L: Un paramètre
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
Unknown1: Aucun paramètre
Unknown2: Aucun paramètre
Unknown3: Aucun paramètre
Unknown4: Aucun paramètre
Unknown5: Aucun paramètre
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
Unknown17: Aucun paramètre
Unknown18: Aucun paramètre
*/

const char *JsmOpcode::opcodes[376] = {
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
	"UNKNOWN1",
	"UNKNOWN2",
	"UNKNOWN3",
	"UNKNOWN4",
	"UNKNOWN5",
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
	"UNKNOWN17",
	"UNKNOWN18"
};
