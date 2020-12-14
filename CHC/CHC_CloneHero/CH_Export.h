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
#include "CHC.h"
#include "ChartFile.h"

class CH_Exporter
{
	CHC* m_song;
	unsigned m_phraseBarPromptType[2] = { 0, 0 };
	unsigned long m_strumFret[2] = { 0, 0 };
	unsigned m_guardPromptType = 0;
	unsigned m_guardOrientation = 2;
	float m_position = 0;
	float m_samepleDuration = 0;
	bool m_modchart = false;
	ChartFileExporter m_exporter;
public:
	CH_Exporter(CHC* song) : m_song(song) {}
	bool exportChart();
	bool convertSong(LinkedList::List<size_t>& sectionIndexes);

	size_t convertGuard(Chart& chart, const float TICKS_PER_SAMPLE, const size_t currentPlayer);
	bool getOrientation(const char* sectionName, const size_t player = 0, const size_t chart = 0);

	void convertTrace(Chart& chart, const float TICKS_PER_SAMPLE, const long sectionDuration, const size_t currentPlayer);

	size_t convertPhrase(SongSection& section, const size_t playerIndex, const size_t chartIndex, const float TICKS_PER_SAMPLE, const size_t currentPlayer);
	bool getFrets(const char* sectionName, unsigned promptType, size_t playerIndex = 0, size_t chartIndex = 0, size_t note = 1, size_t piece = 1);
};
