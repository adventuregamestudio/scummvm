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

#include "titanic/sound/season_noises.h"

namespace Titanic {

CSeasonNoises::CSeasonNoises() : CViewAutoSoundPlayer(), _fieldF0(0),
	_string2("NULL"), _string3("NULL"), _string4("NULL"), _string5("NULL") {
}

void CSeasonNoises::save(SimpleFile *file, int indent) {
	file->writeNumberLine(1, indent);
	file->writeNumberLine(_fieldF0, indent);
	file->writeQuotedLine(_string2, indent);
	file->writeQuotedLine(_string3, indent);
	file->writeQuotedLine(_string4, indent);
	file->writeQuotedLine(_string5, indent);

	CViewAutoSoundPlayer::save(file, indent);
}

void CSeasonNoises::load(SimpleFile *file) {
	file->readNumber();
	_fieldF0 = file->readNumber();
	_string2 = file->readString();
	_string3 = file->readString();
	_string4 = file->readString();
	_string5 = file->readString();

	CViewAutoSoundPlayer::load(file);
}

} // End of namespace Titanic