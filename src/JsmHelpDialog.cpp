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
	// -- Pseudo-code syntax --
	{"if / else / end", "Syntax", "if <condition> begin ... else ... end",
	 "Conditional execution. The else block is optional. Conditions use standard operators: ==, !=, >, <, >=, <=, and, or."},
	{"while / end", "Syntax", "while <condition> begin ... end",
	 "Loop while condition is true. The body executes repeatedly until the condition becomes false."},
	{"wait while", "Syntax", "wait while <condition>",
	 "Pause script execution until the condition becomes false. Equivalent to an empty-body while loop."},
	{"wait forever", "Syntax", "wait forever",
	 "Pause script execution indefinitely. The script will never continue past this point."},
	{"forever / end", "Syntax", "forever begin ... end",
	 "Infinite loop. The body executes forever. Use with caution."},
	{"repeat / until", "Syntax", "repeat ... until <condition> end",
	 "Execute the body at least once, then repeat until the condition becomes true."},
	{"ret", "Syntax", "ret",
	 "Return from the current script. If not called from another script, the entity halts."},
	{"Assignment", "Syntax", "variable = expression",
	 "Assign a value to a variable. Compound assignments are supported: +=, -=, *=, /=, %=, &=, |=, ^=, >>=, <<=."},
	{"Variables", "Syntax", "N_ubyte, N_uword, N_ulong, N_sbyte, N_sword, N_slong",
	 "Memory variables. N is the address. Type suffix determines read width: ubyte=1 byte unsigned, uword=2 bytes unsigned, ulong=4 bytes unsigned, sbyte/sword/slong=signed variants. Example: 1024_uword"},
	{"Temp Variables", "Syntax", "temp_N",
	 "Temporary variables (indices 0-7). Used for passing values between opcodes. Many opcodes store results in temp_0."},
	{"Model Reference", "Syntax", "model_N",
	 "Reference to an entity/model by index. Used with PSHAC for entity targeting."},
	{"Constants", "Syntax", "text_N, map_N, item_N, magic_N, KeyCancel, Squall, etc.",
	 "Named constants that resolve to numeric values. Character names (Squall=0, Zell=1, etc.), key names (KeyCancel=0x10, KeyMenu=0x20, etc.), and resource references."},

	// -- Flow Control --
	{"halt", "Flow Control", "halt()",
	 "Exit the current script AND all scripts waiting on it. Use ret to end only the current script."},
	{"wait", "Flow Control", "wait(frames)",
	 "Pause this script for N frames. 30 frames = 1 second at normal speed."},

	// -- Positioning --
	{"set", "Positioning", "set(walkmeshTriangleID, x, y)",
	 "Place this entity at X, Y on a walkmesh triangle. Z is interpolated from the walkmesh geometry."},
	{"set3", "Positioning", "set3(walkmeshTriangleID, x, y, z)",
	 "Place this entity at X, Y, Z on a walkmesh triangle with explicit Z coordinate."},
	{"idlock", "Positioning", "idlock(walkmeshTriangleID)",
	 "Lock a walkmesh triangle so nothing can walk over it."},
	{"idunlock", "Positioning", "idunlock(walkmeshTriangleID)",
	 "Unlock a walkmesh triangle so things can walk over it again."},
	{"dir", "Positioning", "dir(angle)",
	 "Make this entity face a given angle. FF8 uses 256-degree circles: 0=down, 64=right, 128=up, 192=left."},
	{"dira", "Positioning", "dira(actorID)",
	 "Make this entity rotate to face the given actor."},
	{"dirp", "Positioning", "dirp(partyMemberID, unknown, speed)",
	 "Make this entity rotate to face the given party member."},
	{"getinfo", "Positioning", "getinfo()",
	 "Push entity's X into temp_0, Y into temp_1. May also push Z and facing direction."},
	{"pgetinfo", "Positioning", "pgetinfo()",
	 "Push a party member's X into temp_0, Y into temp_1."},

	// -- Movement --
	{"mspeed", "Movement", "mspeed(speed)",
	 "Set this entity's movement speed."},
	{"move", "Movement", "move(walkmeshID, x, y, unknown)",
	 "Make this entity walk/run to given X, Y coordinates on a walkmesh triangle."},
	{"movea", "Movement", "movea(actorID, unknown)",
	 "Make this entity move towards the given actor."},
	{"pmovea", "Movement", "pmovea(partyMemberID, unknown)",
	 "Move this entity towards the party member with the given ID (0, 1, or 2)."},
	{"cmove", "Movement", "cmove(walkmeshID, x, y, unknown)",
	 "Move without playing walk animation or turning. Retains current facing and animation."},
	{"fmove", "Movement", "fmove(walkmeshID, x, y, unknown)",
	 "Move without walk animation, but entity will face the direction of movement."},
	{"movesync", "Movement", "movesync()",
	 "Pause script execution until this entity has finished its MOVE command."},
	{"moveflush", "Movement", "moveflush()",
	 "Halt the current entity's movements immediately."},
	{"jump", "Movement", "jump(walkmeshID, x, y, speed)",
	 "Jump entity from current position to X, Y with speed on a walkmesh triangle."},
	{"jump3", "Movement", "jump3(walkmeshID, x, y, z, speed)",
	 "Jump entity to X, Y, Z with speed on a walkmesh triangle."},
	{"pjumpa", "Movement", "pjumpa(partyMemberID, unknown)",
	 "Jump this entity to the given party member."},

	// -- Model / Animation --
	{"setmodel", "Animation", "setmodel(modelID)",
	 "Set this entity's display model."},
	{"setpc", "Animation", "setpc(characterID)",
	 "Set which party member this entity represents. 0=Squall, 1=Zell, etc."},
	{"baseanime", "Animation", "baseanime(animID, firstFrame, lastFrame)",
	 "Set the base (idle) looping animation frames for this entity."},
	{"anime", "Animation", "anime(animID)",
	 "Play an animation, return to base animation. Pauses script until done."},
	{"animekeep", "Animation", "animekeep(animID)",
	 "Play an animation, freeze on last frame. Pauses script until done."},
	{"canime", "Animation", "canime(animID, firstFrame, lastFrame)",
	 "Play a range of frames, return to base animation. Pauses script."},
	{"canimekeep", "Animation", "canimekeep(animID, firstFrame, lastFrame)",
	 "Play a range of frames, freeze on last frame. Pauses script."},
	{"ranime", "Animation", "ranime(animID)",
	 "Play an animation, return to base. Does NOT pause script."},
	{"ranimekeep", "Animation", "ranimekeep(animID)",
	 "Play an animation, freeze on last frame. Does NOT pause script."},
	{"animesync", "Animation", "animesync()",
	 "Pause script until the entity's current animation finishes."},
	{"animestop", "Animation", "animestop()",
	 "Return this entity to its base animation immediately."},
	{"animespeed", "Animation", "animespeed(speed)",
	 "Set animation playback speed. Frame speed of 1 = 2 frames per second."},

	// -- Messages / Dialog --
	{"amesw", "Messages", "amesw(channel, textID, x, y)",
	 "Pop up a message window with position, wait for player to dismiss."},
	{"ames", "Messages", "ames(channel, textID, x, y)",
	 "Pop up a message window with position. Stays until WINCLOSE or MESSYNC."},
	{"aask", "Messages", "aask(channel, textID, firstChoice, lastChoice, defaultChoice, unknown, x, y)",
	 "Message window with selectable options. Saves chosen line index to temp_0."},
	{"mes", "Messages", "mes(channel, textID)",
	 "Pop up a message window. Size set with WINSIZE."},
	{"messync", "Messages", "messync(channel)",
	 "Wait for a message window to be closed."},
	{"winclose", "Messages", "winclose(channel)",
	 "Close a message window."},
	{"winsize", "Messages", "winsize(channel, x, y, width, height)",
	 "Set message window position and size for a given channel."},
	{"mesmode", "Messages", "mesmode(mode)",
	 "Set message mode: 0=opaque, 1=translucent, 2=no window."},
	{"ramesw", "Messages", "ramesw(channel, textID, x, y)",
	 "Pop up a message window and wait, but lets the script continue running."},
	{"setmesspeed", "Messages", "setmesspeed(speed, unknown)",
	 "Set message text display speed."},

	// -- Entity Control --
	{"show", "Entity", "show()",
	 "Show this entity's model on the field."},
	{"hide", "Entity", "hide()",
	 "Hide this entity's model on the field."},
	{"use", "Entity", "use()",
	 "Re-enable an entity. Opposite of UNUSE."},
	{"unuse", "Entity", "unuse(entityID)",
	 "Disable this entity's scripts, hide its model, make it passable."},
	{"throughon", "Entity", "throughon()",
	 "Make this entity passable (players walk through it)."},
	{"throughoff", "Entity", "throughoff()",
	 "Make this entity solid (players collide with it)."},
	{"talkon", "Entity", "talkon()",
	 "Enable talking to this entity."},
	{"talkoff", "Entity", "talkoff()",
	 "Disable talking to this entity."},
	{"pushon", "Entity", "pushon()",
	 "Enable push script for this entity."},
	{"pushoff", "Entity", "pushoff()",
	 "Disable push script for this entity."},
	{"talkradius", "Entity", "talkradius(radius)",
	 "Set how close the player must be to trigger talk. Most humanoids use ~200."},
	{"pushradius", "Entity", "pushradius(radius)",
	 "Set the distance for push script. Most events use 48 when active, 1 when inactive."},

	// -- Player Control --
	{"ucon", "Player Control", "ucon()",
	 "Enable player control."},
	{"ucoff", "Player Control", "ucoff()",
	 "Disable player control. Player can only press OK to advance dialogue."},
	{"keyscan", "Player Control", "keyscan(keyMask)",
	 "Check if specific keys are pressed. Result in temp_0. Use key constants: KeyCancel, KeyMenu, KeyChoose, etc."},

	// -- Map Transitions --
	{"mapjump", "Map Transitions", "mapjump(walkmeshID, fieldMapID, x, y, z)",
	 "Jump player to another field at X, Y, Z on a walkmesh triangle."},
	{"mapjump3", "Map Transitions", "mapjump3(walkmeshID, fieldMapID, x, y, z, angle)",
	 "Jump player to another field with facing angle."},
	{"mapjumpo", "Map Transitions", "mapjumpo(fieldMapID, unknown)",
	 "Jump player to a field on walkmesh triangle 0. For cutscene teleports."},
	{"mapjumpon", "Map Transitions", "mapjumpon()",
	 "Enable field exits (player can walk off-screen)."},
	{"mapjumpoff", "Map Transitions", "mapjumpoff()",
	 "Disable field exits."},
	{"worldmapjump", "Map Transitions", "worldmapjump()",
	 "Jump player to the world map."},
	{"discjump", "Map Transitions", "discjump(walkmeshID, fieldMapID, x, y, z, angle)",
	 "Same as MAPJUMP3 but also initiates a disc switch."},

	// -- Battle --
	{"battle", "Battle", "battle(encounterID)",
	 "Start a battle. Flags: +2=no fanfare, +4=inherit timer, +16=current music, +32=preemptive, +64=back attack."},
	{"battleresult", "Battle", "battleresult()",
	 "Push the result of the last battle into temp_0."},
	{"battleon", "Battle", "battleon()",
	 "Enable random battles."},
	{"battleoff", "Battle", "battleoff()",
	 "Disable random battles."},

	// -- Party Management --
	{"addparty", "Party", "addparty(characterID)",
	 "Add a character to the active party. 0=Squall, 1=Zell, etc."},
	{"subparty", "Party", "subparty(characterID)",
	 "Remove a character from the active party."},
	{"setparty", "Party", "setparty(member1, member2, member3)",
	 "Set the full active party. Use 255 for blank slots."},
	{"isparty", "Party", "isparty(characterID)",
	 "Push the character's position in the party to temp_0. -1 if not in party."},
	{"addmember", "Party", "addmember(characterID)",
	 "Add a PC to the available roster."},
	{"submember", "Party", "submember(characterID)",
	 "Remove a PC from the available roster and active party."},
	{"getparty", "Party", "getparty(slotIndex)",
	 "Push the ID of the character in the given party slot to temp_0."},
	{"join", "Party", "join()",
	 "Start the conga line — make party members follow the leader."},
	{"split", "Party", "split()",
	 "Disable the conga line and send each member to a specific spot."},

	// -- Items / Magic --
	{"additem", "Items", "additem(quantity, itemID)",
	 "Add an item to the party's inventory."},
	{"hasitem", "Items", "hasitem(itemID)",
	 "Push 1 to temp_0 if the party has the item, 0 otherwise."},
	{"addmagic", "Items", "addmagic(characterID, magicID)",
	 "Add magic spells to a character."},
	{"drawpoint", "Items", "drawpoint(drawPointID)",
	 "Initialize a draw point with magic from a hardcoded array."},
	{"setdrawpoint", "Items", "setdrawpoint(isHidden)",
	 "If isHidden >= 1, hide the draw point. Otherwise, make it visible."},

	// -- Music / Sound --
	{"musicload", "Music", "musicload(trackID)",
	 "Preload a field music track."},
	{"musicchange", "Music", "musicchange()",
	 "Stop current music and start the preloaded track."},
	{"musicstop", "Music", "musicstop()",
	 "Stop the music."},
	{"musicvol", "Music", "musicvol(volume)",
	 "Set the music volume."},
	{"effectplay2", "Music", "effectplay2(soundID, pan, volume)",
	 "Play a field sound effect. Pan: 0=left, 255=right. Volume: 0-127."},
	{"effectplay", "Music", "effectplay(channel)",
	 "Play a sound effect through a channel."},
	{"sestop", "Music", "sestop(channel)",
	 "Stop the sound effect on a given channel."},

	// -- Visual Effects --
	{"fadeblack", "Visual", "fadeblack()",
	 "Fade the screen to black."},
	{"fadein", "Visual", "fadein()",
	 "Fade the screen in from black."},
	{"fadeout", "Visual", "fadeout()",
	 "Fade the screen out to black."},
	{"fadesync", "Visual", "fadesync()",
	 "Wait for fade to complete."},

	// -- Camera --
	{"dscroll", "Camera", "dscroll()",
	 "Scroll the screen."},
	{"lscroll", "Camera", "lscroll(x, y, frameCount)",
	 "Scroll the screen by X and Y pixels over a frame count."},
	{"cscroll", "Camera", "cscroll(x, y, frameCount)",
	 "Scroll the screen by X and Y pixels over a frame count."},
	{"dscrolla", "Camera", "dscrolla(actorID)",
	 "Scroll the screen onto the given actor."},
	{"scrollsync", "Camera", "scrollsync()",
	 "Pause script until the last scroll command finishes."},

	// -- Menus --
	{"menunormal", "Menus", "menunormal()",
	 "Open the normal menu."},
	{"menuphs", "Menus", "menuphs()",
	 "Open the PHS (party switch) menu."},
	{"menushop", "Menus", "menushop(shopID)",
	 "Open a shop menu."},
	{"menusave", "Menus", "menusave()",
	 "Open the save menu."},
	{"menuname", "Menus", "menuname(characterOrGfID)",
	 "Open the name entry screen. For GFs, this also unlocks the GF."},

	// -- Game State --
	{"gameover", "Game State", "gameover()",
	 "End the game in failure."},
	{"rnd", "Game State", "rnd()",
	 "Push a random number (0-255) into temp_0."},
	{"setplace", "Game State", "setplace(locationID)",
	 "Set the location name shown in the menu and save slots."},
	{"saveenable", "Game State", "saveenable(enabled)",
	 "Enable or disable saving. 1=enable, 0=disable."},
	{"runenable", "Game State", "runenable()",
	 "Enable running."},
	{"rundisable", "Game State", "rundisable()",
	 "Disable running."},

	// -- Movies --
	{"movieready", "Movies", "movieready(movieID)",
	 "Prepare/load a movie for playback."},
	{"movie", "Movies", "movie()",
	 "Play a movie previously prepared by movieready."},
	{"moviesync", "Movies", "moviesync()",
	 "Pause script until the current FMV finishes."},

	// -- Script Execution --
	{"req", "Script Execution", "req(entityGroupID, methodLabel, priority)",
	 "Request remote execution: jump to method in another entity. Non-blocking, may fail if entity is busy."},
	{"reqsw", "Script Execution", "reqsw(entityGroupID, methodLabel, priority)",
	 "Request remote execution, async, guaranteed to execute."},
	{"reqew", "Script Execution", "reqew(entityGroupID, methodLabel, priority)",
	 "Request remote execution, sync (waits for completion), guaranteed."},

	// -- Turning --
	{"cturnr", "Turning", "cturnr(angle, speed)",
	 "Turn this entity clockwise. 256-degree system."},
	{"cturnl", "Turning", "cturnl(angle, speed)",
	 "Turn this entity counter-clockwise."},
	{"cturn", "Turning", "cturn()",
	 "Make this entity face the entity set with PSHAC."},
	{"pcturn", "Turning", "pcturn(partyMemberID, speed)",
	 "Make this entity face a PC. Speed = number of frames."},

	// -- Line / Trigger --
	{"setline", "Lines", "setline(x1, y1, z1, x2, y2, z2)",
	 "Set the 3D hitbox bounds of a line object."},
	{"lineon", "Lines", "lineon()",
	 "Enable line collisions with this entity."},
	{"lineoff", "Lines", "lineoff()",
	 "Disable line collisions with this entity."},

	// -- Timers --
	{"settimer", "Timers", "settimer(value)",
	 "Set the countdown timer value."},
	{"disptimer", "Timers", "disptimer()",
	 "Display the countdown timer and start counting down."},

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

	QPushButton *closeBtn = new QPushButton(tr("Close"), this);
	QHBoxLayout *btnLayout = new QHBoxLayout();
	btnLayout->addStretch();
	btnLayout->addWidget(closeBtn);
	outerLayout->addLayout(btnLayout);

	connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
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

	// Show intro text
	_detailView->setHtml(
	    "<h2>FF8 Field Script Reference</h2>"
	    "<p>Select an opcode from the list to see its documentation.</p>"
	    "<p>The pseudo-code language supports:</p>"
	    "<ul>"
	    "<li><b>Control flow:</b> if/else/end, while/end, wait while, forever, repeat/until</li>"
	    "<li><b>Assignments:</b> var = expr, var += expr, etc.</li>"
	    "<li><b>Function calls:</b> opcodeName(arg1, arg2, ...)</li>"
	    "<li><b>Expressions:</b> arithmetic (+, -, *, /, %), comparison (==, !=, >, <), "
	    "bitwise (&, |, ^, ~, >>, <<), logical (and, or, !)</li>"
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
		// Category header — show all children
		QTreeWidgetItem *catItem = items.first();
		QString html = QString("<h2>%1</h2>").arg(catItem->text(0));
		for (int c = 0; c < catItem->childCount(); ++c) {
			int childIdx = catItem->child(c)->data(0, Qt::UserRole).toInt();
			if (childIdx >= 0) {
				const OpcodeHelp &h = opcodeHelpData[childIdx];
				html += QString("<h3>%1</h3><pre>%2</pre><p>%3</p><hr>")
				        .arg(QString::fromLatin1(h.name),
				             QString::fromLatin1(h.signature),
				             QString::fromLatin1(h.description));
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
	    .arg(QString::fromLatin1(h.name),
	         QString::fromLatin1(h.category),
	         QString::fromLatin1(h.signature),
	         QString::fromLatin1(h.description))
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
