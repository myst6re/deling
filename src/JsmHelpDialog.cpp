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
#include "JsmHelpDialog.h"

struct OpcodeHelp {
	const char *name;
	const char *category;
	const char *signature;
	const char *description;
};

static const OpcodeHelp opcodeHelpData[] = {
	// -- Syntax --
	{"if / else / end", "Syntax", "if <condition> begin ... else ... end",
	 "Conditional execution. The else block is optional. Conditions use ==, !=, >, <, >=, <=, and, or."},
	{"while / end", "Syntax", "while <condition> begin ... end",
	 "Loop while condition is true."},
	{"wait while", "Syntax", "wait while <condition>",
	 "Pause script until condition becomes false. Empty-body while loop."},
	{"wait forever", "Syntax", "wait forever",
	 "Pause script execution indefinitely."},
	{"forever / end", "Syntax", "forever begin ... end",
	 "Infinite loop."},
	{"repeat / until", "Syntax", "repeat ... until <condition> end",
	 "Execute body at least once, repeat until condition is true."},
	{"ret", "Syntax", "ret(scriptID)", "Return from the current script."},
	{"Assignment", "Syntax", "variable = expression",
	 "Assign a value. Compound: +=, -=, *=, /=, %=, &=, |=, ^=, >>=, <<=."},
	{"Variables", "Syntax", "N_ubyte, N_uword, N_ulong, N_sbyte, N_sword, N_slong",
	 "Memory variables at address N. Type suffix sets read width and signedness."},
	{"Temp Variables", "Syntax", "temp_N",
	 "Temporary registers 0-7. Many opcodes store results in temp_0."},
	{"Model Reference", "Syntax", "model_N", "Entity/model reference by index."},
	{"Constants", "Syntax", "text_N, map_N, item_N, magic_N, KeyCancel, Squall, etc.",
	 "Named constants that resolve to numeric values. See the Characters, Keys, and Resources categories for full listings."},

	// -- Character Constants --
	{"Squall", "Characters", "Squall = 0", "Main protagonist."},
	{"Zell", "Characters", "Zell = 1", "Martial artist party member."},
	{"Irvine", "Characters", "Irvine = 2", "Sharpshooter party member."},
	{"Quistis", "Characters", "Quistis = 3", "Instructor party member."},
	{"Rinoa", "Characters", "Rinoa = 4", "Rinoa Heartilly."},
	{"Selphie", "Characters", "Selphie = 5", "Selphie Tilmitt."},
	{"Seifer", "Characters", "Seifer = 6", "Seifer Almasy."},
	{"Edea", "Characters", "Edea = 7", "Edea Kramer."},
	{"Laguna", "Characters", "Laguna = 8", "Laguna Loire. Used in dream sequences."},
	{"Kiros", "Characters", "Kiros = 9", "Kiros Seagill. Used in dream sequences."},
	{"Ward", "Characters", "Ward = 10", "Ward Zabac. Used in dream sequences."},

	// -- Key Constants --
	{"KeyL1", "Keys", "KeyL1 = 0x1", "L1 button."},
	{"KeyR1", "Keys", "KeyR1 = 0x2", "R1 button."},
	{"KeyL2", "Keys", "KeyL2 = 0x4", "L2 button."},
	{"KeyR2", "Keys", "KeyR2 = 0x8", "R2 button."},
	{"KeyCancel", "Keys", "KeyCancel = 0x10 (16)", "Cancel / X button."},
	{"KeyMenu", "Keys", "KeyMenu = 0x20 (32)", "Menu / Triangle button."},
	{"KeyChoose", "Keys", "KeyChoose = 0x40 (64)", "Confirm / Circle button."},
	{"KeyCard", "Keys", "KeyCard = 0x80 (128)", "Card / Square button."},
	{"KeySelect", "Keys", "KeySelect = 0x100 (256)", "Select button."},
	{"KeyStart", "Keys", "KeyStart = 0x800 (2048)", "Start button."},
	{"KeyUp", "Keys", "KeyUp = 0x1000 (4096)", "D-pad up."},
	{"KeyRight", "Keys", "KeyRight = 0x2000 (8192)", "D-pad right."},
	{"KeyDown", "Keys", "KeyDown = 0x4000 (16384)", "D-pad down."},
	{"KeyLeft", "Keys", "KeyLeft = 0x8000 (32768)", "D-pad left."},

	// -- Resource Constants --
	{"text_N", "Resources", "text_N (e.g. text_0, text_5)", "Reference to a text entry in the field's MSD file. N is the text index."},
	{"map_N", "Resources", "map_N (e.g. map_100, map_256)", "Reference to a field map by ID. Used with mapjump and related opcodes."},
	{"item_N", "Resources", "item_N (e.g. item_1, item_32)", "Reference to an item by ID. "
	 "0=Empty, 1=Potion, 2=Potion+, 3=Hi-Potion, 4=Hi-Potion+, 5=X-Potion, 6=Mega-Potion, "
	 "7=Phoenix Down, 8=Mega-Phoenix, 9=Elixir, 10=Megalixir, 11=Antidote, 12=Soft, "
	 "13=Eyedrops, 14=Echo Screen, 15=Holy Water, 16=Remedy, 17=Remedy+, 18=Hero-Trial, "
	 "19=Hero, 20=Holy War-Trial, 21=Holy War, 22=Shell Stone, 23=Protect Stone, "
	 "24=Aura Stone, 25=Death Stone, 26=Holy Stone, 27=Flare Stone, 28=Meteor Stone, "
	 "29=Ultima Stone, 30=Gysahl Greens, 31=Phoenix Pinion, 32=Friendship."},
	{"magic_N", "Resources", "magic_N (e.g. magic_1, magic_20)", "Reference to a magic spell by ID. "
	 "0=Empty, 1=Fire, 2=Fira, 3=Firaga, 4=Blizzard, 5=Blizzara, 6=Blizzaga, "
	 "7=Thunder, 8=Thundara, 9=Thundaga, 10=Water, 11=Aero, 12=Bio, 13=Demi, "
	 "14=Holy, 15=Flare, 16=Meteor, 17=Quake, 18=Tornado, 19=Ultima, 20=Apocalypse, "
	 "21=Cure, 22=Cura, 23=Curaga, 24=Life, 25=Full-Life, 26=Regan, 27=Esuna, "
	 "28=Dispel, 29=Protect, 30=Shell, 31=Reflect, 32=Aura, 33=Double, 34=Triple, "
	 "35=Haste, 36=Slow, 37=Stop, 38=Blind, 39=Confuse, 40=Sleep, 41=Silence, "
	 "42=Break, 43=Death, 44=Drain, 45=Pain, 46=Berserk, 47=Float, 48=Zombie, "
	 "49=Meltdown, 50=Scan, 51=Full-Cure, 52=Wall, 53=Rapture, 54=Percent, "
	 "55=Catastrophe, 56=The End."},

	// -- Flow Control --
	{"nop", "Flow Control", "nop()", "No operation. Does nothing."},
	{"halt", "Flow Control", "halt()", "Exit current script AND all scripts waiting on it."},
	{"wait", "Flow Control", "wait(frames)", "Pause this script for N frames (30fps)."},
	{"debug", "Flow Control", "debug()", "No function. Development only."},

	// -- Script Execution --
	{"req", "Script Execution", "req(entityGroupID, methodLabel, priority)",
	 "Request remote execution. Non-blocking, may fail if busy."},
	{"reqsw", "Script Execution", "reqsw(entityGroupID, methodLabel, priority)",
	 "Request remote execution, async, guaranteed."},
	{"reqew", "Script Execution", "reqew(entityGroupID, methodLabel, priority)",
	 "Request remote execution, sync (waits), guaranteed."},
	{"preq", "Script Execution", "preq(partySlot, methodLabel, priority)",
	 "Request execution on party member entity."},
	{"preqsw", "Script Execution", "preqsw(partySlot, methodLabel, priority)",
	 "Request party entity execution, async, guaranteed."},
	{"preqew", "Script Execution", "preqew(partySlot, methodLabel, priority)",
	 "Request party entity execution, sync, guaranteed."},

	// -- Entity Control --
	{"unuse", "Entity", "unuse(entityID)", "Disable entity scripts, hide model, make passable."},
	{"use", "Entity", "use()", "Re-enable an entity. Opposite of unuse."},
	{"show", "Entity", "show()", "Show this entity's model."},
	{"hide", "Entity", "hide()", "Hide this entity's model."},
	{"throughon", "Entity", "throughon()", "Make entity passable (walk through)."},
	{"throughoff", "Entity", "throughoff()", "Make entity solid (collide)."},
	{"talkon", "Entity", "talkon()", "Enable talk script."},
	{"talkoff", "Entity", "talkoff()", "Disable talk script."},
	{"pushon", "Entity", "pushon()", "Enable push script."},
	{"pushoff", "Entity", "pushoff()", "Disable push script."},
	{"istouch", "Entity", "istouch(actorID)", "Push 1 to temp_0 if actor is in touch range, else 0."},
	{"talkradius", "Entity", "talkradius(radius)", "Set talk trigger distance. Humanoids ~200."},
	{"pushradius", "Entity", "pushradius(radius)", "Set push trigger distance. Active=48, inactive=1."},
	{"actormode", "Entity", "actormode(mode)", "Set transparency/blinking effects on models."},
	{"whoami", "Entity", "whoami()", "Push this character's real ID into temp_0. For Laguna dreams."},

	// -- Positioning --
	{"set", "Positioning", "set(walkmeshTriID, x, y)", "Place entity at X,Y. Z from walkmesh."},
	{"set3", "Positioning", "set3(walkmeshTriID, x, y, z)", "Place entity at X,Y,Z explicitly."},
	{"idlock", "Positioning", "idlock(walkmeshTriID)", "Lock a walkmesh triangle."},
	{"idunlock", "Positioning", "idunlock(walkmeshTriID)", "Unlock a walkmesh triangle."},
	{"dir", "Positioning", "dir(angle)", "Face angle. 256-degree: 0=down, 64=right, 128=up, 192=left."},
	{"dirp", "Positioning", "dirp(partyMemberID, unknown, speed)", "Face a party member."},
	{"dira", "Positioning", "dira(actorID)", "Face an actor."},
	{"pdira", "Positioning", "pdira(partyMemberID)", "Face a party entity."},
	{"getinfo", "Positioning", "getinfo()", "Push entity X to temp_0, Y to temp_1."},
	{"pgetinfo", "Positioning", "pgetinfo()", "Push party member X to temp_0, Y to temp_1."},
	{"facedira", "Positioning", "facedira(actorID, frames)", "Turn head to face actor over N frames."},
	{"facedirp", "Positioning", "facedirp(partyMemberID, frames)", "Turn head to face party member."},
	{"facedirlimit", "Positioning", "facedirlimit(limit)", "Limit head turn range."},
	{"facediroff", "Positioning", "facediroff(frames)", "Stop head facing over N frames."},
	{"facedirinit", "Positioning", "facedirinit()", "Unlock head from model. Call before FACEDIR."},
	{"facedir", "Positioning", "facedir(angle, frames)", "Set face direction over frames."},
	{"facediri", "Positioning", "facediri(angle)", "Set face direction immediately."},
	{"rfacedir", "Positioning", "rfacedir(angle, frames)", "Resume-script face direction."},
	{"rfacedira", "Positioning", "rfacedira(actorID, frames)", "Resume-script face towards actor."},
	{"rfacedirp", "Positioning", "rfacedirp(partyMemberID, frames)", "Resume-script face towards party member."},
	{"rfacediri", "Positioning", "rfacediri(angle)", "Resume-script face direction immediately."},
	{"rfacediroff", "Positioning", "rfacediroff(frames)", "Resume-script stop face direction."},
	{"facedirsync", "Positioning", "facedirsync()", "Wait for face direction to finish."},

	// -- Movement --
	{"mspeed", "Movement", "mspeed(speed)", "Set movement speed."},
	{"move", "Movement", "move(walkmeshID, x, y, unknown)", "Walk/run to X,Y."},
	{"movea", "Movement", "movea(actorID, unknown)", "Move towards actor."},
	{"pmovea", "Movement", "pmovea(partyMemberID, unknown)", "Move towards party member."},
	{"cmove", "Movement", "cmove(walkmeshID, x, y, unknown)", "Move without animation or turning."},
	{"fmove", "Movement", "fmove(walkmeshID, x, y, unknown)", "Move without animation, face direction."},
	{"movesync", "Movement", "movesync()", "Wait until MOVE finishes."},
	{"moveflush", "Movement", "moveflush()", "Halt current movement."},
	{"jump", "Movement", "jump(walkmeshID, x, y, speed)", "Jump to X,Y."},
	{"jump3", "Movement", "jump3(walkmeshID, x, y, z, speed)", "Jump to X,Y,Z."},
	{"pjumpa", "Movement", "pjumpa(partyMemberID, unknown)", "Jump to party member."},
	{"ladderup", "Movement", "ladderup(walkmeshID, x, y, z)", "Ladder up movement."},
	{"ladderdown", "Movement", "ladderdown(walkmeshID, x, y, z)", "Ladder down movement."},
	{"ladderup2", "Movement", "ladderup2(...)", "Ladder up variant (9 stack params)."},
	{"ladderdown2", "Movement", "ladderdown2(...)", "Ladder down variant (9 stack params)."},
	{"ladderanime", "Movement", "ladderanime(animID, firstFrame, lastFrame)", "Ladder animation."},
	{"rmove", "Movement", "rmove(walkmeshID, x, y, unknown)", "Resume-script move (no pause)."},
	{"rmovea", "Movement", "rmovea(actorID, unknown)", "Resume-script move to actor."},
	{"rpmovea", "Movement", "rpmovea(partyMemberID, unknown)", "Resume-script move to party member."},
	{"rcmove", "Movement", "rcmove(walkmeshID, x, y, unknown)", "Resume-script cmove."},
	{"rfmove", "Movement", "rfmove(walkmeshID, x, y, unknown)", "Resume-script fmove."},
	{"fmovea", "Movement", "fmovea(actorID, unknown)", "Face-move to actor."},
	{"fmovep", "Movement", "fmovep(partyMemberID, unknown)", "Face-move to party member."},
	{"movecancel", "Movement", "movecancel()", "Cancel current movement."},
	{"pmovecancel", "Movement", "pmovecancel()", "Cancel party member movement."},
	{"maccel", "Movement", "maccel(acceleration)", "Set movement acceleration."},
	{"mlimit", "Movement", "mlimit(limit)", "Set movement speed limit."},

	// -- Turning --
	{"cturnr", "Turning", "cturnr(angle, speed)", "Turn clockwise. 256-degree system."},
	{"cturnl", "Turning", "cturnl(angle, speed)", "Turn counter-clockwise."},
	{"lturnr", "Turning", "lturnr(angle, speed)", "Linear turn right."},
	{"lturnl", "Turning", "lturnl(angle, speed)", "Linear turn left."},
	{"cturn", "Turning", "cturn()", "Face entity set with PSHAC."},
	{"lturn", "Turning", "lturn()", "Linear turn to face entity."},
	{"pcturn", "Turning", "pcturn(partyMemberID, speed)", "Face a PC. Speed = frame count."},
	{"plturn", "Turning", "plturn(partyMemberID, speed)", "Linear turn to face PC."},

	// -- Model / Animation --
	{"setmodel", "Animation", "setmodel(modelID)", "Set display model."},
	{"setpc", "Animation", "setpc(characterID)", "Set which PC this entity represents."},
	{"baseanime", "Animation", "baseanime(animID, firstFrame, lastFrame)", "Set idle animation."},
	{"anime", "Animation", "anime(animID)", "Play animation, return to base. Pauses script."},
	{"animekeep", "Animation", "animekeep(animID)", "Play animation, freeze last frame. Pauses."},
	{"canime", "Animation", "canime(animID, firstFrame, lastFrame)", "Play frame range, return to base. Pauses."},
	{"canimekeep", "Animation", "canimekeep(animID, firstFrame, lastFrame)", "Play frame range, freeze. Pauses."},
	{"ranime", "Animation", "ranime(animID)", "Play animation, return to base. No pause."},
	{"ranimekeep", "Animation", "ranimekeep(animID)", "Play animation, freeze. No pause."},
	{"rcanime", "Animation", "rcanime(animID, firstFrame, lastFrame)", "Play frame range, return. No pause."},
	{"rcanimekeep", "Animation", "rcanimekeep(animID, firstFrame, lastFrame)", "Play frame range, freeze. No pause."},
	{"ranimeloop", "Animation", "ranimeloop(animID)", "Loop animation. No pause."},
	{"rcanimeloop", "Animation", "rcanimeloop(animID, firstFrame, lastFrame)", "Loop frame range. No pause."},
	{"animesync", "Animation", "animesync()", "Wait for current animation to finish."},
	{"animestop", "Animation", "animestop()", "Return to base animation immediately."},
	{"animespeed", "Animation", "animespeed(speed)", "Set animation speed. 1 = 2fps."},
	{"footstep", "Animation", "footstep(flag)", "Footstep sound/effect control."},

	// -- Messages / Dialog --
	{"mesw", "Messages", "mesw(channel, textID)", "Show message, wait for dismiss. Rarely used."},
	{"mes", "Messages", "mes(channel, textID)", "Show message. Size set with winsize."},
	{"messync", "Messages", "messync(channel)", "Wait for message to close."},
	{"mesvar", "Messages", "mesvar(varIndex, value)", "Set message variable {Var0}, {Var1}, etc."},
	{"ask", "Messages", "ask(channel, textID, firstLine, lastLine, defaultLine, cancelLine)", "Show choices. Result in temp_0."},
	{"winsize", "Messages", "winsize(channel, x, y, width, height)", "Set message window size/position."},
	{"winclose", "Messages", "winclose(channel)", "Close message window."},
	{"amesw", "Messages", "amesw(channel, textID, x, y)", "Show positioned message, wait."},
	{"ames", "Messages", "ames(channel, textID, x, y)", "Show positioned message. Stays until winclose."},
	{"aask", "Messages", "aask(channel, textID, first, last, default, unknown, x, y)", "Positioned choices. Result in temp_0."},
	{"setmesspeed", "Messages", "setmesspeed(speed, unknown)", "Set text display speed."},
	{"mesmode", "Messages", "mesmode(mode)", "0=opaque, 1=translucent, 2=no window."},
	{"ramesw", "Messages", "ramesw(channel, textID, x, y)", "Show message and wait, script continues."},

	// -- Player Control --
	{"ucon", "Player Control", "ucon()", "Enable player control."},
	{"ucoff", "Player Control", "ucoff()", "Disable player control."},
	{"keyscan", "Player Control", "keyscan(keyMask)", "Check keys pressed. Result in temp_0."},
	{"keyon", "Player Control", "keyon(keyMask)", "Re-enable specific keys."},
	{"keyscan2", "Player Control", "keyscan2(keyMask)", "Alternate key scan."},
	{"keyon2", "Player Control", "keyon2(keyMask)", "Alternate key enable."},

	// -- Lines / Triggers --
	{"setline", "Lines", "setline(x1, y1, z1, x2, y2, z2)", "Set 3D hitbox bounds."},
	{"lineon", "Lines", "lineon()", "Enable line collisions."},
	{"lineoff", "Lines", "lineoff()", "Disable line collisions."},

	// -- Map Transitions --
	{"mapjump", "Map Transitions", "mapjump(walkmeshID, fieldMapID, x, y, z)", "Jump to another field."},
	{"mapjump3", "Map Transitions", "mapjump3(walkmeshID, fieldMapID, x, y, z, angle)", "Jump to field with angle."},
	{"mapjumpo", "Map Transitions", "mapjumpo(fieldMapID, unknown)", "Jump to field at walkmesh 0. For cutscenes."},
	{"mapjumpon", "Map Transitions", "mapjumpon()", "Enable field exits."},
	{"mapjumpoff", "Map Transitions", "mapjumpoff()", "Disable field exits."},
	{"discjump", "Map Transitions", "discjump(walkmeshID, fieldMapID, x, y, z, angle)", "Map jump + disc switch."},
	{"disc", "Map Transitions", "disc(discNumber)", "Set which disc for discjump."},
	{"worldmapjump", "Map Transitions", "worldmapjump()", "Jump to world map."},
	{"premapjump", "Map Transitions", "premapjump(...)", "Prepare map jump."},

	// -- Camera / Scrolling --
	{"dscroll", "Camera", "dscroll()", "Scroll the screen."},
	{"lscroll", "Camera", "lscroll(x, y, frameCount)", "Scroll by X,Y pixels over frames."},
	{"cscroll", "Camera", "cscroll(x, y, frameCount)", "Scroll by X,Y pixels over frames."},
	{"dscrolla", "Camera", "dscrolla(actorID)", "Scroll onto actor."},
	{"lscrolla", "Camera", "lscrolla(actorID, frameCount)", "Linear scroll to actor."},
	{"cscrolla", "Camera", "cscrolla(actorID, frameCount)", "Curved scroll to actor."},
	{"scrollsync", "Camera", "scrollsync()", "Wait for scroll to finish."},
	{"dscrollp", "Camera", "dscrollp(partyMemberID)", "Scroll to party member."},
	{"lscrollp", "Camera", "lscrollp(partyMemberID, frameCount)", "Linear scroll to party member."},
	{"cscrollp", "Camera", "cscrollp(partyMemberID, frameCount)", "Curved scroll to party member."},
	{"dscroll2", "Camera", "dscroll2()", "Scroll variant 2."},
	{"lscroll2", "Camera", "lscroll2(x, y, frameCount)", "Linear scroll variant 2."},
	{"cscroll2", "Camera", "cscroll2(x, y, frameCount)", "Curved scroll variant 2."},
	{"dscrolla2", "Camera", "dscrolla2(actorID)", "Scroll to actor variant 2."},
	{"lscrolla2", "Camera", "lscrolla2(actorID, frameCount)", "Linear scroll to actor variant 2."},
	{"cscrolla2", "Camera", "cscrolla2(actorID, frameCount)", "Curved scroll to actor variant 2."},
	{"dscrollp2", "Camera", "dscrollp2(partyMemberID)", "Scroll to party member variant 2."},
	{"lscrollp2", "Camera", "lscrollp2(partyMemberID, frameCount)", "Linear scroll to party variant 2."},
	{"cscrollp2", "Camera", "cscrollp2(partyMemberID, frameCount)", "Curved scroll to party variant 2."},
	{"scrollsync2", "Camera", "scrollsync2()", "Wait for scroll2 to finish."},
	{"scrollmode2", "Camera", "scrollmode2(mode)", "Set scroll mode variant 2."},
	{"dscroll3", "Camera", "dscroll3()", "Scroll variant 3."},
	{"lscroll3", "Camera", "lscroll3(x, y, frameCount)", "Linear scroll variant 3."},
	{"cscroll3", "Camera", "cscroll3(x, y, frameCount)", "Curved scroll variant 3."},
	{"scrollratio2", "Camera", "scrollratio2(ratio)", "Set scroll ratio variant 2."},
	{"setcamera", "Camera", "setcamera()", "Set camera parameters."},
	{"setdcamera", "Camera", "setdcamera()", "Set dynamic camera."},
	{"doffset", "Camera", "doffset(x, y)", "Direct camera offset."},
	{"loffsets", "Camera", "loffsets(x, y, frameCount)", "Linear camera offsets."},
	{"coffsets", "Camera", "coffsets(x, y, frameCount)", "Curved camera offsets."},
	{"loffset", "Camera", "loffset(x, y, frameCount)", "Linear camera offset."},
	{"coffset", "Camera", "coffset(x, y, frameCount)", "Curved camera offset."},
	{"offsetsync", "Camera", "offsetsync()", "Wait for camera offset to finish."},

	// -- Movies / FMV --
	{"movie", "Movies", "movie()", "Play prepared movie."},
	{"moviesync", "Movies", "moviesync()", "Wait for movie to finish."},
	{"movieready", "Movies", "movieready(movieID)", "Prepare/load a movie."},
	{"moviecut", "Movies", "moviecut()", "Debug only. No function."},

	// -- Sound Effects --
	{"effectplay2", "Sound", "effectplay2(soundID, pan, volume)", "Play field SE. Pan 0=left 255=right. Vol 0-127."},
	{"effectplay", "Sound", "effectplay(channel)", "Play SE through channel."},
	{"effectload", "Sound", "effectload(soundID)", "Start background looping sound."},
	{"sestop", "Sound", "sestop(channel)", "Stop SE on channel."},
	{"spuready", "Sound", "spuready(frames)", "Set async timer to 0 frames."},
	{"spusync", "Sound", "spusync(frames)", "Wait N frames since spuready."},

	// -- Music --
	{"musicload", "Music", "musicload(trackID)", "Preload field music."},
	{"musicchange", "Music", "musicchange()", "Stop current, start preloaded music."},
	{"musicstop", "Music", "musicstop()", "Stop music."},
	{"musicvol", "Music", "musicvol(volume)", "Set music volume."},
	{"musicvoltrans", "Music", "musicvoltrans(volume, duration)", "Transition music volume."},
	{"setbattlemusic", "Music", "setbattlemusic(musicID)", "Set battle music."},
	{"allsevol", "Music", "allsevol(volume)", "Set volume for all SE."},
	{"allsevoltrans", "Music", "allsevoltrans(volume, duration)", "Transition all SE volume."},
	{"allsepos", "Music", "allsepos(pan)", "Set pan for all SE."},
	{"allsepostrans", "Music", "allsepostrans(pan, duration)", "Transition all SE pan."},
	{"sevol", "Music", "sevol(channel, volume)", "Set SE channel volume."},
	{"sevoltrans", "Music", "sevoltrans(channel, volume, frames)", "Transition SE channel volume."},
	{"sepos", "Music", "sepos(channel, pan)", "Set SE channel pan. 0=left, 255=right."},
	{"sepostrans", "Music", "sepostrans(channel, pan, frames)", "Transition SE channel pan."},

	// -- Visual Effects --
	{"fadeblack", "Visual", "fadeblack()", "Fade to black."},
	{"fadein", "Visual", "fadein()", "Fade in from black."},
	{"fadeout", "Visual", "fadeout()", "Fade out to black."},
	{"fadesync", "Visual", "fadesync()", "Wait for fade to complete."},
	{"shadelevel", "Visual", "shadelevel(level)", "Set actor shading."},
	{"fcolsub", "Visual", "fcolsub(r, g, b)", "Color transition on screen."},
	{"setvibrate", "Visual", "setvibrate()", "Vibrate controller."},
	{"clear", "Visual", "clear()", "Reset all variables. New game only."},
	{"shake", "Visual", "shake()", "Screen shake."},
	{"shakeoff", "Visual", "shakeoff()", "Stop screen shake."},

	// -- Battle --
	{"battle", "Battle", "battle(encounterID)", "Start battle. Flags: +2=no fanfare, +4=timer, +16=music, +32=preemptive, +64=back attack."},
	{"battleresult", "Battle", "battleresult()", "Push last battle result to temp_0."},
	{"battleon", "Battle", "battleon()", "Enable random battles."},
	{"battleoff", "Battle", "battleoff()", "Disable random battles."},
	{"battlemode", "Battle", "battlemode(flags)", "Set random battle flags."},
	{"battlecut", "Battle", "battlecut()", "Debug only."},

	// -- Party / Characters --
	{"addparty", "Party", "addparty(characterID)", "Add to active party. 0=Squall..10=Ward."},
	{"subparty", "Party", "subparty(characterID)", "Remove from active party."},
	{"setparty", "Party", "setparty(member1, member2, member3)", "Set full party. 255=blank."},
	{"isparty", "Party", "isparty(characterID)", "Push party position to temp_0. -1 if absent."},
	{"addmember", "Party", "addmember(characterID)", "Add to available roster."},
	{"submember", "Party", "submember(characterID)", "Remove from roster and party."},
	{"ismember", "Party", "ismember(characterID)", "Check if character is in roster."},
	{"getparty", "Party", "getparty(slotIndex)", "Push character ID in slot to temp_0."},
	{"join", "Party", "join()", "Start conga line (followers follow leader)."},
	{"split", "Party", "split()", "Disable conga line, send members to spots."},
	{"junction", "Party", "junction(mode)", "0=end dream, 1=Squall junctions carry, 3=whole party."},
	{"setdress", "Party", "setdress(characterID)", "Set character costume."},
	{"getdress", "Party", "getdress(characterID)", "Get character costume."},
	{"resetgf", "Party", "resetgf(characterID)", "Reset GF for character."},

	// -- Items / Magic / Cards --
	{"additem", "Items", "additem(quantity, itemID)", "Add item to inventory."},
	{"hasitem", "Items", "hasitem(itemID)", "Push 1 to temp_0 if party has item, else 0."},
	{"addmagic", "Items", "addmagic(characterID, magicID)", "Add magic to character."},
	{"setcard", "Items", "setcard(cardID, deckID)", "Move card to deck. Rare cards are unique."},
	{"getcard", "Items", "getcard(cardID)", "Get/check a card. Result in temp_0."},
	{"howmanycard", "Items", "howmanycard(cardID)", "Push the count of a card the player owns to temp_0."},
	{"wherecard", "Items", "wherecard(cardID)", "Push which deck a card is in to temp_0."},
	{"drawpoint", "Items", "drawpoint(drawPointID)", "Initialize draw point."},
	{"setdrawpoint", "Items", "setdrawpoint(isHidden)", "Hide (>=1) or show (0) draw point."},
	{"cardgame", "Items", "cardgame(deckID, knownRules, regionRules, rareChance, u1, u2, u3)",
	 "Start Triple Triad game."},
	{"addgil", "Items", "addgil(amount)", "Add gil."},

	// -- Menus / UI --
	{"menunormal", "Menus", "menunormal()", "Open normal menu."},
	{"menuphs", "Menus", "menuphs()", "Open PHS (party switch) menu."},
	{"menushop", "Menus", "menushop(shopID)", "Open shop."},
	{"menusave", "Menus", "menusave()", "Open save menu."},
	{"menuname", "Menus", "menuname(characterOrGfID)", "Name entry screen. Unlocks GFs."},
	{"saveenable", "Menus", "saveenable(enabled)", "Enable/disable saving."},
	{"phsenable", "Menus", "phsenable(enabled)", "Enable/disable PHS submenu."},
	{"phspower", "Menus", "phspower(enabled)", "Enable/disable party switching."},
	{"menuenable", "Menus", "menuenable()", "Enable menu access."},
	{"menudisable", "Menus", "menudisable()", "Disable menu access."},

	// -- Timers --
	{"settimer", "Timers", "settimer(value)", "Set countdown timer value."},
	{"disptimer", "Timers", "disptimer()", "Display and start countdown timer."},
	{"gettimer", "Timers", "gettimer()", "Get current timer value."},
	{"killtimer", "Timers", "killtimer()", "Stop and hide timer."},

	// -- Background Animation --
	{"bganime", "Background", "bganime(paramID)", "Animate background object."},
	{"rbganime", "Background", "rbganime(paramID)", "Resume background animation."},
	{"rbganimeloop", "Background", "rbganimeloop(paramID)", "Loop background animation."},
	{"bganimesync", "Background", "bganimesync()", "Wait for background animation."},
	{"bgdraw", "Background", "bgdraw(state)", "Set background draw state."},
	{"bgoff", "Background", "bgoff(paramID)", "Turn off background layer."},
	{"bganimespeed", "Background", "bganimespeed(speed)", "Set background animation speed."},
	{"bgshade", "Background", "bgshade(r, g, b, r2, g2, b2, unknown)", "Set background shading."},

	// -- Salary / SeeD --
	{"saralyoff", "Salary", "saralyoff()", "Disable salary payments."},
	{"saralyon", "Salary", "saralyon()", "Enable salary payments."},
	{"saralydispoff", "Salary", "saralydispoff()", "Hide salary alerts."},
	{"saralydispon", "Salary", "saralydispon()", "Show salary alerts."},
	{"addseedlevel", "Salary", "addseedlevel(points)", "Add SeeD level points. 100/rank, max 3100."},

	// -- Game State --
	{"gameover", "Game State", "gameover()", "End game in failure."},
	{"rnd", "Game State", "rnd()", "Push random 0-255 to temp_0."},
	{"setplace", "Game State", "setplace(locationID)", "Set location name for menu/saves."},
	{"runenable", "Game State", "runenable()", "Enable running."},
	{"rundisable", "Game State", "rundisable()", "Disable running."},
	{"hold", "Game State", "hold(characterID, locked)", "Lock/unlock PC in switch menu."},
	{"dying", "Game State", "dying()", "Resurrect dead characters. Used for Laguna party switch."},
	{"setwitch", "Game State", "setwitch()", "Unlock Rinoa's sorceress limit break."},
	{"setodin", "Game State", "setodin()", "Unlock Odin."},
	{"disableangelo", "Game State", "disableangelo(flag)", "Disable Angelo in battle."},
	{"setgeta", "Game State", "setgeta()", "Unknown/unfinished."},

	// -- Last Dungeon --
	{"lastin", "Last Dungeon", "lastin()", "Disable features for Ultimecia's Castle."},
	{"lastout", "Last Dungeon", "lastout()", "End lastin effects."},
	{"sealedoff", "Last Dungeon", "sealedoff(flags)",
	 "Enable sealed features. 1=Items, 2=Magic, 4=GF, 8=Draw, 16=Command, 32=Limit, 64=Resurrect, 128=Save."},

	// -- Tutorial --
	{"showtutorial", "Tutorial", "showtutorial(tutorialID)", "Show a tutorial screen."},
	{"menututo", "Tutorial", "menututo()", "Open tutorial menu."},
	{"menutips", "Tutorial", "menutips()", "Open tips menu."},

	// -- Misc --
	{"followon", "Misc", "followon()", "Enable camera follow."},
	{"followoff", "Misc", "followoff()", "Disable camera follow."},
	{"polycolor", "Misc", "polycolor(r, g, b)", "Set polygon color."},
	{"polycolorall", "Misc", "polycolorall(r, g, b)", "Set all polygon colors."},
	{"dcoladd", "Misc", "dcoladd(r, g, b)", "Add color to display."},
	{"dcolsub", "Misc", "dcolsub(r, g, b)", "Subtract color from display."},
	{"tcoladd", "Misc", "tcoladd(r, g, b)", "Add color transition."},
	{"tcolsub", "Misc", "tcolsub(r, g, b)", "Subtract color transition."},
	{"fcoladd", "Misc", "fcoladd(r, g, b)", "Add fade color."},
	{"fcolsub", "Misc", "fcolsub(r, g, b)", "Subtract fade color."},
	{"colsync", "Misc", "colsync()", "Wait for color transition."},
	{"inittrace", "Misc", "inittrace()", "Initialize trace."},
	{"copyinfo", "Misc", "copyinfo()", "Copy entity info."},
	{"openeyes", "Misc", "openeyes()", "Open character eyes."},
	{"closeeyes", "Misc", "closeeyes()", "Close character eyes."},
	{"blinkeyes", "Misc", "blinkeyes()", "Blink character eyes."},
	{"swap", "Misc", "swap()", "Swap entities."},

	// -- Footstep Control --
	{"footstepon", "Footsteps", "footstepon()", "Enable footstep sounds."},
	{"footstepoff", "Footsteps", "footstepoff()", "Disable footstep sounds."},
	{"footstepoffall", "Footsteps", "footstepoffall()", "Disable all footstep sounds."},
	{"footstepcut", "Footsteps", "footstepcut()", "Cut footstep sounds."},
	{"footstepcopy", "Footsteps", "footstepcopy()", "Copy footstep settings from another entity."},

	// -- Animation State --
	{"pushanime", "Animation", "pushanime()", "Save current animation state to stack."},
	{"popanime", "Animation", "popanime()", "Restore saved animation state from stack."},

	// -- Particles --
	{"particleon", "Visual", "particleon()", "Enable particle effects."},
	{"particleoff", "Visual", "particleoff()", "Disable particle effects."},
	{"particleset", "Visual", "particleset(params)", "Set particle parameters."},

	// -- Additional Visual --
	{"ending", "Visual", "ending()", "Play ending sequence."},
	{"shadeform", "Visual", "shadeform()", "Set shade form effect."},
	{"shadeset", "Visual", "shadeset()", "Set shade parameters."},
	{"fadenone", "Visual", "fadenone()", "Cancel fade / no fade."},
	{"mapfadeoff", "Visual", "mapfadeoff()", "Disable map transition fade."},
	{"mapfadeon", "Visual", "mapfadeon()", "Enable map transition fade."},
	{"stopvibrate", "Visual", "stopvibrate()", "Stop controller vibration."},

	// -- Additional Music/Sound --
	{"crossmusic", "Music", "crossmusic()", "Crossfade between music tracks."},
	{"dualmusic", "Music", "dualmusic()", "Play dual music tracks."},
	{"musicvolfade", "Music", "musicvolfade(volume, duration)", "Fade music volume."},
	{"musicstatus", "Music", "musicstatus()", "Get music playback status."},
	{"musicreplay", "Music", "musicreplay()", "Replay current music."},
	{"musicskip", "Music", "musicskip()", "Skip current music."},
	{"musicvolsync", "Music", "musicvolsync()", "Wait for music volume transition."},
	{"choicemusic", "Music", "choicemusic()", "Set choice/menu music."},
	{"initsound", "Music", "initsound()", "Initialize sound system."},
	{"loadsync", "Music", "loadsync()", "Wait for sound load to complete."},

	// -- Additional Party --
	{"changeparty", "Party", "changeparty()", "Open party change screen."},
	{"refreshparty", "Party", "refreshparty()", "Refresh party display."},
	{"setparty2", "Party", "setparty2(member1, member2, member3)", "Set party variant 2."},

	// -- Additional Background --
	{"bganimeflag", "Background", "bganimeflag(flag)", "Set background animation flag."},
	{"bgshadestop", "Background", "bgshadestop()", "Stop background shading."},
	{"rbgshadeloop", "Background", "rbgshadeloop()", "Loop background shading."},
	{"bgshadeoff", "Background", "bgshadeoff()", "Turn off background shade."},
	{"bgclear", "Background", "bgclear()", "Clear background."},

	// -- Additional Entity --
	{"mesforcus", "Entity", "mesforcus()", "Force message cursor to this entity."},
	{"shadetimer", "Entity", "shadetimer()", "Set shade timer."},
	{"setroottrans", "Entity", "setroottrans()", "Set root transform."},
	{"pcopyinfo", "Entity", "pcopyinfo()", "Copy party member entity info."},
	{"axis", "Entity", "axis()", "Set entity axis."},
	{"axissync", "Entity", "axissync()", "Wait for axis operation."},

	// -- Lines/Triggers --
	{"doorlineoff", "Lines", "doorlineoff()", "Disable door line trigger."},
	{"doorlineon", "Lines", "doorlineon()", "Enable door line trigger."},

	// -- UI Bars --
	{"setbar", "Menus", "setbar()", "Set progress/gauge bar."},
	{"dispbar", "Menus", "dispbar()", "Display gauge bar."},
	{"killbar", "Menus", "killbar()", "Remove gauge bar."},

	// -- Player Input --
	{"key", "Player Control", "key(keyMask)", "Key input check."},
	{"keysighnchange", "Player Control", "keysighnchange()", "Key sign change. Dummied out."},

	// -- Additional Game State --
	{"sethp", "Game State", "sethp(characterID, hp)", "Set character HP."},
	{"gethp", "Game State", "gethp(characterID)", "Get character HP. Result in temp_0."},
	{"rest", "Game State", "rest()", "Rest/inn — restore party."},
	{"addpastgil", "Game State", "addpastgil(amount)", "Add gil in Laguna's time period."},
	{"broken", "Game State", "broken()", "Broken/unused opcode."},

	// -- Map Transitions --
	{"premapjump2", "Map Transitions", "premapjump2(fieldMapID)", "Prepare map jump variant 2."},

	// -- Unknown --
	{"unknown2", "Unknown", "unknown2()", "Unknown opcode."},
	{"unknown3", "Unknown", "unknown3()", "Unknown opcode."},
	{"unknown4", "Unknown", "unknown4()", "Used on Ragnarok hatch screen only."},
	{"unknown6", "Unknown", "unknown6()", "Possibly clockwise turn variant."},
	{"unknown7", "Unknown", "unknown7()", "Possibly counter-clockwise turn variant."},
	{"unknown8", "Unknown", "unknown8()", "Possibly clockwise turn variant 2."},
	{"unknown9", "Unknown", "unknown9()", "Possibly counter-clockwise turn variant 2."},
	{"unknown10", "Unknown", "unknown10()", "Possibly music sample time."},
	{"unknown11", "Unknown", "unknown11()", "Some kind of wait command."},
	{"unknown12", "Unknown", "unknown12()", "Ends unknown11."},
	{"unknown13", "Unknown", "unknown13()", "Possibly sound channel control."},
	{"unknown14", "Unknown", "unknown14()", "Preserve sound channel across field loads."},
	{"unknown15", "Unknown", "unknown15()", "Ladder area entry related."},
	{"unknown16", "Unknown", "unknown16()", "Possibly set draw point ID."},

	{nullptr, nullptr, nullptr, nullptr}
};

