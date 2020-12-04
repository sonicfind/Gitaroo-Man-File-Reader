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
#include "..\..\Header\pch.h"
#include "Global_Functions.h"
#include "NoteTrack.h"
using namespace std;

size_t NoteTrack::addModifier(double pos, CHNote::Modifier mod)
{
	m_modifiers.emplace_back(pos, 5 + (mod == CHNote::Modifier::TAP), 0, true, mod);
	return m_allNotes.insert_ordered(&m_modifiers.back());
}

size_t NoteTrack::addEvent(double pos, std::string name)
{
	m_events.emplace_back(pos, 0, 0, true, CHNote::Modifier::NORMAL, CHNote::NoteType::EVENT, name);
	return m_allNotes.insert_ordered(&m_events.back());
}

size_t NoteTrack::addStarPower(double pos, double sustain)
{
	m_star.emplace_back(pos, 0, sustain, true, CHNote::Modifier::NORMAL, CHNote::NoteType::STAR);
	return m_allNotes.insert_ordered(&m_star.back());
}

void NoteTrack::write(FILE* outFile, const size_t player)
{
	const char* headers[2] = { "[ExpertSingle]\n{\n", "[ExpertDoubleRhythm]\n{\n" };
	if (m_allNotes.size())
	{
		fprintf(outFile, headers[player]);
		for (CHNote* note : m_allNotes)
			note->write(outFile);
		fprintf(outFile, "}\n");
	}
}

void NoteTrack::clear()
{
	m_allNotes.clear();
	for (LinkedList::List<CHNote>& track : m_colors)
		track.clear();
	m_modifiers.clear();
	m_events.clear();
}
