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
#include "..\..\Header\pch.h"
#include "Chart.h"
#include <algorithm>

//Creates Chart object with 1 Trace line
Chart::Chart(const bool tracelines) : m_size(76), m_pivotTime(0), m_endTime(0)
{
	m_size = 60;
	m_pivotTime = m_endTime = 0;
	if (tracelines)
	{
		emplaceTraceline();
		++m_endTime;
	}
}

Chart::Chart() : Chart(true) {}

void Chart::operator=(const Chart& chart)
{
	m_size = chart.m_size;
	std::copy(chart.m_junk, chart.m_junk + 16, m_junk);
	m_pivotTime = chart.m_pivotTime;
	m_endTime = chart.m_endTime;
	m_tracelines = chart.m_tracelines;
	m_phrases = chart.m_phrases;
	m_guards = chart.m_guards;
}

void Chart::adjustSize(long difference)
{
	if ((long)m_size + difference >= 0)
		m_size += difference;
	else
		m_size = 0;
}

void Chart::setJunk(char* newJunk, rsize_t count)
{
	std::copy(newJunk, newJunk + count, m_junk);
}

Traceline& Chart::getTraceline(size_t index)
{
	try
	{
		return m_tracelines[index];
	}
	catch (...)
	{
		throw "Index out of Trace line range: " + std::to_string(m_tracelines.size()) + '.';
	}
}

Phrase& Chart::getPhrase(size_t index)
{
	try
	{
		return m_phrases[index];
	}
	catch (...)
	{
		throw "Index out of Phrase bar range: " + std::to_string(m_phrases.size()) + '.';
	}
}

Guard& Chart::getGuard(size_t index)
{
	try
	{
		return m_guards[index];
	}
	catch (...)
	{
		throw "Index out of Guard mark range: " + std::to_string(m_guards.size()) + '.';
	}
}

//Add a note to its corresponding List in ascending pivotAlpha order
size_t Chart::add(Note* note)
{
	if (dynamic_cast<Traceline*>(note) != nullptr)		//Checks if it's a Traceline object
	{
		m_size += 16;
		return m_tracelines.emplace_ordered(*static_cast<Traceline*>(note));
	}
	else if (dynamic_cast<Phrase*>(note) != nullptr)		//Checks if it's a Phrase Bar object
	{
		m_size += 32;
		return m_phrases.emplace_ordered(*static_cast<Phrase*>(note));
	}
	else if (dynamic_cast<Guard*>(note) != nullptr)		//Checks if it's a Guard Mark object
	{
		m_size += 16;
		return m_guards.emplace_ordered(*static_cast<Guard*>(note));
	}
	else
		throw "Invalid note type";
}

//Quickly push a new note to the end of its corresponding list based on type
void Chart::add_back(Note* note)
{
	if (dynamic_cast<Traceline*>(note) != nullptr)		//Checks if it's a Traceline object
	{
		m_size += 16;
		m_tracelines.emplace_back(*static_cast<Traceline*>(note));
	}
	else if (dynamic_cast<Phrase*>(note) != nullptr)	//Checks if it's a Phrase Bar object
	{
		m_size += 32;
		m_phrases.emplace_back(*static_cast<Phrase*>(note));
	}
	else if (dynamic_cast<Guard*>(note) != nullptr)		//Checks if it's a Guard Mark object
	{
		m_size += 16;
		m_guards.emplace_back(*static_cast<Guard*>(note));
	}
	else
	{
		throw "Not a valid Note sub-type";
	}
}

//Resizes the list of the chosen note type to the value provided
//Type - Defaults to trace line ('t').
bool Chart::resize(long numElements, char type)
{
	switch (type)
	{
	case 'T':
	case 't':
		m_size += 16 * (numElements - (long)m_tracelines.size());
		m_tracelines.resize(numElements);
		return true;
	case 'P':
	case 'p':
		m_size += 32 * (numElements - (long)m_phrases.size());
		m_phrases.resize(numElements);
		return true;
	case 'G':
	case 'g':
		m_size += 16 * (numElements - (long)m_guards.size());
		m_guards.resize(numElements);
		return true;
	default:
		return false;
	}
}

//Removes the element at the given index out of the chosen list.
//If removed, it will print to the output window the event using the "extra" parameter to adjust the position value and return true.
//Otherwise, it'll print a failure and return false.
//Type -- defaults to trace line ('t').
//Extra - defaults to 0.
bool Chart::remove(size_t index, char type, size_t extra)
{
	switch (type)
	{
	case 'T':
	case 't':
		if ((m_phrases.size() + m_guards.size() || m_tracelines.size() > 1) && index < m_tracelines.size())
		{
			m_size -= 16;
			m_tracelines.erase(index);
			return true;
		}
		else
		{
			if (m_tracelines.size() != 1)
				printf("Index out of range - # of Trace Lines: %zu\n", m_tracelines.size());
			return false;
		}
	case 'P':
	case 'p':
		if (index < m_phrases.size())
		{
			m_size -= 32;
			m_phrases.erase(index);
			return true;
		}
		else
		{
			printf("Index out of range - # of Phrase bars: %zu\n", m_phrases.size());
			return false;
		}
	case 'G':
	case 'g':
		if ((m_phrases.size() + m_tracelines.size() || m_guards.size() > 1) && index < m_guards.size())
		{
			m_size -= 16;
			m_guards.erase(index);
			return true;
		}
		else
		{
			if (m_guards.size() != 1)
				printf("Index out of range - # of Guard mark: %zu\n", m_guards.size());
			return false;
		}
	default:
		return false;
	}
}

