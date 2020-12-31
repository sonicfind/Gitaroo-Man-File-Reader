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
#include "pch.h"
#include "CH_NoteTrack.h"
using namespace std;

NoteTrack::~NoteTrack()
{
	for (CHNote* note : m_allNotes)
		delete note;
}

size_t NoteTrack::addModifier(float pos, CHNote::Modifier mod)
{
	return GlobalFunctions::insert_ordered(m_allNotes, new CHNote(pos, 5U + (mod == CHNote::Modifier::TAP), 0.0f, true, mod));
}

size_t NoteTrack::addEvent(float pos, std::string name)
{
	return GlobalFunctions::insert_ordered(m_allNotes, new CHNote(pos, 0U, 0.0f, true, CHNote::Modifier::NORMAL, CHNote::NoteType::EVENT, name));
}

size_t NoteTrack::addStarPower(float pos, float sustain)
{
	return GlobalFunctions::insert_ordered(m_allNotes, new CHNote(pos, 0U, sustain, true, CHNote::Modifier::NORMAL, CHNote::NoteType::STAR));
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

void NoteTrack::writeDuet(FILE* outFile, const size_t player)
{
	const char* headers[2] = { "[ExpertSingle]\n{\n", "[ExpertDoubleGuitar]\n{\n" };
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
	for (std::vector<CHNote*>& track : m_colors)
		track.clear();
	for (CHNote* note : m_allNotes)
		delete note;
	m_allNotes.clear();
}
