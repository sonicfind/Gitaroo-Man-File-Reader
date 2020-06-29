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
	double position;
	unsigned long timeSig;
	unsigned long bpm;
	SyncTrack() : position(0), timeSig(4), bpm(120) {}
	SyncTrack(FILE* inFile);
	SyncTrack(double pos, unsigned long ts = 4, unsigned long tempo = 120) : position(pos), timeSig(ts), bpm(tempo) {}
};

struct Event
{
	double position;
	std::string name;
	Event() : position(0), name("") {}
	Event(FILE* inFile);
	Event(double pos, std::string nam = "") : position(pos), name(nam) {}
};

struct CHNote
{
	enum class NoteType { NOTE, EVENT, STAR };
	enum class Modifier { NORMAL, FORCED, TAP };
	double position;
	char fret;
	double sustain;
	Modifier mod;
	NoteType type;
	std::string name;
	CHNote() : position(0), fret(1), sustain(0), mod(Modifier::NORMAL), type(NoteType::NOTE), name("") {}
	CHNote(FILE* inFile);
	CHNote(double pos, char ft = 1, double sus = 0, Modifier md = Modifier::NORMAL, NoteType tp = NoteType::NOTE, std::string nam = "") 
		: position(pos), fret(ft), sustain(sus), mod(md), type(tp), name(nam) {}
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