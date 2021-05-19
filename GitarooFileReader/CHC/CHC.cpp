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
#include "pch.h"
#include "CHC.h"
#include <algorithm>
using namespace std;

//Creates a CHC object with 1 songsection
CHC::CHC() : m_sections(1), m_stage(0), m_speed(0), m_unorganized(0), m_optimized(false), m_saved(2) {}

//Creates a CHC object using values from the CHC file pointed to by the provided filename.
//
//Value names chosen to be kept are based off the CHC tab in the Gitaroo Pals shoutwiki
CHC::CHC(string filename) : m_filename(filename + ".CHC"), m_saved(2)
{
	{
		size_t pos = filename.find_last_of('\\');
		m_shortname = filename.substr(pos != string::npos ? pos + 1 : 0);
	}
	GlobalFunctions::banner(" Loading " + m_shortname + ".CHC ");
	try
	{
		m_stage = stoi(m_shortname.substr(2, 2));
	}
	catch (...)
	{
		m_stage = -1;
	}
	union {
		char c[4];
		unsigned ui;
	} u;
	FILE* inFile;
	if (fopen_s(&inFile, m_filename.c_str(), "rb"))
		throw "Error: " + m_filename + " does not exist.";
	fread(m_header, 1, 36, inFile);
	if (!strstr(m_header, "SNGS"))
	{
		fclose(inFile);
		throw "Error: No 'SNGS' Tag at byte 0.";
	}
	std::copy(m_header + 4, m_header + 8, u.c);
	m_optimized = u.ui == 6145;
	fread(m_imc, 1, 256, inFile);
	bool duet = m_imc[0] == 0;
	fread(&m_events, sizeof(SSQRange), 4, inFile);
	fread(&m_audio, sizeof(AudioChannel), 8, inFile);
	fread(&m_speed, 4, 1, inFile);
	fread(u.c, 4, 1, inFile);
	m_unorganized = u.ui;
	// Uses FILE* constructor to read section cue data
	for (size_t index = 0; index <  u.ui; ++index)
		m_sections.emplace_back(inFile);
	fseek(inFile, 4, SEEK_CUR);
	bool reorganized = false;
	for (size_t sectIndex = 0; sectIndex < m_sections.size(); sectIndex++) //SongSections
	{
		fread(u.c, 1, 4, inFile);
		if (!strstr(u.c, "CHLS"))
		{
			fclose(inFile);
			throw "Error: No 'CHLS' Tag for section #" + to_string(sectIndex) + " [File offset: " + to_string(ftell(inFile) - 4) + "].";
		}
		SongSection& section = m_sections[sectIndex];
		fread(u.c, 1, 4, inFile);
		if (u.ui & 1)
		{
			section.m_organized = true;
			m_unorganized--;
		}
		section.m_swapped = (u.ui - 4864) / 2;
		fread(&section.m_size, 4, 1, inFile);
		if (!section.m_organized)
		{
			fread(&section.m_organized, 4, 1, inFile);
			m_unorganized -= section.m_organized;
		}
		else
			fseek(inFile, 4, SEEK_CUR);
		if (!section.m_swapped)
			fread(&section.m_swapped, 4, 1, inFile);
		else
			fseek(inFile, 4, SEEK_CUR);
		if ((m_stage == 11 || m_stage == 12) && !duet && section.m_swapped < 4)
		{
			section.m_swapped += 4;
			printf("%sSection #%zu (%s)'s swap value was adjusted to match current implementation for Duet->PS2 conversions. Make sure to save this file to apply this change.\n", g_global.tabs.c_str(), sectIndex, section.m_name);
			m_saved = 0;
		}
		fread(section.m_junk, 1, 16, inFile);
		fread(&section.m_battlePhase, 4, 1, inFile);
		fread(&section.m_tempo, 4, 1, inFile);
		fread(&section.m_duration, 4, 1, inFile);
		fseek(inFile, 4, SEEK_CUR);
		fread(u.c, 4, 1, inFile);
		for (unsigned condIndex = 0; condIndex < u.ui; condIndex++)
			section.m_conditions.emplace_back(inFile);
		fread(&section.m_numPlayers, 4, 1, inFile);
		fread(&section.m_numCharts, 4, 1, inFile);
		section.m_charts.clear();
		for (unsigned playerIndex = 0; playerIndex < section.m_numPlayers; playerIndex++)
		{
			for (unsigned chartIndex = 0; chartIndex < section.m_numCharts; chartIndex++)
			{
				fread(u.c, 1, 4, inFile);
				if (!strstr(u.c, "CHCH"))
				{
					fclose(inFile);
					throw "Error: No 'CHCH' Tag for section " + to_string(sectIndex) + " - subsection " + to_string(playerIndex * section.m_numCharts + chartIndex) +
						" [File offset: " + to_string(ftell(inFile) - 4) + "].";
				}
				Chart chart(false);
				//Skip Chart size as the embedded value can be wrong
				fseek(inFile, 16, SEEK_CUR);
				fread(chart.m_junk, 1, 16, inFile);
				fread(&chart.m_pivotTime, 4, 1, inFile);
				fread(&chart.m_endTime, 4, 1, inFile);
				fread(u.c, 1, 4, inFile); //Read number of trace lines
				//Uses Traceline FILE* constructor
				for (unsigned traceIndex = 0; traceIndex < u.ui; traceIndex++)
					chart.emplaceTraceline(inFile);
				fread(u.c, 1, 4, inFile); //Read number of Phrase bars
				//Uses Phrase FILE* constructor
				for (unsigned phraseIndex = 0; phraseIndex < u.ui; phraseIndex++)
					chart.emplacePhrase(inFile);
				fread(u.c, 1, 4, inFile); //Read number of Guard marks
				//Uses Guard FILE* constructor
				for (unsigned guardIndex = 0; guardIndex < u.ui; guardIndex++)
					chart.emplaceGuard(inFile);
				fseek(inFile, 4, SEEK_CUR);
				section.m_charts.push_back(chart);
			}
		}
	}
	fread(u.c, 1, 4, inFile);
	if (u.ui != 20)
	{
		fclose(inFile);
		throw "Error: Incorrect constant value found when attempting to read Player Damage / Energy Factors.\n" + 
			g_global.tabs + "Needed: '20' (or '0x00000014')| Found: " + to_string(u.ui) + " [File offset: " + to_string(ftell(inFile) - 4) + "].";
	}
	fread(&m_energyDamageFactors, sizeof(EnergyDamage), 20, inFile);
	fclose(inFile);
}

