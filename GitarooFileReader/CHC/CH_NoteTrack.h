#pragma once
/*  Gitaroo Man File Reader
 *  Copyright (C) 2020-2021 Gitaroo Pals
 *
 *  Gitaroo Man File Reader is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  Gitaroo Man File Reader is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with Gitaroo Man File Reader.
 *  If not, see <http://www.gnu.org/licenses/>.
 */
#include "CH_Object.h"
#include "Global_Functions.h"

struct NoteTrack
{
	std::list<CHNote> m_allNotes;

	// Green-Red-Yellow-Blue-Orange-Open
	std::vector<CHNote::Fret*> m_colors[6];
	std::vector<CHNote::Fret*> m_stars;
	std::vector<std::list<std::string>*> m_events;

	std::list<CHNote>::iterator addNote(float pos, int frets, float sustain = 0, bool writeSustain = true);
	std::list<CHNote>::iterator addEvent(float pos, std::string nam);
	std::list<CHNote>::iterator addStarPower(float pos, float sustain);
	void pushNote(const CHNote& note, const int version);
	void write(FILE* outFile, const size_t player);
	void writeDuet(FILE* outFile, const size_t player);
};
