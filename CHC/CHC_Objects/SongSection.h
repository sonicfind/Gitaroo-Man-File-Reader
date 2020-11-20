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
	unsigned long m_index = 0;
	//Section name
	//16 characters + a null character
	char m_name[17] = { 0 };

	//Name of Audio Section
	//16 characters + a null character
	char m_audio[17] = { 0 };

	//SSQ frame range
	SSQ m_frames;
	//Saves whether the subsections are correctly ordered
	unsigned long m_organized = false;
	//Value that holds how the section is swapped
	unsigned long m_swapped = 0;
	//Total size in bytes
	unsigned long m_size = 384;
	//Just junk, saved for consistency
	char m_junk[16] = { 0 };

	//Phase type
	Phase m_battlePhase = Phase::INTRO;
	//BPM for the section
	float m_tempo = 0;
	//Total duration in samples
	unsigned long m_duration = 0;
	//Determines what actions to take after the section
	struct Condition
	{
		unsigned long m_type;
		float m_argument;
		long m_trueEffect;
		long m_falseEffect;
		Condition();
		Condition(FILE* inFile);
		Condition(const Condition& cond) = default;
	};
	//LinkedList::List of all conditions
	LinkedList::List<Condition> m_conditions;
	//Number of players assigned to the section, always 4
	unsigned long m_numPlayers = 4;
	//Number of charts/subsections per player
	unsigned long m_numCharts = 1;
	//LinkedList::List of all charts/subsections
	LinkedList::List<Chart> m_charts;
public:
	static const long s_SAMPLE_GAP = 1800;
	SongSection();
	SongSection(FILE* inFile);
	SongSection(const SongSection&) = default;
	void operator=(SongSection&);
	//Returns name C-string (size: 16)
	char* getName() { return m_name; }
	//Returns audio C-string (size: 16)
	char* getAudio() { return m_audio; }
	//Returns whether the section is organized
	bool getOrganized() const { return m_organized; }
	//Sets organized to the provided value
	void setOrganized(bool org) { m_organized = org; }
	//Returns swap value
	unsigned long getSwapped() const { return m_swapped; }
	//Sets swap value
	void setSwapped(char swap) { m_swapped = swap; }
	//Returns the byte size of the section
	unsigned long getSize() const { return m_size; }
	//Returns a section's phase type
	Phase getPhase() const { return m_battlePhase; }
	//Sets the section's phase type to the provided value
	void setPhase(Phase ph) { m_battlePhase = ph; }
	//Sets the section's phase type to the provided value converted into a phase type
	//If the provided value is 6 or greater, battlePhase is set to FINAL_I
	void setPhase(unsigned long ph)
	{
		if (ph < 6)
			m_battlePhase = static_cast<SongSection::Phase>(ph);
		else
			m_battlePhase = Phase::FINAL_I;
	}
	//Returns the tempo for the section
	float getTempo() const { return m_tempo; }
	//Returns the duration of the section
	unsigned long getDuration() const { return m_duration; }
	//Sets the duration of the section to the provided value
	void setDuration(unsigned long dur) { m_duration = dur; }
	//Returns the number of conditions in the section
	size_t getNumCondtions() const { return m_conditions.size(); }
	//Adds a new condition to the end of the section's condition list
	template<class...Args>
	size_t addCondition(size_t index, Args&&...args)
	{
		if (index > m_conditions.size())
			index = m_conditions.size();
		m_size += 16;
		m_conditions.emplace(index, 1, args...);
		return index;
	}
	Condition& getCondition(size_t index);
	bool removeCondition(size_t);
	//Returns the num of players assigned to this section.
	//Will usually be 4
	unsigned long getNumPlayers() const { return m_numPlayers; }
	//Returns the total number of the charts/subsections in this section
	unsigned long getNumCharts() const { return m_numCharts; }
	//Returns the total number of the charts/subsections in this section
	Chart& getChart(size_t index)
	{
		try
		{
			return m_charts[index];
		}
		catch (...)
		{
			throw "Index out of Chart range: " + std::to_string(m_numCharts) + '.';
		}
	}
	void clearConditions();
	void operator++();
	bool operator--();
};
