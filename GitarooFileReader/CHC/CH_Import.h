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
#include "CHC.h"
#include "CH_ChartFile.h"

struct Section : public CHObject
{
	struct Tempo : public CHObject
	{
		float m_sample_offset_from_section;
		unsigned long m_bpm;
		Tempo(float pos_ticks, float sample_offset, unsigned long bpm);
	};

	float m_position_samples;
	std::string m_name;
	std::vector<Tempo> m_tempos;
	// Two lists for two players
	std::vector<Chart> m_subs[2];
	std::queue<SongSection*> m_insertions;

	Section(std::string nam, float pos_ticks);
	void replaceNotes(const bool charted[2]);
	void replaceNotes_Duet(const bool charted[2]);
};

class CloneHero_To_CHC : public ChartFile
{
	std::vector<Section> m_sections;
	NoteTrack m_notes[2];
public:
	constexpr static float s_SPT_CONSTANT = s_SAMPLES_PER_MIN * 1000;
	bool open(std::string filename);
	CHC* convertNotes(CHC* song);

	// Returns whether the traceline was added to the current chart
	int addTraceLine(float pos, std::string name, const size_t sectIndex, const size_t playerIndex);
	void addPhraseBar(Chart* currChart, const CHNote& note, const float position, const float SAMPLES_PER_TICK);
	bool addGuardMark(Chart* currChart, const CHNote& note, const long position);
};
