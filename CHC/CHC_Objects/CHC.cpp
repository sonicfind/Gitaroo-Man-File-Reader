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
#include "CHC.h"
#include <algorithm>
using namespace std;

//Creates a CHC object with 1 songsection
CHC::CHC() : m_sections(1), m_filename(""), m_shortname(""), m_stage(0), m_speed(0), m_unorganized(0), m_optimized(false), m_saved(2) {}

CHC::CHC(const CHC& song)
	: m_sections(song.m_sections), m_filename(song.m_filename), m_shortname(song.m_shortname), m_stage(song.m_stage),
		m_speed(song.m_speed), m_unorganized(song.m_unorganized), m_optimized(song.m_optimized), m_saved(song.m_saved)
{
	std::copy(song.m_header, song.m_header + 36, m_header);
	std::copy(song.m_imc, song.m_imc + 256, m_imc);
	std::copy(song.m_events, song.m_events + 4, m_events);
	std::copy(song.m_audio, song.m_audio + 8, m_audio);
	std::copy(song.m_energyDamageFactors[0], song.m_energyDamageFactors[0] + 20, m_energyDamageFactors[0]);
}

CHC& CHC::operator=(CHC& song)
{
	m_filename = song.m_filename;
	m_shortname = song.m_shortname;
	m_stage = song.m_stage;
	std::copy(song.m_header, song.m_header + 36, m_header);
	std::copy(song.m_imc, song.m_imc + 256, m_imc);
	std::copy(song.m_events, song.m_events + 4, m_events);
	std::copy(song.m_audio, song.m_audio + 8, m_audio);
	m_speed = song.m_speed;
	m_unorganized = song.m_unorganized;
	m_optimized = song.m_optimized;
	m_sections = song.m_sections;
	std::copy(song.m_energyDamageFactors[0], song.m_energyDamageFactors[0] + 20, m_energyDamageFactors[0]);
	m_saved = song.m_saved;
	return *this;
}

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
	std::copy(u.c, u.c + 4, m_header + 4);
	m_optimized = u.ui == 6145;
	fread(m_imc, 1, 256, inFile);
	bool duet = m_imc[0] == 0;
	fread(&m_events, sizeof(SSQ), 4, inFile);
	fread(&m_audio, sizeof(AudioChannel), 8, inFile);
	fread(&m_speed, 4, 1, inFile);
	fread(u.c, 4, 1, inFile);
	m_unorganized = u.ui;
	//Uses FILE* constructor to read section cue data
	for (unsigned sectIndex = 0; sectIndex < u.ui; sectIndex++)
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
				Chart chart;
				//Skip Chart size as the embedded value can be wrong
				fseek(inFile, 16, SEEK_CUR);
				fread(chart.m_junk, 1, 16, inFile);
				fread(&chart.m_pivotTime, 4, 1, inFile);
				fread(&chart.m_endTime, 4, 1, inFile);
				chart.clearTracelines();
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
	fwrite(m_events, sizeof(SSQ), 4, outFile);
	fwrite(m_audio, sizeof(AudioChannel), 8, outFile);
	fwrite(&m_speed, 4, 1, outFile);
	fwrite((unsigned long*)&m_sections.size(), 4, 1, outFile);
	for (unsigned sectIndex = 0; sectIndex < m_sections.size(); sectIndex++)	//Cues
	{
		fwrite(&m_sections[sectIndex].m_index, 4, 1, outFile);
		fwrite(m_sections[sectIndex].m_name, 1, 16, outFile);
		fwrite(m_sections[sectIndex].m_audio, 1, 16, outFile);
		fwrite(&m_sections[sectIndex].m_frames, 4, 2, outFile);
		fwrite("\0\0\0\0", 1, 4, outFile);
	}
	fwrite((unsigned long*)&m_sections.size(), 4, 1, outFile);
	for (unsigned sectIndex = 0; sectIndex < m_sections.size(); sectIndex++) //SongSections
	{
		SongSection& section = m_sections[sectIndex];
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
		fwrite((unsigned long*)&section.m_conditions.size(), 4, 1, outFile);
		for (size_t condIndex = 0; condIndex < section.m_conditions.size(); condIndex++)
			fwrite(&section.m_conditions[condIndex], 16, 1, outFile);
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
				fwrite((unsigned long*)&chart.m_tracelines.size(), 4, 1, outFile);
				for (size_t traceIndex = 0; traceIndex < chart.m_tracelines.size(); traceIndex++)
				{
					fwrite(&chart.m_tracelines[traceIndex].m_pivotAlpha, 4, 1, outFile);
					fwrite(&chart.m_tracelines[traceIndex].m_duration, 4, 1, outFile);
					fwrite(&chart.m_tracelines[traceIndex].m_angle, 4, 1, outFile);
					fwrite(&chart.m_tracelines[traceIndex].m_curve, 4, 1, outFile);
				}
				fwrite((unsigned long*)&chart.m_phrases.size(), 4, 1, outFile);
				for (size_t phraseIndex = 0; phraseIndex < chart.m_phrases.size(); phraseIndex++)
				{
					fwrite(&chart.m_phrases[phraseIndex].m_pivotAlpha, 4, 1, outFile);
					fwrite(&chart.m_phrases[phraseIndex].m_duration, 4, 1, outFile);
					fwrite(&chart.m_phrases[phraseIndex].m_start, 4, 1, outFile);
					fwrite(&chart.m_phrases[phraseIndex].m_end, 4, 1, outFile);
					fwrite(&chart.m_phrases[phraseIndex].m_animation, 4, 1, outFile);
					if (chart.m_phrases[phraseIndex].m_color != -1)
					{
						fputs("NOTECOLR", outFile);
						fwrite(&chart.m_phrases[phraseIndex].m_color, 4, 1, outFile);
					}
					else
						fwrite(chart.m_phrases[phraseIndex].m_junk, 1, 12, outFile);
				}
				fwrite((unsigned long*)&chart.m_guards.size(), 4, 1, outFile);
				for (size_t guardIndex = 0; guardIndex < chart.m_guards.size(); guardIndex++)
				{
					fwrite(&chart.m_guards[guardIndex].m_pivotAlpha, 4, 1, outFile);
					fwrite(&chart.m_guards[guardIndex].m_button, 4, 1, outFile);
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