JsmHelpDialog::JsmHelpDialog(QWidget *parent)
    : QDialog(parent)
{
	setWindowTitle(tr("Script Reference"));
	resize(800, 600);

	_searchField = new QLineEdit(this);
	_searchField->setPlaceholderText(tr("Search opcodes..."));

	_categoryTree = new QTreeWidget(this);
	_categoryTree->setHeaderHidden(true);
	_categoryTree->setMaximumWidth(220);
	_categoryTree->setMinimumWidth(160);

	_detailView = new QTextBrowser(this);
	_detailView->setOpenExternalLinks(true);

	QHBoxLayout *mainLayout = new QHBoxLayout();
	QVBoxLayout *leftLayout = new QVBoxLayout();
	leftLayout->addWidget(_searchField);
	leftLayout->addWidget(_categoryTree);
	mainLayout->addLayout(leftLayout);
	mainLayout->addWidget(_detailView, 1);

	QVBoxLayout *outerLayout = new QVBoxLayout(this);
	outerLayout->addLayout(mainLayout);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
	outerLayout->addWidget(buttonBox);

	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	connect(_categoryTree, &QTreeWidget::itemSelectionChanged, this, &JsmHelpDialog::onCategorySelected);
	connect(_categoryTree, &QTreeWidget::itemDoubleClicked, this, &JsmHelpDialog::onItemDoubleClicked);
	connect(_searchField, &QLineEdit::textChanged, this, &JsmHelpDialog::onSearch);

	buildContent();
}

