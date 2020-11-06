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
	LinkedList::List<CHNote> colors[6];

	LinkedList::List<CHNote> modifiers;
	LinkedList::List<CHNote> star;
	LinkedList::List<CHNote> events;
	LinkedList::List<CHNote*> allNotes;
	template<class... Args>
	size_t addNote(Args&&... args)
	{
		CHNote note(args...);
		switch (note.type)
		{
		case CHNote::NoteType::NOTE:
		{
			size_t firstIndex = -1;
			for (size_t index = 0, lane = note.fret.lane; index < 6; index++)
			{
				if (lane & (size_t(1) << index))
				{
					if (index < 5)
						note.fret.lane = index;
					else
						note.fret.lane = 7;
					colors[index].push_back(note);
					if (firstIndex == -1)
						firstIndex = allNotes.insert_ordered(&colors[index].back());
					else
						allNotes.insert_ordered(&colors[index].back());
				}
			}
			switch (note.mod)
			{
			case CHNote::Modifier::FORCED:
				note.fret.lane = 5;
				modifiers.push_back(note);
				if (firstIndex == -1)
					firstIndex = allNotes.insert_ordered(&modifiers.back());
				else
					allNotes.insert_ordered(&modifiers.back());
			case CHNote::Modifier::TAP:
				note.fret.lane = 6;
				modifiers.push_back(note);
				if (firstIndex == -1)
					firstIndex = allNotes.insert_ordered(&modifiers.back());
				else
					allNotes.insert_ordered(&modifiers.back());
			}
			return firstIndex;
		}
		case CHNote::NoteType::STAR:
			note.fret.lane = 0;
			star.push_back(note);
			return allNotes.insert_ordered(&star.back());
		default:
			events.push_back(note);
			return allNotes.insert_ordered(&events.back());
		}
	}
	size_t addModifier(double pos, CHNote::Modifier mod);
	size_t addEvent(double pos, std::string nam);
	size_t addStarPower(double pos, double sustain);
	void write(FILE* outFile, const size_t player);
	void clear();
};
