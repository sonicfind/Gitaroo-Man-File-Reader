/*  Gitaroo Man File Reader
 *  Copyright (C) 2020-2021 Gitaroo Pals
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
#include "pch.h"
#include "CH_Object.h"
using namespace std;

CHObject::CHObject()
	: m_position_ticks(0) {}

CHObject::CHObject(float pos_ticks)
	: m_position_ticks(pos_ticks) {}

SyncTrack::SyncTrack(float pos, unsigned long timeSigNumer, unsigned long tempo, unsigned long timeSigDenom)
	: CHObject(pos)
	, m_timeSigNumerator(timeSigNumer)
	, m_bpm(tempo)
	, m_timeSigDenomSelection(timeSigDenom) {}

SyncTrack::SyncTrack(FILE* inFile)
{
	char type;
	fscanf_s(inFile, " %f = %c", &m_position_ticks, &type, 1);
	if (type == 'T')
	{
		fscanf_s(inFile, "S %lu", &m_timeSigNumerator);
		fscanf_s(inFile, "%c", &type, 1);
		if (type == ' ')
			fscanf_s(inFile, "%lu", &m_timeSigDenomSelection);
	}
	else if (type == 'B')
		fscanf_s(inFile, " %lu", &m_bpm);
}

void SyncTrack::write(FILE* outFile)
{
	if (m_timeSigNumerator)
	{
		if (m_timeSigDenomSelection != 2)
			fprintf(outFile, "  %lu = TS %lu %lu\n", (unsigned long)round(m_position_ticks), m_timeSigNumerator, m_timeSigDenomSelection);
		else
			fprintf(outFile, "  %lu = TS %lu\n", (unsigned long)round(m_position_ticks), m_timeSigNumerator);
	}
		
	if (m_bpm)
		fprintf(outFile, "  %lu = B %lu\n", (unsigned long)round(m_position_ticks), m_bpm);
};

Event::Event(FILE* inFile)
{
	char bagel[101] = { 0 };
	fscanf_s(inFile, " %f = E %[^\n]", &m_position_ticks, bagel, 100);
	m_name = bagel;
	if (m_name[0] == '\"')
		m_name = m_name.substr(1, m_name.length() - 2); //Gets rid of ""
};

void Event::write(FILE* outFile)
{
	fprintf(outFile, "  %lu = E \"%s\"\n", (unsigned long)round(m_position_ticks), m_name.c_str());
};

CHNote::CHNote(float pos)
	: CHObject(pos)
{
	for (unsigned index = 0; index < 6; index++)
		m_colors[index].m_parent = this;
	m_star.m_parent = this;
}

CHNote::CHNote(float pos, std::string& ev)
	: CHNote(pos)
{
	m_events.push_back(ev);
}

CHNote::CHNote(float pos, int frets, float sustain, bool write)
	: CHNote(pos)
{
	for (unsigned index = 0; index < 6; index++)
		if (frets & (1 << index))
		{
			m_colors[index].m_active = true;
			m_colors[index].m_sustain = sustain;
			m_colors[index].m_writeSustain = write;
		}
}

CHNote::CHNote(float pos, float end)
	: CHNote(pos)
{
	m_star.m_active = true;
	m_star.m_sustain = end - pos;
}

CHNote::CHNote(FILE* inFile)
{
	char type = 0;
	fscanf_s(inFile, " %f = %c", &m_position_ticks, &type, 1);
	if (type == 'E')
	{
		char bagel[101] = { 0 };
		fscanf_s(inFile, " %[^\n]s", bagel, 100);
		m_events.push_back(bagel);
	}
	else
	{
		unsigned lane;
		float sustain;
		fscanf_s(inFile, " %u %f", &lane, &sustain);
		if (type != 'S')
		{
			switch (lane)
			{
			case 6:
				m_isTap = true;
				break;
			case 5:
				m_isForced = true;
				break;
			default:
				if (lane == 7)
					lane = 5;
				m_colors[lane].m_active = true;
				m_colors[lane].m_sustain = sustain;
			}
		}
		else
		{
			m_star.m_active = true;
			m_star.m_sustain = sustain;
		}
	}
};

CHNote::CHNote(const CHNote& note)
	: CHNote(note.m_position_ticks)
{
	for (unsigned index = 0; index < 6; index++)
		if (note.m_colors[index].m_active)
			m_colors[index] = note.m_colors[index];
	m_star = note.m_star;
	m_isForced = note.m_isForced;
	m_isTap = note.m_isTap;
	for (const string& ev : note.m_events)
		m_events.push_back(ev);
	m_events = note.m_events;
}

CHNote& CHNote::operator=(const CHNote& note)
{
	for (unsigned index = 0; index < 6; index++)
		if (note.m_colors[index].m_active)
			m_colors[index] = note.m_colors[index];

	if (m_colors[5].m_active)
	{
		if (m_isTap || note.m_isTap)
		{
			m_isForced = true;
			m_isTap = false;
		}
		else
			m_isForced |= note.m_isForced;
	}
	else
	{
		m_isForced |= note.m_isForced;
		m_isTap |= note.m_isTap;
	}

	if (note.m_star.m_active)
		m_star = note.m_star;

	for (const string& ev : note.m_events)
		m_events.push_back(ev);

	return *this;
}

void CHNote::Fret::setEndPoint(float endTick, float threshold)
{
	m_sustain = endTick - m_parent->m_position_ticks;

	if (threshold > 0)
		m_writeSustain = m_sustain >= threshold;
}

void CHNote::write(FILE* outFile) const
{
	const unsigned long position = (unsigned long)round(m_position_ticks);
	for (int lane = 0; lane < 6; ++lane)
	{
		if (m_colors[lane].m_active)
		{
			fprintf(outFile, "  %lu = N %u %lu\n"
				, position
				, lane != 5 ? lane : 7
				, m_colors[lane].m_writeSustain ? (unsigned long)round(m_colors[lane].m_sustain) : 0UL);
		}
	}

	if (m_isForced)
		fprintf(outFile, "  %lu = N 5 0\n", position);

	if (m_isTap)
		fprintf(outFile, "  %lu = N 6 0\n", position);

	if (m_star.m_active)
		fprintf(outFile, "  %lu = S 2 %lu\n", position, (unsigned long)round(m_star.m_sustain));

	for (const string& ev : m_events)
		fprintf(outFile, "  %lu = E %s\n", position, ev.c_str());	
};

bool CHNote::hasColors() const
{
	for (int lane = 0; lane < 6; ++lane)
		if (m_colors[lane].m_active)
			return true;
	return false;
}

CHNote::Fret& CHNote::Fret::operator=(const Fret& note)
{
	m_active = note.m_active;
	m_sustain = note.m_sustain;
	m_writeSustain = note.m_writeSustain;
	return *this;
}
