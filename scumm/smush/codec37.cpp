/* ScummVM - Scumm Interpreter
 * Copyright (C) 2002-2003 The ScummVM project
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 *
 */

#include <stdafx.h>
#include "codec37.h"
#include "chunk.h"
#include "blitter.h"

#include "common/engine.h"

#include <assert.h>
#include <string.h>

bool Codec37Decoder::initSize(const Point &p, const Rect &r) {
	if(r.width() != getRect().width() && r.height() != getRect().height()) {
		if(
			(r.width() != 320 || r.height() != 200) &&
			(r.width() != 384 || r.height() != 242) &&
			(r.width() != 640 || r.height() != 480)
			)
			return false;
		Decoder::initSize(p, r);
		clean();
		int32 frame_size = getRect().width() * getRect().height();
		_deltaSize = frame_size * 3 + 0x13600;
		_deltaBuf = new byte[_deltaSize];
		memset(_deltaBuf, 0, _deltaSize);
		if(_deltaBuf == 0) error("unable to allocate decoder buffer");
		_deltaBufs[0] = _deltaBuf + 0x4D80;
		_deltaBufs[1] = _deltaBuf + 0xE880 + frame_size;
		_offsetTable = new int16[255];
		_curtable = 0;
		if(_offsetTable == 0)
			error("unable to allocate decoder offset table");
		_tableLastPitch = -1;
		_tableLastIndex = -1;
		return true; 
	}
	return false;
}

Codec37Decoder::Codec37Decoder() {
	_deltaSize = 0;
	_deltaBuf = 0;
	_deltaBufs[0] = 0;
	_deltaBufs[1] = 0;
	_curtable = 0;
	_offsetTable = 0;
	_tableLastPitch = -1;
	_tableLastIndex = -1;
	_prevSeqNb = 0;
}

void Codec37Decoder::clean() {
	if(_offsetTable) {
		delete []_offsetTable;
		_offsetTable = 0;
		_tableLastPitch = -1;
		_tableLastIndex = -1;
	}
	if(_deltaBuf) {
		delete []_deltaBuf;
		_deltaSize = 0;
		_deltaBuf = 0;
		_deltaBufs[0] = 0;
		_deltaBufs[1] = 0;
	}
}

Codec37Decoder::~Codec37Decoder() {
	clean();
}

