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

struct NoteTrack
{
	//Green-Red-Yellow-Blue-Orange-Open
	LinkedList::List<CHNote> m_colors[6];

	LinkedList::List<CHNote> m_modifiers;
	LinkedList::List<CHNote> m_star;
	LinkedList::List<CHNote> m_events;
	LinkedList::List<CHNote*> m_allNotes;
	template<class... Args>
	size_t addNote(Args&&... args)
	{
		CHNote note(args...);
		switch (note.m_type)
		{
		case CHNote::NoteType::NOTE:
		{
			size_t firstIndex = -1;
			{
				const unsigned origLane = note.m_fret.m_lane;
				for (unsigned index = 0; index < 6; index++)
				{
					if (origLane & (1 << index))
					{
						if (index < 5)
							note.m_fret.m_lane = index;
						else
							note.m_fret.m_lane = 7;
						m_colors[index].push_back(note);
						if (firstIndex == -1)
							firstIndex = m_allNotes.insert_ordered(&m_colors[index].back());
						else
							m_allNotes.insert_ordered(&m_colors[index].back());
					}
				}
			}
			switch (note.m_mod)
			{
			case CHNote::Modifier::FORCED:
				note.m_fret.m_lane = 5;
				m_modifiers.push_back(note);
				if (firstIndex == -1)
					firstIndex = m_allNotes.insert_ordered(&m_modifiers.back());
				else
					m_allNotes.insert_ordered(&m_modifiers.back());
			case CHNote::Modifier::TAP:
				note.m_fret.m_lane = 6;
				m_modifiers.push_back(note);
				if (firstIndex == -1)
					firstIndex = m_allNotes.insert_ordered(&m_modifiers.back());
				else
					m_allNotes.insert_ordered(&m_modifiers.back());
			}
			return firstIndex;
		}
		case CHNote::NoteType::STAR:
			note.m_fret.m_lane = 0;
			m_star.push_back(note);
			return m_allNotes.insert_ordered(&m_star.back());
		default:
			m_events.push_back(note);
			return m_allNotes.insert_ordered(&m_events.back());
		}
	}
	size_t addModifier(double pos, CHNote::Modifier mod);
	size_t addEvent(double pos, std::string nam);
	size_t addStarPower(double pos, double sustain);
	void write(FILE* outFile, const size_t player);
	void writeDuet(FILE* outFile, const size_t player);
	void clear();
};