//Create or update a CHC file
void CHC::create(string filename)
{
	{
		size_t pos = filename.find_last_of('\\');
		GlobalFunctions::banner(" Saving " + filename.substr(pos != string::npos ? pos + 1 : 0) + ' ');
	}
	FILE* outFile;
	fopen_s(&outFile, filename.c_str(), "wb");
	union {
		char c[4];
		unsigned ui;
	} u;
	u.ui = 6144UL + m_optimized;
	std::copy(u.c, u.c + 4, m_header + 4);
	fwrite(m_header, 1, 36, outFile);
	fwrite(m_imc, 1, 256, outFile);

	fwrite(m_events, sizeof(SSQRange), 4, outFile);
	fwrite(m_audio, sizeof(AudioChannel), 8, outFile);
	fwrite(&m_speed, 4, 1, outFile);

	unsigned long size = (unsigned long)m_sections.size();
	fwrite(&size, 4, 1, outFile);
	for (SongSection& section : m_sections)	// Cues
	{
		fwrite(&section.m_index, 4, 1, outFile);
		fwrite(section.m_name, 1, 16, outFile);
		fwrite(section.m_audio, 1, 16, outFile);
		fwrite(&section.m_frames, 4, 2, outFile);
		fwrite("\0\0\0\0", 1, 4, outFile);
	}

	fwrite(&size, 4, 1, outFile);
	for (SongSection& section : m_sections) // SongSections
	{
		fputs("CHLS", outFile);
		u.ui = 4864UL;
		fwrite(u.c, 1, 4, outFile);

		fwrite(&section.m_size, 4, 1, outFile);
		fwrite(&section.m_organized, 4, 1, outFile);
		fwrite(&section.m_swapped, 4, 1, outFile);
		fwrite(section.m_junk, 1, 16, outFile);

		fwrite(&section.m_battlePhase, 4, 1, outFile);
		fwrite(&section.m_tempo, 4, 1, outFile);
		fwrite(&section.m_duration, 4, 1, outFile);
		fwrite("\0\0\0\0", 1, 4, outFile);

		size = (unsigned long)section.m_conditions.size();
		fwrite(&size, 4, 1, outFile);
		for (SongSection::Condition& cond : section.m_conditions)
			fwrite(&cond, 16, 1, outFile);

		fwrite(&section.m_numPlayers, 4, 1, outFile);
		fwrite(&section.m_numCharts, 4, 1, outFile);
		for (unsigned playerIndex = 0; playerIndex < section.m_numPlayers; playerIndex++)
		{
			for (unsigned chartIndex = 0; chartIndex < section.m_numCharts; chartIndex++)
			{
				Chart& chart = section.m_charts[(size_t)playerIndex * section.m_numCharts + chartIndex];
				fputs("CHCH", outFile);
				u.ui = 4864UL;
				fwrite(u.c, 1, 4, outFile);

				fwrite(&chart.m_size, 4, 1, outFile);
				fwrite("\0\0\0\0\0\0\0\0", 1, 8, outFile);
				fwrite(chart.m_junk, 1, 16, outFile);
				fwrite(&chart.m_pivotTime, 4, 1, outFile);
				fwrite(&chart.m_endTime, 4, 1, outFile);

				size = (unsigned long)chart.m_tracelines.size();
				fwrite(&size, 4, 1, outFile);
				for (Traceline& trace : chart.m_tracelines)
				{
					fwrite(&trace.m_pivotAlpha, 4, 1, outFile);
					fwrite(&trace.m_duration, 4, 1, outFile);
					fwrite(&trace.m_angle, 4, 1, outFile);
					fwrite(&trace.m_curve, 4, 1, outFile);
				}

				size = (unsigned long)chart.m_phrases.size();
				fwrite(&size, 4, 1, outFile);
				for (Phrase& phrase : chart.m_phrases)
				{
					fwrite(&phrase.m_pivotAlpha, 4, 1, outFile);
					fwrite(&phrase.m_duration, 4, 1, outFile);
					fwrite(&phrase.m_start, 4, 1, outFile);
					fwrite(&phrase.m_end, 4, 1, outFile);
					fwrite(&phrase.m_animation, 4, 1, outFile);
					if (phrase.m_color != -1)
					{
						fputs("NOTECOLR", outFile);
						fwrite(&phrase.m_color, 4, 1, outFile);
					}
					else
						fwrite(phrase.m_junk, 1, 12, outFile);
				}

				size = (unsigned long)chart.m_guards.size();
				fwrite(&size, 4, 1, outFile);
				for (Guard& guard : chart.m_guards)
				{
					fwrite(&guard.m_pivotAlpha, 4, 1, outFile);
					fwrite(&guard.m_button, 4, 1, outFile);
					fwrite("\0\0\0\0\0\0\0\0", 1, 8, outFile);
				}

				fwrite("\0\0\0\0", 1, 4, outFile);
			}
		}
		fflush(outFile);
	}
	u.ui = 20UL;
	fwrite(u.c, 1, 4, outFile);
	fwrite(m_energyDamageFactors, sizeof(EnergyDamage), 20, outFile);
	fclose(outFile);
	m_saved = 1;
}

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
	fread(&m_frames, sizeof(SSQRange), 1, inFile);
	fseek(inFile, 4, SEEK_CUR);
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
		m_conditions.erase(m_conditions.begin() + index);
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
		m_conditions.assign(1, m_conditions.back());
	}
}