void Codec37Decoder::maketable(int32 pitch, int32 index) {
	static const int8 maketable_bytes[] = {
    0,   0,   1,   0,   2,   0,   3,   0,   5,   0,
    8,   0,  13,   0,  21,   0,  -1,   0,  -2,   0,
   -3,   0,  -5,   0,  -8,   0, -13,   0, -17,   0,
  -21,   0,   0,   1,   1,   1,   2,   1,   3,   1,
    5,   1,   8,   1,  13,   1,  21,   1,  -1,   1,
   -2,   1,  -3,   1,  -5,   1,  -8,   1, -13,   1,
  -17,   1, -21,   1,   0,   2,   1,   2,   2,   2,
    3,   2,   5,   2,   8,   2,  13,   2,  21,   2,
   -1,   2,  -2,   2,  -3,   2,  -5,   2,  -8,   2,
  -13,   2, -17,   2, -21,   2,   0,   3,   1,   3,
    2,   3,   3,   3,   5,   3,   8,   3,  13,   3,
   21,   3,  -1,   3,  -2,   3,  -3,   3,  -5,   3,
   -8,   3, -13,   3, -17,   3, -21,   3,   0,   5,
    1,   5,   2,   5,   3,   5,   5,   5,   8,   5,
   13,   5,  21,   5,  -1,   5,  -2,   5,  -3,   5,
   -5,   5,  -8,   5, -13,   5, -17,   5, -21,   5,
    0,   8,   1,   8,   2,   8,   3,   8,   5,   8,
    8,   8,  13,   8,  21,   8,  -1,   8,  -2,   8,
   -3,   8,  -5,   8,  -8,   8, -13,   8, -17,   8,
  -21,   8,   0,  13,   1,  13,   2,  13,   3,  13,
    5,  13,   8,  13,  13,  13,  21,  13,  -1,  13,
   -2,  13,  -3,  13,  -5,  13,  -8,  13, -13,  13,
  -17,  13, -21,  13,   0,  21,   1,  21,   2,  21,
    3,  21,   5,  21,   8,  21,  13,  21,  21,  21,
   -1,  21,  -2,  21,  -3,  21,  -5,  21,  -8,  21,
  -13,  21, -17,  21, -21,  21,   0,  -1,   1,  -1,
    2,  -1,   3,  -1,   5,  -1,   8,  -1,  13,  -1,
   21,  -1,  -1,  -1,  -2,  -1,  -3,  -1,  -5,  -1,
   -8,  -1, -13,  -1, -17,  -1, -21,  -1,   0,  -2,
    1,  -2,   2,  -2,   3,  -2,   5,  -2,   8,  -2,
   13,  -2,  21,  -2,  -1,  -2,  -2,  -2,  -3,  -2,
   -5,  -2,  -8,  -2, -13,  -2, -17,  -2, -21,  -2,
    0,  -3,   1,  -3,   2,  -3,   3,  -3,   5,  -3,
    8,  -3,  13,  -3,  21,  -3,  -1,  -3,  -2,  -3,
   -3,  -3,  -5,  -3,  -8,  -3, -13,  -3, -17,  -3,
  -21,  -3,   0,  -5,   1,  -5,   2,  -5,   3,  -5,
    5,  -5,   8,  -5,  13,  -5,  21,  -5,  -1,  -5,
   -2,  -5,  -3,  -5,  -5,  -5,  -8,  -5, -13,  -5,
  -17,  -5, -21,  -5,   0,  -8,   1,  -8,   2,  -8,
    3,  -8,   5,  -8,   8,  -8,  13,  -8,  21,  -8,
   -1,  -8,  -2,  -8,  -3,  -8,  -5,  -8,  -8,  -8,
  -13,  -8, -17,  -8, -21,  -8,   0, -13,   1, -13,
    2, -13,   3, -13,   5, -13,   8, -13,  13, -13,
   21, -13,  -1, -13,  -2, -13,  -3, -13,  -5, -13,
   -8, -13, -13, -13, -17, -13, -21, -13,   0, -17,
    1, -17,   2, -17,   3, -17,   5, -17,   8, -17,
   13, -17,  21, -17,  -1, -17,  -2, -17,  -3, -17,
   -5, -17,  -8, -17, -13, -17, -17, -17, -21, -17,
    0, -21,   1, -21,   2, -21,   3, -21,   5, -21,
    8, -21,  13, -21,  21, -21,  -1, -21,  -2, -21,
   -3, -21,  -5, -21,  -8, -21, -13, -21, -17, -21,
    0,   0,  -8, -29,   8, -29, -18, -25,  17, -25,
    0, -23,  -6, -22,   6, -22, -13, -19,  12, -19,
    0, -18,  25, -18, -25, -17,  -5, -17,   5, -17,
  -10, -15,  10, -15,   0, -14,  -4, -13,   4, -13,
   19, -13, -19, -12,  -8, -11,  -2, -11,   0, -11,
    2, -11,   8, -11, -15, -10,  -4, -10,   4, -10,
   15, -10,  -6,  -9,  -1,  -9,   1,  -9,   6,  -9,
  -29,  -8, -11,  -8,  -8,  -8,  -3,  -8,   3,  -8,
    8,  -8,  11,  -8,  29,  -8,  -5,  -7,  -2,  -7,
    0,  -7,   2,  -7,   5,  -7, -22,  -6,  -9,  -6,
   -6,  -6,  -3,  -6,  -1,  -6,   1,  -6,   3,  -6,
    6,  -6,   9,  -6,  22,  -6, -17,  -5,  -7,  -5,
   -4,  -5,  -2,  -5,   0,  -5,   2,  -5,   4,  -5,
    7,  -5,  17,  -5, -13,  -4, -10,  -4,  -5,  -4,
   -3,  -4,  -1,  -4,   0,  -4,   1,  -4,   3,  -4,
    5,  -4,  10,  -4,  13,  -4,  -8,  -3,  -6,  -3,
   -4,  -3,  -3,  -3,  -2,  -3,  -1,  -3,   0,  -3,
    1,  -3,   2,  -3,   4,  -3,   6,  -3,   8,  -3,
  -11,  -2,  -7,  -2,  -5,  -2,  -3,  -2,  -2,  -2,
   -1,  -2,   0,  -2,   1,  -2,   2,  -2,   3,  -2,
    5,  -2,   7,  -2,  11,  -2,  -9,  -1,  -6,  -1,
   -4,  -1,  -3,  -1,  -2,  -1,  -1,  -1,   0,  -1,
    1,  -1,   2,  -1,   3,  -1,   4,  -1,   6,  -1,
    9,  -1, -31,   0, -23,   0, -18,   0, -14,   0,
  -11,   0,  -7,   0,  -5,   0,  -4,   0,  -3,   0,
   -2,   0,  -1,   0,   0, -31,   1,   0,   2,   0,
    3,   0,   4,   0,   5,   0,   7,   0,  11,   0,
   14,   0,  18,   0,  23,   0,  31,   0,  -9,   1,
   -6,   1,  -4,   1,  -3,   1,  -2,   1,  -1,   1,
    0,   1,   1,   1,   2,   1,   3,   1,   4,   1,
    6,   1,   9,   1, -11,   2,  -7,   2,  -5,   2,
   -3,   2,  -2,   2,  -1,   2,   0,   2,   1,   2,
    2,   2,   3,   2,   5,   2,   7,   2,  11,   2,
   -8,   3,  -6,   3,  -4,   3,  -2,   3,  -1,   3,
    0,   3,   1,   3,   2,   3,   3,   3,   4,   3,
    6,   3,   8,   3, -13,   4, -10,   4,  -5,   4,
   -3,   4,  -1,   4,   0,   4,   1,   4,   3,   4,
    5,   4,  10,   4,  13,   4, -17,   5,  -7,   5,
   -4,   5,  -2,   5,   0,   5,   2,   5,   4,   5,
    7,   5,  17,   5, -22,   6,  -9,   6,  -6,   6,
   -3,   6,  -1,   6,   1,   6,   3,   6,   6,   6,
    9,   6,  22,   6,  -5,   7,  -2,   7,   0,   7,
    2,   7,   5,   7, -29,   8, -11,   8,  -8,   8,
   -3,   8,   3,   8,   8,   8,  11,   8,  29,   8,
   -6,   9,  -1,   9,   1,   9,   6,   9, -15,  10,
   -4,  10,   4,  10,  15,  10,  -8,  11,  -2,  11,
    0,  11,   2,  11,   8,  11,  19,  12, -19,  13,
   -4,  13,   4,  13,   0,  14, -10,  15,  10,  15,
   -5,  17,   5,  17,  25,  17, -25,  18,   0,  18,
  -12,  19,  13,  19,  -6,  22,   6,  22,   0,  23,
  -17,  25,  18,  25,  -8,  29,   8,  29,   0,  31,
    0,   0,  -6, -22,   6, -22, -13, -19,  12, -19,
    0, -18,  -5, -17,   5, -17, -10, -15,  10, -15,
    0, -14,  -4, -13,   4, -13,  19, -13, -19, -12,
   -8, -11,  -2, -11,   0, -11,   2, -11,   8, -11,
  -15, -10,  -4, -10,   4, -10,  15, -10,  -6,  -9,
   -1,  -9,   1,  -9,   6,  -9, -11,  -8,  -8,  -8,
   -3,  -8,   0,  -8,   3,  -8,   8,  -8,  11,  -8,
   -5,  -7,  -2,  -7,   0,  -7,   2,  -7,   5,  -7,
  -22,  -6,  -9,  -6,  -6,  -6,  -3,  -6,  -1,  -6,
    1,  -6,   3,  -6,   6,  -6,   9,  -6,  22,  -6,
  -17,  -5,  -7,  -5,  -4,  -5,  -2,  -5,  -1,  -5,
    0,  -5,   1,  -5,   2,  -5,   4,  -5,   7,  -5,
   17,  -5, -13,  -4, -10,  -4,  -5,  -4,  -3,  -4,
   -2,  -4,  -1,  -4,   0,  -4,   1,  -4,   2,  -4,
    3,  -4,   5,  -4,  10,  -4,  13,  -4,  -8,  -3,
   -6,  -3,  -4,  -3,  -3,  -3,  -2,  -3,  -1,  -3,
    0,  -3,   1,  -3,   2,  -3,   3,  -3,   4,  -3,
    6,  -3,   8,  -3, -11,  -2,  -7,  -2,  -5,  -2,
   -4,  -2,  -3,  -2,  -2,  -2,  -1,  -2,   0,  -2,
    1,  -2,   2,  -2,   3,  -2,   4,  -2,   5,  -2,
    7,  -2,  11,  -2,  -9,  -1,  -6,  -1,  -5,  -1,
   -4,  -1,  -3,  -1,  -2,  -1,  -1,  -1,   0,  -1,
    1,  -1,   2,  -1,   3,  -1,   4,  -1,   5,  -1,
    6,  -1,   9,  -1, -23,   0, -18,   0, -14,   0,
  -11,   0,  -7,   0,  -5,   0,  -4,   0,  -3,   0,
   -2,   0,  -1,   0,   0, -23,   1,   0,   2,   0,
    3,   0,   4,   0,   5,   0,   7,   0,  11,   0,
   14,   0,  18,   0,  23,   0,  -9,   1,  -6,   1,
   -5,   1,  -4,   1,  -3,   1,  -2,   1,  -1,   1,
    0,   1,   1,   1,   2,   1,   3,   1,   4,   1,
    5,   1,   6,   1,   9,   1, -11,   2,  -7,   2,
   -5,   2,  -4,   2,  -3,   2,  -2,   2,  -1,   2,
    0,   2,   1,   2,   2,   2,   3,   2,   4,   2,
    5,   2,   7,   2,  11,   2,  -8,   3,  -6,   3,
   -4,   3,  -3,   3,  -2,   3,  -1,   3,   0,   3,
    1,   3,   2,   3,   3,   3,   4,   3,   6,   3,
    8,   3, -13,   4, -10,   4,  -5,   4,  -3,   4,
   -2,   4,  -1,   4,   0,   4,   1,   4,   2,   4,
    3,   4,   5,   4,  10,   4,  13,   4, -17,   5,
   -7,   5,  -4,   5,  -2,   5,  -1,   5,   0,   5,
    1,   5,   2,   5,   4,   5,   7,   5,  17,   5,
  -22,   6,  -9,   6,  -6,   6,  -3,   6,  -1,   6,
    1,   6,   3,   6,   6,   6,   9,   6,  22,   6,
   -5,   7,  -2,   7,   0,   7,   2,   7,   5,   7,
  -11,   8,  -8,   8,  -3,   8,   0,   8,   3,   8,
    8,   8,  11,   8,  -6,   9,  -1,   9,   1,   9,
    6,   9, -15,  10,  -4,  10,   4,  10,  15,  10,
   -8,  11,  -2,  11,   0,  11,   2,  11,   8,  11,
   19,  12, -19,  13,  -4,  13,   4,  13,   0,  14,
  -10,  15,  10,  15,  -5,  17,   5,  17,   0,  18,
  -12,  19,  13,  19,  -6,  22,   6,  22,   0,  23,
	};

	if (_tableLastPitch == pitch && _tableLastIndex == index)
		return;
	_tableLastPitch = pitch;
	_tableLastIndex = index;
	index *= 255;
	assert(index + 254 < (int32)(sizeof(maketable_bytes) / 2));

	for (int32 i = 0; i < 255; i++) {
		int32 j = (i + index) << 1;
		_offsetTable[i] = maketable_bytes[j + 1] * pitch + maketable_bytes[j];
	}
}

