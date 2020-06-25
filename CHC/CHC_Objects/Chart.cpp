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
Chart::Chart() : tracelines(1)
{
	size = 76;
	pivotTime = endTime = 0;
	numTracelines = 1;
	numPhrases = 0;
	numGuards = 0;
}

Chart::Chart(const Chart& chart) : tracelines(chart.tracelines), phrases(chart.phrases), guards(chart.guards)
{
	size = chart.size;
	for (unsigned index = 0; index < 16; index++) junk[index] = chart.junk[index];
	pivotTime = chart.pivotTime;
	endTime = chart.endTime;
	numTracelines = chart.numTracelines;
	numPhrases = chart.numPhrases;
	numGuards = chart.numGuards;
}

void Chart::operator=(const Chart chart)
{
	size = chart.size;
	for (unsigned index = 0; index < 16; index++) junk[index] = chart.junk[index];
	pivotTime = chart.pivotTime;
	endTime = chart.endTime;
	numTracelines = chart.numTracelines;
	numPhrases = chart.numPhrases;
	numGuards = chart.numGuards;
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
		throw "Index out of Trace line range: " + std::to_string(numTracelines) + '.';
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
		throw "Index out of Phrase bar range: " + std::to_string(numPhrases) + '.';
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
		throw "Index out of Guard mark range: " + std::to_string(numGuards) + '.';
	}
}

//Add a note to its corresponding List in ascending pivotAlpha order
unsigned Chart::add(Note* note)
{
	if (dynamic_cast<Traceline*>(note) != nullptr)		//Checks if it's a Traceline object
	{
		Traceline* trace = static_cast<Traceline*>(note);
		unsigned index = 0;
		for (; index < numTracelines; index++)
		{
			if (*trace == tracelines[index])		//Replace the note values
			{
				tracelines[index] = *trace;
				return index;
			}
			else if (*trace < tracelines[index])		//Insert before current iteration
				break;
		}
		numTracelines++;
		size += 16;
		tracelines.insert(index, *trace);
		return index;
	}
	else if (dynamic_cast<Phrase*>(note) != nullptr)		//Checks if it's a Phrase Bar object
	{
		Phrase* phrase = static_cast<Phrase*>(note);
		unsigned index = 0;
		for (; index < numPhrases; index++)
		{
			if (*phrase == phrases[index])		//Replace the note values
			{
				phrases[index] = *phrase;
				return index;
			}
			else if (*phrase < phrases[index])		//Insert before current iteration
				break;
		}
		numPhrases++;
		size += 32;
		phrases.insert(index, *phrase);
		return index;
	}
	else if (dynamic_cast<Guard*>(note) != nullptr)		//Checks if it's a Guard Mark object
	{
		Guard* guard = static_cast<Guard*>(note);
		unsigned index = 0;
		for (; index < numGuards; index++)
		{
			if (*guard == guards[index])		//Replace the note values
			{
				guards[index] = *guard;
				return index;
			}
			else if (*guard < guards[index])		//Insert before current iteration
				break;
		}
		numGuards++;
		size += 16;
		guards.insert(index, *guard);
		return index;
	}
	else
		throw "Invalid note type";
}

//Quickly push a new note to the end of its corresponding list based on type
void Chart::push_back(Note* note)
{
	if (dynamic_cast<Traceline*>(note) != nullptr)		//Checks if it's a Traceline object
	{
		numTracelines++;
		size += 16;
		tracelines.push_back(*static_cast<Traceline*>(note));
	}
	else if (dynamic_cast<Phrase*>(note) != nullptr)	//Checks if it's a Phrase Bar object
	{
		numPhrases++;
		size += 32;
		phrases.push_back(*static_cast<Phrase*>(note));
	}
	else if (dynamic_cast<Guard*>(note) != nullptr)		//Checks if it's a Guard Mark object
	{
		numGuards++;
		size += 16;
		guards.push_back(*static_cast<Guard*>(note));
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
		tracelines.resize(numElements);
		size += 16 * (numElements - numTracelines);
		numTracelines = numElements;
		return true;
	case 'P':
	case 'p':
		phrases.resize(numElements);
		size += 32 * (numElements - numPhrases);
		numPhrases = numElements;
		return true;
	case 'G':
	case 'g':
		guards.resize(numElements);
		size += 16 * (numElements - numGuards);
		numGuards = numElements;
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
bool Chart::remove(unsigned index, char type, unsigned long extra)
{
	switch (type)
	{
	case 'T':
	case 't':
		if ((numPhrases + numGuards || numTracelines > 1) && index < numTracelines)
		{
			tracelines.erase(index);
			size -= 16;
			numTracelines--;
			std::cout << "Trace line #" << index + extra << " removed" << std::endl;
			return true;
		}
		else
		{
			if (numTracelines == 1)
				std::cout << "Cannot delete this trace line as a chart must always have at least one note" << std::endl;
			else
				std::cout << "Index out of range - # of Trace Lines: " << numTracelines << std::endl;
			return false;
		}
	case 'P':
	case 'p':
		if (index < numPhrases)
		{
			phrases.erase(index);
			size -= 32;
			numPhrases--;
			std::cout << "Phrase bar #" << index + extra << " removed" << std::endl;
			return true;
		}
		else
		{
			std::cout << "Index out of range - # of Phrase bars: " << numPhrases << std::endl;
			return false;
		}
	case 'G':
	case 'g':
		if ((numPhrases + numTracelines || numGuards > 1) && index < numGuards)
		{
			guards.erase(index);
			size -= 16;
			numGuards--;
			std::cout << "Guard mark #" << index + extra << " removed" << std::endl;
			return true;
		}
		else
		{
			if (numGuards == 1)
				std::cout << "Cannot delete this Guard Mark as a chart must always have at least one note" << std::endl;
			else
				std::cout << "Index out of range - # of Guard mark: " << numGuards << std::endl;
			return false;
		}
	default:
		return false;
	}
}

//Full clear minus 1 Trace line
void Chart::clear()
{
	if (numTracelines > 1)
	{
		tracelines.erase(0, numTracelines - 1);
		size -= 16 * (numTracelines - 1);
		numTracelines = 1;
	}
	phrases.clear();
	size -= 32 * numPhrases;
	numPhrases = 0;
	guards.clear();
	size -= 16 * numGuards;
	numGuards = 0;

}

//Full clear of Trace lines
void Chart::clearTracelines()
{
	tracelines.clear();
	size -= 16 * numTracelines;
	numTracelines = 0;
}

//Full clear of Phrase bars
void Chart::clearPhrases()
{
	phrases.clear();
	size -= 32 * numPhrases;
	numPhrases = 0;
}

//Full clear of Guard marks
void Chart::clearGuards()
{
	guards.clear();
	size -= 16 * numGuards;
	numGuards = 0;
}
