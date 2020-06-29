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
#ifndef GLOBALFUNCTIONS_API
#include "Global_Functions.h"
#endif
#include "Chart.h"

struct SSQ
{
	float first = 0;
	float last = 0;
};

class SongSection
{
	friend class CHC_Main;
	friend class CHC_Editor;
	friend class CHC;
public:
	enum class Phase { INTRO, CHARGE, BATTLE, FINAL_AG, HARMONY, END, FINAL_I };
private:
	//Index pulled from the list of SSQs in the CHC
	unsigned long index = 0;
	//Section name
	//16 characters + a null character
	char name[17] = { 0 };

	//Name of Audio Section
	//16 characters + a null character
	char audio[17] = { 0 };

	//SSQ frame range
	SSQ frames;
	//Saves whether the subsections are correctly ordered
	unsigned long organized = false;
	//Value that holds how the section is swapped
	unsigned long swapped = 0;
	//Total size in bytes
	unsigned long size = 384;
	//Just junk, saved for consistency
	char junk[16] = { 0 };

	//Phase type
	Phase battlePhase = Phase::INTRO;
	//BPM for the section
	float tempo = 0;
	//Total duration in samples
	unsigned long duration = 0;
	//Determines what actions to take after the section
	struct Condition
	{
		unsigned long type;
		float argument;
		long trueEffect;
		long falseEffect;
		Condition();
		Condition(FILE* inFile);
		Condition(Condition& cond);
	};
	//List of all conditions
	List<Condition> conditions;
	//Number of players assigned to the section, always 4
	unsigned long numPlayers = 4;
	//Number of charts/subsections per player
	unsigned long numCharts = 1;
	//List of all charts/subsections
	List<Chart> charts;
public:
	static const unsigned SAMPLE_GAP = 1800;
	SongSection();
	SongSection(FILE* inFile);
	SongSection(const SongSection&);
	void operator=(const SongSection);
	//Returns name C-string (size: 16)
	char* getName() { return name; }
	//Returns audio C-string (size: 16)
	char* getAudio() { return audio; }
	//Returns whether the section is organized
	bool getOrganized() const { return organized; }
	//Sets organized to the provided value
	void setOrganized(bool org) { organized = org; }
	//Returns swap value
	unsigned long getSwapped() const { return swapped; }
	//Sets swap value
	void setSwapped(char swap) { swapped = swap; }
	//Returns the byte size of the section
	unsigned long getSize() const { return size; }
	//Returns a section's phase type
	Phase getPhase() const { return battlePhase; }
	//Sets the section's phase type to the provided value
	void setPhase(Phase ph) { battlePhase = ph; }
	//Sets the section's phase type to the provided value converted into a phase type
	//If the provided value is 6 or greater, battlePhase is set to FINAL_I
	void setPhase(unsigned long ph)
	{
		if (ph < 6)
			battlePhase = static_cast<SongSection::Phase>(ph);
		else
			battlePhase = Phase::FINAL_I;
	}
	//Returns the tempo for the section
	float getTempo() const { return tempo; }
	//Returns the duration of the section
	unsigned long getDuration() const { return duration; }
	//Sets the duration of the section to the provided value
	void setDuration(unsigned long dur) { duration = dur; }
	//Returns the number of conditions in the section
	size_t getNumCondtions() const { return conditions.size(); }
	//Adds a new condition to the end of the section's condition list
	template<class...Args>
	size_t addCondition(size_t index, Args&&...args)
	{
		if (index > conditions.size())
			index = conditions.size();
		size += 16;
		conditions.emplace(index, 1, args...);
		return index;
	}
	Condition& getCondition(size_t index);
	bool removeCondition(unsigned);
	//Returns the num of players assigned to this section.
	//Will usually be 4
	unsigned long getNumPlayers() const { return numPlayers; }
	//Returns the total number of the charts/subsections in this section
	unsigned long getNumCharts() const { return numCharts; }
	//Returns the total number of the charts/subsections in this section
	Chart& getChart(size_t index)
	{
		try
		{
			return charts[index];
		}
		catch (...)
		{
			throw "Index out of Chart range: " + std::to_string(numCharts) + '.';
		}
	}
	void clearConditions();
	void operator++();
	bool operator--();
};