void Codec37Decoder::bompDecode(byte *dst, byte *src, int32 len) {
	byte code;
	byte color;
	int32 num;

	do {
		code = *src++;
		num = (code >> 1) + 1;
		if (code & 1) {
			color = *src++;
			memset(dst, color, num);
		} else {
			memcpy(dst, src, num);
			src += num;
		}
		dst += num;
		len -= num;
	} while (len > 0);
	assert(len == 0);
}

#if defined(SCUMM_NEED_ALIGNMENT)

#define DECLARE_LITERAL_TEMP(v)			\
	byte v

#define READ_LITERAL_PIXEL(src, v)		\
	v = *src++

#define WRITE_4X1_LINE(dst, v)			\
	do {					\
		int j;				\
		for(j=0; j<4; j++)		\
			(dst)[j] = v;		\
	} while(0)

#define COPY_4X1_LINE(dst, src)			\
	do {					\
		int j;				\
		for(j=0; j<4; j++)		\
			(dst)[j] = (src)[j];	\
	} while(0)

#else /* SCUMM_NEED_ALIGNMENT */

#define DECLARE_LITERAL_TEMP(v)			\
	uint32 v

#define READ_LITERAL_PIXEL(src, v)			\
	do {						\
		v = *src++;				\
		v += (v << 8) + (v << 16) + (v << 24);	\
	} while(0)

