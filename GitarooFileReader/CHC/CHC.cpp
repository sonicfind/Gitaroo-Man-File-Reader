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
#include "CHC.h"
using namespace GlobalFunctions;

//Creates a CHC object with 1 songsection
CHC::CHC()
	: FileType(".CHC")
	, m_stage(0)
	, m_speed(0)
	, m_sections(1)
	, m_unorganized(0)
	, m_optimized(false) {}

CHC::CHC(const CHC& chc)
	: FileType(chc)
	, m_stage(chc.m_stage)
	, m_speed(chc.m_speed)
	, m_unorganized(chc.m_unorganized)
	, m_optimized(chc.m_optimized)
	, m_sections(chc.m_sections)
{
	std::copy(chc.m_header, chc.m_header + sizeof(m_header), m_header);
	std::copy(chc.m_imc, chc.m_imc + sizeof(m_imc), m_imc);
	std::copy(chc.m_events, chc.m_events + _countof(m_events), m_events);
	std::copy(chc.m_audio, chc.m_audio + _countof(m_audio), m_audio);
	memcpy(m_energyDamageFactors, chc.m_energyDamageFactors, sizeof(m_energyDamageFactors));
	for (auto& section : m_sections)
		section.m_parent = this;
}

//Creates a CHC object using values from the CHC file pointed to by the provided filename.
//
//Value names chosen to be kept are based off the CHC tab in the Gitaroo Pals shoutwiki
CHC::CHC(std::string filename) : FileType(filename, ".CHC")
{
	union {
		char c[4];
		unsigned ui = 0;
	} u;

	try
	{
		m_stage = stoi(m_filename.substr(2, 2));
	}
	catch (...)
	{
		m_stage = -1;
	}

	fread(m_header, 1, 36, m_filePtr);
	if (!strstr(m_header, "SNGS"))
	{
		fclose(m_filePtr);
		throw "Error: No 'SNGS' Tag at byte 0.";
	}
	std::copy(m_header + 4, m_header + 8, u.c);
	m_optimized = u.ui == 6145;
	fread(m_imc, 1, 256, m_filePtr);
	fread(&m_events, sizeof(SSQRange), 4, m_filePtr);
	fread(&m_audio, sizeof(AudioChannel), 8, m_filePtr);
	fread(&m_speed, 4, 1, m_filePtr);
	fread(&m_unorganized, 4, 1, m_filePtr);

	// Uses FILE* constructor to read section cue data
	for (size_t index = 0; index < m_unorganized; ++index)
		m_sections.push_back({ this, m_filePtr });

	fseek(m_filePtr, 4, SEEK_CUR);
	for (size_t sectIndex = 0; sectIndex < m_sections.size(); sectIndex++) //SongSections
	{
		fread(u.c, 1, 4, m_filePtr);
		if (!strstr(u.c, "CHLS"))
		{
			const int position = ftell(m_filePtr) - 4;
			fclose(m_filePtr);
			throw "Error: No 'CHLS' Tag for section #" + std::to_string(sectIndex) + " [File offset: " + std::to_string(position) + "].";
		}

		m_sections[sectIndex].continueRead(m_filePtr, sectIndex, m_stage, !m_imc[0]);
	}

	fread(u.c, 1, 4, m_filePtr);
	if (u.ui != 20)
	{
		const int position = ftell(m_filePtr) - 4;
		fclose(m_filePtr);
		throw "Error: Incorrect constant value found when attempting to read Player Damage / Energy Factors.\n" + 
			g_global.tabs + "Needed: '20' (or '0x00000014')| Found: " + std::to_string(u.ui) + " [File offset: " + std::to_string(position) + "].";
	}
	fread(&m_energyDamageFactors, sizeof(EnergyDamage), 20, m_filePtr);
	fclose(m_filePtr);
}

//Create or update a CHC file
bool CHC::create(std::string filename)
{
	if (FileType::create(filename))
	{
		union {
			char c[4];
			unsigned ui = 0;
		} u;
		u.ui = 6144UL + m_optimized;
		std::copy(u.c, u.c + 4, m_header + 4);
		fwrite(m_header, 1, 36, m_filePtr);
		fwrite(m_imc, 1, 256, m_filePtr);

		fwrite(m_events, sizeof(SSQRange), 4, m_filePtr);
		fwrite(m_audio, sizeof(AudioChannel), 8, m_filePtr);
		fwrite(&m_speed, 4, 1, m_filePtr);

		// Cues
		const unsigned long size = (unsigned long)m_sections.size();
		fwrite(&size, 4, 1, m_filePtr);
		for (SongSection& section : m_sections)	
		{
			fwrite(&section.m_index, 4, 1, m_filePtr);
			fwrite(section.m_name, 1, 16, m_filePtr);
			fwrite(section.m_audio, 1, 16, m_filePtr);
			fwrite(&section.m_frames, 4, 2, m_filePtr);
			fwrite("\0\0\0\0", 1, 4, m_filePtr);
		}

		// SongSection chart data
		fwrite(&size, 4, 1, m_filePtr);
		for (SongSection& section : m_sections) 
			section.create(m_filePtr);

		u.ui = 20UL;
		fwrite(u.c, 1, 4, m_filePtr);
		fwrite(m_energyDamageFactors, sizeof(EnergyDamage), 20, m_filePtr);
		fclose(m_filePtr);
		m_saved = true;
		return true;
	}
	return false;
}

