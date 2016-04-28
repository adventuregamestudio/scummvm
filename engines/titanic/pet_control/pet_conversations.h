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

#ifndef TITANIC_PET_CONVERSATIONS_H
#define TITANIC_PET_CONVERSATIONS_H

#include "titanic/pet_control/pet_section.h"
#include "titanic/pet_control/pet_text.h"
#include "titanic/pet_control/pet_gfx_element.h"

namespace Titanic {

enum SummonResult { SUMMON_CANT = 0, SUMMON_CAN = 1, SUMMON_ABORT = 2 };

class CPetConversations : public CPetSection {
private:
	CPetGfxElement _scrollUp;
	CPetGfxElement _scrollDown;
	CPetGfxElement _val3;
	CPetGfxElement _gfxList[3];
	CPetGfxElement _val4;
	CPetGfxElement _val5;
	CPetGfxElement _val6;
	int  _field14C;
	CPetGfxElement _doorBot;
	CPetGfxElement _bellBot;
	CPetGfxElement _val9;
	CPetGfxElement _valArray2[9];
	int _field30C;
	CPetText _log;
	CPetText _text2;
	int _valArray3[3];
	int _field414;
	int _field418;
	CString _string1;
private:
	/**
	 * Sets up the control
	 */
	bool setupControl(CPetControl *petControl);

	/**
	 * Scroll up the conversation log
	 */
	void scrollUp();

	/**
	 * Scroll down the conversation log
	 */
	void scrollDown();

	/**
	 * Scroll to the bottom of the conversation log
	 */
	void scrollToBottom();

	/**
	 * Check whether an NPC can be summoned
	 */
	SummonResult canSummonNPC(const CString &name);

	/**
	 * Summon an NPC
	 */
	void summonNPC(const CString &name);
public:
	CPetConversations();

	/**
	 * Save the data for the class to file
	 */
	virtual void save(SimpleFile *file, int indent) const;

	/**
	 * Load the data for the class from file
	 */
	virtual void load(SimpleFile *file, int param);

	/**
	 * Returns true if the object is in a valid state
	 */
	virtual bool isValid(CPetControl *petControl);

	/**
	 * Following are handlers for the various messages that the PET can
	 * pass onto the currently active section/area
	 */
	virtual bool MouseButtonDownMsg(CMouseButtonDownMsg *msg);
	virtual bool MouseButtonUpMsg(CMouseButtonUpMsg *msg);
};

} // End of namespace Titanic

#endif /* TITANIC_PET_CONVERSATIONS_H */
