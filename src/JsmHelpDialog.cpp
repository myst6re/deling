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
	 "Named constants: character names (Squall=0..Ward=10), key names, resource refs."},

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

	// -- Turning --
	{"cturnr", "Turning", "cturnr(angle, speed)", "Turn clockwise. 256-degree system."},
	{"cturnl", "Turning", "cturnl(angle, speed)", "Turn counter-clockwise."},
	{"cturn", "Turning", "cturn()", "Face entity set with PSHAC."},
	{"pcturn", "Turning", "pcturn(partyMemberID, speed)", "Face a PC. Speed = frame count."},

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
	{"scrollsync", "Camera", "scrollsync()", "Wait for scroll to finish."},
	{"dscroll2", "Camera", "dscroll2()", "Scroll variant 2."},
	{"lscroll2", "Camera", "lscroll2(x, y, frameCount)", "Scroll variant 2."},
	{"cscroll2", "Camera", "cscroll2(x, y, frameCount)", "Scroll variant 2."},
	{"scrollsync2", "Camera", "scrollsync2()", "Wait for scroll2 to finish."},

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
