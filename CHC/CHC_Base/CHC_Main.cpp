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
#include "Global_Functions.h"
#include "CHC_Main.h"
using namespace std;

//Loads a CHC file and runs the single-file function
bool loadSingleCHC(string& filename)
{
	bool val = false;
	try
	{
		CHC_Main chc(filename);
		val = chc.menu();
	}
	catch (string str)
	{
		printf("%s%s\n", g_global.tabs.c_str(), str.c_str());
		printf("%sLoad cancelled for %s.CHC\n", g_global.tabs.c_str(), filename.c_str());
	}
	catch (const char* str)
	{
		printf("%s%s\n", g_global.tabs.c_str(), str);
		printf("%sLoad cancelled for %s.CHC\n", g_global.tabs.c_str(), filename.c_str());
	}
	g_global.adjustTabs(0);
	return val;
}

/*
Cycles through every file in the provided list and performs the function chosen in the nested menu.
Returns true if the user wishes to end the program, false otherwise.
*/
bool loadMultiCHC(LinkedList::List<string>* files)
{
	do
	{
		GlobalFunctions::ResultType result = GlobalFunctions::ResultType::Success;
		string choices = "ewpfbdck";
		if (files->size() > 1)
		{
			GlobalFunctions::banner(" CHC Mode Selection ");
			printf("%sE - Evaluate each CHC individually\n", g_global.tabs.c_str());
			printf("%sW - Write all CHCs included to readable .txts\n", g_global.tabs.c_str());
			printf("%sP - Swap players in all CHCs\n", g_global.tabs.c_str());
			printf("%sF - Fix & organize all CHCs where necessary (may not fix all bugs)\n", g_global.tabs.c_str());
			printf("%sB - Both 'S' & 'F'\n", g_global.tabs.c_str());
			printf("%sD - Perform detailed modifications on each CHC\n", g_global.tabs.c_str());
			if (GlobalFunctions::LoadLib(g_dlls[s_DLL_INDEX].m_libraries[1]))
			{
				printf("%sT - Convert each CHC into a PCSX2 TAS (only works with CHCs compatible with the PS2 version)\n", g_global.tabs.c_str());
				choices += 't';
			}
			if (GlobalFunctions::LoadLib(g_dlls[s_DLL_INDEX].m_libraries[2]))
			{
				printf("%sG - Export each CHC as a \".chart\" file for Clone/Guitar Hero\n", g_global.tabs.c_str());
				choices += 'g';
				printf("%sI - Import notes from Clone/Guitar Hero \".chart\"s into the CHC files\n", g_global.tabs.c_str());
				choices += 'i';
			}
			printf("%sC - Create a PCSX2 Phrase Bar Color Cheat template for each CHC\n", g_global.tabs.c_str());
			for (size_t i = 1; i < 20 && !g_global.quit; i++)
			{
				for (size_t s = 0; s < g_dlls[i].m_extensions.size() && !g_global.quit; s++)
				{
					if (g_dlls[i].m_extensions[s].m_files.size() > 0)
					{
						printf("%sN - Proceed to the next filetype (%s)\n", g_global.tabs.c_str(), g_dlls[i].m_extensions[s].m_ext.c_str());
						choices += 'n';
						g_global.quit = true;
					}
				}
			}
			g_global.quit = false;
			printf("%sQ - Quit Program\n", g_global.tabs.c_str());
			result = GlobalFunctions::menuChoices(choices);
		}
		else
			g_global.answer.character = 'e';
		switch (result)
		{
		case GlobalFunctions::ResultType::Quit:
			return true;
		case GlobalFunctions::ResultType::Help:
			printf("%sW - Write all CHC's included to readable .txts\n", g_global.tabs.c_str());
			printf("%sWrites out the data present in each CHC file to a readable txt file. This would include all SSQ & cue data, audio values,\n", g_global.tabs.c_str());
			printf("%ssection & subsection data - including values for every separate note -, and damage & energy factors. Creates two files -\n", g_global.tabs.c_str());
			printf("%swith one being a simplistic form with only the bare minimum necessary information.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());

			printf("%sP - Swap players in all CHCs\n", g_global.tabs.c_str());
			printf("%sGoes through and swaps the charts between the two main payers. For PS2 compatible CHCs, this will swap all charts for P1/P3 with\n", g_global.tabs.c_str());
			printf("%sP2/P4 respectively. For duet CHCs, this will only swap the charts of P1 with P3 as P2 is not important enough to warrant a swap & P4 is empty.\n", g_global.tabs.c_str());
			printf("%sNote: only use this on either 1. A base CHC that was not swapped using an alternate method, or 2. a CHC pre-swapped with this program.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());

			printf("%sF - Fix & organize all CHCs where necessary (may not fix all bugs)\n", g_global.tabs.c_str());
			printf("%sMany CHC files provided from the base game are very prone to a multitude of glitches or contain sizable mistakes.\n", g_global.tabs.c_str());
			printf("%sFor example: Stage 8 having two guard marks being only 2 samples away from eachother, or Stage 3 having a broken attack phrase note.\n", g_global.tabs.c_str());
			printf("%sThis function aims to fix many of these issues while also applying an chart organization to evenly balance out chart counts amongst players.\n", g_global.tabs.c_str());
			printf("%sNote: if you come across an issue with it or have the program crash, send me the CHC file used in the Gitaroo Pals discord: https://discord.gg/ed6P8Jt \n", g_global.tabs.c_str());
			printf("%s(sonicfind#6404)%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());

			printf("%sB - Both 'P' & 'F'\n", g_global.tabs.c_str());
			printf("%sDoes both option 'P' & option 'F' in a single function.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());

			printf("%sD - Perform detailed modifications on each CHC\n", g_global.tabs.c_str());
			printf("%sTakes you to a menu where you can manually edit some of the smaller details of each CHC file, including but not limited to: IMC and audio settings,\n", g_global.tabs.c_str());
			printf("%sgameplay speed/speed of all notes, .SSQ data, and player damage/energy multiplier factors.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());

			if (GlobalFunctions::LoadLib(g_dlls[s_DLL_INDEX].m_libraries[1]))
			{
				printf("%sT - Convert each CHC into a PCSX2 TAS (only works with CHCs compatible with the PS2 version)\n", g_global.tabs.c_str());
				printf("%sGenerates a TAS file that can be played in PCSX2 from each CHC's note data. Very useful for testing charts that have been edited.\n", g_global.tabs.c_str());
				printf("%sNote 1: obviously, this only works on CHCs compatible with the PS2 version of the game.\n", g_global.tabs.c_str());
				printf("%sNote 2: currently requires using a custom build (for multitap support and m_optimized file sizes);\n", g_global.tabs.c_str());
				printf("%sContact sonicfind#6404 in the Gitaroo Pals discord for the build (https://discord.gg/5BdQnsN).\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
			}

			if (GlobalFunctions::LoadLib(g_dlls[s_DLL_INDEX].m_libraries[2]))
			{
				printf("%sG - Export each CHC as a \".chart\" file for Clone/Guitar Hero\n", g_global.tabs.c_str());
				printf("%sConverts each CHC into a .chart for use in the Guitar Hero/Clone Hero chart editor, Moonscraper. You can download Moonscraper from the #test-builds channel.\n", g_global.tabs.c_str());
				printf("%sin their discord: https://discord.gg/5BdQnsN \n", g_global.tabs.c_str());
				printf("%sNote: this can only be used with CHCs that are fixed & organized.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());

				printf("%sI - Import notes from Clone/Guitar Hero \".chart\"s into the CHC files\n", g_global.tabs.c_str());
				printf("%sInserts & replaces section and note data from correctly formatted .chart files into each given CHC file. This is will not generate any new files or \n", g_global.tabs.c_str());
				printf("%soverwrite the current file automatically. That decision will be made during the process with each separate CHC.\n", g_global.tabs.c_str());
				printf("%s(Further formatting details in the Import_Readme.txt)\n", g_global.tabs.c_str());
				printf("%sNote: this can only be used with CHCs that are fixed & organized.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
			}

			printf("%sC - Create PCSX2 Phrase Bar Color Cheat templates for each CHC\n", g_global.tabs.c_str());
			printf("%sGenerates outline .txt files for use in conjunction with BoringPerson#2570's phrase bar coloring Pyhton script. It will leave all chosen\n", g_global.tabs.c_str());
			printf("%ssections blank for you to fill in and all others filled with a base color. Feel free to change the colors after the files are generated.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
		case GlobalFunctions::ResultType::Failed:
			break;
		case GlobalFunctions::ResultType::Success:
			if (g_global.answer.character == 'n')
				return false;
			else
			{
				const char choice = g_global.answer.character;
				++g_global;
				while (files->size())
				{
					try
					{
						CHC_Main chc(files->front());
						switch (choice)
						{
						case 'e':
							if (chc.menu(files->size()))
							{
								--g_global;
								return false;
							}
							else
								break;
						case 'w':
							chc.writeTxt();
							break;
						case 'p':
							chc.applyChanges(false, true, true);
							break;
						case 'f':
							chc.applyChanges(true, false, true);
							break;
						case 'b':
							chc.applyChanges(true, true, true);
							break;
						case 'd':
							chc.edit(true);
							break;
						case 't':
							chc.makeTAS();
							break;
						case 'g':
							chc.exportChart();
							break;
						case 'i':
							chc.importChart();
							break;
						case 'c':
							chc.createColorTemplate();
						}
					}
					catch (string str)
					{
						printf("%s%s\n", g_global.tabs.c_str(), str.c_str());
						printf("%sLoad cancelled for %s.CHC\n", g_global.tabs.c_str(), files->front().c_str());
					}
					catch (const char* str)
					{
						printf("%s%s\n", g_global.tabs.c_str(), str);
						printf("%sLoad cancelled for %s.CHC\n", g_global.tabs.c_str(), files->front().c_str());
					}
					files->pop_front();
				}
				--g_global;
				g_global.quit = true;
			}
		}
	} while (!g_global.quit);
	g_global.quit = false;
	printf("%sAll provided CHC files have been evaluated.\n", g_global.tabs.c_str());
	return false;
}

/*
Main menu prompt used for choosing what action to perform on the loaded CHC file.
Returns false if this is used from the multimenu and if the user wants to proceed to the next CHC.
*/
bool CHC_Main::menu(size_t fileCount)
{
	while(!g_global.quit)
	{
		GlobalFunctions::banner(" " + song.m_shortname + ".CHC - Mode Selection ");
		string choices = "swpd";
		printf("%sS - Save\n", g_global.tabs.c_str());
		printf("%sW - Write %s.txt\n", g_global.tabs.c_str(), song.m_shortname.c_str());
		printf("%sP - Swap players\n", g_global.tabs.c_str());
		if (song.m_unorganized || !song.m_optimized)
		{
			printf("%sF - Fix & organize %s.CHC where necessary (may not get all present bugs)\n", g_global.tabs.c_str(), song.m_shortname.c_str());
			printf("%sB - Both 'P' & 'F'\n", g_global.tabs.c_str());
			choices += "fb";
		}
		printf("%sD - Detailed modification\n", g_global.tabs.c_str());
		if (song.m_imc[0])
		{
			if (GlobalFunctions::LoadLib(g_dlls[s_DLL_INDEX].m_libraries[1]))
			{
				printf("%sT - Convert into a PCSX2 TAS (only works with songs available in the PS2 version)\n", g_global.tabs.c_str());
				choices += 't';
			}
		}
		if (GlobalFunctions::LoadLib(g_dlls[s_DLL_INDEX].m_libraries[2]))
		{
			printf("%sE - Export as \".chart\" for chart editing in Moonscraper\n", g_global.tabs.c_str());
			choices += 'e';
			printf("%sI - Import notes from a Clone/Guitar Hero \".chart\" file\n", g_global.tabs.c_str());
			choices += 'i';
		}
		if (song.m_imc[0])
		{
			printf("%sC - Create a PCSX2 Phrase Bar Color Cheat template\n", g_global.tabs.c_str());
			choices += 'c';
		}
		if (fileCount > 1)
		{
			printf("%sN - Next CHC file\n", g_global.tabs.c_str());
			choices += 'n';
		}
		printf("%s? - Help info\n", g_global.tabs.c_str());
		if (fileCount > 0)
		{
			for (size_t i = 1; i < 20 && !g_global.quit; i++)
			{
				for (size_t s = 0; s < g_dlls[i].m_extensions.size() && !g_global.quit; s++)
				{
					if (g_dlls[i].m_extensions[s].m_files.size() > 0)
					{
						printf("%sQ - Proceed to the next filetype (%s)\n", g_global.tabs.c_str(), g_dlls[i].m_extensions[s].m_ext.c_str());
						g_global.quit = true;
					}
				}
			}
			if (g_global.quit)
				g_global.quit = false;
			else
				printf("%sQ - Quit Program\n", g_global.tabs.c_str());
		}
		else
			printf("%sQ - Main Menu\n", g_global.tabs.c_str());
		switch (GlobalFunctions::menuChoices(choices))
		{
		case GlobalFunctions::ResultType::Quit:
			if (!song.m_saved)
			{
				printf("%s\n", g_global.tabs.c_str());
				printf("%sRecent changes have not been saved externally to a CHC file. Which action will you take?\n", g_global.tabs.c_str());
				printf("%sS - Save CHC and Exit to Main Menu\n", g_global.tabs.c_str());
				printf("%sQ - Exit without saving\n", g_global.tabs.c_str());
				printf("%sC - Cancel\n", g_global.tabs.c_str());
				switch (GlobalFunctions::menuChoices("sc"))
				{
				case GlobalFunctions::ResultType::Success:
					if (g_global.answer.character == 'c')
						break;
					else
					{
						saveFile(true);
						if (!song.m_saved)
							break;
					}
				case GlobalFunctions::ResultType::Quit:
					g_global.quit = true;
				}
			}
			else
				g_global.quit = true;
			break;
		case GlobalFunctions::ResultType::Help:
		{
			printf("%sW - Write %s.txt\n", g_global.tabs.c_str(), song.m_shortname.c_str());
			printf("%sWrites out the data present in the CHC file to a readable txt file. This would include all SSQ & cue data, audio values,\n", g_global.tabs.c_str());
			printf("%ssection & subsection data - including values for every separate note -, and damage & energy factors. Creates two files -\n", g_global.tabs.c_str());
			printf("%swith one being a simplistic form with only the bare minimum necessary information.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());

			printf("%sP - Swap players\n", g_global.tabs.c_str());
			printf("%sGoes through and swaps the charts between the two main payers. For PS2 compatible CHCs, this will swap all charts for P1/P3 with\n", g_global.tabs.c_str());
			printf("%sP2/P4 respectively. For duet CHCs, this will only swap the charts of P1 with P3 as P2 is not important enough to warrant a swap & P4 is empty.\n", g_global.tabs.c_str());
			printf("%sNote: only use this on either 1. A base CHC that was not swapped using an alternate method, or 2. a CHC pre-swapped with this program.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());

			if (song.m_unorganized > 0 || !song.m_optimized)
			{
				printf("%sF - Fix & organize %s.CHC where necessary (may not get all present bugs)\n", g_global.tabs.c_str(), song.m_shortname.c_str());
				printf("%sMany CHC files provided from the base game are very prone to a multitude of glitches or contain sizable mistakes.\n", g_global.tabs.c_str());
				printf("%sFor example: Stage 8 having two guard marks being only 2 samples away from eachother, or Stage 3 having a broken attack phrase note.\n", g_global.tabs.c_str());
				printf("%sThis function aims to fix many of these issues while also applying an chart organization to evenly balance out chart counts amongst players.\n", g_global.tabs.c_str());
				printf("%sNote: if you come across an issue with it or have the program crash, send me the CHC file used in the Gitaroo Pals discord: https://discord.gg/ed6P8Jt \n", g_global.tabs.c_str());
				printf("%s(sonicfind#6404)\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());

				printf("%sB - Both 'P' & 'F'\n", g_global.tabs.c_str());
				printf("%sDoes both option 'S' & option 'F' in a single function.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
			}

			printf("%sD - Detailed modification\n", g_global.tabs.c_str());
			printf("%sTakes you to a menu where you can manually edit some of the smaller details of the CHC, including but not limited to: IMC and audio settings,\n", g_global.tabs.c_str());
			printf("%sgameplay speed/speed of all notes, .SSQ data, and player damage/energy multiplier factors.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());

			if (GlobalFunctions::LoadLib(g_dlls[s_DLL_INDEX].m_libraries[1]))
			{
				printf("%sT - Convert into a PCSX2 TAS (only works with songs compatible with the PS2 version)\n", g_global.tabs.c_str());
				printf("%sGenerates a TAS file that can be played in PCSX2 from the CHC's note data. Very useful for testing charts that have been edited.\n", g_global.tabs.c_str());
				printf("%sNote 1: obviously, this only works on CHCs compatible with the PS2 version of the game.\n", g_global.tabs.c_str());
				printf("%sNote 2: currently requires using my own custom build (for multitap support and optimized file sizes)\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
			}

			if (GlobalFunctions::LoadLib(g_dlls[s_DLL_INDEX].m_libraries[2]))
			{
				printf("%sE - Export as \".chart\" for chart editing in Moonscraper\n", g_global.tabs.c_str());
				printf("%sConverts the CHC into .chart for use in the Guitar Hero/Clone Hero chart editor, Moonscraper. You can download Moonscraper from the #test-builds channel.\n", g_global.tabs.c_str());
				printf("%sin their discord: https://discord.gg/5BdQnsN \n", g_global.tabs.c_str());
				printf("%sNote: this can only be used with CHCs that are fixed & organized.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());

				printf("%sI - Import notes from a Clone/Guitar Hero \".chart\" file\n", g_global.tabs.c_str());
				printf("%sInserts & replaces section and note data from correctly formatted .chart files into each given CHC file. This is will not generate any new files or \n", g_global.tabs.c_str());
				printf("%soverwrite the current file automatically. That decision will be made during the process with each separate CHC.\n", g_global.tabs.c_str());
				printf("%sRefer to the included Chart_import_README.txt for details on formatting a .chart file for this use\n", g_global.tabs.c_str());
				printf("%sNote: this can only be used with CHCs that are fixed & organized.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
			}

			printf("%sC - Create a PCSX2 Phrase Bar Color Cheat template\n", g_global.tabs.c_str());
			printf("%sGenerates a outline txt file for use in conjunction with BoringPerson#2570's phrase bar coloring Pyhton script. It will leave all chosen\n", g_global.tabs.c_str());
			printf("%ssections blank for you to fill in and all others filled with a base color. Feel free to change the colors in the file after it's generated.\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
		}
		case GlobalFunctions::ResultType::Success:
			if (g_global.answer.character == 'n')
				return false;
			else
			{
				++g_global;
				switch (g_global.answer.character)
				{
				case 's':
					saveFile();
					break;
				case 'w':
					writeTxt();
					break;
				case 'p':
					applyChanges(false, true);
					break;
				case 'f':
					applyChanges(true, false);
					break;
				case 'b':
					applyChanges(true, true);
					break;
				case 'd':
					edit();
					break;
				case 't':
					makeTAS();
					break;
				case 'e':
					exportChart();
					break;
				case 'i':
					importChart();
					break;
				case 'c':
					createColorTemplate();
				}
				--g_global;
			}
		}
	}
	return true;
}

void CHC_Main::saveFile(bool onExit)
{
	GlobalFunctions::banner(" " + song.m_shortname + ".CHC Save Prompt ");
	string ext = "_T";
	string filename = song.m_filename.substr(0, song.m_filename.length() - 4);
	do
	{
		string choices = "a";
		if (song.m_saved != 2)
		{
			printf("%sS - Save & Overwrite %s.CHC\n", g_global.tabs.c_str(), song.m_shortname.c_str());
			choices += 's';
		}
		printf("%sA - Save as \"%s_T.CHC\"\n", g_global.tabs.c_str(), song.m_shortname.c_str());
		printf("%sQ - Back Out\n", g_global.tabs.c_str());
		switch (GlobalFunctions::menuChoices(choices))
		{
		case GlobalFunctions::ResultType::Quit:
			return;
		case GlobalFunctions::ResultType::Success:
			switch (g_global.answer.character)
			{
			case 's':
				printf("%sSaving %s.CHC\n", g_global.tabs.c_str(), song.m_shortname.c_str());
				song.create(song.m_filename);
				song.m_saved = 2;
				g_global.quit = true;
				break;
			case 'a':
				do
				{
					switch (GlobalFunctions::fileOverwriteCheck(filename + ext + ".CHC"))
					{
					case GlobalFunctions::ResultType::Quit:
						return;
					case GlobalFunctions::ResultType::Success:
						if (g_global.answer.character == 'n')
						{
							printf("%s\n", g_global.tabs.c_str());
							ext += "_T";
							break;
						}
						else
						{
							song.create(filename + ext + ".CHC");
							if (!onExit)
							{
								do
								{
									printf("%sSwap loaded file to %s.CHC? [Y/N]\n", g_global.tabs.c_str(), (filename + ext).c_str());
									switch (GlobalFunctions::menuChoices("yn"))
									{
									case GlobalFunctions::ResultType::Success:
										if (g_global.answer.character == 'y')
										{
											song.m_filename = filename + ext + ".CHC";
											song.m_shortname += ext;
											song.m_saved = 2;
										}
									case GlobalFunctions::ResultType::Quit:
										g_global.quit = true;
									}
								} while (!g_global.quit);
							}
						}
					}
				} while (!g_global.quit);
			}
		}
	} while (!g_global.quit);
	g_global.quit = false;
}

/*
Writes out the data present in the CHC file to a readable txt file. This would include all SSQ & cue data, audio values,
section & subsection data - including values for every separate note -, and damage & energy factors. Creates two files -
with one being a simplistic form with only the bare minimum necessary information.
*/
void CHC_Main::writeTxt()
{
	GlobalFunctions::banner(" Writing " + song.m_shortname + "_CHC.txt ");
	FILE* outTXT, * outSimpleTXT;
	fopen_s(&outTXT, (song.m_filename.substr(0, song.m_filename.length() - 4) + "_CHC.txt").c_str(), "w");
	fopen_s(&outSimpleTXT, (song.m_filename.substr(0, song.m_filename.length() - 4) + "_CHC_SIMPLIFIED.txt").c_str(), "w");
	try
	{
		fprintf(outTXT, "Header: %s", song.m_header);
		GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "IMC: ");
		song.m_imc[0] ? GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, song.m_imc)		//IMC
			: GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "Unused in PSP version");
		GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\n\t   SSQ Events:\n");
		for (size_t index = 0; index < 4; index++)
		{
			switch (index)
			{
			case 0: GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t    Win A:\n"); break;
			case 1: GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t    Win B:\n"); break;
			case 2: GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t Lose Pre:\n"); break;
			case 3: GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\tLose Loop:\n");
			}
			GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\tFirst Frame: %g\n", song.m_events[index].first);
			GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t Last Frame: %g\n", song.m_events[index].last);
		}
		GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "       Audio Channels:\n");
		if (song.m_imc[0])
		{
			for (size_t index = 0; index < 8; index++)
			{
				GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t Channel %zu:\n", index + 1);
				GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t       Volume: %lu (%g%%)\n", song.m_audio[index].volume, song.m_audio[index].volume * 100.0 / 32767);
				switch (song.m_audio[index].pan)
				{
				case 0: GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t  Pan: Left (0)\n"); break;
				case 16383: GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t  Pan: Center (16383)\n"); break;
				case 32767: GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t  Pan: Right (32767)\n"); break;
				default: GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t  Pan: %g%% Left | %g%% Right (%lu)\n", 100 - (song.m_audio[index].pan * 100.0 / 32767),
					song.m_audio[index].pan * 100.0 / 32767, song.m_audio[index].pan);
				}
			}
		}
		else
		{
			GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t Channel 1: Unused in PSP version\n");
			GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t Channel 2: Unused in PSP version\n");
			for (size_t index = 2; index < 8; index++)
			{
				GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t Channel %zu:\n", index + 1);
				GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t       Volume: %lu (%g%%)\n", song.m_audio[index].volume, song.m_audio[index].volume * 100.0 / 32767);
				GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t  Pan: Unused in PSP version\n");
			}
		}
		GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t       Speed: %g\n", song.m_speed);
		fprintf(outTXT, "\t   # of Cues: %zu\n", song.m_sections.size());
		fputs("\t    SSQ Cues:\n", outTXT);
		for (size_t cueIndex = 0; cueIndex < song.m_sections.size(); cueIndex++)	//Cues
		{
			SongSection& section = song.m_sections[cueIndex];
			fprintf(outTXT, "\t       Cue %s:\n", section.m_name);
			fprintf(outTXT, "\t\t\tAudio Used: %s\n", section.m_audio);
			fprintf(outTXT, "\t\t\t     Index: %lu\n", section.m_index);
			fprintf(outTXT, "\t\t       First frame: %5g\n", section.m_frames.first);
			fprintf(outTXT, "\t\t\tLast frame: %5g\n", section.m_frames.last);
		}
		fflush(outTXT);
		fflush(outSimpleTXT);
		GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "       # of Sections: %zu\n", song.m_sections.size());
		GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "       Song Sections:\n");
		for (size_t sectIndex = 0; sectIndex < song.m_sections.size(); sectIndex++) //SongSections
		{
			SongSection& section = song.m_sections[sectIndex];
			GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t       Section %s:\n", section.m_name);
			section.m_organized & 1 ? GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t       Organized: TRUE\n")
				: GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t       Organized: FALSE\n");
			if (section.m_swapped < 4)
			{
				if (section.m_swapped == 0)
					GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t Swapped Players: FALSE\n");
				else if (section.m_swapped == 1)
					GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t Swapped Players: TRUE (P2/P1/P4/P3)\n");
				else if (section.m_swapped == 2)
				{
					if (song.m_imc[0])
						GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t Swapped Players: TRUE (P3/P4/P1/P2)\n");
					else
						GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t Swapped Players: TRUE (P3/P2/P1/P4) [DUET]\n");
				}
				else
					GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t Swapped Players: TRUE (P4/P3/P2/P1)\n");
			}
			else
			{
				if (section.m_swapped == 4)
					GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t Swapped Players: FALSE (P1/P2D/P3/P4D) [Duet->PS2 Conversion]\n");
				else if (section.m_swapped == 5)
					GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t Swapped Players: TRUE (P2D/P1/P4D/P3) [Duet->PS2 Conversion]\n");
				else if (section.m_swapped == 6)
					GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t Swapped Players: TRUE (P3/P4D/P1/P2D) [Duet->PS2 Conversion]\n");
				else
					GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t Swapped Players: TRUE (P4D/P3/P2D/P1) [Duet->PS2 Conversion]\n");
			}
			fprintf(outTXT, "\t\t       Size (32bit) - 44: %lu\n", section.m_size);
			fprintf(outTXT, "\t\t\t\t    Junk: 0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(section.m_junk)));
			fprintf(outTXT, "\t\t\t\t\t  0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(section.m_junk + 4)));
			fprintf(outTXT, "\t\t\t\t\t  0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(section.m_junk + 8)));
			fprintf(outTXT, "\t\t\t\t\t  0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(section.m_junk + 12)));
			switch (section.m_battlePhase)
			{
			case SongSection::Phase::INTRO:
				GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t   Phase: INTRO\n");
				break;
			case SongSection::Phase::CHARGE:
				GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t   Phase: CHARGE\n");
				break;
			case SongSection::Phase::BATTLE:
				GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t   Phase: BATTLE\n");
				break;
			case SongSection::Phase::FINAL_AG:
				GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t   Phase: FINAL_AG\n");
				break;
			case SongSection::Phase::HARMONY:
				GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t   Phase: HARMONY\n");
				break;
			case SongSection::Phase::END:
				GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t   Phase: END\n");
				break;
			default:
				GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t   Phase: FINAL_I\n");
			}
			GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t   Tempo: %g\n", section.m_tempo);
			fprintf(outTXT, "\t\t\t    Samples/Beat: %Lg\n", s_SAMPLES_PER_MIN / section.m_tempo);
			GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\tDuration: %lu samples\n", section.m_duration);
			GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t # of Conditions: %lu\n", section.m_conditions.size());
			for (size_t condIndex = 0; condIndex < section.m_conditions.size(); condIndex++)
			{
				SongSection::Condition& cond = section.m_conditions[condIndex];
				GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t   Condition %zu:\n", condIndex + 1);
				switch (cond.m_type)
				{
				case 0:
					fputs("\t\t\t\t\t\tType: Unconditional\n", outTXT);
					GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t      Result: Always true.\n");
					break;
				case 1:
					fputs("\t\t\t\t\t\tType: Left Side Energy\n", outTXT);
					fprintf(outTXT, "\t\t\t\t\t    Argument: %g\n", cond.m_argument);
					GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t      Result: True if Left Side's energy is less than %g%%. False otherwise.\n", cond.m_argument * 100.0);
					break;
				case 2:
					fputs("\t\t\t\t\t\tType: Right Side Energy\n", outTXT);
					fprintf(outTXT, "\t\t\t\t\t    Argument: %g\n", cond.m_argument);
					GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t      Result: True if Right Side's energy is less than %g%%. False otherwise.\n", cond.m_argument * 100.0);
					break;
				case 3:
					fputs("\t\t\t\t\t\tType: Random\n", outTXT);
					fprintf(outTXT, "\t\t\t\t\t    Argument: %g\n", cond.m_argument);
					GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t      Result: Generates a random number between 0.0 and 1.0, True if the random number is less than %g%%. False otherwise.\n", cond.m_argument);
					break;
				case 4:
					fputs("\t\t\t\t\t\tType: Left Side Unavailable\n", outTXT);
					GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t      Result: False if this match has a Player 3 participating (only possible in Versus mode). True otherwise.\n");
					break;
				case 5:
					fputs("\t\t\t\t\t\tType: Right Side Unavailable\n", outTXT);
					GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t      Result: False if this match has a Player 4 participating (only possible in Versus mode). True otherwise.\n");
					break;
				case 6:
					fputs("\t\t\t\t\t\tType: Left Side < Right Side\n", outTXT);
					GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t      Result: True if Left Side's energy is less than Right Side's energy. False otherwise.\n");
					break;
				}
				if (cond.m_trueEffect < 0)
				{
					if (cond.m_trueEffect == -1) GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t True Effect: Move to the next condition.\n");
					else GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t True Effect: Move over %li conditions.\n", abs(cond.m_trueEffect));
				}
				else
				{
					if ((size_t)cond.m_trueEffect < song.m_sections.size()) GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t True Effect: Move to Section %s.\n", song.m_sections[cond.m_trueEffect].m_name);
					else GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t True Effect: End song.\n");
				}
				if (cond.m_type != 0)
				{
					if (cond.m_falseEffect < 0)
					{
						if (cond.m_falseEffect == -1) GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\tFalse Effect: Move to the next condition\n");
						else GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\tFalse Effect: Move over %li conditions.\n", abs(cond.m_falseEffect));
					}
					else
					{
						if ((size_t)cond.m_falseEffect < song.m_sections.size()) GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\tFalse Effect: Move to Section %s.\n", song.m_sections[cond.m_falseEffect].m_name);
						else GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\tFalse Effect: End song.\n");
					}
				}
			}
			GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t    # of Players: %lu\n", section.m_numPlayers);
			GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t       Charts per Player: %lu\n", section.m_numCharts);
			for (size_t playerIndex = 0; playerIndex < section.m_numPlayers; playerIndex++)
			{
				for (size_t chartIndex = 0; chartIndex < section.m_numCharts; chartIndex++)
				{
					Chart& chart = section.m_charts[playerIndex * section.m_numCharts + chartIndex];
					GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t Player %zu - Chart %02zu\n", playerIndex + 1, chartIndex + 1);
					fprintf(outTXT, "\t\t\t\t\t  Size (32bit): %lu\n", chart.getSize());
					fprintf(outTXT, "\t\t\t\t\t\t  Junk: 0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(chart.getJunk())));
					fprintf(outTXT, "\t\t\t\t\t\t\t0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(chart.getJunk() + 4)));
					fprintf(outTXT, "\t\t\t\t\t\t\t0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(chart.getJunk() + 8)));
					fprintf(outTXT, "\t\t\t\t\t\t\t0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(chart.getJunk() + 12)));
					GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t   Pivot Point: %lu samples\n", chart.getPivotTime());
					fprintf(outTXT, "\t\t\t\t\t      End Time: %lu samples\n", chart.getEndTime());
					GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t      # of Trace Lines: %zu\n", chart.getNumTracelines());
					for (size_t traceIndex = 0; traceIndex < chart.getNumTracelines(); traceIndex++)
					{
						Traceline& trace = chart.getTraceline(traceIndex);
						GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t\t  Trace Line %03zu:\n", traceIndex + 1);
						GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t\t\t Pivot Alpha: %+li samples\n", trace.getPivotAlpha());
						GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t\t\t  Start Time: %li samples (Relative to SongSection)\n", trace.getPivotAlpha() + chart.getPivotTime());
						fprintf(outTXT, "\t\t\t\t\t\t\t    Duration: %lu samples\n", trace.getDuration());
						fprintf(outTXT, "\t\t\t\t\t\t\t       Angle: %s*PI radians | %li degrees\n", GlobalFunctions::angleToFraction(trace.getAngle()).c_str(), GlobalFunctions::radiansToDegrees(trace.getAngle()));
						trace.getCurve() ? fputs("\t\t\t\t\t\t\t       Curve: True\n", outTXT)
							: fputs("\t\t\t\t\t\t\t       Curve: False\n", outTXT);
						fprintf(outTXT, "\t\t\t\t\t\t\t    End Time: %li samples (Relative to SongSection)\n", trace.getEndAlpha() + chart.getPivotTime());
						if (traceIndex + 1 == chart.getNumTracelines())
							fprintf(outSimpleTXT, "\t\t\t\t\t\t\t    End Time: %li samples (Relative to SongSection)\n", trace.getEndAlpha() + chart.getPivotTime());
					}
					GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t # of Phrase Fragments: %zu\n", chart.getNumPhrases());
					for (size_t phraseIndex = 0, traceIndex = 0, note = 0, piece = 0; phraseIndex < chart.getNumPhrases(); phraseIndex++)
					{
						Phrase& phrase = chart.getPhrase(phraseIndex);
						for (; traceIndex < chart.getNumTracelines(); traceIndex++)
							if (chart.getTraceline(traceIndex).contains(phrase.getPivotAlpha()))
								break;
						GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t     Phrase Fragment %03zu:\n", phraseIndex + 1);
						GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t     [Note #%03zu", note + 1);
						if (piece)
							GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, " - Piece ##%02zu", piece + 1);
						fprintf(outTXT, "| Trace Line #%03zu]:\n", traceIndex + 1);
						fputs("]:\n", outSimpleTXT);
						GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t\t\t Pivot Alpha: %+li samples\n", phrase.getPivotAlpha());
						GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t\t\t  Start Time: %li samples (Relative to SongSection)\n", phrase.getPivotAlpha() + chart.getPivotTime());
						fprintf(outTXT, "\t\t\t\t\t\t\t    Duration: %lu samples\n", phrase.getDuration());

						phrase.getStart() ? fputs("\t\t\t\t\t\t\t       Start: True\n", outTXT)
							: fputs("\t\t\t\t\t\t\t       Start: False\n", outTXT);
						if (phrase.getEnd())
						{
							fputs("\t\t\t\t\t\t\t         End: True\n", outTXT);
							note++;
							piece = 0;
						}
						else
						{
							fputs("\t\t\t\t\t\t\t         End: False\n", outTXT);
							piece++;
						}
						fprintf(outTXT, "\t\t\t\t\t\t\t   Animation: %lu\n", phrase.getAnimation());
						if (phrase.getEnd())
							GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t\t\t    End Time: %li samples (Relative to SongSection)\n", phrase.getEndAlpha() + chart.getPivotTime());
						else
							fprintf(outTXT, "\t\t\t\t\t\t\t    End Time: %li samples (Relative to SongSection)\n", phrase.getEndAlpha() + chart.getPivotTime());
						if (phrase.getColor())
						{
							GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t\t\tColor Export: ");
							if (phrase.getColor() & 1)
								GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "G");
							if (phrase.getColor() & 2)
								GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "R");
							if (phrase.getColor() & 4)
								GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "Y");
							if (phrase.getColor() & 8)
								GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "B");
							if (phrase.getColor() & 16)
								GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "O");
							if (phrase.getColor() & 32)
								GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "P");
							if (phrase.getColor() & 64)
								GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "(Tap)");
							GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\n");
						}
						else
						{
							fprintf(outTXT, "\t\t\t\t\t\t\t\tJunk: 0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(phrase.getJunk())));
							fprintf(outTXT, "\t\t\t\t\t\t\t\t      0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(phrase.getJunk() + 4)));
							fprintf(outTXT, "\t\t\t\t\t\t\t\t      0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(phrase.getJunk() + 8)));
						}
					}
					GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t      # of Guard Marks: %zu\n", chart.getNumGuards());
					for (size_t guardIndex = 0; guardIndex < chart.getNumGuards(); guardIndex++)
					{
						Guard& guard = chart.getGuard(guardIndex);
						GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t\t  Guard Mark %03zu:\n", guardIndex + 1);
						GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t\t\t Pivot Alpha: %+li samples\n", guard.getPivotAlpha());
						GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t\t\t  Start Time: %li samples (Relative to SongSection)\n", guard.getPivotAlpha() + chart.getPivotTime());
						GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t\t\t      Button: ");
						switch (guard.getButton())
						{
						case 0: fputs("Left\n", outTXT); fputs("Square\n", outSimpleTXT); break;
						case 1: fputs("Down\n", outTXT); fputs("X/Cross\n", outSimpleTXT); break;
						case 2: fputs("Right\n", outTXT); fputs("Circle\n", outSimpleTXT); break;
						case 3: fputs("Up\n", outTXT); fputs("Triangle\n", outSimpleTXT);
						}
					}
				}
			}
			fflush(outTXT);
			fflush(outSimpleTXT);
		}
		GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "Damage/Energy Factors:\n");
		for (size_t player = 0; player < 4; player++)
		{
			GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "       Player %zu ||", player + 1);
			GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, " Starting Energy || Initial-Press Energy || Initial-Press Damage || Guard Energy Gain ||");
			GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, " Attack Miss Damage || Guard Miss Damage || Release Max Energy || Release Max Damage ||\n");
			GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "       %s||\n", string(184, '='));
			for (size_t section = 0; section < 5; section++)
			{
				switch (section)
				{
				case 0: GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t  Intro ||"); break;
				case 1: GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t Charge ||"); break;
				case 2: GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t Battle ||"); break;
				case 3: GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\tHarmony ||"); break;
				case 4: GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t    End ||");
				}
				GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "%015g%% ||%020g%% ||%020g%% ||%017g%% ||%018g%% ||%017g%% ||%018g%% ||%018g%% ||\n",
					song.m_energyDamageFactors[player][section].initialEnergy * 100.0,
					song.m_energyDamageFactors[player][section].chargeInitial * 100.0,
					song.m_energyDamageFactors[player][section].attackInitial * 100.0,
					song.m_energyDamageFactors[player][section].guardEnergy * 100.0,
					song.m_energyDamageFactors[player][section].attackMiss * 100.0,
					song.m_energyDamageFactors[player][section].guardMiss * 100.0,
					song.m_energyDamageFactors[player][section].chargeRelease * 100.0,
					song.m_energyDamageFactors[player][section].attackRelease * 100.0);
			}
			GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\n");
		}
	}
	catch (string str)
	{
		printf("%s%s\n", g_global.tabs.c_str(), str.c_str());
	}
	catch (const char* str)
	{
		printf("%s%s\n", g_global.tabs.c_str(), str);
	}
	fclose(outTXT);
	fclose(outSimpleTXT);
}

/*
Does either of these two things:
--Calls the editor to fix and organize the CHC
--Swaps players in every section of the CHC
Afterwards, it saves & overwrites over the original file.
@param fix - "Apply fixes"
@param swap - "Swap players"
@return Returns whether the process was successful
*/
bool CHC_Main::applyChanges(const bool fix, const bool swap, const bool save)
{
	try
	{
		CHC_Editor editor(song);
		if (fix)
		{
			editor.fixNotes();
			editor.organizeAll();
		}
		if (swap)
		{
			editor.playerSwapAll();
		}
		if (save)
			song.create(song.m_filename);
		return true;
	}
	catch (exception e)
	{
		printf("%sException Caught: %s\n", g_global.tabs.c_str(), e.what());
		return false;
	}
}

/*
Calls the editor for... well, editing. If the file currently pointed at in the editor is overwritten, it will swap the CHC file currently held
in the CHC object with the file from the editor. Otherwise, after the editor is closed, it will reassert the CHC object's file back into editor
as if no changes were made to the original file.
@param multi - Is the multimenu being used?
*/
void CHC_Main::edit(const bool multi)
{
	CHC_Editor editor(song);
	editor.editSong(multi);
}

void CHC_Main::makeTAS()
{
	GlobalFunctions::loadProc(g_dlls[s_DLL_INDEX].m_libraries[1].m_dll, "chcTAS", song);
}

void CHC_Main::exportChart()
{
	GlobalFunctions::banner(" " + song.m_shortname + " - .CHART creation ");
	if (song.m_unorganized != 0)
	{
		do
		{
			printf("%sTo use the CH Chart Export option on %s.CHC, the file must fully fixed & organized.\n", g_global.tabs.c_str(), song.m_shortname.c_str());
			printf("%sFix & organize the file now? [Y/N]\n", g_global.tabs.c_str());
			switch (GlobalFunctions::menuChoices("yn"))
			{
			case GlobalFunctions::ResultType::Success:
				if (g_global.answer.character == 'y')
				{
					applyChanges(true, false);
					GlobalFunctions::banner(" " + song.m_shortname + " - .CHART creation ");
					g_global.quit = true;
					break;
				}
			case GlobalFunctions::ResultType::Quit:
				printf("%sCH Chart Import on %s aborted due to not being fully organized.\n", g_global.tabs.c_str(), song.m_shortname.c_str());
				return;
			}
		} while (!g_global.quit);
		g_global.quit = false;
	}
	GlobalFunctions::loadProc(g_dlls[s_DLL_INDEX].m_libraries[2].m_dll, "exportChart", song);
}

void CHC_Main::importChart()
{
	GlobalFunctions::banner(" " + song.m_shortname + " - .CHART Note Import/Replacement ");
	if (song.m_unorganized != 0)
	{
		do
		{
			printf("%sTo use the CH Chart Import option on %s.CHC, the file must fully fixed & organized.\n", g_global.tabs.c_str(), song.m_shortname.c_str());
			printf("%sFix & organize the file now? [Y/N]\n", g_global.tabs.c_str());
			switch (GlobalFunctions::menuChoices("yn"))
			{
			case GlobalFunctions::ResultType::Success:
				if (g_global.answer.character == 'y')
				{
					applyChanges(true, false);
					GlobalFunctions::banner(" " + song.m_shortname + " - .CHART Note Import/Replacement ");
					g_global.quit = true;
					break;
				}
			case GlobalFunctions::ResultType::Quit:
				printf("%sCH Chart Import on %s aborted due to not being fully organized.\n", g_global.tabs.c_str(), song.m_shortname.c_str());
				return;
			}
		} while (!g_global.quit);
		g_global.quit = false;
	}
	GlobalFunctions::loadProc(g_dlls[s_DLL_INDEX].m_libraries[2].m_dll, "importChart", song);
}

bool CHC_Main::createColorTemplate()
{
	GlobalFunctions::banner(" " + song.m_shortname + ".CHC - Color Sheet Creation ");
	bool multiplayer = toupper(song.m_shortname.back()) == 'M';
	if (!multiplayer)
	{
		do
		{
			printf("%sIs this chart for multiplayer? [Y/N]\n", g_global.tabs.c_str());
			switch (GlobalFunctions::menuChoices("yn"))
			{
			case GlobalFunctions::ResultType::Quit:
				return false;
			case GlobalFunctions::ResultType::Success:
				if (g_global.answer.character == 'y')
					multiplayer = true;
				g_global.quit = true;
			}
		} while (!g_global.quit);
		g_global.quit = false;
	}
	bool generate = false;
	LinkedList::List<size_t> sectionIndexes;
	do
	{
		printf("%sType the number for each section that you wish to outline colors for - w/ spaces inbetween.\n", g_global.tabs.c_str());
		for (size_t sectIndex = 0; sectIndex < song.m_sections.size(); sectIndex++)
			printf("%s%zu - %s\n", g_global.tabs.c_str(), sectIndex, song.m_sections[sectIndex].getName());
		if (sectionIndexes.size())
		{
			printf("%sCurrent LinkedList::List: ", g_global.tabs.c_str());
			for (size_t index = 0; index < sectionIndexes.size(); index++)
				printf("%s ", song.m_sections[sectionIndexes[index]].getName());
			putchar('\n');
		}
		switch (GlobalFunctions::listValueInsert(sectionIndexes, "ac", song.m_sections.size(), false))
		{
		case GlobalFunctions::ResultType::Quit:
			printf("%sColor Sheet creation cancelled.\n", g_global.tabs.c_str());
			return false;
		case GlobalFunctions::ResultType::Help:
			printf("%sHelp: [TBD]\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
			break;
		case GlobalFunctions::ResultType::SpecialCase:
			if (sectionIndexes.size())
			{
				g_global.multi = false;
				g_global.quit = true;
				break;
			}
		case GlobalFunctions::ResultType::Success:
			if (!sectionIndexes.size())
			{
				do
				{
					printf("%sNo sections have been selected.\n", g_global.tabs.c_str());
					printf("%sA - Add section values\n", g_global.tabs.c_str());
					printf("%sC - Create template file with default colors\n", g_global.tabs.c_str());
					printf("%sQ - Quit Color Sheet creation\n", g_global.tabs.c_str());
					switch (GlobalFunctions::menuChoices("ac"))
					{
					case GlobalFunctions::ResultType::Quit:
						printf("%sColor Sheet creation cancelled.\n", g_global.tabs.c_str());
						return false;
					case GlobalFunctions::ResultType::Success:
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
		printf("%sIf found, use any colors that are pre-saved in a phrase bar? [Y/N]\n", g_global.tabs.c_str());
		switch (GlobalFunctions::menuChoices("yn"))
		{
		case GlobalFunctions::ResultType::Quit:
			return false;
		case GlobalFunctions::ResultType::Success:
			if (g_global.answer.character == 'y')
				writeColors = true;
			g_global.quit = true;
		}
	} while (!g_global.quit);
	g_global.quit = false;
	string filename = song.m_filename.substr(0, song.m_filename.length() - 4) + "_COLORDEF";
	string filename2 = song.m_filename.substr(0, song.m_filename.length() - 4) + "_COLORDEF_FRAGS";
	FILE *outSheet = nullptr, *outSheet2 = nullptr;
	do
	{
		switch (GlobalFunctions::fileOverwriteCheck(filename + ".txt"))
		{
		case GlobalFunctions::ResultType::No:
				printf("%s\n", g_global.tabs.c_str());
				filename += "_T";
				break;
		case GlobalFunctions::ResultType::Yes:
			fopen_s(&outSheet, (filename + ".txt").c_str(), "w");
			__fallthrough;
		case GlobalFunctions::ResultType::Quit:
			g_global.quit = true;
		}
	} while (!g_global.quit);
	printf("%s\n", g_global.tabs.c_str());
	g_global.quit = false;
	do
	{
		switch (GlobalFunctions::fileOverwriteCheck(filename2 + ".txt"))
		{
		case GlobalFunctions::ResultType::No:
			printf("%s\n", g_global.tabs.c_str());
			filename2 += "_T";
			break;
		case GlobalFunctions::ResultType::Yes:
			fopen_s(&outSheet2, (filename2 + ".txt").c_str(), "w");
			__fallthrough;
		case GlobalFunctions::ResultType::Quit:
			g_global.quit = true;
		}
	} while (!g_global.quit);
	g_global.quit = false;
	if (outSheet != nullptr || outSheet2 != nullptr)
	{
		fputs("[phrasemode fragments]\n", outSheet2);
		GlobalFunctions::dualvfprintf_s(outSheet, outSheet2, "[attack point palette]\nG: 00ff00\nR: ff0000\nY: ffff00\nB: 0000ff\nO: ff7f00\nP: ff00ff\nN: f89b44\ng: ffffff\nr: ffffff\ny: ffffff\nb: ffffff\no: ffffff\np: ffffff\n\n");
		GlobalFunctions::dualvfprintf_s(outSheet, outSheet2, "[phrase bar palette]\nG: 40ff40\nR: ff4040\nY: ffff40\nB: 4040c8\nO: ff9f40\nP: ff40ff\nN: f07b7b\ng: 40ff40\nr: ff4040\ny: ffff40\nb: 4040c8\no: ff9f40\np: ff40ff\n\n");
		unsigned long chartCount = 0;
		const size_t size = song.m_sections.size();
		bool* inputs = new bool[size]();
		for (size_t sect = 0; sect < sectionIndexes.size(); sect++)
			inputs[sectionIndexes[sect]] = true;
		string colors = "GRYBOPgrybop";
		for (unsigned long sectIndex = 0; sectIndex < size; sectIndex++)
		{
			SongSection& section = song.m_sections[sectIndex];
			for (unsigned playerIndex = 0; playerIndex < section.m_numPlayers; playerIndex++)
			{
				for (unsigned chartIndex = 0; chartIndex < section.m_numCharts; chartIndex++)
				{
					if (!(playerIndex & 1) || multiplayer)
					{
						Chart& chart = section.m_charts[(unsigned long long)playerIndex * section.m_numCharts + chartIndex];
						if (chart.getNumPhrases())
						{
							GlobalFunctions::dualvfprintf_s(outSheet, outSheet2, "#SongSection %lu [%s], P%lu CHCH %lu\n", sectIndex, section.getName(), playerIndex + 1, chartIndex);
							GlobalFunctions::dualvfprintf_s(outSheet, outSheet2, "[drawn chart %lu]\n", chartCount);
							if (inputs[sectIndex] )
							{
								for (unsigned long phrIndex = 0; phrIndex < chart.getNumPhrases(); phrIndex++)
								{
									Phrase& phr = chart.getPhrase(phrIndex);
									if (writeColors && phr.getColor())
									{
										size_t colIndex = 0;
										while (colIndex < 5)
										{
											if (phr.getColor() & (1 << colIndex))
												break;
											else
												colIndex++;
										}
										if (phr.getColor() & 64)
											colIndex += 6;
										if (!phrIndex)
										{
											fprintf(outSheet, "%c", colors[colIndex]);
											fprintf(outSheet2, "!%c", colors[colIndex]);
										}
										else if (phr.getStart()) //Start
										{
											fprintf(outSheet, " %c", colors[colIndex]);
											fprintf(outSheet2, " !%c", colors[colIndex]);
										}
										else
											fprintf(outSheet2, "-%c", colors[colIndex]);
									}
									else
									{
										if (!phrIndex)
										{
											fputc('_', outSheet);
											fputs("!_", outSheet2);
										}
										else if (phr.getStart()) //Start
										{
											fputs(" _", outSheet);
											fputs(" !_", outSheet2);
										}
										else
											fputs("-_", outSheet2);
									}
								}
							}
							else
							{
								for (unsigned long phrIndex = 0; phrIndex < chart.getNumPhrases(); phrIndex++)
								{
									Phrase& phr = chart.getPhrase(phrIndex);
									if (writeColors && phr.getColor())
									{
										size_t colIndex = 0;
										while (colIndex < 5)
										{
											if (phr.getColor() & (1 << colIndex))
												break;
											else
												colIndex++;
										}
										if (phr.getColor() & 64)
											colIndex += 6;
										if (!phrIndex)
										{
											fprintf(outSheet, "%c", colors[colIndex]);
											fprintf(outSheet2, "!%c", colors[colIndex]);
										}
										else if (phr.getStart()) //Start
										{
											fprintf(outSheet, " %c", colors[colIndex]);
											fprintf(outSheet2, " !%c", colors[colIndex]);
										}
										else
											fprintf(outSheet2, "-%c", colors[colIndex]);
									}
									else
									{
										if (!phrIndex)
										{
											fputc('N', outSheet);
											fputs("!N", outSheet2);
										}
										else if (phr.getStart()) //Start
										{
											fputs(" N", outSheet);
											fputs(" !N", outSheet2);
										}
										else
											fputs("-N", outSheet2);
									}
								}
							}
							fprintf(outSheet, "\n\n");
							fprintf(outSheet2, "\n\n");
						}
						chartCount++;
					}
				}
			}
			printf("%sColored %s", g_global.tabs.c_str(), section.getName());
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
		printf("%sColor Sheet creation cancelled.\n", g_global.tabs.c_str());
		return false;
	}
}
