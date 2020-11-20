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
SongSection::Condition::Condition() : m_type(0), m_argument(0), m_trueEffect(0), m_falseEffect(0) {}

SongSection::Condition::Condition(FILE* inFile)
{
	fread(&m_type, 4, 1, inFile);
	fread(&m_argument, 4, 1, inFile);
	fread(&m_trueEffect, 4, 1, inFile);
	fread(&m_falseEffect, 4, 1, inFile);
}

//Create a SongSection object with 1 Condition and 4 Charts
SongSection::SongSection()
	: m_index(0), m_organized(false), m_size(384), m_battlePhase(Phase::INTRO), m_tempo(0), m_duration(0), m_conditions(1), m_numPlayers(4), m_numCharts(1), m_charts(4) {}

//Uses a file to read in a few of the values
//Due to the context, no conditions or charts are created
SongSection::SongSection(FILE* inFile) : m_organized(false), m_size(384), m_battlePhase(Phase::INTRO), m_tempo(0), m_duration(0), m_numPlayers(0), m_numCharts(0)
{
	fread(&m_index, 4, 1, inFile);
	fread(&m_name, 1, 16, inFile);
	fread(&m_audio, 1, 16, inFile);
	fread(&m_frames, sizeof(SSQ), 1, inFile);
	fseek(inFile, 4, SEEK_CUR);
}

void SongSection::operator=(SongSection& section)
{
	m_index = section.m_index;
	m_frames = section.m_frames;
	m_organized = section.m_organized;
	m_swapped = section.m_swapped;
	std::copy(section.m_name, section.m_name + 16, m_name);
	std::copy(section.m_audio, section.m_audio + 16, m_audio);
	m_size = section.m_size;
	std::copy(section.m_junk, section.m_junk + 16, m_junk);
	m_battlePhase = section.m_battlePhase;
	m_tempo = section.m_tempo;
	m_duration = section.m_duration;
	m_conditions = section.m_conditions;
	m_numPlayers = section.m_numPlayers;
	m_numCharts = section.m_numCharts;
	m_charts = section.m_charts;
}

//Returns the condition at the provided index
//An index >= the number of conditions will return the last condition
SongSection::Condition& SongSection::getCondition(size_t index)
{
	try
	{
		return m_conditions[index];
	}
	catch (...)
	{
		printf("%sIndex out of Condition range: %zu. Returning the last condition.\n", g_global.tabs.c_str(), m_conditions.size());
		return m_conditions.back();
	}
}

//I think this is obvious
bool SongSection::removeCondition(size_t index)
{
	if (m_conditions.size() != 1 && index < m_conditions.size())
	{
		m_size -= 16;
		m_conditions.erase(index);
		printf("Condition %zu removed\n", index + 1);
		return true;
	}
	else
	{
		if (m_conditions.size() == 1)
			printf("%sCannot delete condition - a section must have at least 1 condition\n", g_global.tabs.c_str());
		else
			printf("%sIndex out of range - # of Conditions: %zu\n", g_global.tabs.c_str(), m_conditions.size());
		return false;
	}
}

//Clears all conditions minus 1
void SongSection::clearConditions()
{
	if (m_conditions.size() > 1)
	{
		m_size -= unsigned long(16 * (m_conditions.size() - 1));
		m_conditions.erase(0, m_conditions.size() - 1); //Leave the last condition as it will ALWAYS point to another section
	}
}

//Add a chart to the end of every player's sector
void SongSection::operator++()
{
	m_size += m_numPlayers * 72;
	for (size_t player = 0; player < m_numPlayers; player++)
		m_charts.emplace(player + (player * m_numCharts) + m_numCharts, 1);
	m_numCharts++;
}

//Remove a chart from the end of every player's sector
bool SongSection::operator--()
{
	if (m_numCharts > 1)
	{
		for (size_t player = m_numPlayers; player > 0;)
		{
			m_size -= m_charts[(--player * m_numCharts) + (m_numCharts - 1)].getSize();
			m_charts.erase((player * m_numCharts) + (m_numCharts - 1));
		}
		m_numCharts--;
		return true;
	}
	else
		return false;
}
