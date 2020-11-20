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
		unsigned long m_bpm;
		//In reference to the beginning of the song
		double m_position_ticks;
		//In reference to the beginning of the section is resides in
		double m_position_samples;
		Tempo(unsigned long bpm = 120, double pos_ticks = 0, double pos_samples = 0) : m_bpm(bpm), m_position_ticks(pos_ticks), m_position_samples(pos_samples) {}
	};
	std::string m_name = "";
	//In reference to the beginning of the song
	double m_position_ticks = 0;
	//In reference to the beginning of the song, but calculated 
	//using the previous songsection (if one exists) as a base
	double m_position_samples = 0;
	LinkedList::List<Tempo> m_tempos;
	//Two lists for two players
	LinkedList::List<Chart> m_subs[2];
	Section(std::string nam, double pos_T = 0, double pos_S = 0, unsigned long bpm = 120) : m_name(nam), m_position_ticks(pos_T), m_position_samples(pos_S)
	{
		m_tempos.emplace_back(bpm, m_position_ticks);
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
	CHC m_song;
	LinkedList::List<Section> m_sections;
	NoteTrack m_notes[2];
public:
	CH_Importer(CHC& song) : m_song(song) {}
	CHC& getSong() { return m_song; }
	bool importChart();
	void fillSections();
	void addTraceLine(double pos, std::string name, const size_t sectIndex, const size_t playerIndex, Chart* currChart);
	void addPhraseBar(long pos, unsigned long sus, unsigned long lane, Chart* currChart, const long SAMPLES_PER_TICK_ROUNDED);
	void addGuardMark(const long pos, const unsigned long button, Chart* currChart);
	void applyForced(const long pos, Chart* currChart, const Section* currSection, const size_t playerIndex);
};
