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

class CHC_To_CloneHero : public ChartFileExporter
{
	unsigned m_phraseBarPromptType[2] = { 0, 0 };
	uint32_t m_strumFret[2] = { 0, 0 };
	unsigned m_guardPromptType = 0;
	unsigned m_guardOrientation = 2;
	float m_position_ticks = 0;
	float m_samepleDuration = 0;
	bool m_modchart = true;
	bool m_grdFound = false;
	bool m_phraseFound[2] = { false, false };
public:
	CHC_To_CloneHero();
	bool writeChart(CHC* song);
	bool convertSong(CHC* song, std::vector<size_t>& sectionIndexes);

	bool checkNoteHandling(SongSection& section, const bool multiplayer);

	void convertGuards(Chart& chart, const float TICKS_PER_SAMPLE, const size_t currentPlayer);
	bool getOrientation(const char* sectionName, const size_t player = 0, const size_t chart = 0);

	void convertTrace(Chart& chart, const float TICKS_PER_SAMPLE, const int32_t sectionDuration, const size_t currentPlayer);

	void convertPhrases(Chart& chart, const float TICKS_PER_SAMPLE, const size_t currentPlayer, const SongSection::Phase phase);
	bool getFrets(unsigned promptType, const size_t currentplayer, size_t note = 1, size_t piece = 1);
};
