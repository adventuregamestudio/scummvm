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
 */

namespace Tony {

static const TonyGameDescription gameDescriptions[] = {

	{
		// Tony Tough English Demo
		{
			"tony",
			"Demo",
			AD_ENTRY1s("roasted.mpr", "06203dbbc85fdd1e6dc8fc211c1a6207", 14972409),
			Common::EN_ANY,
			Common::kPlatformPC,
			ADGF_DEMO,
			GUIO1(GUIO_NONE)
		},
	},

	{
		// Tony Tough English Demo (Compressed)
		{
			"tony",
			"Demo",
			AD_ENTRY1s("data1.cab", "7d8b6d308f96aee3968ad7910fb11e6d", 58660608),
			Common::EN_ANY,
			Common::kPlatformPC,
			ADGF_DEMO | GF_COMPRESSED,
			GUIO1(GUIO_NONE)
		},
	},

	{ AD_TABLE_END_MARKER }
};

} // End of namespace Tony
