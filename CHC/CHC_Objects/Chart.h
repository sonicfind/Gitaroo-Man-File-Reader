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
#ifndef GLOBALFUNCTIONS_API
#include "List.h"
#endif
#include "Notes.h"

class Chart
{
	friend class CHC_Editor;
	friend class CHC;
	friend class CH_Importer;
private:
	//Total size in bytes
	unsigned long m_size = 76;
	//Just junk, saved for consistency
	char m_junk[16] = { 0 };

	//'Center' point that all notes revolve around
	long m_pivotTime = 0;
	//Optional value noting when to transition to another chart/subsection
	//Unused in the game
	long m_endTime = 0;
	//Linked list of all trace lines
	LinkedList::List<Traceline> m_tracelines;
	//Linked list of all phrase bars
	LinkedList::List<Phrase> m_phrases;
	//Linked list of all guard marks
	LinkedList::List<Guard> m_guards;
public:
	Chart(const bool tracelines);
	Chart();
	Chart(const Chart&) = default;
	void operator=(const Chart&);
	//Returns the byte size of the chart/subsection
	unsigned long getSize() const { return m_size; }
	//Sets the byte size of the chart/subsection to the provided value
	void setSize(unsigned long siz) { m_size = siz; }
	//Adjusts the byte size of the chart/subsection by the provided value
	void adjustSize(long difference);
	//Returns junk C-string (size: 16)
	char* getJunk() { return m_junk; }
	//Copies C-string newJunk to junk
	//Size "count" will be limited to a max of 16
	void setJunk(char* newJunk, rsize_t count = 16);
	//Returns the pivot time for the chart/subsection
	long getPivotTime() const { return m_pivotTime; }
	//Sets the pivot time for the chart/subsection to the provided value
	void setPivotTime(long piv) { m_pivotTime = piv; }
	//Adjusts the pivot time for the chart/subsection by the provided value
	void adjustPivotTime(long difference) { m_pivotTime += difference; }
	//Returns the end time for the chart/subsection
	long getEndTime() const { return m_endTime; }
	//Sets the end time for the chart/subsection to the provided value
	void setEndTime(long piv) { m_endTime = piv; }
	//Returns the number of the trace lines in the chart/subsection
	size_t getNumTracelines() const { return m_tracelines.size(); }
	//Returns the Trace line at the provided index
	Traceline& getTraceline(size_t index);
	template<class... Args>
	size_t emplaceTraceline(Args&&... args)
	{
		m_size += 16;
		return m_tracelines.emplace_ordered(args...);
	}

	//Returns the number of the phrase bars in the chart/subsection
	size_t getNumPhrases() const { return m_phrases.size(); }
	//Returns the Phrase bar at the provided index
	Phrase& getPhrase(size_t index);
	template<class... Args>
	size_t emplacePhrase(Args&&... args)
	{
		m_size += 32;
		return m_phrases.emplace_ordered(args...);
	}

	//Returns the number of the guard marks in the chart/subsection
	size_t getNumGuards() const { return m_guards.size(); }
	//Returns the Guard mark at the provided index
	Guard& getGuard(size_t index);
	template<class... Args>
	size_t emplaceGuard(Args&&... args)
	{
		m_size += 16;
		return m_guards.emplace_ordered(args...);
	}

	size_t add(Note*);
	void add_back(Note*);
	bool resize(long numElements, char type = 't');
	bool remove(size_t, char type = 't', size_t extra = 0);
	void clearTracelines();
	void clearPhrases();
	void clearGuards();
	void clear();
	//Takes the notes from the source chart and moves them into
	//the current chart (replacing any old notes that would overlap
	long insertNotes(Chart* source);
};