void JsmHelpDialog::buildContent()
{
	QMap<QString, QTreeWidgetItem *> categories;

	for (int i = 0; opcodeHelpData[i].name != nullptr; ++i) {
		const OpcodeHelp &h = opcodeHelpData[i];
		QString cat = QString::fromLatin1(h.category);

		if (!categories.contains(cat)) {
			QTreeWidgetItem *catItem = new QTreeWidgetItem(QStringList() << cat);
			catItem->setData(0, Qt::UserRole, -1);
			QFont f = catItem->font(0);
			f.setBold(true);
			catItem->setFont(0, f);
			_categoryTree->addTopLevelItem(catItem);
			categories[cat] = catItem;
		}

		QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << QString::fromLatin1(h.name));
		item->setData(0, Qt::UserRole, i);
		categories[cat]->addChild(item);
	}

	_categoryTree->expandAll();

	_detailView->setHtml(
	    "<h2>FF8 Field Script Reference</h2>"
	    "<p>Select an opcode from the list to see its documentation.</p>"
	    "<p>The code language supports:</p>"
	    "<ul>"
	    "<li><b>Control flow:</b> if/else/end, while/end, wait while, forever, repeat/until</li>"
	    "<li><b>Assignments:</b> var = expr, var += expr, etc.</li>"
	    "<li><b>Function calls:</b> opcodeName(arg1, arg2, ...)</li>"
	    "<li><b>Expressions:</b> arithmetic (+, -, *, /, %), comparison (==, !=, &gt;, &lt;), "
	    "bitwise (&amp;, |, ^, ~, &gt;&gt;, &lt;&lt;), logical (and, or, !)</li>"
	    "<li><b>Variables:</b> N_ubyte, N_uword, N_ulong, N_sbyte, N_sword, N_slong, temp_N, model_N</li>"
	    "<li><b>Constants:</b> text_N, map_N, item_N, magic_N, character names, key names</li>"
	    "</ul>"
	    "<p>Source: <a href='https://hobbitdur.github.io/FF8ModdingWiki/technical-reference/field/field-opcodes/'>"
	    "FF8 Modding Wiki</a></p>"
	);
}

