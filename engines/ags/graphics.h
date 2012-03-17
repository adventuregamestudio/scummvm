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
 */

/* Based on the Adventure Game Studio source code, copyright 1999-2011 Chris Jones,
 * which is licensed under the Artistic License 2.0.
 */

#ifndef AGS_GRAPHICS_H
#define AGS_GRAPHICS_H

#include "graphics/surface.h"

namespace Common {
class SeekableReadStream;
}

namespace Graphics {
class Font;
}

namespace AGS {

class Drawable;
class Room;

class AGSGraphics {
public:
	AGSGraphics(class AGSEngine *vm);
	~AGSGraphics();

	bool getScreenSize();
	bool initGraphics();
	Graphics::PixelFormat getPixelFormat() const;

	uint32 resolveHardcodedColor(uint32 color) const;
	uint32 getTransparentColor() const;

	void loadFonts();
	Graphics::Font *getFont(uint id);

	void initPalette();
	void newRoomPalette();

	void draw();

	void setMouseCursor(uint32 cursor);
	void mouseSetHotspot(uint32 x, uint32 y);
	void setCursorGraphic(uint32 spriteId);

	uint32 getCurrentCursor();

	uint16 _width, _height;
	uint16 _baseWidth, _baseHeight;
	uint32 _screenResolutionMultiplier;
	uint16 _textMultiply;
	bool _forceLetterbox;

protected:
	AGSEngine *_vm;

	byte _palette[256 * 3];
	Graphics::Surface _backBuffer;

	Common::Array<Graphics::Font *> _fonts;

	void draw(Drawable *item);

	class CursorDrawable *_cursorObj;

	void updateCachedMouseCursor();
};

} // End of namespace AGS

#endif // AGS_GRAPHICS_H