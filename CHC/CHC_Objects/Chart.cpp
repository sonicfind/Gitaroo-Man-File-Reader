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

//Creates Chart object with 1 Trace line
Chart::Chart() : size(76), pivotTime(0), endTime(0), tracelines(1)
{
	size = 76;
	pivotTime = endTime = 0;
}

Chart::Chart(const Chart& chart)
	: size(chart.size), pivotTime(chart.pivotTime), endTime(chart.endTime), tracelines(chart.tracelines), phrases(chart.phrases), guards(chart.guards) {
	for (unsigned index = 0; index < 16; index++)
		junk[index] = chart.junk[index];
}

void Chart::operator=(const Chart chart)
{
	size = chart.size;
	for (unsigned index = 0; index < 16; index++)
		junk[index] = chart.junk[index];
	pivotTime = chart.pivotTime;
	endTime = chart.endTime;
	tracelines = chart.tracelines;
	phrases = chart.phrases;
	guards = chart.guards;
}

void Chart::adjustSize(long difference)
{
	if ((long)size + difference >= 0)
		size += difference;
	else
		size = 0;
}

void Chart::setJunk(char* newJunk, size_t count)
{
	if (count > 16)
		count = 16;
	for (unsigned index = 0; index < count; index++)
		junk[index] = newJunk[index];
}

Traceline& Chart::getTraceline(size_t index)
{
	try
	{
		return tracelines[index];
	}
	catch (...)
	{
		throw "Index out of Trace line range: " + std::to_string(tracelines.size()) + '.';
	}
}

Phrase& Chart::getPhrase(size_t index)
{
	try
	{
		return phrases[index];
	}
	catch (...)
	{
		throw "Index out of Phrase bar range: " + std::to_string(phrases.size()) + '.';
	}
}

Guard& Chart::getGuard(size_t index)
{
	try
	{
		return guards[index];
	}
	catch (...)
	{
		throw "Index out of Guard mark range: " + std::to_string(guards.size()) + '.';
	}
}

//Add a note to its corresponding List in ascending pivotAlpha order
size_t Chart::add(Note* note)
{
	if (dynamic_cast<Traceline*>(note) != nullptr)		//Checks if it's a Traceline object
	{
		size += 16;
		return tracelines.emplace_ordered(*static_cast<Traceline*>(note));
	}
	else if (dynamic_cast<Phrase*>(note) != nullptr)		//Checks if it's a Phrase Bar object
	{
		size += 32;
		return phrases.emplace_ordered(*static_cast<Phrase*>(note));
	}
	else if (dynamic_cast<Guard*>(note) != nullptr)		//Checks if it's a Guard Mark object
	{
		size += 16;
		return guards.emplace_ordered(*static_cast<Guard*>(note));
	}
	else
		throw "Invalid note type";
}

//Quickly push a new note to the end of its corresponding list based on type
void Chart::add_back(Note* note)
{
	if (dynamic_cast<Traceline*>(note) != nullptr)		//Checks if it's a Traceline object
	{
		size += 16;
		tracelines.emplace_back(*static_cast<Traceline*>(note));
	}
	else if (dynamic_cast<Phrase*>(note) != nullptr)	//Checks if it's a Phrase Bar object
	{
		size += 32;
		phrases.emplace_back(*static_cast<Phrase*>(note));
	}
	else if (dynamic_cast<Guard*>(note) != nullptr)		//Checks if it's a Guard Mark object
	{
		size += 16;
		guards.emplace_back(*static_cast<Guard*>(note));
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
		size += 16 * (numElements - (long)tracelines.size());
		tracelines.resize(numElements);
		return true;
	case 'P':
	case 'p':
		size += 32 * (numElements - (long)phrases.size());
		phrases.resize(numElements);
		return true;
	case 'G':
	case 'g':
		size += 16 * (numElements - (long)guards.size());
		guards.resize(numElements);
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
		if ((phrases.size() + guards.size() || tracelines.size() > 1) && index < tracelines.size())
		{
			size -= 16;
			tracelines.erase(index);
			printf("Trace line %zu removed\n", index + extra);
			return true;
		}
		else
		{
			if (tracelines.size() == 1)
				printf("Cannot delete this trace line as a chart must always have at least one note\n");
			else
				printf("Index out of range - # of Trace Lines: %zu\n", tracelines.size());
			return false;
		}
	case 'P':
	case 'p':
		if (index < phrases.size())
		{
			size -= 32;
			phrases.erase(index);
			printf("Phrase bar %zu removed\n", index + extra);
			return true;
		}
		else
		{
			printf("Index out of range - # of Phrase bars: %zu\n", phrases.size());
			return false;
		}
	case 'G':
	case 'g':
		if ((phrases.size() + tracelines.size() || guards.size() > 1) && index < guards.size())
		{
			size -= 16;
			guards.erase(index);
			printf("Guard mark %zu removed\n", index + extra);
			return true;
		}
		else
		{
			if (guards.size() == 1)
				printf("Cannot delete this Guard Mark as a chart must always have at least one note\n");
			else
				printf("Index out of range - # of Guard mark: %zu\n", guards.size());
			return false;
		}
	default:
		return false;
	}
}

//Full clear minus 1 Trace line
void Chart::clear()
{
	if (tracelines.size() > 1)
	{
		size -= unsigned long(16 * (tracelines.size() - 1));
		tracelines.erase(0, tracelines.size() - 1);
	}
	size -= unsigned long(32 * phrases.size());
	phrases.clear();
	size -= unsigned long(16 * guards.size());
	guards.clear();
}

//Full clear of Trace lines
void Chart::clearTracelines()
{
	size -= unsigned long(16 * tracelines.size());
	tracelines.clear();
}

//Full clear of Phrase bars
void Chart::clearPhrases()
{
	size -= unsigned long(32 * phrases.size());
	phrases.clear();
}

//Full clear of Guard marks
void Chart::clearGuards()
{
	size -= unsigned long(16 * guards.size());
	guards.clear();
}