void JsmHelpDialog::onCategorySelected()
{
	QList<QTreeWidgetItem *> items = _categoryTree->selectedItems();
	if (items.isEmpty()) return;

	int idx = items.first()->data(0, Qt::UserRole).toInt();
	if (idx < 0) {
		QTreeWidgetItem *catItem = items.first();
		QString html = QString("<h2>%1</h2>").arg(catItem->text(0).toHtmlEscaped());
		for (int c = 0; c < catItem->childCount(); ++c) {
			int childIdx = catItem->child(c)->data(0, Qt::UserRole).toInt();
			if (childIdx >= 0) {
				const OpcodeHelp &h = opcodeHelpData[childIdx];
				html += QString("<h3>%1</h3><pre>%2</pre><p>%3</p><hr>")
				        .arg(QString::fromLatin1(h.name).toHtmlEscaped(),
				             QString::fromLatin1(h.signature).toHtmlEscaped(),
				             QString::fromLatin1(h.description).toHtmlEscaped());
			}
		}
		_detailView->setHtml(html);
		return;
	}

	const OpcodeHelp &h = opcodeHelpData[idx];
	_detailView->setHtml(
	    QString("<h2>%1</h2>"
	            "<p><b>Category:</b> %2</p>"
	            "<h3>Signature</h3><pre>%3</pre>"
	            "<h3>Description</h3><p>%4</p>")
	    .arg(QString::fromLatin1(h.name).toHtmlEscaped(),
	         QString::fromLatin1(h.category).toHtmlEscaped(),
	         QString::fromLatin1(h.signature).toHtmlEscaped(),
	         QString::fromLatin1(h.description).toHtmlEscaped())
	);
}

void JsmHelpDialog::onSearch(const QString &text)
{
	QString lower = text.toLower();
	for (int i = 0; i < _categoryTree->topLevelItemCount(); ++i) {
		QTreeWidgetItem *cat = _categoryTree->topLevelItem(i);
		bool anyVisible = false;
		for (int c = 0; c < cat->childCount(); ++c) {
			QTreeWidgetItem *child = cat->child(c);
			bool match = lower.isEmpty() || child->text(0).toLower().contains(lower);
			child->setHidden(!match);
			if (match) anyVisible = true;
		}
		cat->setHidden(!anyVisible && !lower.isEmpty());
		if (anyVisible) cat->setExpanded(true);
	}
}

void JsmHelpDialog::onItemDoubleClicked(QTreeWidgetItem *item, int column)
{
	Q_UNUSED(column)
	int idx = item->data(0, Qt::UserRole).toInt();
	if (idx < 0) return; // Category header, ignore

	QString sig = QString::fromLatin1(opcodeHelpData[idx].signature);
	emit insertSignature(sig);
}
