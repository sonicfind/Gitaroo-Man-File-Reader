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
#include "FileMainList.h"
using namespace std;
using namespace GlobalFunctions;

//Loads a CHC file and runs the single-file function
/*
Main menu prompt used for choosing what action to perform on the loaded CHC file.
Returns false if this is used from the multimenu and if the user wants to proceed to the next CHC.
*/
bool FileMain<CHC>::singleFile()
{
	bool val = false;
	if (CHC* chart = loadFile())
	{
		while (!g_global.quit)
		{
			banner(" " + chart->m_filename + ".CHC - Mode Selection ");
			string choices = "swpdtei";
			printf_tab("S - Save\n");
			printf_tab("W - Write %s_CHC.txt\n", chart->m_filename.c_str());
			printf_tab("P - Swap players\n");
			if (!chart->isOrganized() || !chart->isOptimized())
			{
				printf_tab("F - Fix & organize %s.CHC where necessary (may not get all present bugs)\n", chart->m_filename.c_str());
				printf_tab("B - Both 'P' & 'F'\n");
				choices += "fb";
			}
			printf_tab("D - Detailed modification\n");
			if (chart->isPS2Compatible())
				printf_tab("T - Convert into a PCSX2 TAS (only works with songs available in the PS2 version)\n");
			printf_tab("E - Export as \".chart\" for chart editing in Moonscraper\n");
			printf_tab("I - Import notes from a Clone/Guitar Hero \".chart\" file\n");
			if (chart->isPS2Compatible())
			{
				printf_tab("C - Create a PCSX2 Phrase Bar Color Cheat template\n");
				choices += 'c';
			}
			printf_tab("? - Help info\n");
			
			if (g_fileMains.checkFilenameLists(this, 0))
				choices += 'n';

			switch (menuChoices(choices))
			{
			case ResultType::Quit:
				if (chart->getSaveStatus() == 0)
				{
					printf_tab("\n");
					printf_tab("Recent changes have not been saved externally to a CHC file. Which action will you take?\n");
					printf_tab("S - Save CHC and Exit to Main Menu\n");
					printf_tab("Q - Exit without saving\n");
					printf_tab("C - Cancel\n");
					switch (menuChoices("sc"))
					{
					case ResultType::Success:
						if (g_global.answer.character == 'c'
							|| !saveFile(true))
							break;
						__fallthrough;
					case ResultType::Quit:
						g_global.quit = true;
					}
				}
				else
					g_global.quit = true;
				break;
			case ResultType::Help:
			{
				printf_tab("W - Write %s.txt\n", chart->m_filename.c_str());
				printf_tab("Writes out the data present in the CHC file to a readable txt file. This would include all SSQ & cue data, audio values,\n");
				printf_tab("section & subsection data - including values for every separate note -, and damage & energy factors. Creates two files -\n");
				printf_tab("with one being a simplistic form with only the bare minimum necessary information.\n%s\n", g_global.tabs.c_str());

				printf_tab("P - Swap players\n");
				printf_tab("Goes through and swaps the charts between the two main payers. For PS2 compatible CHCs, this will swap all charts for P1/P3 with\n");
				printf_tab("P2/P4 respectively. For duet CHCs, this will only swap the charts of P1 with P3 as P2 is not important enough to warrant a swap & P4 is empty.\n");
				printf_tab("Note: only use this on either 1. A base CHC that was not swapped using an alternate method, or 2. a CHC pre-swapped with this program.\n%s\n", g_global.tabs.c_str());

				if (!chart->isOrganized() || !chart->isOptimized())
				{
					printf_tab("F - Fix & organize %s.CHC where necessary (may not get all present bugs)\n", chart->m_filename.c_str());
					printf_tab("Many CHC files provided from the base game are very prone to a multitude of glitches or contain sizable mistakes.\n");
					printf_tab("For example: Stage 8 having two guard marks being only 2 samples away from eachother, or Stage 3 having a broken attack phrase note.\n");
					printf_tab("This function aims to fix many of these issues while also applying an chart organization to evenly balance out chart counts amongst players.\n");
					printf_tab("Note: if you come across an issue with it or have the program crash, send me the CHC file used in the Gitaroo Pals discord: https://discord.gg/ed6P8Jt \n");
					printf_tab("(sonicfind#6404)\n%s\n", g_global.tabs.c_str());

					printf_tab("B - Both 'P' & 'F'\n");
					printf_tab("Does both option 'S' & option 'F' in a single function.\n%s\n", g_global.tabs.c_str());
				}

				printf_tab("D - Detailed modification\n");
				printf_tab("Takes you to a menu where you can manually edit some of the smaller details of the CHC, including but not limited to: IMC and audio settings,\n");
				printf_tab("gameplay speed/speed of all notes, .SSQ data, and player damage/energy multiplier factors.\n%s\n", g_global.tabs.c_str());

				printf_tab("T - Convert into a PCSX2 TAS (only works with songs compatible with the PS2 version)\n");
				printf_tab("Generates a TAS file that can be played in PCSX2 from the CHC's note data. Very useful for testing charts that have been edited.\n");
				printf_tab("Note 1: obviously, this only works on CHCs compatible with the PS2 version of the game.\n");
				printf_tab("Note 2: currently requires using my own custom build (for multitap support and optimized file sizes)\n%s\n", g_global.tabs.c_str());

				printf_tab("E - Export as \".chart\" for chart editing in Moonscraper\n");
				printf_tab("Converts the CHC into .chart for use in the Guitar Hero/Clone Hero chart editor, Moonscraper. You can download Moonscraper from the #test-builds channel.\n");
				printf_tab("in their discord: https://discord.gg/5BdQnsN \n");
				printf_tab("Note: this can only be used with CHCs that are fixed & organized.\n%s\n", g_global.tabs.c_str());

				printf_tab("I - Import notes from a Clone/Guitar Hero \".chart\" file\n");
				printf_tab("Inserts & replaces section and note data from correctly formatted .chart files into each given CHC file. This is will not generate any new files or \n");
				printf_tab("overwrite the current file automatically. That decision will be made during the process with each separate CHC.\n");
				printf_tab("Refer to the included Chart_import_README.txt for details on formatting a .chart file for this use\n");
				printf_tab("Note: this can only be used with CHCs that are fixed & organized.\n%s\n", g_global.tabs.c_str());

				printf_tab("C - Create a PCSX2 Phrase Bar Color Cheat template\n");
				printf_tab("Generates a outline txt file for use in conjunction with BoringPerson#2570's phrase bar coloring Pyhton script. It will leave all chosen\n");
				printf_tab("sections blank for you to fill in and all others filled with a base color. Feel free to change the colors in the file after it's generated.\n%s\n", g_global.tabs.c_str());
			}
				break;
			case ResultType::Success:
				if (g_global.answer.character == 'n')
				{
					delete chart;
					return false;
				}
				else
				{
					++g_global;
					switch (g_global.answer.character)
					{
					case 's':
						saveFile();
						break;
					case 'w':
						chart->write_to_txt();
						break;
					case 'p':
						chart->applyChanges(false, true);
						break;
					case 'f':
						chart->applyChanges(true, false);
						break;
					case 'b':
						chart->applyChanges(true, true);
						break;
					case 'd':
						chart->edit();
						break;
					case 't':
						chart->buildTAS();
						break;
					case 'e':
						chart->exportForCloneHero();
						break;
					case 'i':
						if (CHC* song = chart->importFromCloneHero())
						{
							delete chart;
							chart = song;
						}
						break;
					case 'c':
						chart->colorCheatTemplate();
					}
					--g_global;
				}
			}
		}
		g_global.quit = false;
		delete chart;
		chart = nullptr;
	}
	return val;
}

