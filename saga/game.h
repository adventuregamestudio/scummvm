/* ScummVM - Scumm Interpreter
 * Copyright (C) 2004 The ScummVM project
 *
 * The ReInherit Engine is (C)2000-2003 by Daniel Balsom.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 *
 */

// Game detection, general game parameters

#ifndef SAGA_GAME_H_
#define SAGA_GAME_H_

namespace Saga {

#define R_GAME_LANGSTR_LIMIT 3
#define R_GAME_PATH_LIMIT 512

#define R_GAME_ITE_LANG_PREFIX "ite_"
#define R_GAME_LANG_EXT "lng"

// Script lookup table entry sizes for game verification
#define R_SCR_LUT_ENTRYLEN_ITECD 22
#define R_SCR_LUT_ENTRYLEN_ITEDISK 16

typedef int (*R_GAME_VERIFYFUNC) (const char *);

struct R_GAME_FILEDESC {
	const char *gf_fname;
	uint16 gf_type;
};

struct R_GAMEDESC {
	int gd_game_type;
	int gd_game_id;
	const char *gd_title;
	int gd_logical_w;
	int gd_logical_h;
	int gd_scene_h;
	int gd_startscene;
	R_GAME_RESOURCEDESC *gd_resource_desc;
	int gd_filect;
	R_GAME_FILEDESC *gd_filedescs;
	int gd_fontct;
	R_GAME_FONTDESC *gd_fontdescs;
	R_GAME_SOUNDINFO *gd_soundinfo;
	R_GAME_VERIFYFUNC gd_verifyf;
	int gd_supported;
};

struct R_GAME_FILEDATA {
	R_RSCFILE_CONTEXT *file_ctxt;
	uint16 file_types;
	uint16 file_flags;
};

struct R_GAMEMODULE {
	int game_init;
	int game_number;
	R_GAMEDESC *gamedesc;
	int g_skipintro;
	char game_dir[R_MAXPATH];
	char data_dir[R_MAXPATH];
	char game_language[R_GAME_LANGSTR_LIMIT];
	uint16 gfile_n;
	R_GAME_FILEDATA *gfile_data;
	uint16 gd_fontct;
	R_GAME_FONTDESC *gd_fontdescs;
	int err_n;
	const char *err_str;
};

int LoadLanguage();
int DetectGame(const char *game_dir, uint16 * game_n_p);
int LoadGame(const char *game_dir, uint16 game_n_p);
int Verify_ITEDEMO(const char *game_dir);
int Verify_ITEDISK(const char *game_dir);
int Verify_ITECD(const char *game_dir);
int Verify_IHNMDEMO(const char *game_dir);
int Verify_IHNMCD(const char *game_dit);

} // End of namespace Saga

#endif