/*
Writes out the data present in the CHC file to a readable txt file. This would include all SSQ & cue data, audio values,
section & subsection data - including values for every separate note -, and damage & energy factors. Creates two files -
with one being a simplistic form with only the bare minimum necessary information.
*/
bool CHC::write_to_txt()
{
	FILE* txtFile, *simpleTxtFile;
	if (FileType::write_to_txt(txtFile, simpleTxtFile))
	{
		try
		{
			fprintf(txtFile, "Header: %s", m_header);
			dualvfprintf_s(txtFile, simpleTxtFile, "IMC: %s\n", m_imc[0] ? m_imc : "Unused in PSP version");
			dualvfprintf_s(txtFile, simpleTxtFile, "\t   SSQ Events:\n");

			dualvfprintf_s(txtFile, simpleTxtFile, "\t\t    Win A:\n");
			dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\tFirst Frame: %g\n", m_events[0].first);
			dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t Last Frame: %g\n", m_events[0].last);

			dualvfprintf_s(txtFile, simpleTxtFile, "\t\t    Win B:\n");
			dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\tFirst Frame: %g\n", m_events[1].first);
			dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t Last Frame: %g\n", m_events[1].last);

			dualvfprintf_s(txtFile, simpleTxtFile, "\t\t Lose Pre:\n");
			dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\tFirst Frame: %g\n", m_events[2].first);
			dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t Last Frame: %g\n", m_events[2].last);

			dualvfprintf_s(txtFile, simpleTxtFile, "\t\tLose Loop:\n");
			dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\tFirst Frame: %g\n", m_events[3].first);
			dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t Last Frame: %g\n", m_events[3].last);

			dualvfprintf_s(txtFile, simpleTxtFile, "       Audio Channels:\n");

			if (m_imc[0])
			{
				for (size_t index = 0; index < 8; index++)
				{
					dualvfprintf_s(txtFile, simpleTxtFile, "\t\t Channel %zu:\n", index + 1);
					dualvfprintf_s(txtFile, simpleTxtFile, "\t\t       Volume: %lu (%g%%)\n", m_audio[index].volume, m_audio[index].volume * 100.0 / 32767);
					switch (m_audio[index].pan)
					{
					case 0:
						dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t  Pan: Left (0)\n");
						break;
					case 16383:
						dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t  Pan: Center (16383)\n");
						break;
					case 32767:
						dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t  Pan: Right (32767)\n");
						break;
					default:
						dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t  Pan: %g%% Left | %g%% Right (%lu)\n",
							100 - (m_audio[index].pan * 100.0 / 32767),
							m_audio[index].pan * 100.0 / 32767, m_audio[index].pan);
					}
				}
			}
			else
			{
				dualvfprintf_s(txtFile, simpleTxtFile, "\t\t Channel 1: Unused in PSP version\n");
				dualvfprintf_s(txtFile, simpleTxtFile, "\t\t Channel 2: Unused in PSP version\n");
				for (size_t index = 2; index < 8; index++)
				{
					dualvfprintf_s(txtFile, simpleTxtFile, "\t\t Channel %zu:\n", index + 1);
					dualvfprintf_s(txtFile, simpleTxtFile, "\t\t       Volume: %lu (%g%%)\n", m_audio[index].volume, m_audio[index].volume * 100.0 / 32767);
					dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t  Pan: Unused in PSP version\n");
				}
			}
			dualvfprintf_s(txtFile, simpleTxtFile, "\t       Speed: %g\n", m_speed);

			fprintf(txtFile, "\t   # of Cues: %zu\n", m_sections.size());
			fputs("\t    SSQ Cues:\n", txtFile);

			for (auto& section : m_sections)	//Cues
			{
				fprintf(txtFile, "\t       Cue %s:\n", section.m_name);
				fprintf(txtFile, "\t\t\tAudio Used: %s\n", section.m_audio);
				fprintf(txtFile, "\t\t\t     Index: %lu\n", section.m_index);
				fprintf(txtFile, "\t\t       First frame: %5g\n", section.m_frames.first);
				fprintf(txtFile, "\t\t\tLast frame: %5g\n", section.m_frames.last);
			}

			fflush(txtFile);
			fflush(simpleTxtFile);

			dualvfprintf_s(txtFile, simpleTxtFile, "       # of Sections: %zu\n", m_sections.size());
			dualvfprintf_s(txtFile, simpleTxtFile, "       Song Sections:\n");

			for (auto& section : m_sections) //SongSections
			{
				
			}

			dualvfprintf_s(txtFile, simpleTxtFile, "Damage/Energy Factors:\n");
			for (size_t player = 0; player < 4; player++)
			{
				dualvfprintf_s(txtFile, simpleTxtFile, "       Player %zu ||", player + 1);
				dualvfprintf_s(txtFile, simpleTxtFile, " Starting Energy || Initial-Press Energy || Initial-Press Damage || Guard Energy Gain ||");
				dualvfprintf_s(txtFile, simpleTxtFile, " Attack Miss Damage || Guard Miss Damage || Release Max Energy || Release Max Damage ||\n");
				dualvfprintf_s(txtFile, simpleTxtFile, "       %s||\n", std::string(184, '='));
				for (size_t section = 0; section < 5; section++)
				{
					switch (section)
					{
					case 0: dualvfprintf_s(txtFile, simpleTxtFile, "\t  Intro ||"); break;
					case 1: dualvfprintf_s(txtFile, simpleTxtFile, "\t Charge ||"); break;
					case 2: dualvfprintf_s(txtFile, simpleTxtFile, "\t Battle ||"); break;
					case 3: dualvfprintf_s(txtFile, simpleTxtFile, "\tHarmony ||"); break;
					case 4: dualvfprintf_s(txtFile, simpleTxtFile, "\t    End ||");
					}
					dualvfprintf_s(txtFile, simpleTxtFile, "%015g%% ||%020g%% ||%020g%% ||%017g%% ||%018g%% ||%017g%% ||%018g%% ||%018g%% ||\n",
						m_energyDamageFactors[player][section].initialEnergy * 100.0,
						m_energyDamageFactors[player][section].chargeInitial * 100.0,
						m_energyDamageFactors[player][section].attackInitial * 100.0,
						m_energyDamageFactors[player][section].guardEnergy * 100.0,
						m_energyDamageFactors[player][section].attackMiss * 100.0,
						m_energyDamageFactors[player][section].guardMiss * 100.0,
						m_energyDamageFactors[player][section].chargeRelease * 100.0,
						m_energyDamageFactors[player][section].attackRelease * 100.0);
				}
				dualvfprintf_s(txtFile, simpleTxtFile, "\n");
			}
		}
		catch (std::string str)
		{
			printf_tab("%s\n", str.c_str());
		}
		catch (const char* str)
		{
			printf_tab("%s\n", str);
		}
		fclose(txtFile);
		fclose(simpleTxtFile);
		return true;
	}
	return false;
}

