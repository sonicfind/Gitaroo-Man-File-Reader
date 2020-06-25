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
#include "SongSection.h"
//Create a SongSection object with 1 Condition and 4 Charts
SongSection::SongSection() : conditions(1), charts(4) {}
SongSection::SongSection(const SongSection& section) : conditions(section.conditions), charts(section.charts)
{
	index = section.index;
	frames = section.frames;
	organized = section.organized;
	swapped = section.swapped;
	for (unsigned index = 0; index < 16; index++) name[index] = section.name[index];
	for (unsigned index = 0; index < 16; index++) audio[index] = section.audio[index];
	size = section.size;
	for (unsigned index = 0; index < 16; index++) junk[index] = section.junk[index];
	battlePhase = section.battlePhase;
	tempo = section.tempo;
	duration = section.duration;
	numConditions = section.numConditions;
	numPlayers = section.numPlayers;
	numCharts = section.numCharts;
}

void SongSection::operator=(const SongSection section)
{
	index = section.index;
	frames = section.frames;
	organized = section.organized;
	swapped = section.swapped;
	for (unsigned index = 0; index < 16; index++) name[index] = section.name[index];
	for (unsigned index = 0; index < 16; index++) audio[index] = section.audio[index];
	size = section.size;
	for (unsigned index = 0; index < 16; index++) junk[index] = section.junk[index];
	battlePhase = section.battlePhase;
	tempo = section.tempo;
	duration = section.duration;
	numConditions = section.numConditions;
	conditions = section.conditions;
	numPlayers = section.numPlayers;
	numCharts = section.numCharts;
	charts = section.charts;
}

//Add the condition to the list, at the given index
unsigned SongSection::add(unsigned index, Condition& condition)
{
	if (index > numConditions) index = numConditions;
	numConditions++;
	size += 16;
	conditions.insert(index, condition);
	return index;
}

//Returns the condition at the provided index
//An index >= the number of conditions will return the last condition
SongSection::Condition& SongSection::getCondition(size_t index)
{
	try
	{
		return conditions[index];
	}
	catch (...)
	{
		std::cout << global.tabs << "Index out of Condition range: " << numConditions << ". Returning the last condition." << std::endl;
		return conditions.back();
	}
}

//I think this is obvious
bool SongSection::removeCondition(unsigned index)
{
	if (numConditions != 1 && index < numConditions)
	{
		conditions.erase(index);
		size -= 16;
		numConditions--;
		std::cout << "Condition " << index + 1 << " removed" << std::endl;
		return true;
	}
	else
	{
		if (numConditions == 1)
			std::cout << global.tabs << "Cannot delete condition - a section must have at least 1 condition" << std::endl;
		else
			std::cout << global.tabs << "Index out of range - # of Conditions: " << numConditions << std::endl;
		return false;
	}
}

//Clears all conditions minus 1
void SongSection::clearConditions()
{
	if (numConditions > 1)
	{
		conditions.erase(0, numConditions - 1); //Leave the last condition as it will ALWAYS point to another section
		size -= 16 * (numConditions - 1);
		numConditions = 1;
	}
}

//Add a chart to the end of every player's sector
void SongSection::operator++()
{
	size += numPlayers * 72;
	for (size_t player = 0; player < numPlayers; player++)
		charts.insert(player + (player * numCharts) + numCharts, Chart());
	numCharts++;
}

//Remove a chart from the end of every player's sector
bool SongSection::operator--()
{
	if (numCharts > 1)
	{
		for (size_t player = numPlayers; player > 0;)
		{
			size -= charts[(--player * numCharts) + (numCharts - 1)].getSize();
			charts.erase((player * numCharts) + (numCharts - 1));
		}
		numCharts--;
		return true;
	}
	else
		return false;
}