//Add a chart to the end of every player's sector
void SongSection::operator++()
{
	m_size += m_numPlayers * 72;
	for (size_t player = m_numPlayers; player > 0; --player)
		m_charts.emplace(m_charts.begin() + player * m_numCharts);
	m_numCharts++;
}

//Remove a chart from the end of every player's sector
bool SongSection::operator--()
{
	if (m_numCharts > 1)
	{
		for (size_t player = m_numPlayers; player > 0; --player)
		{
			m_size -= m_charts[player * m_numCharts - 1].getSize();
			m_charts.erase(m_charts.begin() + player * m_numCharts - 1);
		}
		m_numCharts--;
		return true;
	}
	else
		return false;
}

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

//Add a note to its corresponding List in ascending pivotAlpha order
size_t Chart::add(Note* note)
{
	if (dynamic_cast<Traceline*>(note) != nullptr)
		return add(static_cast<Traceline*>(note));
	else if (dynamic_cast<Phrase*>(note) != nullptr)
		return add(static_cast<Phrase*>(note));
	else
		return add(static_cast<Guard*>(note));
}

//Add a note to its corresponding List in ascending pivotAlpha order
void Chart::add(Traceline* note)
{
	m_size += 16;
	GlobalFunctions::emplace_ordered(m_tracelines, *note);
}