bool CHC::colorCheatTemplate()
{
	banner(" " + m_filename + ".CHC - Color Sheet Creation ");
	bool multiplayer = toupper(m_filename.back()) == 'M';
	if (!multiplayer)
	{
		do
		{
			printf_tab("Is this chart for multiplayer? [Y/N]\n");
			switch (menuChoices("yn"))
			{
			case ResultType::Quit:
				return false;
			case ResultType::Success:
				if (g_global.answer.character == 'y')
					multiplayer = true;
				g_global.quit = true;
			}
		} while (!g_global.quit);
		g_global.quit = false;
	}

	bool generate = false;
	std::vector<size_t> sectionIndexes;
	do
	{
		printf_tab("Type the number for each section that you wish to outline colors for - w/ spaces inbetween.\n");
		for (size_t sectIndex = 0; sectIndex < m_sections.size(); sectIndex++)
			printf_tab("%zu - %s\n", sectIndex, m_sections[sectIndex].getName());

		if (sectionIndexes.size())
		{
			printf_tab("Current list: ");
			for (size_t index : sectionIndexes)
				printf_tab(" ", m_sections[index].getName());
			putchar('\n');
		}

		switch (insertIndexValues(sectionIndexes, "ac", m_sections.size(), false))
		{
		case ResultType::Quit:
			printf_tab("Color Sheet creation cancelled.\n");
			return false;
		case ResultType::Help:
			printf_tab("Help: [TBD]\n%s\n", g_global.tabs.c_str());
			break;
		case ResultType::SpecialCase:
			if (sectionIndexes.size())
			{
				g_global.multi = false;
				g_global.quit = true;
				break;
			}
			__fallthrough;
		case ResultType::Success:
			if (!sectionIndexes.size())
			{
				do
				{
					printf_tab("No sections have been selected.\n");
					printf_tab("A - Add section values\n");
					printf_tab("C - Create template file with default colors\n");
					printf_tab("Q - Quit Color Sheet creation\n");
					switch (menuChoices("ac"))
					{
					case ResultType::Quit:
						printf_tab("Color Sheet creation cancelled.\n");
						return false;
					case ResultType::Success:
						if (g_global.answer.character == 'c')
							generate = true;
						g_global.quit = true;
					}
				} while (!g_global.quit);
				g_global.quit = false;
			}
			else
				g_global.quit = true;
		}
	} while (!g_global.quit && !generate);
	g_global.quit = false;

	bool writeColors = false;
	do
	{
		printf_tab("If found, use any colors that are pre-saved in a phrase bar? [Y/N]\n");
		switch (menuChoices("yn"))
		{
		case ResultType::Quit:
			return false;
		case ResultType::Success:
			if (g_global.answer.character == 'y')
				writeColors = true;
			g_global.quit = true;
		}
	} while (!g_global.quit);
	g_global.quit = false;

	std::string filename = m_directory + m_filename + "_COLORDEF";
	std::string filename2 = m_directory + m_filename + "_COLORDEF_FRAGS";
	FILE* outSheet = nullptr, * outSheet2 = nullptr;
	do
	{
		switch (fileOverwriteCheck(filename + ".txt"))
		{
		case ResultType::No:
			filename += "_T";
			break;
		case ResultType::Yes:
			fopen_s(&outSheet, (filename + ".txt").c_str(), "w");
			__fallthrough;
		case ResultType::Quit:
			g_global.quit = true;
		}
	} while (!g_global.quit);
	printf_tab("\n");
	g_global.quit = false;

	do
	{
		switch (fileOverwriteCheck(filename2 + ".txt"))
		{
		case ResultType::No:
			filename2 += "_T";
			break;
		case ResultType::Yes:
			fopen_s(&outSheet2, (filename2 + ".txt").c_str(), "w");
			__fallthrough;
		case ResultType::Quit:
			g_global.quit = true;
		}
	} while (!g_global.quit);
	g_global.quit = false;

	if (outSheet != nullptr || outSheet2 != nullptr)
	{
		fputs("[phrasemode fragments]\n", outSheet2);
		dualvfprintf_s(outSheet, outSheet2, "[attack point palette]\nG: 00ff00\nR: ff0000\nY: ffff00\nB: 0000ff\nO: ff7f00\nP: ff00ff\nN: f89b44\ng: ffffff\nr: ffffff\ny: ffffff\nb: ffffff\no: ffffff\np: ffffff\n\n");
		dualvfprintf_s(outSheet, outSheet2, "[phrase bar palette]\nG: 40ff40\nR: ff4040\nY: ffff40\nB: 4040c8\nO: ff9f40\nP: ff40ff\nN: f07b7b\ng: 40ff40\nr: ff4040\ny: ffff40\nb: 4040c8\no: ff9f40\np: ff40ff\n\n");
		unsigned long chartCount = 0;
		const size_t size = m_sections.size();
		bool* inputs = new bool[size]();
		for (size_t sect = 0; sect < sectionIndexes.size(); sect++)
			inputs[sectionIndexes[sect]] = true;
		std::string colors = "GRYBOPgrybop";
		for (unsigned long sectIndex = 0; sectIndex < size; sectIndex++)
		{
			SongSection& section = m_sections[sectIndex];
			for (unsigned playerIndex = 0; playerIndex < section.m_numPlayers; playerIndex++)
			{
				for (unsigned chartIndex = 0; chartIndex < section.m_numCharts; chartIndex++)
				{
					if (!(playerIndex & 1) || multiplayer)
					{
						Chart& chart = section.m_charts[(unsigned long long)playerIndex * section.m_numCharts + chartIndex];
						if (chart.getNumPhrases())
						{
							dualvfprintf_s(outSheet, outSheet2, "#SongSection %lu [%s], P%lu CHCH %lu\n", sectIndex, section.getName(), playerIndex + 1, chartIndex);
							dualvfprintf_s(outSheet, outSheet2, "[drawn chart %lu]\n", chartCount);
							if (inputs[sectIndex])
							{
								for (unsigned long phrIndex = 0; phrIndex < chart.getNumPhrases(); phrIndex++)
								{
									Phrase& phr = chart.m_phrases[phrIndex];
									if (writeColors && phr.getColor())
									{
										size_t colIndex = 0;
										while (colIndex < 5 && !(phr.getColor() & (1 << colIndex)))
											colIndex++;

										if (phr.getColor() & 64)
											colIndex += 6;

										if (!phrIndex)
										{
											fprintf(outSheet, "%c", colors[colIndex]);
											fprintf(outSheet2, "!%c", colors[colIndex]);
										}
										else if (phr.m_start) //Start
										{
											fprintf(outSheet, " %c", colors[colIndex]);
											fprintf(outSheet2, " !%c", colors[colIndex]);
										}
										else
											fprintf(outSheet2, "-%c", colors[colIndex]);
									}
									else if (!phrIndex)
									{
										fputc('_', outSheet);
										fputs("!_", outSheet2);
									}
									else if (phr.m_start) //Start
									{
										fputs(" _", outSheet);
										fputs(" !_", outSheet2);
									}
									else
										fputs("-_", outSheet2);
								}
							}
							else
							{
								for (unsigned long phrIndex = 0; phrIndex < chart.getNumPhrases(); phrIndex++)
								{
									Phrase& phr = chart.m_phrases[phrIndex];
									if (writeColors && phr.getColor())
									{
										size_t colIndex = 0;
										while (colIndex < 5 && !(phr.getColor() & (1 << colIndex)))
											colIndex++;

										if (phr.getColor() & 64)
											colIndex += 6;

										if (!phrIndex)
										{
											fprintf(outSheet, "%c", colors[colIndex]);
											fprintf(outSheet2, "!%c", colors[colIndex]);
										}
										else if (phr.m_start) //Start
										{
											fprintf(outSheet, " %c", colors[colIndex]);
											fprintf(outSheet2, " !%c", colors[colIndex]);
										}
										else
											fprintf(outSheet2, "-%c", colors[colIndex]);
									}
									else if (!phrIndex)
									{
										fputc('N', outSheet);
										fputs("!N", outSheet2);
									}
									else if (phr.m_start) //Start
									{
										fputs(" N", outSheet);
										fputs(" !N", outSheet2);
									}
									else
										fputs("-N", outSheet2);
								}
							}
							fprintf(outSheet, "\n\n");
							fprintf(outSheet2, "\n\n");
						}
						chartCount++;
					}
				}
			}
			printf_tab("Colored %s", section.getName());
			if (inputs[sectIndex])
				printf(" - With added outlines");
			printf("\n");
		}

		delete[size] inputs;
		if (outSheet != nullptr)
			fclose(outSheet);
		if (outSheet2 != nullptr)
			fclose(outSheet2);
		return true;
	}
	else
	{
		printf_tab("Color Sheet creation cancelled.\n");
		return false;
	}
}

