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
#include "SongSection.h"
constexpr size_t s_DLL_INDEX = 0;

constexpr float s_SAMPLES_PER_MIN = 2880000.0f;

class CHC
{
	friend class CHC_Main;
	friend class CHC_Editor;
	friend class TAS;
	friend class CH_Exporter;
	friend class CH_Importer;
private:
	//Full filename
	std::string m_filename;
	//Short version
	std::string m_shortname;
	//Stage number
	int m_stage;
	//Holds header data from original file
	char m_header[36] = { 0 };

	//String for an IMC's location
	char m_imc[256] = { 0 };

	//Win-Loss Animations
	SSQ m_events[4];

	//Hold data for all 8 audio channels
	struct AudioChannel
	{
		unsigned long volume = 32767;
		unsigned long pan = 16383;
	} m_audio[8];

	//Gameplay speed
	float m_speed;
	//Hold the number of unorganized sections
	unsigned m_unorganized;
	//Saves whether all notes were readjusted for minimal glitching
	bool m_optimized;
	//LinkedList::List of all sections
	LinkedList::List<SongSection> m_sections;
	//Holds all life-value data for every player and phase type
	struct EnergyDamage
	{
		float initialEnergy = .2f;
		float chargeInitial = .01f;
		float attackInitial = .015f;
		float guardEnergy = .015f;
		float attackMiss = .05f;
		float guardMiss = .05f;
		float chargeRelease = .025f;
		float attackRelease = .025f;
	} m_energyDamageFactors[4][5];
	//0 - Not saved
	//1 - Saved
	//2 - Saved at the currently pointed location
	char m_saved;
public:
	CHC();
	CHC(const CHC&);
	CHC(std::string filename);
	CHC& operator=(CHC&);
	void create(std::string filename);
	size_t getNumSections() { return m_sections.size(); }
};
