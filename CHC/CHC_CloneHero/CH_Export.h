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
	CHC* song;
	NoteTrack modchartNotes[2];
	NoteTrack reimportNotes[2];
public:
	CH_Exporter(CHC* song) : song(song) {}
	bool exportChart();
	bool getFrets(size_t& strumFret, const char* sectionName, size_t promptType, size_t playerIndex = 0, size_t chartIndex = 0, size_t note = 1, size_t piece = 1, size_t prevFret = 0);
	size_t getOrientation(const char* sectionName, size_t player = 0, size_t chart = 0);
	bool convertSong(List<size_t>& sectionIndexes, List<SyncTrack>& sync, List<Event>& events, bool modchart, double& totalDuration);
	size_t convertGuard(Chart& chart, const double position, const double TICKS_PER_SAMPLE, const size_t currentPlayer, const size_t orientation, const bool modchart);
	void convertTrace(Chart& chart, const double position, const double TICKS_PER_SAMPLE, const long sectionDuration, const size_t currentPlayer, const bool modchart);
	size_t convertPhrase(SongSection& section, const size_t playerIndex, const size_t chartIndex, const double position,
		const double TICKS_PER_SAMPLE, const size_t currentPlayer, const size_t colorType, size_t& strumFret, const bool modchart);
};
