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

#include "director/dib.h"

#include "common/stream.h"
#include "common/substream.h"
#include "common/textconsole.h"
#include "graphics/pixelformat.h"
#include "graphics/surface.h"
#include "image/codecs/codec.h"
#include "common/util.h"
#include "common/debug.h"

namespace Director {

DIBDecoder::DIBDecoder() {
	_surface = 0;
	_palette = 0;
	_paletteColorCount = 0;
	_codec = 0;
}

DIBDecoder::~DIBDecoder() {
	destroy();
}

void DIBDecoder::destroy() {
	_surface = 0;

	delete[] _palette;
	_palette = 0;
	_paletteColorCount = 0;

	delete _codec;
	_codec = 0;
}

void DIBDecoder::loadPalette(Common::SeekableReadStream &stream) {
	_palette = new byte[1024];

	uint16 steps = stream.size()/6;
	uint16 index = (steps * 4) - 1;

	for (uint8 i = 0; i < steps; i++) {
		_palette[index--] = 0;

		_palette[index--] = stream.readByte();
		stream.readByte();

		_palette[index--] = stream.readByte();
		stream.readByte();

		_palette[index--] = stream.readByte();
		stream.readByte();
	}

	index = (steps * 4) - 1;

	while (index < 1024) {
		_palette[index++] = 0;
	}
}	

bool DIBDecoder::loadStream(Common::SeekableReadStream &stream) {
	destroy();
	if (stream.readByte() != 40)
		return false;
	if (stream.readByte() != 0)
		return false;

	stream.seek(4);
	uint16 width = stream.readUint32LE();
	uint16 height = stream.readUint32LE();
	_paletteColorCount = (stream.readUint32LE() + stream.readUint32LE()) << 8;
	_paletteColorCount = (_paletteColorCount == 0) ? 255: _paletteColorCount;
	uint16 totalsize = 14 + stream.size() + sizeof(_palette)/sizeof(byte);

	debug("%d", _paletteColorCount);
	debug("%d", width);
	debug("%d", height);
	debug("%d", totalsize);
	return true;
}

} // End of namespace Director