#define WRITE_4X1_LINE(dst, v)			\
	*(uint32 *)(dst) = v

#define COPY_4X1_LINE(dst, src)			\
	*(uint32 *)(dst) = *(uint32 *)(src)

#endif /* SCUMM_NEED_ALIGNMENT */

/* Fill a 4x4 pixel block with a literal pixel value */

#define LITERAL_4X4(src, dst, pitch)				\
	do {							\
		int x;						\
		DECLARE_LITERAL_TEMP(t);			\
		READ_LITERAL_PIXEL(src, t);			\
		for(x=0; x<4; x++) {				\
			WRITE_4X1_LINE(dst + pitch * x, t);	\
		}						\
		dst += 4;					\
	} while(0)

/* Fill four 4x1 pixel blocks with literal pixel values */

#define LITERAL_4X1(src, dst, pitch)				\
	do {							\
  		int x;						\
		DECLARE_LITERAL_TEMP(t);			\
		for(x=0; x<4; x++) {				\
			READ_LITERAL_PIXEL(src, t);		\
			WRITE_4X1_LINE(dst + pitch * x, t);	\
		}						\
		dst += 4;					\
	} while(0)

/* Fill sixteen 1x1 pixel blocks with literal pixel values */

#define LITERAL_1X1(src, dst, pitch)				\
	do {							\
		int x;						\
		for(x=0; x<4; x++) {				\
			COPY_4X1_LINE(dst + pitch * x, src);	\
			src += 4;				\
		}						\
		dst += 4;					\
	} while(0)

