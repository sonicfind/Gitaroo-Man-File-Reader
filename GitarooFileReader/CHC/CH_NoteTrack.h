#pragma once
/*  Gitaroo Man File Reader
 *  Copyright (C) 2020 Gitaroo Pals
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
#include "CH_Item.h"
#include "Global_Functions.h"

struct NoteTrack
{
	//Green-Red-Yellow-Blue-Orange-Open
	std::vector<CHNote*> m_colors[6];

	// Including modifiers, star power notes, and events
	std::vector<CHNote*> m_allNotes;
	~NoteTrack();
	template<class... Args>
	size_t addNote(Args&&... args)
	{
		CHNote note(args...);
		size_t firstIndex = -1;
		const unsigned origLane = note.m_fret.m_lane;
		for (unsigned index = 0; index < 6; index++)
		{
			if (origLane & (1 << index))
			{
				if (index < 5)
					note.m_fret.m_lane = index;
				else
					note.m_fret.m_lane = 7;
				m_colors[index].push_back(new CHNote(note));
				if (firstIndex == -1)
					firstIndex = GlobalFunctions::insert_ordered(m_allNotes, m_colors[index].back());
				else
					GlobalFunctions::insert_ordered(m_allNotes, m_colors[index].back());
			}
		}
		return firstIndex;
	}
	size_t addModifier(float pos, CHNote::Modifier mod);
	size_t addEvent(float pos, std::string nam);
	size_t addStarPower(float pos, float sustain);
	void write(FILE* outFile, const size_t player);
	void writeDuet(FILE* outFile, const size_t player);
	void clear();
};