//Create a SongSection object with 1 Condition and 4 Charts
SongSection::SongSection(CHC* parent)
	: m_parent(parent)
	, m_conditions(1)
	, m_charts(4) {}

SongSection::SongSection()
	: SongSection(nullptr) {}

//Uses a file to read in a few of the values
//Due to the context, no conditions or charts are created
SongSection::SongSection(CHC* parent, FILE* inFile)
	: m_parent(parent)
	, m_organized(false)
	, m_battlePhase(Phase::INTRO)
	, m_tempo(0)
	, m_duration(0)
	, m_numPlayers(0)
	, m_numCharts(0)
{
	fread(&m_index, 4, 1, inFile);
	fread(&m_name, 1, 16, inFile);
	fread(&m_audio, 1, 16, inFile);
	fread(&m_frames, sizeof(SSQRange), 1, inFile);
	fseek(inFile, 4, SEEK_CUR);
}

SongSection& SongSection::operator=(const SongSection& section)
{
	setOrganized(section.m_organized);
	m_swapped = section.m_swapped;
	m_tempo = section.m_tempo;
	m_duration = section.m_duration;
	m_numPlayers = section.m_numPlayers;
	m_numCharts = section.m_numCharts;
	m_charts = section.m_charts;
	return *this;
}

// Fills in rest of values from the given file
// Returns if the section is organized
void SongSection::continueRead(FILE* inFile, const size_t index, const int stage, bool isDuet)
{
	unsigned long value;
	fread(&value, 1, 4, inFile);
	setOrganized(value & 1);
	m_swapped = (value - 4864) / 2;
	fseek(inFile, 4, SEEK_CUR);
	if (!m_organized)
	{
		fread(&value, 1, 4, inFile);
		setOrganized(value & 1);
	}
	else
		fseek(inFile, 4, SEEK_CUR);

	if (!m_swapped)
		fread(&m_swapped, 4, 1, inFile);
	else
		fseek(inFile, 4, SEEK_CUR);

	if ((stage == 11 || stage == 12)
		&& !isDuet
		&& m_swapped < 4)
	{
		m_swapped += 4;
		printf_tab("Section #%zu (%s)'s swap value was adjusted to match current implementation for Duet->PS2 conversions. "
				"Make sure to save this file to apply this change.\n",  index, m_name);
	}

	fread(m_junk, 1, 16, inFile);
	fread(&m_battlePhase, 4, 1, inFile);
	fread(&m_tempo, 4, 1, inFile);
	fread(&m_duration, 4, 1, inFile);
	fseek(inFile, 4, SEEK_CUR);

	// Reads in the number of conditions
	fread(&value, 4, 1, inFile);
	for (unsigned condIndex = 0; condIndex < value; condIndex++)
		m_conditions.push_back({ inFile });

	fread(&m_numPlayers, 4, 1, inFile);
	fread(&m_numCharts, 4, 1, inFile);
	char tmp[5] = { 0 };
	for (unsigned chartIndex = 0; chartIndex < m_numPlayers * m_numCharts; ++chartIndex)
	{
		fread(tmp, 1, 4, inFile);
		if (!strstr(tmp, "CHCH"))
		{
			const int position = ftell(inFile) - 4;
			fclose(inFile);
			throw "Error: No 'CHCH' Tag for section " + std::string(m_name) + " - subsection " + std::to_string(chartIndex) +
				" [File offset: " + std::to_string(position) + "].";
		}
		m_charts.push_back({ inFile });
	}
}

void SongSection::create(FILE* outFile)
{
	fputs("CHLS", outFile);
	unsigned long value = 4864UL;
	fwrite(&value, 1, 4, outFile);
	value = getSize();
	fwrite(&value, 1, 4, outFile);
	fwrite(&m_organized, 4, 1, outFile);
	fwrite(&m_swapped, 4, 1, outFile);
	fwrite(m_junk, 1, 16, outFile);

	fwrite(&m_battlePhase, 4, 1, outFile);
	fwrite(&m_tempo, 4, 1, outFile);
	fwrite(&m_duration, 4, 1, outFile);
	fwrite("\0\0\0\0", 1, 4, outFile);

	const unsigned long size = (unsigned long)m_conditions.size();
	fwrite(&size, 4, 1, outFile);
	for (SongSection::Condition& cond : m_conditions)
		fwrite(&cond, 16, 1, outFile);

	fwrite(&m_numPlayers, 4, 1, outFile);
	fwrite(&m_numCharts, 4, 1, outFile);
	for (auto& chart : m_charts)
		chart.create(outFile);
	fflush(outFile);
}

