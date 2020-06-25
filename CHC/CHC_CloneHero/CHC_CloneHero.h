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
#ifdef CHCCLONEHERO_EXPORTS
#define CHC_CloneHero_API __declspec(dllexport)
#else
#define CHC_CloneHero_API __declspec(dllimport)
#endif
#include "CHC.h"

struct SyncTrack
{
	double position = 0;
	unsigned long timeSig = 4;
	unsigned long bpm = 120;
};

struct Event
{
	double position = 0;
	std::string name = "";
};

struct CHNote
{
	enum class NoteType { NOTE, EVENT, STAR };
	enum class Modifier { NORMAL, FORCED, HOPO, TAP };
	double position = 0;
	char fret = 1;
	double sustain = 0;
	Modifier mod = Modifier::NORMAL;
	NoteType type = NoteType::NOTE;
	std::string name = "";
	double dif = 0;
};

class Charter
{
	CHC song;
public:
	Charter(CHC& song) { this->song = song; }
	CHC& getSong() { return song; }
	bool exportChart();
	bool importChart();
};

extern "C" CHC_CloneHero_API bool exportChart(CHC & song);
extern "C" CHC_CloneHero_API bool importChart(CHC & song);