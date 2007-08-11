/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 *
 */

#include "common/stdafx.h"
#include "common/system.h"

#include "parallaction/parallaction.h"
#include "parallaction/sound.h"

namespace Parallaction {

int Parallaction_br::init() {

	// Detect game
	if (!detectGame()) {
		GUIErrorMessage("No valid games were found in the specified directory.");
		return -1;
	}

	_screenWidth = 640;
	_screenHeight = 400;

	if (getGameType() == GType_BRA) {
		if (getPlatform() == Common::kPlatformPC) {
			_disk = new DosDisk_br(this);
		} else
			error("unsupported platform for Big Red Adventure");
	} else
		error("unknown game type");

	_soundMan = new DummySoundMan(this);

	initResources();
	initFonts();

	Parallaction::init();

	return 0;
}

Parallaction_br::~Parallaction_br() {
	freeFonts();
}

void Parallaction_br::callFunction(uint index, void* parm) {
	assert(index < 6);	// magic value 6 is maximum # of callables for Big Red Adventure

	(this->*_callables[index])(parm);
}

int Parallaction_br::go() {

	initGame();

	// TODO: game loop :P

	showMenu();

	return 0;
}

void Parallaction_br::splash(const char *name) {

	BackgroundInfo *info;

	_gfx->clearScreen(Gfx::kBitFront);

	info = _disk->loadSlide(name);
	_gfx->setPalette(info->palette);
	_gfx->flatBlitCnv(&info->bg, (640 - info->width) >> 1, (400 - info->height) >> 1, Gfx::kBitFront);
	_gfx->updateScreen();
	_system->delayMillis(600);

	Palette pal;
	for (uint i = 0; i < 64; i++) {
		info->palette.fadeTo(pal, 1);
		_gfx->setPalette(info->palette);
		_gfx->updateScreen();
		_system->delayMillis(20);
	}
	info->bg.free();

	delete info;

	return;
}

#define MENUITEMS_X			250
#define MENUITEMS_Y			200

#define MENUITEM_WIDTH		190
#define MENUITEM_HEIGHT		18

void Parallaction_br::renderMenuItem(Graphics::Surface &surf, const char *text) {
	surf.create(MENUITEM_WIDTH, MENUITEM_HEIGHT, 1);
	_menuFont->setColor(0);
	_menuFont->drawString((byte*)surf.getBasePtr(5, 2), MENUITEM_WIDTH, text);
}

void Parallaction_br::invertMenuItem(Graphics::Surface &surf) {
	for (int i = 0; i < surf.w * surf.h; i++)
		((byte*)surf.pixels)[i] ^= 0xD;
}

int Parallaction_br::showMenu() {
	// TODO: filter menu entries according to progress in game

	_gfx->clearScreen(Gfx::kBitFront);

	BackgroundInfo *info;

	Graphics::Surface	_menuItems[7];

	char *menuStrings[7] = {
		"SEE INTRO",
		"NEW GAME",
		"SAVED GAME",
		"EXIT TO DOS",
		"PART 2",
		"PART 3",
		"PART 4"
	};

	info = _disk->loadSlide("tbra");
	_gfx->setPalette(info->palette);
	_gfx->flatBlitCnv(&info->bg, 20, 50, Gfx::kBitFront);

	for (uint i = 0; i < 7; i++)
		renderMenuItem(_menuItems[i], menuStrings[i]);

	int selectedItem = -1, oldSelectedItem = -2;

	_system->showMouse(true);

	while (_mouseButtons != kMouseLeftUp) {

		updateInput();

		if ((_mousePos.x > MENUITEMS_X) && (_mousePos.x < (MENUITEMS_X+MENUITEM_WIDTH)) && (_mousePos.y > MENUITEMS_Y)) {
			selectedItem = (_mousePos.y - MENUITEMS_Y) / MENUITEM_HEIGHT;
		} else
			selectedItem = -1;

		if (selectedItem != oldSelectedItem) {

			if (selectedItem >= 0 && selectedItem < 7)
				invertMenuItem(_menuItems[selectedItem]);

			if (oldSelectedItem >= 0 && oldSelectedItem < 7)
				invertMenuItem(_menuItems[oldSelectedItem]);

			Common::Rect r(MENUITEM_WIDTH, MENUITEM_HEIGHT);

			for (uint i = 0; i < 7; i++) {
				r.moveTo(MENUITEMS_X, MENUITEMS_Y + i * 20);
				_gfx->copyRect(Gfx::kBitFront, r, (byte*)_menuItems[i].pixels, _menuItems[i].pitch);
			}

			oldSelectedItem = selectedItem;
		}

		_gfx->updateScreen();
		_system->delayMillis(20);
	}

	_system->showMouse(false);

	info->bg.free();
	delete info;

	for (uint i = 0; i < 7; i++)
		_menuItems[i].free();

	return selectedItem;
}

void Parallaction_br::initGame() {

	splash("dyna");
	splash("core");

	return;
}



void Parallaction_br::initFonts() {

	// TODO: find out which font is used for labels

	_menuFont = _disk->loadFont("russia");
	_dialogueFont = _disk->loadFont("comic");

}

void Parallaction_br::freeFonts() {

	delete _menuFont;
	delete _dialogueFont;

	return;
}

} // namespace Parallaction
