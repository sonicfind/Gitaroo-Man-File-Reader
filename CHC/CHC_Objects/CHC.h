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
#define DLL_INDEX 0
constexpr long double SAMPLES_PER_MIN = 2880000.0;
class CHC
{
	friend class CHC_Main;
	friend class CHC_Editor;
	friend class TAS;
	friend class Charter;
private:
	//Full filename
	std::string name;
	//Short version
	std::string shortname;
	//Stage number
	unsigned char stage;
	//Holds header data from original file
	char header[36] = { 0 };

	//String for an IMC's location
	char imc[256] = { 0 };

	//Win-Loss Animations
	SSQ events[4];

	//Hold data for all 8 audio channels
	struct AudioChannel
	{
		unsigned long volume = 32767;
		unsigned long pan = 16383;
	} audio[8];

	//Gameplay speed
	float speed;
	//Hold the number of unorganized sections
	unsigned unorganized;
	//Saves whether all notes were readjusted for minimal glitching
	bool optimized;
	//List of all sections
	List<SongSection> sections;
	//Holds all life-value data for every player and phase type
	struct EnergyDamage
	{
		float start = .2f;
		float chargeInitial = .01f;
		float attackInitial = .015f;
		float guardEnergy = .015f;
		float attackMiss = .05f;
		float guardMiss = .05f;
		float chargeRelease = .025f;
		float attackRelease = .025f;
	} energyDamageFactors[4][5];
	//0 - Not saved
	//1 - Saved
	//2 - Saved at the currently pointed location
	char saved;
public:
	CHC();
	CHC(const CHC&);
	CHC(std::string filename);
	CHC& operator=(CHC&);
	void create(std::string filename);
};




