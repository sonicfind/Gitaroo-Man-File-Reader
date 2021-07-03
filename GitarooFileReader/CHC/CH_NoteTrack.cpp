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
#include "pch.h"
#include "CH_NoteTrack.h"
using namespace std;

void NoteTrack::pushNote(const CHNote& note, const int version)
{
	if (m_allNotes.size() == 0 || note.m_position_ticks != m_allNotes.back().m_position_ticks)
		m_allNotes.push_back(note);
	else if (!note.m_isTap && !note.m_isForced)
		m_allNotes.back() = note;
	else if ((version == 2 && note.m_isForced)
		|| (version == 1 && note.m_isTap))
	{
		m_allNotes.back().m_isForced = true;
		m_allNotes.back().m_isTap = true;
	}
}

std::list<CHNote>::iterator NoteTrack::addNote(float pos, int frets, float sustain, bool writeSustain)
{
	auto ntIterator = GlobalFunctions::emplace_ordered(m_allNotes, pos, frets, sustain, writeSustain);
	for (unsigned index = 0; index < 6; index++)
		if (frets & (1 << index))
		{
			auto location = std::lower_bound(m_colors[index].begin(),
											m_colors[index].end(),
											ntIterator->m_colors[index],
											[](const CHNote::Fret* a, const CHNote::Fret b) { return *a < b; });

			if (location == m_colors[index].end() || **location != ntIterator->m_colors[index])
				m_colors[index].insert(location, &ntIterator->m_colors[index]);
		}
	return ntIterator;
}

std::list<CHNote>::iterator NoteTrack::addEvent(float pos, std::string name)
{
	// Event constructor
	return GlobalFunctions::emplace_ordered(m_allNotes, pos, name);
}

std::list<CHNote>::iterator NoteTrack::addStarPower(float pos, float sustain)
{
	// Star Power constructor
	auto ntIterator = GlobalFunctions::emplace_ordered(m_allNotes, pos, sustain);
	m_star.push_back(&ntIterator->m_star);
	return ntIterator;
}

void NoteTrack::write(FILE* outFile, const size_t player)
{
	const char* headers[2] = { "[ExpertSingle]\n{\n", "[ExpertDoubleRhythm]\n{\n" };
	if (m_allNotes.size())
	{
		fprintf(outFile, headers[player]);
		for (CHNote& note : m_allNotes)
			note.write(outFile);
		fprintf(outFile, "}\n");
	}
}

void NoteTrack::writeDuet(FILE* outFile, const size_t player)
{
	const char* headers[2] = { "[ExpertSingle]\n{\n", "[ExpertDoubleGuitar]\n{\n" };
	if (m_allNotes.size())
	{
		fprintf(outFile, headers[player]);
		for (CHNote& note : m_allNotes)
			note.write(outFile);
		fprintf(outFile, "}\n");
	}
}