void Chart::add(Phrase* note)
{
	m_size += 32;
	GlobalFunctions::emplace_ordered(m_phrases, *note);
}

void Chart::add(Guard* note)
{
	m_size += 16;
	GlobalFunctions::emplace_ordered(m_guards, *note);
}

//Removes the element at the given index out of the chosen list.
//If removed, it will print to the output window the event using the "extra" parameter to adjust the position value and return true.
//Otherwise, it'll print a failure and return false.
//Type -- defaults to trace line ('t').
//Extra - defaults to 0.
bool Chart::removeTraceline(size_t index)
{
	if ((m_phrases.size() + m_guards.size() || m_tracelines.size() > 1) && index < m_tracelines.size())
	{
		m_size -= 16;
		m_tracelines.erase(m_tracelines.begin() + index);
		return true;
	}
	else
	{
		if (m_tracelines.size() != 1)
			printf("Index out of range - # of Trace Lines: %zu\n", m_tracelines.size());
		return false;
	}
}

bool Chart::removePhraseBar(size_t index)
{
	if (index < m_phrases.size())
	{
		m_size -= 32;
		m_phrases.erase(m_phrases.begin() + index);
		return true;
	}
	else
	{
		printf("Index out of range - # of Phrase bars: %zu\n", m_phrases.size());
		return false;
	}
}

bool Chart::removeGuardMark(size_t index)
{
	if ((m_phrases.size() + m_tracelines.size() || m_guards.size() > 1) && index < m_guards.size())
	{
		m_size -= 16;
		m_guards.erase(m_guards.begin() + index);
		return true;
	}
	else
	{
		if (m_guards.size() != 1)
			printf("Index out of range - # of Guard mark: %zu\n", m_guards.size());
		return false;
	}
}

