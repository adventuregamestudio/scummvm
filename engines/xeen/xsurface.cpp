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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "common/algorithm.h"
#include "common/util.h"
#include "xeen/xsurface.h"
#include "xeen/resdata.h"

namespace Xeen {

const byte *XSurface::_fontData;

XSurface::XSurface() : Graphics::Surface(), _bgColor(DEFAULT_BG_COLOR), _fontReduced(false),
		_fontJustify(JUSTIFY_NONE), _msgWraps(false) {
	_textColors[0] = 0;
	_textColors[1] = 0x40;
	_textColors[2] = 0x30;
	_textColors[3] = 0x20;
}

XSurface::XSurface(int w, int h) : Graphics::Surface(), _bgColor(DEFAULT_BG_COLOR),
		_fontReduced(false), _fontJustify(JUSTIFY_NONE), _msgWraps(false) {
	create(w, h);
	_textColors[0] = 0;
	_textColors[1] = 0x40;
	_textColors[2] = 0x30;
	_textColors[3] = 0x20;
}

XSurface::~XSurface() {
	free();
}

void XSurface::create(uint16 w, uint16 h) {
	Graphics::Surface::create(w, h, Graphics::PixelFormat::createFormatCLUT8());
}

void XSurface::transBlitTo(XSurface &dest) const {
	transBlitTo(dest, Common::Point());
}

void XSurface::blitTo(XSurface &dest) const {
	blitTo(dest, Common::Point());
}

void XSurface::transBlitTo(XSurface &dest, const Common::Point &destPos) const {
	if (dest.getPixels() == nullptr)
		dest.create(w, h);

	for (int yp = 0; yp < h; ++yp) {
		const byte *srcP = (const byte *)getBasePtr(0, yp);
		byte *destP = (byte *)dest.getBasePtr(destPos.x, destPos.y + yp);

		for (int xp = 0; xp < w; ++xp, ++srcP, ++destP) {
			if (*srcP != 0)
				*destP = *srcP;
		}
	}

	dest.addDirtyRect(Common::Rect(destPos.x, destPos.y, destPos.x + w, destPos.y));
}

void XSurface::blitTo(XSurface &dest, const Common::Point &destPos) const {
	if (dest.getPixels() == nullptr)
		dest.create(w, h);

	for (int yp = 0; yp < h; ++yp) {
		const byte *srcP = (const byte *)getBasePtr(0, yp);
		byte *destP = (byte *)dest.getBasePtr(destPos.x, destPos.y + yp);

		Common::copy(srcP, srcP + w, destP);
	}

	dest.addDirtyRect(Common::Rect(destPos.x, destPos.y, destPos.x + w, destPos.y));
}

/**
 * Draws a symbol to the surface.
 * @param symbolId	Symbol number from 0 to 19
 */
void XSurface::writeSymbol(int symbolId) {
	const byte *srcP = &SYMBOLS[symbolId][0];

	for (int yp = 0; yp < FONT_HEIGHT; ++yp) {
		byte *destP = (byte *)getBasePtr(_writePos.x, _writePos.y + yp);

		for (int xp = 0; xp < FONT_WIDTH; ++xp, ++destP) {
			byte b = *srcP++;
			if (b)
				*destP = b;
		}
	}

	_writePos.x += 8;
}

/**
 * Write a string to the surface
 * @param s			String to display
 * @param bounds	Window bounds to display string within
 * @returns			Any string remainder that couldn't be displayed
 * @remarks		Note that bounds is just used for wrapping purposes. Unless
 *		justification is set, the message will be written at _writePos
 */
Common::String XSurface::writeString(const Common::String &s, const Common::Rect &bounds) {
	_displayString = s.c_str();

	for (;;) {
		_msgWraps = false;
		
		// Get the size of the string that can be displayed on the likne
		int xp = _fontJustify ? bounds.left : _writePos.x;
		while (!getNextCharWidth(xp)) {
			if (xp >= bounds.right) {
				--_displayString;
				_msgWraps = true;
			}
		}

		// Get the end point of the text that can be displayed
		const char *displayEnd = _displayString;
		_displayString = s.c_str();

		if (*displayEnd && _fontJustify != JUSTIFY_RIGHT && xp >= bounds.right) {
			// Need to handle justification of text
			// First, move backwards to find the end of the previous word
			// for a convenient point to break the line at
			const char *endP = displayEnd;
			while (endP > _displayString && (*endP & 0x7f) != ' ')
				--endP;

			if (endP == _displayString) {
				// There was no word breaks at all in the string
				--displayEnd;
				if (_fontJustify == JUSTIFY_NONE && _writePos.x != bounds.left) {
					// Move to the next line
					if (!newLine(bounds))
						continue;
					// Ran out of space to display string
					break;
				}
			} else {
				// Found word break, find end of previous word 
				while (displayEnd > _displayString && (*displayEnd & 0x7f) == ' ')
					--displayEnd;
			}
		}

		// Main character display loop
		while (_displayString <= displayEnd) {
			char c = getNextChar();
			
			if (c == ' ') {
				_writePos.x += _fontReduced ? 3 : 4;
			} else if (c == '\r') {
				fillRect(bounds, _bgColor);
				_writePos = Common::Point(bounds.left, bounds.top);
			} else if (c == 1) {
				// Turn off reduced font mode
				_fontReduced = false;
			} else if (c == 2) {
				// Turn on reduced font mode
				_fontReduced = true;
			} else if (c == 3) {
				// Justify text
				c = getNextChar();
				if (c == 'r')
					_fontJustify = JUSTIFY_RIGHT;
				else if (c == 'c')
					_fontJustify = JUSTIFY_CENTER;
				else
					_fontJustify = JUSTIFY_NONE;
			} else if (c == 4) {
				// Draw an empty box of a given width
				int w = fontAtoi();
				Common::Point pt = _writePos;
				if (_fontJustify == JUSTIFY_RIGHT)
					pt.x -= w;
				fillRect(Common::Rect(pt.x, pt.y, pt.x + w, pt.y + (_fontReduced ? 9 : 10)),
					_bgColor);
			} else if (c == 5) {
				continue;
			} else if (c == 6) {
				// Non-breakable space
				writeChar(' ');
			} else if (c == 7) {
				// Set text background color
				int c = fontAtoi();
				_bgColor = (c < 0 || c > 255) ? DEFAULT_BG_COLOR : c;
			} else if (c == 8) {
				// Draw a character outline
				c = getNextChar();
				if (c == ' ') {
					c = '\0';
					_writePos.x -= 3;
				} else {
					if (c == 6)
						c = ' ';
					byte charSize = _fontData[0x1000 + (int)c + (_fontReduced ? 0x80 : 0)];
					_writePos.x -= charSize;
				}

				if (_writePos.x < bounds.left)
					_writePos.x = bounds.left;

				if (c) {
					int oldX = _writePos.x;
					byte oldColor[4];
					Common::copy(&_textColors[0], &_textColors[4], &oldColor[0]);

					_textColors[1] = _textColors[2] = _textColors[3] = _bgColor;
					writeChar(c);

					Common::copy(&oldColor[0], &oldColor[4], &_textColors[0]);
					_writePos.x = oldX;
				}
			} else if (c == 9) {
				// Skip x position
				int xp = fontAtoi();
				_writePos.x = MIN(bounds.left + xp, (int)bounds.right);
			} else if (c == 10) {
				// Newline
				if (newLine(bounds))
					break;
			} else if (c == 11) {
				// Skip y position
				int yp = fontAtoi(	);
				_writePos.y = MIN(bounds.top + yp, (int)bounds.bottom);
			} else if (c == 12) {
				// Set text colors
				int idx = fontAtoi();
				if (idx < 0)
					idx = 0;
				setTextColor(idx);
			} else if (c < ' ') {
				// Invalid command
				displayEnd = nullptr;
				break;
			} else {
				// Standard character - write it out
				writeChar(c);
			}
		}

		if (_displayString > displayEnd && _fontJustify != JUSTIFY_RIGHT && _msgWraps
				&& newLine(bounds))
			break;
	}

	return Common::String(_displayString);
}

/**
 * Wrie a character to the surface
 */
void XSurface::writeChar(char c) {
	error("TODO");
}

/**
 * Return the next pending character to display
 */
char XSurface::getNextChar() {
	return  *_displayString++ & 0x7f;
}

/**
* Return the width of a given character
*/
bool XSurface::getNextCharWidth(int &total) {
	char c = getNextChar();

	if (c > ' ') {
		total += _fontData[0x1000 + (int)c + (_fontReduced ? 0x80 : 0)];
		return false;
	} else if (c == ' ') {
		total += 4;
		return false;
	} else if (c == 8) {
		c = getNextChar();
		if (c == ' ') {
			total -= 2;
			return false;
		} else {
			_displayString -= 2;
			return true;
		}
	} else if (c == 12) {
		c = getNextChar();
		if (c != 'd')
			getNextChar();
		return false;
	} else {
		--_displayString;
		return true;
	}
}

bool XSurface::newLine(const Common::Rect &bounds) {
	// Move past any spaces currently being pointed to
	while ((*_displayString & 0x7f) == ' ')
		++_displayString;

	_msgWraps = false;
	_writePos.x = bounds.left;
	
	int h = _fontReduced ? 9 : 10;
	_writePos.y += h;

	return ((_writePos.y + h - 1) > bounds.bottom);
}

int XSurface::fontAtoi(int len) {
	int total = 0;
	for (int i = 0; i < len; ++i) {
		char c = getNextChar();
		if (c == ' ')
			c = '0';

		int digit = c - '0';
		if (digit < 0 || digit > 9)
			return -1;

		total = total * 10 + digit;
	}

	return total;
}

void XSurface::setTextColor(int idx) {
	const byte *colP = &TEXT_COLORS[idx][0];
	Common::copy(colP, colP + 4, &_textColors[0]);
}

} // End of namespace Xeen
