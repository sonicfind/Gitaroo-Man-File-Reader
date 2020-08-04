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
	unsigned long size = 76;
	//Just junk, saved for consistency
	char junk[16] = { 0 };

	//'Center' point that all notes revolve around
	long pivotTime = 0;
	//Optional value noting when to transition to another chart/subsection
	//Unused in the game
	long endTime = 0;
	//List of all trace lines
	List<Traceline> tracelines;
	//List of all phrase bars
	List<Phrase> phrases;
	//List of all guard marks
	List<Guard> guards;
public:
	Chart();
	Chart(const Chart&);
	void operator=(const Chart);
	//Returns the byte size of the chart/subsection
	unsigned long getSize() const { return size; }
	//Sets the byte size of the chart/subsection to the provided value
	void setSize(unsigned long siz) { size = siz; }
	//Adjusts the byte size of the chart/subsection by the provided value
	void adjustSize(long difference);
	//Returns junk C-string (size: 16)
	char* getJunk() { return junk; }
	//Copies C-string newJunk to junk
	//Size "count" will be limited to a max of 16
	void setJunk(char* newJunk, size_t count = 16);
	//Returns the pivot time for the chart/subsection
	long getPivotTime() const { return pivotTime; }
	//Sets the pivot time for the chart/subsection to the provided value
	void setPivotTime(long piv) { pivotTime = piv; }
	//Adjusts the pivot time for the chart/subsection by the provided value
	void adjustPivotTime(long difference) { pivotTime += difference; }
	//Returns the end time for the chart/subsection
	long getEndTime() const { return endTime; }
	//Sets the end time for the chart/subsection to the provided value
	void setEndTime(long piv) { endTime = piv; }
	//Returns the number of the trace lines in the chart/subsection
	size_t getNumTracelines() const { return tracelines.size(); }
	//Returns the Trace line at the provided index
	Traceline& getTraceline(size_t index);
	template<class... Args>
	size_t emplaceTraceline(Args&&... args)
	{
		size += 16;
		return tracelines.emplace_ordered(args...);
	}

	//Returns the number of the phrase bars in the chart/subsection
	size_t getNumPhrases() const { return phrases.size(); }
	//Returns the Phrase bar at the provided index
	Phrase& getPhrase(size_t index);
	template<class... Args>
	size_t emplacePhrase(Args&&... args)
	{
		size += 32;
		return phrases.emplace_ordered(args...);
	}

	//Returns the number of the guard marks in the chart/subsection
	size_t getNumGuards() const { return guards.size(); }
	//Returns the Guard mark at the provided index
	Guard& getGuard(size_t index);
	template<class... Args>
	size_t emplaceGuard(Args&&... args)
	{
		size += 16;
		return guards.emplace_ordered(args...);
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
	long insertNotes(Chart& source);
};