/* Copy a 4x4 pixel block from a different place in the framebuffer */

#define COPY_4X4(dst2, dst, pitch)					  \
	do {								  \
		int x;							  \
		for(x=0; x<4; x++) {					  \
			COPY_4X1_LINE(dst + pitch * x, dst2 + pitch * x); \
		}							  \
		dst += 4;						  \
	} while(0)

void Codec37Decoder::proc3WithFDFE(byte *dst, byte *src, int32 next_offs, int32 bw, int32 bh, int32 pitch, int16 *offset_table) {
	do {
		int32 i = bw;
		do {
			int32 code = *src++;
			if (code == 0xFD) {
				LITERAL_4X4(src, dst, pitch);
			} else if (code == 0xFE) {
				LITERAL_4X1(src, dst, pitch);
			} else if (code == 0xFF) {
				LITERAL_1X1(src, dst, pitch);
			} else {
				byte *dst2 = dst + _offsetTable[code] + next_offs;
				COPY_4X4(dst2, dst, pitch);
			}
		} while (--i);
		dst += pitch * 3;
	} while (--bh);
}

void Codec37Decoder::proc3WithoutFDFE(byte *dst, byte *src, int32 next_offs, int32 bw, int32 bh, int32 pitch, int16 *offset_table) {
	do {
		int32 i = bw;
		do {
			int32 code = *src++;
			if (code == 0xFF) {
				LITERAL_1X1(src, dst, pitch);
			} else {
				byte *dst2 = dst + _offsetTable[code] + next_offs;
				COPY_4X4(dst2, dst, pitch);
			}
		} while (--i);
		dst += pitch * 3;
	} while (--bh);
}

void Codec37Decoder::proc4WithFDFE(byte *dst, byte *src, int32 next_offs, int32 bw, int32 bh, int32 pitch, int16 *offset_table) {
	do {
		int32 i = bw;
		do {
			int32 code = *src++;
			if (code == 0xFD) {
				LITERAL_4X4(src, dst, pitch);
			} else if (code == 0xFE) {
				LITERAL_4X1(src, dst, pitch);
			} else if (code == 0xFF) {
				LITERAL_1X1(src, dst, pitch);
			} else if (code == 0x00) {
				int32 length = *src++ + 1;
				for (int32 l = 0; l < length; l++) {
					byte *dst2 = dst + next_offs;
					COPY_4X4(dst2, dst, pitch);
					i--;
					if (i == 0) {
						dst += pitch * 3;
						bh--;
						i = bw;
					}
				}
				if(bh == 0) {
					return;
				}
				i++;
			} else {
				byte *dst2 = dst + _offsetTable[code] + next_offs;
				COPY_4X4(dst2, dst, pitch);
			}
		} while (--i);
		dst += pitch * 3;
	} while (--bh);
}