void SongSection::write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile, const CHC* const chc)
{
	dualvfprintf_s(txtFile, simpleTxtFile, "\t       Section %s:\n", m_name);
	dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t       Organized: %s\n", m_organized & 1 ? "TRUE" : "FALSE");

	{
		std::string swapped = "Swapped Players : ";
		switch (m_swapped & 3)
		{
		case 0:
			swapped += m_swapped & 4 ? "FALSE (P1/P2D/P3/P4D) [Duet->PS2 Conversion]"
									 : "FALSE";
			break;
		case 1:
			swapped += m_swapped & 4 ? "TRUE (P2D/P1/P4D/P3) [Duet->PS2 Conversion]"
									 : "TRUE (P2/P1/P4/P3)";
			break;
		case 2:
			swapped += m_swapped & 4 ? "TRUE (P3/P4D/P1/P2D) [Duet->PS2 Conversion]"
									 : (chc->isPS2Compatible() ? "TRUE (P3/P4/P1/P2)"
															   : "TRUE (P3/P2/P1/P4) [DUET]");
			break;
		default:
			swapped += m_swapped & 4 ? "TRUE (P4D/P3/P2D/P1) [Duet->PS2 Conversion]"
									 : "TRUE (P4/P3/P2/P1)";
		}
		dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t %s\n", swapped.c_str());
	}

	fprintf(txtFile, "\t\t       Size (32bit) - 44: %lu\n", getSize());
	fprintf(txtFile, "\t\t\t\t    Junk: 0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(m_junk)));
	fprintf(txtFile, "\t\t\t\t\t  0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(m_junk + 4)));
	fprintf(txtFile, "\t\t\t\t\t  0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(m_junk + 8)));
	fprintf(txtFile, "\t\t\t\t\t  0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(m_junk + 12)));

	switch (m_battlePhase)
	{
	case SongSection::Phase::INTRO:
		dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t   Phase: INTRO\n");
		break;
	case SongSection::Phase::CHARGE:
		dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t   Phase: CHARGE\n");
		break;
	case SongSection::Phase::BATTLE:
		dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t   Phase: BATTLE\n");
		break;
	case SongSection::Phase::FINAL_AG:
		dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t   Phase: FINAL_AG\n");
		break;
	case SongSection::Phase::HARMONY:
		dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t   Phase: HARMONY\n");
		break;
	case SongSection::Phase::END:
		dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t   Phase: END\n");
		break;
	default:
		dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t   Phase: FINAL_I\n");
	}

	dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t   Tempo: %g\n", m_tempo);
	fprintf(txtFile, "\t\t\t    Samples/Beat: %Lg\n", s_SAMPLES_PER_MIN / m_tempo);
	dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\tDuration: %lu samples\n", m_duration);

	dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t # of Conditions: %lu\n", m_conditions.size());
	for (size_t condIndex = 0; condIndex < m_conditions.size(); condIndex++)
	{
		dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t   Condition %zu:\n", condIndex + 1);
		m_conditions[condIndex].write_to_txt(txtFile, simpleTxtFile, chc);
	}

	dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t    # of Players: %lu\n", m_numPlayers);
	dualvfprintf_s(txtFile, simpleTxtFile, "\t\t       Charts per Player: %lu\n", m_numCharts);
	for (size_t playerIndex = 0; playerIndex < m_numPlayers; playerIndex++)
	{
		for (size_t chartIndex = 0; chartIndex < m_numCharts; chartIndex++)
		{
			dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t Player %zu - Chart %02zu\n", playerIndex + 1, chartIndex + 1);
			m_charts[playerIndex * m_numCharts + chartIndex].write_to_txt(txtFile, simpleTxtFile);
		}
	}

	fflush(txtFile);
	fflush(simpleTxtFile);
}

void SongSection::setOrganized(unsigned long org)
{
	if (m_organized != org)
	{
		m_organized = org;
		org ? --m_parent->m_unorganized : ++m_parent->m_unorganized;
	}
}

SongSection::Condition::Condition()
	: m_type(0)
	, m_argument(0)
	, m_trueEffect(0)
	, m_falseEffect(0) {}

SongSection::Condition::Condition(FILE* inFile)
{
	fread(&m_type, 4, 1, inFile);
	fread(&m_argument, 4, 1, inFile);
	fread(&m_trueEffect, 4, 1, inFile);
	fread(&m_falseEffect, 4, 1, inFile);
}

void SongSection::Condition::write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile, const CHC* const chc)
{
	switch (m_type)
	{
	case 0:
		fputs("\t\t\t\t\t\tType: Unconditional\n", txtFile);
		dualvfprintf_s(txtFile, simpleTxtFile,
			"\t\t\t\t\t      Result: Always true.\n");
		break;
	case 1:
		fputs("\t\t\t\t\t\tType: Left Side Energy\n", txtFile);
		fprintf(txtFile, "\t\t\t\t\t    Argument: %g\n", m_argument);
		dualvfprintf_s(txtFile, simpleTxtFile,
			"\t\t\t\t\t      Result: True if Left Side's energy is less than %g%%. False otherwise.\n", m_argument * 100.0);
		break;
	case 2:
		fputs("\t\t\t\t\t\tType: Right Side Energy\n", txtFile);
		fprintf(txtFile, "\t\t\t\t\t    Argument: %g\n", m_argument);
		dualvfprintf_s(txtFile, simpleTxtFile,
			"\t\t\t\t\t      Result: True if Right Side's energy is less than %g%%. False otherwise.\n", m_argument * 100.0);
		break;
	case 3:
		fputs("\t\t\t\t\t\tType: Random\n", txtFile);
		fprintf(txtFile, "\t\t\t\t\t    Argument: %g\n", m_argument);
		dualvfprintf_s(txtFile, simpleTxtFile,
			"\t\t\t\t\t      Result: Generates a random number between 0.0 and 1.0, True if the random number is less than %g%%. False otherwise.\n", m_argument);
		break;
	case 4:
		fputs("\t\t\t\t\t\tType: Left Side Unavailable\n", txtFile);
		dualvfprintf_s(txtFile, simpleTxtFile,
			"\t\t\t\t\t      Result: False if this match has a Player 3 participating (only possible in Versus mode). True otherwise.\n");
		break;
	case 5:
		fputs("\t\t\t\t\t\tType: Right Side Unavailable\n", txtFile);
		dualvfprintf_s(txtFile, simpleTxtFile,
			"\t\t\t\t\t      Result: False if this match has a Player 4 participating (only possible in Versus mode). True otherwise.\n");
		break;
	case 6:
		fputs("\t\t\t\t\t\tType: Left Side < Right Side\n", txtFile);
		dualvfprintf_s(txtFile, simpleTxtFile,
			"\t\t\t\t\t      Result: True if Left Side's energy is less than Right Side's energy. False otherwise.\n");
		break;
	}

	if (m_trueEffect < 0)
	{
		if (m_trueEffect == -1)
			dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t\t True Effect: Move to the next condition.\n");
		else
			dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t\t True Effect: Move over %li conditions.\n", abs(m_trueEffect));
	}
	else
	{
		if ((size_t)m_trueEffect < chc->m_sections.size())
			dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t\t True Effect: Move to Section %s.\n", chc->m_sections[m_trueEffect].m_name);
		else
			dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t\t True Effect: End song.\n");
	}

	if (m_type != 0)
	{
		if (m_falseEffect < 0)
		{
			if (m_falseEffect == -1)
				dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t\tFalse Effect: Move to the next condition\n");
			else
				dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t\tFalse Effect: Move over %li conditions.\n", abs(m_falseEffect));
		}
		else
		{
			if ((size_t)m_falseEffect < chc->m_sections.size())
				dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t\tFalse Effect: Move to Section %s.\n", chc->m_sections[m_falseEffect].m_name);
			else
				dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t\tFalse Effect: End song.\n");
		}
	}
}

//I think this is obvious
bool SongSection::removeCondition(size_t index)
{
	if (m_conditions.size() != 1 && index < m_conditions.size())
	{
		m_conditions.erase(m_conditions.begin() + index);
		printf("Condition %zu removed\n", index + 1);
		return true;
	}
	else
	{
		if (m_conditions.size() == 1)
			printf_tab("Cannot delete condition - a section must have at least 1 condition\n");
		else
			printf_tab("Index out of range - # of Conditions: %zu\n", m_conditions.size());
		return false;
	}
}

//Clears all conditions minus 1
void SongSection::clearConditions()
{
	if (m_conditions.size() > 1)
		m_conditions.assign(1, m_conditions.back());
}

//Returns the byte size of the section
unsigned long SongSection::getSize() const
{
	unsigned long size = 64 + 16 * (unsigned long)m_conditions.size();
	for (auto& chart : m_charts)
		size += chart.getSize();
	return size;
}

//Add a chart to the end of every player's sector
void SongSection::operator++()
{
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
			m_charts.erase(m_charts.begin() + player * m_numCharts - 1);
		m_numCharts--;
		return true;
	}
	else
		return false;
}

//Creates Chart object with 1 Trace line
Chart::Chart(const bool addTraceline)
	: m_pivotTime(0)
	, m_endTime(0)
{
	if (addTraceline)
	{
		m_tracelines.push_back({});
		m_endTime = 1;
	}
}

Chart::Chart()
	: Chart(true) {}

Chart::Chart(FILE* inFile)
{
	// Skip Chart size as the embedded value can be wrong
	fseek(inFile, 16, SEEK_CUR);
	fread(m_junk, 1, 16, inFile);
	fread(&m_pivotTime, 4, 1, inFile);
	fread(&m_endTime, 4, 1, inFile);

	unsigned long numNotes;
	// Read number of trace lines
	fread(&numNotes, 1, 4, inFile);
	// Uses Traceline FILE* constructor
	for (unsigned traceIndex = 0; traceIndex < numNotes; ++traceIndex)
		emplace_ordered(m_tracelines, inFile);

	// Read number of Phrase bars
	fread(&numNotes, 1, 4, inFile);
	// Uses Phrase FILE* constructor
	for (unsigned phraseIndex = 0; phraseIndex < numNotes; ++phraseIndex)
		emplace_ordered(m_phrases, inFile);

	// Read number of Guard marks
	fread(&numNotes, 1, 4, inFile);
	// Uses Guard FILE* constructor
	for (unsigned guardIndex = 0; guardIndex < numNotes; ++guardIndex)
		emplace_ordered(m_guards, inFile);
	fseek(inFile, 4, SEEK_CUR);
}

void Chart::create(FILE* outFile)
{
	fputs("CHCH", outFile);
	unsigned long value = 4864UL;
	fwrite(&value, 1, 4, outFile);

	value = getSize();
	fwrite(&value, 1, 4, outFile);
	fwrite("\0\0\0\0\0\0\0\0", 1, 8, outFile);
	fwrite(m_junk, 1, 16, outFile);
	fwrite(&m_pivotTime, 4, 1, outFile);
	fwrite(&m_endTime, 4, 1, outFile);

	value = (unsigned long)m_tracelines.size();
	fwrite(&value, 4, 1, outFile);
	for (Traceline& trace : m_tracelines)
		trace.create(outFile);

	value = (unsigned long)m_phrases.size();
	fwrite(&value, 4, 1, outFile);
	for (Phrase& phrase : m_phrases)
		phrase.create(outFile);

	value = (unsigned long)m_guards.size();
	fwrite(&value, 4, 1, outFile);
	for (Guard& guard : m_guards)
		guard.create(outFile);

	fwrite("\0\0\0\0", 1, 4, outFile);
}

void Chart::write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile)
{
	fprintf(txtFile, "\t\t\t\t\t  Size (32bit): %lu\n", getSize());

	fprintf(txtFile, "\t\t\t\t\t\t  Junk: 0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(m_junk)));
	fprintf(txtFile, "\t\t\t\t\t\t\t0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(m_junk + 4)));
	fprintf(txtFile, "\t\t\t\t\t\t\t0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(m_junk + 8)));
	fprintf(txtFile, "\t\t\t\t\t\t\t0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(m_junk + 12)));

	dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t\t   Pivot Point: %lu samples\n", m_pivotTime);
	fprintf(txtFile, "\t\t\t\t\t      End Time: %lu samples\n", m_endTime);

	dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t      # of Trace Lines: %zu\n", m_tracelines.size());
	for (size_t traceIndex = 0; traceIndex < m_tracelines.size(); ++traceIndex)
	{
		dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t\t\t  Trace Line %03zu:\n", traceIndex + 1);
		m_tracelines[traceIndex].write_to_txt(txtFile, simpleTxtFile, m_pivotTime);
		fprintf(txtFile, "\t\t\t\t\t\t\t    End Time: %li samples (Relative to SongSection)\n", m_tracelines[traceIndex].getEndAlpha() + m_pivotTime);

		if (traceIndex + 1 == m_tracelines.size())
			fprintf(simpleTxtFile, "\t\t\t\t\t\t\t    End Time: %li samples (Relative to SongSection)\n", m_tracelines[traceIndex].getEndAlpha() + m_pivotTime);
	}

	dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t # of Phrase Fragments: %zu\n", m_phrases.size());
	for (size_t traceIndex = 0, phraseIndex = 0, note = 0; traceIndex < m_tracelines.size(); ++traceIndex)
	{
		for (size_t piece = 0;
			phraseIndex < m_phrases.size() && m_tracelines[traceIndex].contains(m_phrases[phraseIndex].m_pivotAlpha);
			++phraseIndex, ++piece)
		{
			dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t\t     Phrase Fragment %03zu:\n", phraseIndex + 1);
			dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t\t     [Note #%03zu", note + 1);

			if (piece > 0)
				dualvfprintf_s(txtFile, simpleTxtFile, " - Piece #%02zu", piece + 1);

			fprintf(txtFile, "| Trace Line #%03zu]:\n", traceIndex + 1);
			fputs("]:\n", simpleTxtFile);

			m_phrases[phraseIndex].write_to_txt(txtFile, simpleTxtFile, m_pivotTime);

			if (m_phrases[phraseIndex].m_end)
			{
				++phraseIndex;
				break;
			}
		}
	}

	dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t      # of Guard Marks: %zu\n", getNumGuards());
	for (size_t guardIndex = 0; guardIndex < getNumGuards(); guardIndex++)
	{
		dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t\t\t  Guard Mark %03zu:\n", guardIndex + 1);
		m_guards[guardIndex].write_to_txt(txtFile, simpleTxtFile, m_pivotTime);
	}
}

unsigned long Chart::getSize() const
{
	return 60 + 16 * unsigned long(m_tracelines.size() + m_guards.size() + 2 * m_phrases.size());
}

void Chart::setJunk(char* newJunk, rsize_t count)
{
	std::copy(newJunk, newJunk + count, m_junk);
}

//Add a note to its corresponding List in ascending pivotAlpha order
void Chart::add(Note* note)
{
	if (dynamic_cast<Traceline*>(note) != nullptr)
		add(static_cast<Traceline*>(note));

	else if (dynamic_cast<Phrase*>(note) != nullptr)
		add(static_cast<Phrase*>(note));
	else
		add(static_cast<Guard*>(note));
}

//Add a note to its corresponding List in ascending pivotAlpha order
void Chart::add(Traceline* note)
{
	emplace_ordered(m_tracelines, *note);
}

void Chart::add(Phrase* note)
{
	emplace_ordered(m_phrases, *note);
}

void Chart::add(Guard* note)
{
	emplace_ordered(m_guards, *note);
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
		m_tracelines.erase(m_tracelines.begin(), m_tracelines.begin() + m_tracelines.size() - 1);
	m_phrases.clear();
	m_guards.clear();
}

void Chart::finalizeNotes()
{
	if (m_tracelines.size() > 1)
	{
		for (size_t trIndex = 0; trIndex < m_tracelines.size(); ++trIndex)
		{
			if (trIndex + 1 < m_tracelines.size())
				m_tracelines[trIndex].changeEndAlpha(m_tracelines[trIndex + 1].m_pivotAlpha);
			m_tracelines[trIndex].m_pivotAlpha -= m_pivotTime;
		}

		for (size_t phrIndex = 0; phrIndex < m_phrases.size(); ++phrIndex)
		{
			if (!m_phrases[phrIndex].m_end)
			{
				if (phrIndex + 1 == m_phrases.size())
					m_phrases[phrIndex].m_end = true;
				else
					m_phrases[phrIndex].changeEndAlpha(m_phrases[phrIndex + 1].m_pivotAlpha);
			}
			m_phrases[phrIndex].m_pivotAlpha -= m_pivotTime;
		}

		// Go through every phrase bar & trace line to find places where phrase bars
		// should be split into two pieces
		for (size_t trIndex = 0, phrIndex = 0; phrIndex < m_phrases.size() && trIndex < m_tracelines.size();)
		{
			Traceline& trace = m_tracelines[trIndex];
			Phrase* phrase = &m_phrases[phrIndex];
			if (!trIndex && trace.m_pivotAlpha > phrase->m_pivotAlpha)
				trace.changePivotAlpha(phrase->m_pivotAlpha);

			while (phrIndex + 1 < m_phrases.size() && phrase->getEndAlpha() <= trace.m_pivotAlpha)
				++phrIndex;

			if (phrase->m_pivotAlpha < trace.m_pivotAlpha && trace.m_pivotAlpha < phrase->getEndAlpha())
			{
				m_phrases.insert(m_phrases.begin() + phrIndex + 1,
					{ trace.m_pivotAlpha
					, unsigned long(phrase->getEndAlpha() - trace.m_pivotAlpha)
					, false
					, phrase->m_end
					, 0
					, phrase->getColor() });

				m_phrases[phrIndex].changeEndAlpha(trace.m_pivotAlpha);
				m_phrases[phrIndex].m_end = false;
				phrase = &m_phrases[++phrIndex];
			}

			if (trIndex + 1 == m_tracelines.size())
			{
				// If the phrase bar lands at or after the last trace line, delete
				if (trace.getEndAlpha() <= phrase->m_pivotAlpha)
				{
					if (!phrase->m_start)
						m_phrases[phrIndex - 1].m_end = true;

					m_phrases.erase(m_phrases.begin() + phrIndex);
					printf("Phrase bar %zu removed\n", phrIndex);
				}
				else
				{
					if (phrase->getEndAlpha() > trace.getEndAlpha())
					{
						phrase->changeEndAlpha(trace.getEndAlpha());
						phrase->m_end = true;
					}
					++phrIndex;
				}
			}
			else
				++trIndex;
		}
	}

	for (Guard& grd : m_guards)
		grd.m_pivotAlpha -= m_pivotTime;
}

Note::Note()
	: m_pivotAlpha(0) {}

Note::Note(long alpha) 
	: m_pivotAlpha(alpha) {}

Note::Note(FILE* inFile)
{
	fread(&m_pivotAlpha, 4, 1, inFile);
}

Note& Note::operator=(const Note& note)
{
	m_pivotAlpha = note.m_pivotAlpha;
	return *this;
}

void Note::create(FILE* outFile)
{
	fwrite(&m_pivotAlpha, 4, 1, outFile);
}

Path::Path()
	: Note()
	, m_duration(1) {}

Path::Path(long alpha, unsigned long dur)
	: Note(alpha)
	, m_duration(dur) {}

Path::Path(FILE* inFile)
	: Note(inFile)
{
	fread(&m_duration, 4, 1, inFile);
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

void Path::create(FILE* outFile)
{
	Note::create(outFile);
	fwrite(&m_duration, 4, 1, outFile);
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

Traceline::Traceline()
	: Path()
	, m_angle(0)
	, m_curve(false) {}

Traceline::Traceline(long alpha, unsigned long dur, float ang, unsigned long cur)
	: Path(alpha, dur)
	, m_angle(ang)
	, m_curve(cur) {}

Traceline::Traceline(FILE* inFile)
	: Path(inFile)
{
	fread(&m_angle, 4, 1, inFile);
	fread(&m_curve, 4, 1, inFile);
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

void Traceline::create(FILE* outFile)
{
	Path::create(outFile);
	fwrite(&m_angle, 4, 1, outFile);
	fwrite(&m_curve, 4, 1, outFile);
}

void Traceline::write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile, const long pivotTime)
{
	dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t\t\t\t Pivot Alpha: %+li samples\n", m_pivotAlpha);
	dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t\t\t\t  Start Time: %li samples (Relative to SongSection)\n", m_pivotAlpha + pivotTime);

	fprintf(txtFile, "\t\t\t\t\t\t\t    Duration: %lu samples\n", m_duration);
	fprintf(txtFile, "\t\t\t\t\t\t\t       Angle: %s*PI radians | %li degrees\n", angleToFraction(m_angle).c_str(), radiansToDegrees(m_angle));
	m_curve ? fputs("\t\t\t\t\t\t\t       Curve: True\n", txtFile)
		: fputs("\t\t\t\t\t\t\t       Curve: False\n", txtFile);
}

Phrase::Phrase()
	: Path()
	, m_start(true)
	, m_end(true)
	, m_animation(0)
	, m_color(0) {}

Phrase::Phrase(long alpha, unsigned long dur, unsigned long start, unsigned long end, unsigned long anim, unsigned long color)
	: Path(alpha, dur)
	, m_start(start)
	, m_end(end)
	, m_animation(anim)
	, m_color(color) {}

Phrase::Phrase(FILE* inFile)
	: Path(inFile)
{
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

void Phrase::create(FILE* outFile)
{
	Path::create(outFile);
	fwrite(&m_start, 4, 1, outFile);
	fwrite(&m_end, 4, 1, outFile);
	fwrite(&m_animation, 4, 1, outFile);
	if (m_color != -1)
	{
		fputs("NOTECOLR", outFile);
		fwrite(&m_color, 4, 1, outFile);
	}
	else
		fwrite(m_junk, 1, 12, outFile);
}

void Phrase::write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile, const long pivotTime)
{
	dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t\t\t\t Pivot Alpha: %+li samples\n", m_pivotAlpha);
	dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t\t\t\t  Start Time: %li samples (Relative to SongSection)\n",
		m_pivotAlpha + pivotTime);
	fprintf(txtFile, "\t\t\t\t\t\t\t    Duration: %lu samples\n", m_duration);
	fprintf(txtFile, "\t\t\t\t\t\t\t       Start: %s\n", m_start ? "True" : "False");
	fprintf(txtFile, "\t\t\t\t\t\t\t         End: %s\n", m_end ? "True" : "False");
	fprintf(txtFile, "\t\t\t\t\t\t\t   Animation: %lu\n", m_animation);

	if (m_end)
	{
		dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t\t\t\t    End Time: %li samples (Relative to SongSection)\n",
			getEndAlpha() + pivotTime);
	}
	else
	{
		fprintf(txtFile, "\t\t\t\t\t\t\t    End Time: %li samples (Relative to SongSection)\n",
			getEndAlpha() + pivotTime);
	}

	if (m_color)
	{
		dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t\t\t\tColor Export: %s%s%s%s%s%s%s\n"
			, m_color & 1 ? "G" : ""
			, m_color & 2 ? "R" : ""
			, m_color & 4 ? "Y" : ""
			, m_color & 8 ? "B" : ""
			, m_color & 16 ? "O" : ""
			, m_color & 32 ? "P" : ""
			, m_color & 64 ? " (Tap)" : "");
	}
	else
	{
		fprintf(txtFile, "\t\t\t\t\t\t\t\tJunk: 0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(m_junk)));
		fprintf(txtFile, "\t\t\t\t\t\t\t\t      0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(m_junk + 4)));
		fprintf(txtFile, "\t\t\t\t\t\t\t\t      0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(m_junk + 8)));
	}
}

Guard::Guard()
	: Note()
	, m_button(0) {}

Guard::Guard(long alpha, unsigned long but)
	: Note(alpha)
	, m_button(but)
{
	if (m_button > 3)
		throw "Error: Invalid button choice for Guard Mark";
}

Guard::Guard(FILE* inFile)
	: Note(inFile)
{
	fread(&m_button, 4, 1, inFile);
	fseek(inFile, 8, SEEK_CUR);
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

void Guard::create(FILE* outFile)
{
	Note::create(outFile);
	fwrite(&m_button, 4, 1, outFile);
	fwrite("\0\0\0\0\0\0\0\0", 1, 8, outFile);
}

void Guard::write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile, const long pivotTime)
{
	dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t\t\t\t Pivot Alpha: %+li samples\n", m_pivotAlpha);
	dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t\t\t\t  Start Time: %li samples (Relative to SongSection)\n", m_pivotAlpha + pivotTime);
	dualvfprintf_s(txtFile, simpleTxtFile, "\t\t\t\t\t\t\t      Button: ");
	switch (m_button)
	{
	case 0:
		fputs("Left\n", txtFile);
		fputs("Square\n", simpleTxtFile);
		break;
	case 1:
		fputs("Down\n", txtFile);
		fputs("X/Cross\n", simpleTxtFile);
		break;
	case 2:
		fputs("Right\n", txtFile);
		fputs("Circle\n", simpleTxtFile);
		break;
	case 3:
		fputs("Up\n", txtFile);
		fputs("Triangle\n", simpleTxtFile);
	}
}
