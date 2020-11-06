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

struct Section
{
	struct Tempo
	{
		unsigned long bpm;
		//In reference to the beginning of the song
		double position_ticks;
		//In reference to the beginning of the section is resides in
		double position_samples;
		Tempo(unsigned long bpm = 120, double pos_ticks = 0, double pos_samples = 0) : bpm(bpm), position_ticks(pos_ticks), position_samples(pos_samples) {}
	};
	std::string name = "";
	//In reference to the beginning of the song
	double position_ticks = 0;
	//In reference to the beginning of the song, but calculated 
	//using the previous songsection (if one exists) as a base
	double position_samples = 0;
	LinkedList::List<Tempo> tempos;
	//Two lists for two players
	LinkedList::List<Chart> subs[2];
	Section(std::string nam, double pos_T = 0, double pos_S = 0, unsigned long bpm = 120) : name(nam), position_ticks(pos_T), position_samples(pos_S)
	{
		tempos.emplace_back(bpm, position_ticks);
	}
};

class ChartFileImporter : public ChartFile
{
public:
	ChartFileImporter() : ChartFile() {}
	ChartFileImporter(std::string filename) : ChartFile(filename, false) {}
	bool open(std::string filename) { return ChartFile::open(filename, false); }
	void read(CH_Importer& importer);
};

class CH_Importer
{
	friend ChartFileImporter;
	CHC song;
	LinkedList::List<Section> sections;
	NoteTrack notes[2];
public:
	CH_Importer(CHC& song) : song(song) {}
	CHC& getSong() { return song; }
	bool importChart();
	void fillSections();
	void addTraceLine(double pos, std::string name, const size_t sectIndex, const size_t playerIndex, Chart* currChart);
	void addPhraseBar(long pos, unsigned long sus, unsigned long lane, Chart* currChart, const long SAMPLES_PER_TICK_ROUNDED);
	void addGuardMark(const long pos, const unsigned long button, Chart* currChart);
	void applyForced(const long pos, Chart* currChart, const Section* currSection, const size_t playerIndex);
};