//Full clear minus 1 Trace line
void Chart::clear()
{
	if (m_tracelines.size() > 1)
	{
		m_size -= unsigned long(16 * (m_tracelines.size() - 1));
		m_tracelines.erase(0, m_tracelines.size() - 1);
	}
	m_size -= unsigned long(32 * m_phrases.size());
	m_phrases.clear();
	m_size -= unsigned long(16 * m_guards.size());
	m_guards.clear();
}

//Full clear of Trace lines
void Chart::clearTracelines()
{
	m_size -= unsigned long(16 * m_tracelines.size());
	m_tracelines.clear();
}

//Full clear of Phrase bars
void Chart::clearPhrases()
{
	m_size -= unsigned long(32 * m_phrases.size());
	m_phrases.clear();
}

//Full clear of Guard marks
void Chart::clearGuards()
{
	m_size -= unsigned long(16 * m_guards.size());
	m_guards.clear();
}

long Chart::insertNotes(Chart* source)
{
	long lastNote = 0;
	clearPhrases();
	for (LinkedList::List<Phrase>::Iterator phr = source->m_phrases.begin();
		phr != source->m_phrases.end(); ++phr)
	{
		lastNote = (*phr).getPivotAlpha();
		if (!(*phr).getEnd())
		{
			if (phr + 1 == source->m_phrases.end())
				(*phr).setEnd(true);
			else
				(*phr).changeEndAlpha((*(phr + 1)).getPivotAlpha());
		}
		//Pivot alpha was previous set to the total displacement from the start of the section
		m_phrases.emplace_back(*phr).adjustPivotAlpha(-m_pivotTime);
		m_size += 32;
	}

	clearTracelines();
	for (LinkedList::List<Traceline>::Iterator trace = source->m_tracelines.begin();
		trace != source->m_tracelines.end(); ++trace)
	{
		if ((*trace).getPivotAlpha() > lastNote)
			lastNote = (*trace).getPivotAlpha();
		if (trace + 1 != source->m_tracelines.end())
			(*trace).changeEndAlpha((*(trace + 1)).getPivotAlpha());
		m_tracelines.emplace_back(*trace).adjustPivotAlpha(-m_pivotTime);
		m_size += 16;
	}
	//Go through every phrase bar & trace line to find places where phrase bars
	//should be split into two pieces
	{
		LinkedList::List<Phrase>::Iterator phr = m_phrases.begin();
		for (LinkedList::List<Traceline>::Iterator tr = m_tracelines.begin();
			tr != m_tracelines.end(); ++tr)
		{
			Traceline& trace = *tr;
			while (phr != m_phrases.end())
			{
				Phrase* phrase = &*phr;
				if (tr == m_tracelines.begin())
				{
					if (trace.getPivotAlpha() > phrase->getPivotAlpha())
						trace.changePivotAlpha(phrase->getPivotAlpha());
					break;
				}
				else if (trace.getPivotAlpha() >= phrase->getEndAlpha())
					++phr;
				else if (trace.getPivotAlpha() > phrase->getPivotAlpha())
				{

					unsigned long dur = phrase->getEndAlpha() - trace.getPivotAlpha();
					bool end = phrase->getEnd();
					phrase->changeEndAlpha(trace.getPivotAlpha());
					phrase->setEnd(false);
					emplacePhrase(trace.getPivotAlpha(), dur, false, end, 0, phrase->getColor());
					++phr;
					break;
				}
				else if (tr.getIndex() + 1 == m_tracelines.size())
				{
					//If the phrase bar lands at or after the last trace line, delete
					if (phrase->getPivotAlpha() >= trace.getPivotAlpha())
					{
						if (!phrase->getStart())
							(*(phr - 1)).setEnd(true);
						if (remove((phr--).getIndex(), 'p'))
							printf("Phrase bar %zu removed\n", (++phr).getIndex());
					}
					else if (phrase->getEndAlpha() > trace.getPivotAlpha())
					{
						phrase->changeEndAlpha(trace.getPivotAlpha());
						phrase->setEnd(true);
						++phr;
					}
				}
				else
					break;
			}
		}
	}
	clearGuards();
	for (LinkedList::List<Guard>::Iterator grd = source->m_guards.begin();
		grd != source->m_guards.end(); ++grd)
	{
		if ((*grd).getPivotAlpha() > lastNote)
			lastNote = (*grd).getPivotAlpha();
		m_guards.emplace_back(*grd).adjustPivotAlpha(-m_pivotTime);
		m_size += 16;
	}
	return lastNote;
}