/*
Cycles through every file in the provided list and performs the function chosen in the nested menu.
Returns true if the user wishes to end the program, false otherwise.
*/
bool FileMain<CHC>::multipleFiles()
{
	if (m_filenames.size() == 0)
		return false;

	do
	{
		ResultType result = ResultType::Success;
		string choices = "ewpfbdcktgi";
		if (m_filenames.size() > 1)
		{
			banner(" CHC Mode Selection ");
			printf_tab("E - Evaluate each CHC individually\n");
			printf_tab("W - Write all CHCs included to readable .txts\n");
			printf_tab("P - Swap players in all CHCs\n");
			printf_tab("F - Fix & organize all CHCs where necessary (may not fix all bugs)\n");
			printf_tab("B - Both 'S' & 'F'\n");
			printf_tab("D - Perform detailed modifications on each CHC\n");
			printf_tab("T - Convert each CHC into a PCSX2 TAS (only works with CHCs compatible with the PS2 version)\n");
			printf_tab("G - Export each CHC as a \".chart\" file for Clone/Guitar Hero\n");
			printf_tab("I - Import notes from Clone/Guitar Hero \".chart\"s into the CHC files\n");
			printf_tab("C - Create a PCSX2 Phrase Bar Color Cheat template for each CHC\n");

			if (g_fileMains.checkFilenameLists(this, 0))
				choices += 'n';

			printf_tab("Q - Quit Program\n");
			result = menuChoices(choices);
		}
		else
			g_global.answer.character = 'e';
		switch (result)
		{
		case ResultType::Quit:
			return true;
		case ResultType::Help:
			printf_tab("W - Write all CHC's included to readable .txts\n");
			printf_tab("Writes out the data present in each CHC file to a readable txt file. This would include all SSQ & cue data, audio values,\n");
			printf_tab("section & subsection data - including values for every separate note -, and damage & energy factors. Creates two files -\n");
			printf_tab("with one being a simplistic form with only the bare minimum necessary information.\n%s\n", g_global.tabs.c_str());

			printf_tab("P - Swap players in all CHCs\n");
			printf_tab("Goes through and swaps the charts between the two main payers. For PS2 compatible CHCs, this will swap all charts for P1/P3 with\n");
			printf_tab("P2/P4 respectively. For duet CHCs, this will only swap the charts of P1 with P3 as P2 is not important enough to warrant a swap & P4 is empty.\n");
			printf_tab("Note: only use this on either 1. A base CHC that was not swapped using an alternate method, or 2. a CHC pre-swapped with this program.\n%s\n", g_global.tabs.c_str());

			printf_tab("F - Fix & organize all CHCs where necessary (may not fix all bugs)\n");
			printf_tab("Many CHC files provided from the base game are very prone to a multitude of glitches or contain sizable mistakes.\n");
			printf_tab("For example: Stage 8 having two guard marks being only 2 samples away from eachother, or Stage 3 having a broken attack phrase note.\n");
			printf_tab("This function aims to fix many of these issues while also applying an chart organization to evenly balance out chart counts amongst players.\n");
			printf_tab("Note: if you come across an issue with it or have the program crash, send me the CHC file used in the Gitaroo Pals discord: https://discord.gg/ed6P8Jt \n");
			printf_tab("(sonicfind#6404)%s\n", g_global.tabs.c_str());

			printf_tab("B - Both 'P' & 'F'\n");
			printf_tab("Does both option 'P' & option 'F' in a single function.\n%s\n", g_global.tabs.c_str());

			printf_tab("D - Perform detailed modifications on each CHC\n");
			printf_tab("Takes you to a menu where you can manually edit some of the smaller details of each CHC file, including but not limited to: IMC and audio settings,\n");
			printf_tab("gameplay speed/speed of all notes, .SSQ data, and player damage/energy multiplier factors.\n%s\n", g_global.tabs.c_str());

			printf_tab("T - Convert each CHC into a PCSX2 TAS\n");
			printf_tab("Generates a TAS file that can be played in PCSX2 from each CHC's note data. Very useful for testing charts that have been edited.\n");
			printf_tab("Note 1: obviously, this only works on CHCs compatible with the PS2 version of the game.\n");
			printf_tab("Note 2: currently requires using a custom build (for multitap support and optimized file sizes);\n");
			printf_tab("Contact sonicfind#6404 in the Gitaroo Pals discord for the build (https://discord.gg/5BdQnsN).\n%s\n", g_global.tabs.c_str());

			printf_tab("G - Export each CHC as a \".chart\" file for Clone/Guitar Hero\n");
			printf_tab("Converts each CHC into a .chart for use in the Guitar Hero/Clone Hero chart editor, Moonscraper. You can download Moonscraper from the #test-builds channel.\n");
			printf_tab("in their discord: https://discord.gg/5BdQnsN \n");
			printf_tab("Note: this can only be used with CHCs that are fixed & organized.\n%s\n", g_global.tabs.c_str());

			printf_tab("I - Import notes from Clone/Guitar Hero \".chart\"s into the CHC files\n");
			printf_tab("Inserts & replaces section and note data from correctly formatted .chart files into each given CHC file. This is will not generate any new files or \n");
			printf_tab("overwrite the current file automatically. That decision will be made during the process with each separate CHC.\n");
			printf_tab("(Further formatting details in the Import_Readme.txt)\n");
			printf_tab("Note: this can only be used with CHCs that are fixed & organized.\n%s\n", g_global.tabs.c_str());

			printf_tab("C - Create PCSX2 Phrase Bar Color Cheat templates for each CHC\n");
			printf_tab("Generates outline .txt files for use in conjunction with BoringPerson#2570's phrase bar coloring Pyhton script. It will leave all chosen\n");
			printf_tab("sections blank for you to fill in and all others filled with a base color. Feel free to change the colors after the files are generated.\n%s\n", g_global.tabs.c_str());
			__fallthrough;
		case ResultType::Failed:
			break;
		case ResultType::Success:
			if (g_global.answer.character == 'n')
				return false;
			else
			{
				const char choice = g_global.answer.character;
				++g_global;
				while (m_filenames.size())
				{
					if (CHC* chart = loadFile())
					{
						switch (choice)
						{
						case 'e':
							if (singleFile())
							{
								--g_global;
								return false;
							}
							else
								break;
						case 'w':
							chart->write_to_txt();
							break;
						case 'p':
							chart->applyChanges(false, true, true);
							break;
						case 'f':
							chart->applyChanges(true, false, true);
							break;
						case 'b':
							chart->applyChanges(true, true, true);
							break;
						case 'd':
							chart->edit(true);
							break;
						case 't':
							chart->buildTAS();
							break;
						case 'g':
							chart->exportForCloneHero();
							break;
						case 'i':
							if (CHC* song = chart->importFromCloneHero())
							{
								delete chart;
								chart = song;
							}
							break;
						case 'c':
							chart->colorCheatTemplate();
						}
					}
				}
				--g_global;
				g_global.quit = true;
			}
		}
	} while (!g_global.quit);
	g_global.quit = false;
	printf_tab("All provided CHC files have been evaluated.\n");
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

	string filename = m_directory + m_filename + "_COLORDEF";
	string filename2 = m_directory + m_filename + "_COLORDEF_FRAGS";
	FILE *outSheet = nullptr, *outSheet2 = nullptr;
	do
	{
		switch (fileOverwriteCheck(filename + ".txt"))
		{
		case ResultType::No:
				printf_tab("\n");
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
			printf_tab("\n");
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
		string colors = "GRYBOPgrybop";
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
							if (inputs[sectIndex] )
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