//Full clear minus 1 Trace line
void Chart::clear()
{
	if (m_tracelines.size() > 1)
	{
		m_size -= unsigned long(16 * (m_tracelines.size() - 1));
		m_tracelines.erase(m_tracelines.begin(), m_tracelines.begin() + m_tracelines.size() - 1);
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
	for (size_t phrIndex = 0; phrIndex < source->m_phrases.size(); ++phrIndex)
	{
		Phrase& phr = source->m_phrases[phrIndex];
		lastNote = phr.m_pivotAlpha;
		if (!phr.m_end)
		{
			if (phrIndex + 1 == source->m_phrases.size())
				phr.m_end = true;
			else
				phr.changeEndAlpha(source->m_phrases[phrIndex + 1].m_pivotAlpha);
		}
		//Pivot alpha was previous set to the total displacement from the start of the section
		m_phrases.emplace_back(phr);
		m_phrases.back().adjustPivotAlpha(-m_pivotTime);
		m_size += 32;
	}

	clearTracelines();
	for (size_t trIndex = 0; trIndex < source->m_tracelines.size(); ++trIndex)
	{
		Traceline& trace = source->m_tracelines[trIndex];
		if (trace.m_pivotAlpha > lastNote)
			lastNote = trace.m_pivotAlpha;

		if (trIndex + 1 != source->m_tracelines.size())
			trace.changeEndAlpha(source->m_tracelines[trIndex + 1].m_pivotAlpha);
		m_tracelines.emplace_back(trace);
		m_tracelines.back().adjustPivotAlpha(-m_pivotTime);
		m_size += 16;
	}
	//Go through every phrase bar & trace line to find places where phrase bars
	//should be split into two pieces
	for (size_t trIndex = 0, phrIndex = 0; phrIndex < m_phrases.size() && trIndex < m_tracelines.size();)
	{
		Traceline& trace = m_tracelines[trIndex];
		Phrase& phrase = m_phrases[phrIndex];
		if (!trIndex && trace.m_pivotAlpha > phrase.m_pivotAlpha)
			trace.changePivotAlpha(phrase.m_pivotAlpha);

		if (trace.m_pivotAlpha >= phrase.getEndAlpha())
			++phrIndex;
		else
		{
			if (trace.m_pivotAlpha > phrase.m_pivotAlpha)
			{
				unsigned long dur = phrase.getEndAlpha() - trace.m_pivotAlpha;
				bool end = phrase.m_end;
				phrase.changeEndAlpha(trace.m_pivotAlpha);
				phrase.m_end = false;
				emplacePhrase(trace.m_pivotAlpha, dur, false, end, 0, phrase.getColor());
				++phrIndex;
			}

			if (trIndex + 1 == m_tracelines.size())
			{
				//If the phrase bar lands at or after the last trace line, delete
				if (trace.getEndAlpha() <= phrase.m_pivotAlpha)
				{
					if (!phrase.m_start)
						m_phrases[phrIndex - 1].m_end = true;
					if (removePhraseBar(phrIndex))
						printf("Phrase bar %zu removed\n", phrIndex);
				}
				else
				{
					if (phrase.getEndAlpha() > trace.getEndAlpha())
					{
						phrase.changeEndAlpha(trace.getEndAlpha());
						phrase.m_end = true;
					}
					++phrIndex;
				}
			}
			else
				++trIndex;
		}
	}
	clearGuards();
	for (size_t grdIndex = 0; grdIndex < source->m_guards.size(); ++grdIndex)
	{
		Guard& grd = source->m_guards[grdIndex];
		if (grd.m_pivotAlpha > lastNote)
			lastNote = grd.m_pivotAlpha;
		m_guards.emplace_back(grd);
		m_guards.back().adjustPivotAlpha(-m_pivotTime);
		m_size += 16;
	}
	return lastNote;
}

Note& Note::operator=(const Note& note)
{
	m_pivotAlpha = note.m_pivotAlpha;
	return *this;
}

Path::Path(const Note& note) : Note(note)
{
	const Path* path = dynamic_cast<const Path*>(&note);
	if (path != nullptr)
		m_duration = path->m_duration;
	else
		m_duration = 1;
}

Note& Path::operator=(const Note& note)
{
	Note::operator=(note);
	const Path* path = dynamic_cast<const Path*>(&note);
	if (path != nullptr)
		m_duration = path->m_duration;
	else
		m_duration = 1;
	return *this;
}

bool Path::adjustDuration(long change)
{
	if ((long)m_duration + change >= 1)
	{
		m_duration += change;
		return true;
	}
	else
	{
		m_duration = 1;
		return false;
	}
}

bool Path::changePivotAlpha(const long alpha)
{
	if (alpha < m_pivotAlpha + (long)m_duration)
	{
		m_duration -= alpha - m_pivotAlpha;
		m_pivotAlpha = alpha;
		return true;
	}
	else
		return false;
}

bool Path::changeEndAlpha(const long endAlpha)
{
	if (endAlpha > m_pivotAlpha)
	{
		m_duration = endAlpha - m_pivotAlpha;
		return true;
	}
	else
		return false;
}

Traceline::Traceline(FILE* inFile)
{
	if (inFile != nullptr)
	{
		fread(&m_pivotAlpha, 4, 1, inFile);
		fread(&m_duration, 4, 1, inFile);
		fread(&m_angle, 4, 1, inFile);
		fread(&m_curve, 4, 1, inFile);
	}
	else
		throw "Error creating Trace line: Bruh, open a file first.";
}

Traceline::Traceline(const Note& note) : Path(note)
{
	const Traceline* trace = dynamic_cast<const Traceline*>(&note);
	if (trace != nullptr)
	{
		m_angle = trace->m_angle;
		m_curve = trace->m_curve;
	}
	else
	{
		m_angle = 0;
		m_curve = false;
	}
}

Note& Traceline::operator=(const Note& note)
{
	Path::operator=(note);
	const Traceline* trace = dynamic_cast<const Traceline*>(&note);
	if (trace != nullptr)
	{
		m_angle = trace->m_angle;
		m_curve = trace->m_curve;
	}
	else
	{
		m_angle = 0;
		m_curve = false;
	}
	return *this;
}

Phrase::Phrase(FILE* inFile)
{
	if (inFile != nullptr)
	{
		fread(&m_pivotAlpha, 4, 1, inFile);
		fread(&m_duration, 4, 1, inFile);
		fread(&m_start, 4, 1, inFile);
		fread(&m_end, 4, 1, inFile);
		fread(&m_animation, 4, 1, inFile);
		fread(m_junk, 1, 8, inFile);
		if (strstr(m_junk, "NOTECOLR"))
			fread(&m_color, 4, 1, inFile);
		else
		{
			fread(m_junk + 8, 1, 4, inFile);
			m_color = 0;
		}
	}
	else
		throw "Error creating Phrase bar: Bruh, open a file first.";
}

Phrase::Phrase(const Note& note) : Path(note)
{
	const Phrase* phrase = dynamic_cast<const Phrase*>(&note);
	if (phrase != nullptr)
	{
		m_start = phrase->m_start;
		m_end = phrase->m_end;
		m_animation = phrase->m_animation;
		std::copy(phrase->m_junk, phrase->m_junk + 12, m_junk);
	}
	else
	{
		m_start = true;
		m_end = true;
		m_animation = 0;
	}
}

Note& Phrase::operator=(const Note& note)
{
	Path::operator=(note);
	const Phrase* phrase = dynamic_cast<const Phrase*>(&note);
	if (phrase != nullptr)
	{
		m_start = phrase->m_start;
		m_end = phrase->m_end;
		m_animation = phrase->m_animation;
		std::copy(phrase->m_junk, phrase->m_junk + 12, m_junk);
	}
	else
	{
		m_start = true;
		m_end = true;
		m_animation = 0;
		for (unsigned char i = 0; i < 12; i++)
			m_junk[i] = 0;
	}
	return *this;
}

Guard::Guard(FILE* inFile)
{
	if (inFile != nullptr)
	{
		fread(&m_pivotAlpha, 4, 1, inFile);
		fread(&m_button, 4, 1, inFile);
		fseek(inFile, 8, SEEK_CUR);
	}
	else
		throw "Error creating Guard Mark: Bruh, open a file first.";
}

Guard::Guard(const Note& note) : Note(note)
{
	const Guard* guard = dynamic_cast<const Guard*>(&note);
	if (guard != nullptr)
		m_button = guard->m_button;
	else
		m_button = 0;
}

Note& Guard::operator=(const Note& note)
{
	Note::operator=(note);
	const Guard* guard = dynamic_cast<const Guard*>(&note);
	if (guard != nullptr)
		m_button = guard->m_button;
	else
		m_button = 0;
	return *this;
}