void Codec37Decoder::proc4WithoutFDFE(byte *dst, byte *src, int32 next_offs, int32 bw, int32 bh, int32 pitch, int16 *offset_table) {
	do {
		int32 i = bw;
		do {
			int32 code = *src++;
			if (code == 0xFF) {
				LITERAL_1X1(src, dst, pitch);
			} else if (code == 0x00) {
				int32 length = *src++ + 1;
				for (int32 l = 0; l < length; l++) {
					byte *dst2 = dst + next_offs;
					COPY_4X4(dst2, dst, pitch);
					i--;
					if (i == 0) {
						dst += pitch * 3;
						bh--;
						i = bw;
					}
				}
				if(bh == 0) {
					return;
				}
				i++;
			} else {
				byte *dst2 = dst + _offsetTable[code] + next_offs;
				COPY_4X4(dst2, dst, pitch);
			}
		} while (--i);
		dst += pitch * 3;
	} while (--bh);
}

bool Codec37Decoder::decode(Blitter & dst, Chunk & src) {
	int32 width = getRect().width();
	int32 height = getRect().height();
	int32 bw = (width + 3) >> 2, bh = (height + 3) >> 2;
	int32 pitch = bw << 2;

	int32 chunk_size = src.getSize() - 14;
	byte *chunk_buffer = (byte *)malloc(chunk_size);
	src.read(chunk_buffer, chunk_size);

	int16 seq_nb = READ_LE_UINT16(chunk_buffer + 2);
	int32 decoded_size = READ_LE_UINT32(chunk_buffer + 4);
	byte mask_flags = chunk_buffer[12];
	maketable(pitch, chunk_buffer[1]);
	int32 tmp;

	switch(chunk_buffer[0]) {
	case 0:
		if ((_deltaBufs[_curtable] - _deltaBuf) > 0) {
			memset(_deltaBuf, 0, _deltaBufs[_curtable] - _deltaBuf);
		}
		tmp = (_deltaBuf + _deltaSize) - _deltaBufs[_curtable] - decoded_size;
		if (tmp > 0) {
			memset(_deltaBufs[_curtable] + decoded_size, 0, tmp);
		}
		memcpy(_deltaBufs[_curtable], chunk_buffer + 16, decoded_size);
		break;
	case 1:
		error("codec37: missing opcode 1");
		break;
	case 2:
		bompDecode(_deltaBufs[_curtable], chunk_buffer + 16, decoded_size);
		if ((_deltaBufs[_curtable] - _deltaBuf) > 0) {
			memset(_deltaBuf, 0, _deltaBufs[_curtable] - _deltaBuf);
		}
		tmp = (_deltaBuf + _deltaSize) - _deltaBufs[_curtable] - decoded_size;
		if (tmp > 0) {
			memset(_deltaBufs[_curtable] + decoded_size, 0, tmp);
		}
		break;
	case 3:
		if ((seq_nb & 1) || !(mask_flags & 1)) {
			_curtable ^= 1;
		}

		if((mask_flags & 4) != 0) {
			proc3WithFDFE(_deltaBufs[_curtable], chunk_buffer + 16,
										_deltaBufs[_curtable ^ 1] - _deltaBufs[_curtable], bw, bh,
										pitch, _offsetTable);
		} else {
			proc3WithoutFDFE(_deltaBufs[_curtable], chunk_buffer + 16,
										_deltaBufs[_curtable ^ 1] - _deltaBufs[_curtable], bw, bh,
										pitch, _offsetTable);
		}
		break;
	case 4:
		if ((seq_nb & 1) || !(mask_flags & 1)) {
			_curtable ^= 1;
		}

		if((mask_flags & 4) != 0) {
			proc4WithFDFE(_deltaBufs[_curtable], chunk_buffer + 16,
										_deltaBufs[_curtable ^ 1] - _deltaBufs[_curtable], bw, bh,
										pitch, _offsetTable);
		} else {
			proc4WithoutFDFE(_deltaBufs[_curtable], chunk_buffer + 16,
										_deltaBufs[_curtable ^ 1] - _deltaBufs[_curtable], bw, bh,
										pitch, _offsetTable);
		}
		break;
	default:
		break;
	}
	_prevSeqNb = seq_nb;

	dst.blit(_deltaBufs[_curtable], width * height);
	
	free(chunk_buffer);
	return true;
}

