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
		printf("%s%s\n", global.tabs.c_str(), str.c_str());
		printf("%sLoad cancelled for %s.CHC\n", global.tabs.c_str(), filename.c_str());
	}
	catch (const char* str)
	{
		printf("%s%s\n", global.tabs.c_str(), str);
		printf("%sLoad cancelled for %s.CHC\n", global.tabs.c_str(), filename.c_str());
	}
	adjustTabs(0);
	return val;
}

/*
Cycles through every file in the provided list and performs the function chosen in the nested menu.
Returns true if the user wishes to end the program, false otherwise.
*/
bool loadMultiCHC(List<string>* files)
{
	do
	{
		size_t result = 0;
		string choices = "ewpfbdck";
		if (files->size() > 1)
		{
			banner(" CHC Mode Selection ");
			printf("%sE - Evaluate each CHC individually\n", global.tabs.c_str());
			printf("%sW - Write all CHCs included to readable .txts\n", global.tabs.c_str());
			printf("%sP - Swap players in all CHCs\n", global.tabs.c_str());
			printf("%sF - Fix & organize all CHCs where necessary (may not fix all bugs)\n", global.tabs.c_str());
			printf("%sB - Both 'S' & 'F'\n", global.tabs.c_str());
			printf("%sD - Perform detailed modifications on each CHC\n", global.tabs.c_str());
			if (LoadLib(dlls[DLL_INDEX].libraries[1]))
			{
				printf("%sT - Convert each CHC into a PCSX2 TAS (only works with CHCs compatible with the PS2 version)\n", global.tabs.c_str());
				choices += 't';
			}
			if (LoadLib(dlls[DLL_INDEX].libraries[2]))
			{
				printf("%sG - Export each CHC as a \".chart\" file for Clone/Guitar Hero\n", global.tabs.c_str());
				choices += 'g';
				printf("%sI - Import notes from Clone/Guitar Hero \".chart\"s into the CHC files\n", global.tabs.c_str());
				choices += 'i';
			}
			printf("%sC - Create a PCSX2 Phrase Bar Color Cheat template for each CHC\n", global.tabs.c_str());
			for (size_t i = 1; i < 20 && !global.quit; i++)
			{
				for (size_t s = 0; s < dlls[i].extensions.size() && !global.quit; s++)
				{
					if (dlls[i].extensions[s].files.size() > 0)
					{
						printf("%sN - Proceed to the next filetype (%s)\n", global.tabs.c_str(), dlls[i].extensions[s].ext.c_str());
						choices += 'n';
						global.quit = true;
					}
				}
			}
			printf("%sQ - Quit Program\n", global.tabs.c_str());
			result = menuChoices(choices);
		}
		switch (result)
		{
		case 'q':
			return true;
		case '?':
			printf("%sW - Write all CHC's included to readable .txts\n", global.tabs.c_str());
			printf("%sWrites out the data present in each CHC file to a readable txt file. This would include all SSQ & cue data, audio values,\n", global.tabs.c_str());
			printf("%ssection & subsection data - including values for every separate note -, and damage & energy factors. Creates two files -\n", global.tabs.c_str());
			printf("%swith one being a simplistic form with only the bare minimum necessary information.\n%s\n", global.tabs.c_str(), global.tabs.c_str());

			printf("%sP - Swap players in all CHCs\n", global.tabs.c_str());
			printf("%sGoes through and swaps the charts between the two main payers. For PS2 compatible CHCs, this will swap all charts for P1/P3 with\n", global.tabs.c_str());
			printf("%sP2/P4 respectively. For duet CHCs, this will only swap the charts of P1 with P3 as P2 is not important enough to warrant a swap & P4 is empty.\n", global.tabs.c_str());
			printf("%sNote: only use this on either 1. A base CHC that was not swapped using an alternate method, or 2. a CHC pre-swapped with this program.\n%s\n", global.tabs.c_str(), global.tabs.c_str());

			printf("%sF - Fix & organize all CHCs where necessary (may not fix all bugs)\n", global.tabs.c_str());
			printf("%sMany CHC files provided from the base game are very prone to a multitude of glitches or contain sizable mistakes.\n", global.tabs.c_str());
			printf("%sFor example: Stage 8 having two guard marks being only 2 samples away from eachother, or Stage 3 having a broken attack phrase note.\n", global.tabs.c_str());
			printf("%sThis function aims to fix many of these issues while also applying an chart organization to evenly balance out chart counts amongst players.\n", global.tabs.c_str());
			printf("%sNote: if you come across an issue with it or have the program crash, send me the CHC file used in the Gitaroo Pals discord: https://discord.gg/ed6P8Jt \n", global.tabs.c_str());
			printf("%s(sonicfind#6404)%s\n", global.tabs.c_str(), global.tabs.c_str());

			printf("%sB - Both 'P' & 'F'\n", global.tabs.c_str());
			printf("%sDoes both option 'P' & option 'F' in a single function.\n%s\n", global.tabs.c_str(), global.tabs.c_str());

			printf("%sD - Perform detailed modifications on each CHC\n", global.tabs.c_str());
			printf("%sTakes you to a menu where you can manually edit some of the smaller details of each CHC file, including but not limited to: IMC and audio settings,\n", global.tabs.c_str());
			printf("%sgameplay speed/speed of all notes, .SSQ data, and player damage/energy multiplier factors.\n%s\n", global.tabs.c_str(), global.tabs.c_str());

			if (LoadLib(dlls[DLL_INDEX].libraries[1]))
			{
				printf("%sT - Convert each CHC into a PCSX2 TAS (only works with CHCs compatible with the PS2 version)\n", global.tabs.c_str());
				printf("%sGenerates a TAS file that can be played in PCSX2 from each CHC's note data. Very useful for testing charts that have been edited.\n", global.tabs.c_str());
				printf("%sNote 1: obviously, this only works on CHCs compatible with the PS2 version of the game.\n", global.tabs.c_str());
				printf("%sNote 2: currently requires using a custom build (for multitap support and optimized file sizes);\n", global.tabs.c_str());
				printf("%sContact sonicfind#6404 in the Gitaroo Pals discord for the build (https://discord.gg/5BdQnsN).\n%s\n", global.tabs.c_str(), global.tabs.c_str());
			}

			if (LoadLib(dlls[DLL_INDEX].libraries[2]))
			{
				printf("%sG - Export each CHC as a \".chart\" file for Clone/Guitar Hero\n", global.tabs.c_str());
				printf("%sConverts each CHC into a .chart for use in the Guitar Hero/Clone Hero chart editor, Moonscraper. You can download Moonscraper from the #test-builds channel.\n", global.tabs.c_str());
				printf("%sin their discord: https://discord.gg/5BdQnsN \n", global.tabs.c_str());
				printf("%sNote: this can only be used with CHCs that are fixed & organized.\n%s\n", global.tabs.c_str(), global.tabs.c_str());

				printf("%sI - Import notes from Clone/Guitar Hero \".chart\"s into the CHC files\n", global.tabs.c_str());
				printf("%sInserts & replaces section and note data from correctly formatted .chart files into each given CHC file. This is will not generate any new files or \n", global.tabs.c_str());
				printf("%soverwrite the current file automatically. That decision will be made during the process with each separate CHC.\n", global.tabs.c_str());
				printf("%s(Further formatting details in the Import_Readme.txt)\n", global.tabs.c_str());
				printf("%sNote: this can only be used with CHCs that are fixed & organized.\n%s\n", global.tabs.c_str(), global.tabs.c_str());
			}

			printf("%sC - Create PCSX2 Phrase Bar Color Cheat templates for each CHC\n", global.tabs.c_str());
			printf("%sGenerates outline .txt files for use in conjunction with BoringPerson#2570's phrase bar coloring Pyhton script. It will leave all chosen\n", global.tabs.c_str());
			printf("%ssections blank for you to fill in and all others filled with a base color. Feel free to change the colors after the files are generated.\n%s\n", global.tabs.c_str(), global.tabs.c_str());
		case '*':
			break;
		case 'n':
			return false;
		default:
			adjustTabs(1);
			while (files->size())
			{
				try
				{
					CHC_Main chc(files->front());
					switch (result)
					{
					case 'e':
						if (chc.menu(files->size()))
						{
							adjustTabs(0);
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
					printf("%s%s\n", global.tabs.c_str(), str.c_str());
					printf("%sLoad cancelled for %s.CHC\n", global.tabs.c_str(), files->front().c_str());
				}
				catch (const char* str)
				{
					printf("%s%s\n", global.tabs.c_str(), str);
					printf("%sLoad cancelled for %s.CHC\n", global.tabs.c_str(), files->front().c_str());
				}
				files->pop_front();
			}
			adjustTabs(0);
			global.quit = true;
			break;
		}
	} while (!global.quit);
	global.quit = false;
	printf("%sAll provided CHC files have been evaluated.\n", global.tabs.c_str());
	return false;
}

/*
Main menu prompt used for choosing what action to perform on the loaded CHC file.
Returns false if this is used from the multimenu and if the user wants to proceed to the next CHC.
*/
bool CHC_Main::menu(size_t fileCount)
{
	while(!global.quit)
	{
		banner(" " + song.shortname + ".CHC - Mode Selection ");
		string choices = "swpd";
		printf("%sS - Save\n", global.tabs.c_str());
		printf("%sW - Write %s.txt\n", global.tabs.c_str(), song.shortname.c_str());
		printf("%sP - Swap players\n", global.tabs.c_str());
		if (song.unorganized || !song.optimized)
		{
			printf("%sF - Fix & organize %s.CHC where necessary (may not get all present bugs)\n", global.tabs.c_str(), song.shortname.c_str());
			printf("%sB - Both 'P' & 'F'\n", global.tabs.c_str());
			choices += "fb";
		}
		printf("%sD - Detailed modification\n", global.tabs.c_str());
		if (song.imc[0])
		{
			if (LoadLib(dlls[DLL_INDEX].libraries[1]))
			{
				printf("%sT - Convert into a PCSX2 TAS (only works with songs available in the PS2 version)\n", global.tabs.c_str());
				choices += 't';
			}
		}
		if (LoadLib(dlls[DLL_INDEX].libraries[2]))
		{
			printf("%sG - Export as \".chart\" for chart editing in Moonscraper\n", global.tabs.c_str());
			choices += 'g';
			printf("%sI - Import notes from a Clone/Guitar Hero \".chart\" file\n", global.tabs.c_str());
			choices += 'i';
		}
		if (song.imc[0])
		{
			printf("%sC - Create a PCSX2 Phrase Bar Color Cheat template\n", global.tabs.c_str());
			choices += 'c';
		}
		if (fileCount > 1)
		{
			printf("%sN - Next CHC file\n", global.tabs.c_str());
			choices += 'n';
		}
		printf("%s? - Help info\n", global.tabs.c_str());
		if (fileCount > 0)
		{
			for (size_t i = 1; i < 20 && !global.quit; i++)
			{
				for (size_t s = 0; s < dlls[i].extensions.size() && !global.quit; s++)
				{
					if (dlls[i].extensions[s].files.size() > 0)
					{
						printf("%sQ - Proceed to the next filetype (%s)\n", global.tabs.c_str(), dlls[i].extensions[s].ext.c_str());
						global.quit = true;
					}
				}
			}
			if (global.quit)
				global.quit = false;
			else
				printf("%sQ - Quit Program\n", global.tabs.c_str());
		}
		else
			printf("%sQ - Main Menu\n", global.tabs.c_str());
		size_t result = menuChoices(choices);
		switch (result)
		{
		case 'q':
			if (!song.saved)
			{
				printf("%s\n", global.tabs.c_str());
				printf("%sRecent changes have not been saved externally to a CHC file. Which action will you take?\n", global.tabs.c_str());
				printf("%sS - Save CHC and Exit to Main Menu\n", global.tabs.c_str());
				printf("%sQ - Exit without saving\n", global.tabs.c_str());
				printf("%sC - Cancel\n", global.tabs.c_str());
				switch (menuChoices("sc"))
				{
				case 's':
					saveFile();
					if (!song.saved)
						break;
				case 'q':
					global.quit = true;
				}
			}
			else
				global.quit = true;
			break;
		case '?':
			printf("%sW - Write %s.txt\n", global.tabs.c_str(), song.shortname.c_str());
			printf("%sWrites out the data present in the CHC file to a readable txt file. This would include all SSQ & cue data, audio values,\n", global.tabs.c_str());
			printf("%ssection & subsection data - including values for every separate note -, and damage & energy factors. Creates two files -\n", global.tabs.c_str());
			printf("%swith one being a simplistic form with only the bare minimum necessary information.\n%s\n", global.tabs.c_str(), global.tabs.c_str());

			printf("%sP - Swap players\n", global.tabs.c_str());
			printf("%sGoes through and swaps the charts between the two main payers. For PS2 compatible CHCs, this will swap all charts for P1/P3 with\n", global.tabs.c_str());
			printf("%sP2/P4 respectively. For duet CHCs, this will only swap the charts of P1 with P3 as P2 is not important enough to warrant a swap & P4 is empty.\n", global.tabs.c_str());
			printf("%sNote: only use this on either 1. A base CHC that was not swapped using an alternate method, or 2. a CHC pre-swapped with this program.\n%s\n", global.tabs.c_str(), global.tabs.c_str());

			if (song.unorganized > 0 || !song.optimized)
			{
				printf("%sF - Fix & organize %s.CHC where necessary (may not get all present bugs)\n", global.tabs.c_str(), song.shortname.c_str());
				printf("%sMany CHC files provided from the base game are very prone to a multitude of glitches or contain sizable mistakes.\n", global.tabs.c_str());
				printf("%sFor example: Stage 8 having two guard marks being only 2 samples away from eachother, or Stage 3 having a broken attack phrase note.\n", global.tabs.c_str());
				printf("%sThis function aims to fix many of these issues while also applying an chart organization to evenly balance out chart counts amongst players.\n", global.tabs.c_str());
				printf("%sNote: if you come across an issue with it or have the program crash, send me the CHC file used in the Gitaroo Pals discord: https://discord.gg/ed6P8Jt \n", global.tabs.c_str());
				printf("%s(sonicfind#6404)\n%s\n", global.tabs.c_str(), global.tabs.c_str());

				printf("%sB - Both 'P' & 'F'\n", global.tabs.c_str());
				printf("%sDoes both option 'S' & option 'F' in a single function.\n%s\n", global.tabs.c_str(), global.tabs.c_str());
			}

			printf("%sD - Detailed modification\n", global.tabs.c_str());
			printf("%sTakes you to a menu where you can manually edit some of the smaller details of the CHC, including but not limited to: IMC and audio settings,\n", global.tabs.c_str());
			printf("%sgameplay speed/speed of all notes, .SSQ data, and player damage/energy multiplier factors.\n%s\n", global.tabs.c_str(), global.tabs.c_str());
			
			if (LoadLib(dlls[DLL_INDEX].libraries[1]))
			{
				printf("%sT - Convert into a PCSX2 TAS (only works with songs compatible with the PS2 version)\n", global.tabs.c_str());
				printf("%sGenerates a TAS file that can be played in PCSX2 from the CHC's note data. Very useful for testing charts that have been edited.\n", global.tabs.c_str());
				printf("%sNote 1: obviously, this only works on CHCs compatible with the PS2 version of the game.\n", global.tabs.c_str());
				printf("%sNote 2: currently requires using my own custom build (for multitap support and optimized file sizes)\n%s\n", global.tabs.c_str(), global.tabs.c_str());
			}

			if (LoadLib(dlls[DLL_INDEX].libraries[2]))
			{
				printf("%sG - Export as \".chart\" for chart editing in Moonscraper\n", global.tabs.c_str());
				printf("%sConverts the CHC into .chart for use in the Guitar Hero/Clone Hero chart editor, Moonscraper. You can download Moonscraper from the #test-builds channel.\n", global.tabs.c_str());
				printf("%sin their discord: https://discord.gg/5BdQnsN \n", global.tabs.c_str());
				printf("%sNote: this can only be used with CHCs that are fixed & organized.\n%s\n", global.tabs.c_str(), global.tabs.c_str());

				printf("%sI - Import notes from a Clone/Guitar Hero \".chart\" file\n", global.tabs.c_str());
				printf("%sInserts & replaces section and note data from correctly formatted .chart files into each given CHC file. This is will not generate any new files or \n", global.tabs.c_str());
				printf("%soverwrite the current file automatically. That decision will be made during the process with each separate CHC.\n", global.tabs.c_str());
				printf("%sRefer to the included Chart_import_README.txt for details on formatting a .chart file for this use\n", global.tabs.c_str());
				printf("%sNote: this can only be used with CHCs that are fixed & organized.\n%s\n", global.tabs.c_str(), global.tabs.c_str());
			}

			printf("%sC - Create a PCSX2 Phrase Bar Color Cheat template\n", global.tabs.c_str());
			printf("%sGenerates a outline txt file for use in conjunction with BoringPerson#2570's phrase bar coloring Pyhton script. It will leave all chosen\n", global.tabs.c_str());
			printf("%ssections blank for you to fill in and all others filled with a base color. Feel free to change the colors in the file after it's generated.\n%s\n", global.tabs.c_str(), global.tabs.c_str());
		case '*':
			break;
		case 'n':
			return false;
		default:
			adjustTabs(1);
			switch (result)
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
			case 'g':
				exportChart();
				break;
			case 'i':
				importChart();
				break;
			case 'c':
				createColorTemplate();
			}
			adjustTabs(0);
		}
	}
	return true;
}

void CHC_Main::saveFile()
{
	banner(" " + song.shortname + ".CHC Save Prompt ");
	string ext = "_T";
	string filename = song.name.substr(0, song.name.length() - 4);
	do
	{
		string choices = "a";
		if (song.saved != 2)
		{
			printf("%sS - Save & Overwrite %s.CHC\n", global.tabs.c_str(), song.shortname.c_str());
			choices += 's';
		}
		printf("%sA - Save as \"%s_T.CHC\"\n", global.tabs.c_str(), song.shortname.c_str());
		printf("%sQ - Back Out\n", global.tabs.c_str());
		switch (menuChoices(choices))
		{
		case 'q':
			return;
		case 's':
			printf("%sSaving %s.CHC\n", global.tabs.c_str(), song.shortname.c_str());
			song.create(song.name);
			song.saved = 2;
			global.quit = true;
			break;
		case 'a':
			do
			{
				switch (fileOverwriteCheck(filename + ext + ".CHC"))
				{
				case 'q':
					return;
				case 'n':
					printf("%s\n", global.tabs.c_str());
					ext += "_T";
					break;
				case 'y':
					song.create(filename + ext + ".CHC");
					do
					{
						printf("%sSwap loaded file to %s.CHC? [Y/N]\n", global.tabs.c_str(), (filename + ext).c_str());
						switch (menuChoices("yn"))
						{
						case 'y':
							song.name = filename + ext + ".CHC";
							song.shortname += ext;
							song.saved = 2;
						case 'n':
						case 'q':
							global.quit = true;
						}
					} while (!global.quit);
				}
			} while (!global.quit);
		}
	} while (!global.quit);
	global.quit = false;
}

/*
Writes out the data present in the CHC file to a readable txt file. This would include all SSQ & cue data, audio values,
section & subsection data - including values for every separate note -, and damage & energy factors. Creates two files -
with one being a simplistic form with only the bare minimum necessary information.
*/
void CHC_Main::writeTxt()
{
	banner(" Writing " + song.shortname + "_CHC.txt ");
	FILE* outTXT, * outSimpleTXT;
	fopen_s(&outTXT, (song.name.substr(0, song.name.length() - 4) + "_CHC.txt").c_str(), "w");
	fopen_s(&outSimpleTXT, (song.name.substr(0, song.name.length() - 4) + "_CHC_SIMPLIFIED.txt").c_str(), "w");
	try
	{
		fprintf(outTXT, "Header: %s", song.header);
		dualvfprintf_s(outTXT, outSimpleTXT, "IMC: ");
		song.imc[0] ? dualvfprintf_s(outTXT, outSimpleTXT, song.imc)		//IMC
			: dualvfprintf_s(outTXT, outSimpleTXT, "Unused in PSP version");
		dualvfprintf_s(outTXT, outSimpleTXT, "\n\t   SSQ Events:\n");
		for (size_t index = 0; index < 4; index++)
		{
			switch (index)
			{
			case 0: dualvfprintf_s(outTXT, outSimpleTXT, "\t\t    Win A:\n"); break;
			case 1: dualvfprintf_s(outTXT, outSimpleTXT, "\t\t    Win B:\n"); break;
			case 2: dualvfprintf_s(outTXT, outSimpleTXT, "\t\t Lose Pre:\n"); break;
			case 3: dualvfprintf_s(outTXT, outSimpleTXT, "\t\tLose Loop:\n");
			}
			dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\tFirst Frame: %g\n", song.events[index].first);
			dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t Last Frame: %g\n", song.events[index].last);
		}
		dualvfprintf_s(outTXT, outSimpleTXT, "       Audio Channels:\n");
		if (song.imc[0])
		{
			for (size_t index = 0; index < 8; index++)
			{
				dualvfprintf_s(outTXT, outSimpleTXT, "\t\t Channel %zu:\n", index + 1);
				dualvfprintf_s(outTXT, outSimpleTXT, "\t\t       Volume: %lu (%g%%)\n", song.audio[index].volume, song.audio[index].volume * 100.0 / 32767);
				switch (song.audio[index].pan)
				{
				case 0: dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t  Pan: Left (0)\n"); break;
				case 16383: dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t  Pan: Center (16383)\n"); break;
				case 32767: dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t  Pan: Right (32767)\n"); break;
				default: dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t  Pan: %g%% Left | %g%% Right (%lu)\n", 100 - (song.audio[index].pan * 100.0 / 32767),
					song.audio[index].pan * 100.0 / 32767, song.audio[index].pan);
				}
			}
		}
		else
		{
			dualvfprintf_s(outTXT, outSimpleTXT, "\t\t Channel 1: Unused in PSP version\n");
			dualvfprintf_s(outTXT, outSimpleTXT, "\t\t Channel 2: Unused in PSP version\n");
			for (size_t index = 2; index < 8; index++)
			{
				dualvfprintf_s(outTXT, outSimpleTXT, "\t\t Channel %zu:\n", index + 1);
				dualvfprintf_s(outTXT, outSimpleTXT, "\t\t       Volume: %lu (%g%%)\n", song.audio[index].volume, song.audio[index].volume * 100.0 / 32767);
				dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t  Pan: Unused in PSP version\n");
			}
		}
		dualvfprintf_s(outTXT, outSimpleTXT, "\t       Speed: %g\n", song.speed);
		fprintf(outTXT, "\t   # of Cues: %zu\n", song.sections.size());
		fputs("\t    SSQ Cues:\n", outTXT);
		for (size_t cueIndex = 0; cueIndex < song.sections.size(); cueIndex++)	//Cues
		{
			SongSection& section = song.sections[cueIndex];
			fprintf(outTXT, "\t       Cue %s:\n", section.name);
			fprintf(outTXT, "\t\t\tAudio Used: %s\n", section.audio);
			fprintf(outTXT, "\t\t\t     Index: %lu\n", section.index);
			fprintf(outTXT, "\t\t       First frame: %5g\n", section.frames.first);
			fprintf(outTXT, "\t\t\tLast frame: %5g\n", section.frames.last);
		}
		fflush(outTXT);
		fflush(outSimpleTXT);
		dualvfprintf_s(outTXT, outSimpleTXT, "       # of Sections: %zu\n", song.sections.size());
		dualvfprintf_s(outTXT, outSimpleTXT, "       Song Sections:\n");
		for (size_t sectIndex = 0; sectIndex < song.sections.size(); sectIndex++) //SongSections
		{
			SongSection& section = song.sections[sectIndex];
			dualvfprintf_s(outTXT, outSimpleTXT, "\t       Section %s:\n", section.name);
			section.organized & 1 ? dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t       Organized: TRUE\n")
				: dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t       Organized: FALSE\n");
			if (section.swapped < 4)
			{
				if (section.swapped == 0)
					dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t Swapped Players: FALSE\n");
				else if (section.swapped == 1)
					dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t Swapped Players: TRUE (P2/P1/P4/P3)\n");
				else if (section.swapped == 2)
				{
					if (song.imc[0])
						dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t Swapped Players: TRUE (P3/P4/P1/P2)\n");
					else
						dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t Swapped Players: TRUE (P3/P2/P1/P4) [DUET]\n");
				}
				else
					dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t Swapped Players: TRUE (P4/P3/P2/P1)\n");
			}
			else
			{
				if (section.swapped == 0)
					dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t Swapped Players: FALSE (P1/P2D/P3/P4D) [Duet->PS2 Conversion]\n");
				else if (section.swapped == 1)
					dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t Swapped Players: TRUE (P2D/P1/P4D/P3) [Duet->PS2 Conversion]\n");
				else if (section.swapped == 2)
					dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t Swapped Players: TRUE (P3/P4D/P1/P2D) [Duet->PS2 Conversion]\n");
				else
					dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t Swapped Players: TRUE (P4D/P3/P2D/P1) [Duet->PS2 Conversion]\n");
			}
			fprintf(outTXT, "\t\t       Size (32bit) - 44: %lu\n", section.size);
			fprintf(outTXT, "\t\t\t\t    Junk: 0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(section.junk)));
			fprintf(outTXT, "\t\t\t\t\t  0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(section.junk + 4)));
			fprintf(outTXT, "\t\t\t\t\t  0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(section.junk + 8)));
			fprintf(outTXT, "\t\t\t\t\t  0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(section.junk + 12)));
			switch (section.battlePhase)
			{
			case SongSection::Phase::INTRO:
				dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t   Phase: INTRO\n");
				break;
			case SongSection::Phase::CHARGE:
				dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t   Phase: CHARGE\n");
				break;
			case SongSection::Phase::BATTLE:
				dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t   Phase: BATTLE\n");
				break;
			case SongSection::Phase::FINAL_AG:
				dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t   Phase: FINAL_AG\n");
				break;
			case SongSection::Phase::HARMONY:
				dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t   Phase: HARMONY\n");
				break;
			case SongSection::Phase::END:
				dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t   Phase: END\n");
				break;
			default:
				dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t   Phase: FINAL_I\n");
			}
			dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t   Tempo: %g\n", section.tempo);
			fprintf(outTXT, "\t\t\t    Samples/Beat: %Lg\n", SAMPLES_PER_MIN / section.tempo);
			dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\tDuration: %lu samples\n", section.duration);
			dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t # of Conditions: %lu\n", section.conditions.size());
			for (size_t condIndex = 0; condIndex < section.conditions.size(); condIndex++)
			{
				SongSection::Condition& cond = section.conditions[condIndex];
				dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t   Condition %zu:\n", condIndex + 1);
				switch (cond.type)
				{
				case 'q':
					fputs("\t\t\t\t\t\tType: Unconditional\n", outTXT);
					dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t      Result: Always true.\n");
					break;
				case '!':
					fputs("\t\t\t\t\t\tType: Left Side Energy\n", outTXT);
					fprintf(outTXT, "\t\t\t\t\t    Argument: %g\n", cond.argument);
					dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t      Result: True if Left Side's energy is less than %g%%. False otherwise.\n", cond.argument * 100.0);
					break;
				case 2:
					fputs("\t\t\t\t\t\tType: Right Side Energy\n", outTXT);
					fprintf(outTXT, "\t\t\t\t\t    Argument: %g\n", cond.argument);
					dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t      Result: True if Right Side's energy is less than %g%%. False otherwise.\n", cond.argument * 100.0);
					break;
				case 3:
					fputs("\t\t\t\t\t\tType: Random\n", outTXT);
					fprintf(outTXT, "\t\t\t\t\t    Argument: %g\n", cond.argument);
					dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t      Result: Generates a random number between 0.0 and 1.0, True if the random number is less than %g%%. False otherwise.\n", cond.argument);
					break;
				case 4:
					fputs("\t\t\t\t\t\tType: Left Side Unavailable\n", outTXT);
					dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t      Result: False if this match has a Player 3 participating (only possible in Versus mode). True otherwise.\n");
					break;
				case 5:
					fputs("\t\t\t\t\t\tType: Right Side Unavailable\n", outTXT);
					dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t      Result: False if this match has a Player 4 participating (only possible in Versus mode). True otherwise.\n");
					break;
				case 6:
					fputs("\t\t\t\t\t\tType: Left Side < Right Side\n", outTXT);
					dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t      Result: True if Left Side's energy is less than Right Side's energy. False otherwise.\n");
					break;
				}
				if (cond.trueEffect < 0)
				{
					if (cond.trueEffect == -1) dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t True Effect: Move to the next condition.\n");
					else dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t True Effect: Move over %li conditions.\n", abs(cond.trueEffect));
				}
				else
				{
					if ((size_t)cond.trueEffect < song.sections.size()) dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t True Effect: Move to Section %s.\n", song.sections[cond.trueEffect].name);
					else dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t True Effect: End song.\n");
				}
				if (cond.type != 0)
				{
					if (cond.falseEffect < 0)
					{
						if (cond.falseEffect == -1) dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\tFalse Effect: Move to the next condition\n");
						else dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\tFalse Effect: Move over %li conditions.\n", abs(cond.falseEffect));
					}
					else
					{
						if ((size_t)cond.falseEffect < song.sections.size()) dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\tFalse Effect: Move to Section %s.\n", song.sections[cond.falseEffect].name);
						else dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\tFalse Effect: End song.\n");
					}
				}
			}
			dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t    # of Players: %lu\n", section.numPlayers);
			dualvfprintf_s(outTXT, outSimpleTXT, "\t\t       Charts per Player: %lu\n", section.numCharts);
			for (size_t playerIndex = 0; playerIndex < section.numPlayers; playerIndex++)
			{
				for (size_t chartIndex = 0; chartIndex < section.numCharts; chartIndex++)
				{
					Chart& chart = section.charts[playerIndex * section.numCharts + chartIndex];
					dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t Player %zu - Chart %02zu\n", playerIndex + 1, chartIndex + 1);
					fprintf(outTXT, "\t\t\t\t\t  Size (32bit): %lu\n", chart.getSize());
					fprintf(outTXT, "\t\t\t\t\t\t  Junk: 0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(chart.getJunk())));
					fprintf(outTXT, "\t\t\t\t\t\t\t0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(chart.getJunk() + 4)));
					fprintf(outTXT, "\t\t\t\t\t\t\t0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(chart.getJunk() + 8)));
					fprintf(outTXT, "\t\t\t\t\t\t\t0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(chart.getJunk() + 12)));
					dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t   Pivot Point: %lu samples\n", chart.getPivotTime());
					fprintf(outTXT, "\t\t\t\t\t      End Time: %lu samples\n", chart.getEndTime());
					dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t      # of Trace Lines: %zu\n", chart.getNumTracelines());
					for (size_t traceIndex = 0; traceIndex < chart.getNumTracelines(); traceIndex++)
					{
						Traceline& trace = chart.getTraceline(traceIndex);
						dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t\t  Trace Line %03zu:\n", traceIndex + 1);
						dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t\t\t Pivot Alpha: %+li samples\n", trace.getPivotAlpha());
						dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t\t\t  Start Time: %li samples (Relative to SongSection)\n", trace.getPivotAlpha() + chart.getPivotTime());
						fprintf(outTXT, "\t\t\t\t\t\t\t    Duration: %lu samples\n", trace.getDuration());
						fprintf(outTXT, "\t\t\t\t\t\t\t       Angle: %s*PI radians | %li degrees\n", angleToFraction(trace.getAngle()).c_str(), radiansToDegrees(trace.getAngle()));
						trace.getCurve() ? fputs("\t\t\t\t\t\t\t       Curve: True\n", outTXT)
							: fputs("\t\t\t\t\t\t\t       Curve: False\n", outTXT);
						fprintf(outTXT, "\t\t\t\t\t\t\t    End Time: %li samples (Relative to SongSection)\n", trace.getEndAlpha() + chart.getPivotTime());
						if (traceIndex + 1 == chart.getNumTracelines())
							fprintf(outSimpleTXT, "\t\t\t\t\t\t\t    End Time: %li samples (Relative to SongSection)\n", trace.getEndAlpha() + chart.getPivotTime());
					}
					dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t # of Phrase Fragments: %zu\n", chart.getNumPhrases());
					for (size_t phraseIndex = 0, traceIndex = 0, note = 0, piece = 0; phraseIndex < chart.getNumPhrases(); phraseIndex++)
					{
						Phrase& phrase = chart.getPhrase(phraseIndex);
						for (; traceIndex < chart.getNumTracelines(); traceIndex++)
							if (chart.getTraceline(traceIndex).contains(phrase.getPivotAlpha()))
								break;
						dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t     Phrase Fragment %03zu:\n", phraseIndex + 1);
						dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t     [Note #%03zu", note + 1);
						if (piece)
							dualvfprintf_s(outTXT, outSimpleTXT, " - Piece ##%02zu", piece + 1);
						fprintf(outTXT, "| Trace Line #%03zu]:\n", traceIndex + 1);
						fputs("]:\n", outSimpleTXT);
						dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t\t\t Pivot Alpha: %+li samples\n", phrase.getPivotAlpha());
						dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t\t\t  Start Time: %li samples (Relative to SongSection)\n", phrase.getPivotAlpha() + chart.getPivotTime());
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
							dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t\t\t    End Time: %li samples (Relative to SongSection)\n", phrase.getEndAlpha() + chart.getPivotTime());
						else
							fprintf(outTXT, "\t\t\t\t\t\t\t    End Time: %li samples (Relative to SongSection)\n", phrase.getEndAlpha() + chart.getPivotTime());
						fprintf(outTXT, "\t\t\t\t\t\t\t\tJunk: 0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(phrase.getJunk())));
						fprintf(outTXT, "\t\t\t\t\t\t\t\t      0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(phrase.getJunk() + 4)));
						fprintf(outTXT, "\t\t\t\t\t\t\t\t      0x%08x\n", _byteswap_ulong(*reinterpret_cast<unsigned long*>(phrase.getJunk() + 8)));
					}
					dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t      # of Guard Marks: %zu\n", chart.getNumGuards());
					for (size_t guardIndex = 0; guardIndex < chart.getNumGuards(); guardIndex++)
					{
						Guard& guard = chart.getGuard(guardIndex);
						dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t\t  Guard Mark %03zu:\n", guardIndex + 1);
						dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t\t\t Pivot Alpha: %+li samples\n", guard.getPivotAlpha());
						dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t\t\t  Start Time: %li samples (Relative to SongSection)\n", guard.getPivotAlpha() + chart.getPivotTime());
						dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t\t\t\t      Button: ");
						switch (guard.getButton())
						{
						case 'q': fputs("Left\n", outTXT); fputs("Square\n", outSimpleTXT); break;
						case '!': fputs("Down\n", outTXT); fputs("X/Cross\n", outSimpleTXT); break;
						case 2: fputs("Right\n", outTXT); fputs("Circle\n", outSimpleTXT); break;
						case 3: fputs("Up\n", outTXT); fputs("Triangle\n", outSimpleTXT);
						}
					}
				}
			}
			fflush(outTXT);
			fflush(outSimpleTXT);
		}
		dualvfprintf_s(outTXT, outSimpleTXT, "Damage/Energy Factors:\n");
		for (size_t player = 0; player < 4; player++)
		{
			dualvfprintf_s(outTXT, outSimpleTXT, "       Player %zu ||", player + 1);
			dualvfprintf_s(outTXT, outSimpleTXT, " Starting Energy || Initial-Press Energy || Initial-Press Damage || Guard Energy Gain ||");
			dualvfprintf_s(outTXT, outSimpleTXT, " Attack Miss Damage || Guard Miss Damage || Release Max Energy || Release Max Damage ||\n");
			dualvfprintf_s(outTXT, outSimpleTXT, "       %s||\n", string(184, '='));
			for (size_t section = 0; section < 5; section++)
			{
				switch (section)
				{
				case 0: dualvfprintf_s(outTXT, outSimpleTXT, "\t  Intro ||"); break;
				case 1: dualvfprintf_s(outTXT, outSimpleTXT, "\t Charge ||"); break;
				case 2: dualvfprintf_s(outTXT, outSimpleTXT, "\t Battle ||"); break;
				case 3: dualvfprintf_s(outTXT, outSimpleTXT, "\tHarmony ||"); break;
				case 4: dualvfprintf_s(outTXT, outSimpleTXT, "\t    End ||");
				}
				dualvfprintf_s(outTXT, outSimpleTXT, "%015g%% ||%020g%% ||%020g%% ||%017g%% ||%018g%% ||%017g%% ||%018g%% ||%018g%% ||\n",
					song.energyDamageFactors[player][section].start * 100.0,
					song.energyDamageFactors[player][section].chargeInitial * 100.0,
					song.energyDamageFactors[player][section].attackInitial * 100.0,
					song.energyDamageFactors[player][section].guardEnergy * 100.0,
					song.energyDamageFactors[player][section].attackMiss * 100.0,
					song.energyDamageFactors[player][section].guardMiss * 100.0,
					song.energyDamageFactors[player][section].chargeRelease * 100.0,
					song.energyDamageFactors[player][section].attackRelease * 100.0);
			}
			dualvfprintf_s(outTXT, outSimpleTXT, "\n");
		}
	}
	catch (string str)
	{
		printf("%s%s\n", global.tabs.c_str(), str.c_str());
	}
	catch (const char* str)
	{
		printf("%s%s\n", global.tabs.c_str(), str);
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
bool CHC_Main::applyChanges(bool fix, bool swap, bool save)
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
			song.create(song.name);
		return true;
	}
	catch (exception e)
	{
		printf("%sException Caught: %s\n", global.tabs.c_str(), e.what());
		return false;
	}
}

/*
Calls the editor for... well, editing. If the file currently pointed at in the editor is overwritten, it will swap the CHC file currently held
in the CHC object with the file from the editor. Otherwise, after the editor is closed, it will reassert the CHC object's file back into editor
as if no changes were made to the original file.
@param multi - Is the multimenu being used?
*/
void CHC_Main::edit(bool multi)
{
	CHC_Editor editor(song);
	editor.editSong(multi);
}

void CHC_Main::makeTAS()
{
	loadProc(dlls[DLL_INDEX].libraries[1].dll, "chcTAS", song);
}

void CHC_Main::exportChart()
{
	banner(" " + song.shortname + " - .CHART creation ");
	if (song.unorganized != 0)
	{
		do
		{
			printf("%sTo use the CH Chart Export option on %s.CHC, the file must fully fixed & organized.\n", global.tabs.c_str(), song.shortname.c_str());
			printf("%sFix & organize the file now? [Y/N]\n", global.tabs.c_str());
			switch (menuChoices("yn"))
			{
			case 'y':
				applyChanges(true, false);
				banner(" " + song.shortname + " - .CHART creation ");
				global.quit = true;
				break;
			case 'n':
			case 'q':
				printf("%sCH Chart Export on %s aborted due to not being fully organized.\n", global.tabs.c_str(), song.shortname.c_str());
				return;
			}
		} while (!global.quit);
		global.quit = false;
	}
	loadProc(dlls[DLL_INDEX].libraries[2].dll, "exportChart", song);
}

void CHC_Main::importChart()
{
	banner(" " + song.shortname + " - .CHART Note Import/Replacement ");
	if (song.unorganized != 0)
	{
		do
		{
			printf("%sTo use the CH Chart Import option on %s.CHC, the file must fully fixed & organized.\n", global.tabs.c_str(), song.shortname.c_str());
			printf("%sFix & organize the file now? [Y/N]\n", global.tabs.c_str());
			switch (menuChoices("yn"))
			{
			case 'y':
				applyChanges(true, false);
				banner(" " + song.shortname + " - .CHART Note Import/Replacement ");
				global.quit = true;
				break;
			case 'n':
			case 'q':
				printf("%sCH Chart Import on %s aborted due to not being fully organized.\n", global.tabs.c_str(), song.shortname.c_str());
				return;
			}
		} while (!global.quit);
		global.quit = false;
	}
	loadProc(dlls[DLL_INDEX].libraries[2].dll, "importChart", song);
}

bool CHC_Main::createColorTemplate()
{
	banner(" " + song.shortname + ".CHC - Color Sheet Creation ");
	bool multiplayer = song.shortname.find('M') != string::npos;
	List<size_t> sectionIndexes;
	do
	{
		printf("%sType the number for each section that you wish to outline colors for - w/ spaces inbetween.\n", global.tabs.c_str());
		for (size_t sectIndex = 0; sectIndex < song.sections.size(); sectIndex++)
			printf("%s%zu - %s\n", global.tabs.c_str(), sectIndex, song.sections[sectIndex].getName());
		if (sectionIndexes.size())
		{
			printf("%sCurrent List: ", global.tabs.c_str());
			for (size_t index = 0; index < sectionIndexes.size(); index++)
				printf("%s ", song.sections[sectionIndexes[index]].getName());
			putchar('\n');
		}
		switch (listValueInsert(sectionIndexes, "yn", song.sections.size(), false))
		{
		case '?':
			printf("%sHelp: [TBD]\n%s\n", global.tabs.c_str(), global.tabs.c_str());
			break;
		case 'q':
			printf("%sColor Sheet creation cancelled.\n", global.tabs.c_str());
			return false;
		case '!':
			if (!sectionIndexes.size())
			{
				do
				{
					printf("%sNo sections have been selected. Quit Color Sheet creation? [Y/N]\n", global.tabs.c_str());
					switch (menuChoices("yn"))
					{
					case 'q':
					case 'y':
						printf("%sColor Sheet creation cancelled.\n", global.tabs.c_str());
						return false;
					case 'n':
						global.quit = true;
					}
				} while (!global.quit);
				global.quit = false;
			}
			else
				global.quit = true;
			break;
		case 'y':
			if (!sectionIndexes.size())
			{
				printf("%sColor Sheet creation cancelled.\n", global.tabs.c_str());
				return false;
			}
			else
			{
				global.quit = true;
				break;
			}
		case 'n':
			printf("%s\n", global.tabs.c_str());
			printf("%sOk... If you're not quitting this process, there's no need to say 'N' ya' silly goose.\n", global.tabs.c_str());
			printf("%s\n", global.tabs.c_str());
		}
	} while (!global.quit);
	global.quit = false;
	string filename = song.name.substr(0, song.name.length() - 4) + "_COLORDEF";
	string filename2 = song.name.substr(0, song.name.length() - 4) + "_COLORDEF_FRAGS";
	FILE *outSheet = nullptr, *outSheet2 = nullptr;
	do
	{
		switch (fileOverwriteCheck(filename + ".txt"))
		{
		case 'n':
			printf("%s\n", global.tabs.c_str());
			filename += "_T";
			break;
		case 'y':
			fopen_s(&outSheet, (filename + ".txt").c_str(), "w");
		case 'q':
			global.quit = true;
		}
	} while (!global.quit);
	printf("%s\n", global.tabs.c_str());
	global.quit = false;
	do
	{
		switch (fileOverwriteCheck(filename2 + ".txt"))
		{
		case 'n':
			printf("%s\n", global.tabs.c_str());
			filename2 += "_T";
			break;
		case 'y':
			fopen_s(&outSheet2, (filename2 + ".txt").c_str(), "w");
		case 'q':
			global.quit = true;
		}
	} while (!global.quit);
	if (outSheet != nullptr || outSheet2 != nullptr)
	{
		fputs("[phrasemode fragments]\n", outSheet2);
		dualvfprintf_s(outSheet, outSheet2, "[attack point palette]\nG: 00ff00\nR: ff0000\nY: ffff00\nB: 0000ff\nO: ff7f00\nP: ff00ff\nN: f89b44\ng: ffffff\nr: ffffff\ny: ffffff\nb: ffffff\no: ffffff\np: ffffff\n\n");
		dualvfprintf_s(outSheet, outSheet2, "[phrase bar palette]\nG: 40ff40\nR: ff4040\nY: ffff40\nB: 4040c8\nO: ff9f40\nP: ff40ff\nN: f07b7b\ng: 40ff40\nr: ff4040\ny: ffff40\nb: 4040c8\no: ff9f40\np: ff40ff\n\n");
		unsigned long chartCount = 0;
		bool* inputs = new bool[song.sections.size()]();
		for (size_t sect = 0; sect < sectionIndexes.size(); sect++)
			inputs[sectionIndexes[sect]] = true;
		for (unsigned long sectIndex = 0; sectIndex < song.sections.size(); sectIndex++)
		{
			SongSection& section = song.sections[sectIndex];
			for (unsigned playerIndex = 0; playerIndex < section.numPlayers; playerIndex++)
			{
				for (unsigned chartIndex = 0; chartIndex < section.numCharts; chartIndex++)
				{
					if (!(playerIndex & 1) || multiplayer)
					{
						Chart& chart = section.charts[(unsigned long long)playerIndex * section.numCharts + chartIndex];
						if (chart.getNumPhrases())
						{
							dualvfprintf_s(outSheet, outSheet2, "#SongSection %lu [%s], P%lu CHCH %lu\n", sectIndex, section.getName(), playerIndex + 1, chartIndex);
							dualvfprintf_s(outSheet, outSheet2, "[drawn chart %lu]\n", chartCount);
							if (inputs[sectIndex])
							{
								for (unsigned long phrIndex = 0; phrIndex < chart.getNumPhrases(); phrIndex++)
								{
									if (!phrIndex)
									{
										fputc('_', outSheet);
										fputs("!_", outSheet2);
									}
									else if (chart.getPhrase(phrIndex).getStart()) //Start
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
									if (!phrIndex)
									{
										fprintf_s(outSheet, "N");
										fprintf_s(outSheet2, "!N");
									}
									else if (chart.getPhrase(phrIndex).getStart())
									{
										fprintf_s(outSheet, " N");
										fprintf_s(outSheet2, " !N");
									}
									else
										fprintf_s(outSheet2, "-N");
								}
							}
							fprintf_s(outSheet, "\n\n");
							fprintf_s(outSheet2, "\n\n");
						}
						chartCount++;
					}
				}
			}
			if (inputs[sectIndex])
				printf("%sColored %s\n", global.tabs.c_str(), section.getName());
		}
		delete[song.sections.size()] inputs;
		if (outSheet != nullptr)
			fclose(outSheet);
		if (outSheet2 != nullptr)
			fclose(outSheet2);
		return true;
	}
	else
	{ 
		printf("%sColor Sheet creation cancelled.\n", global.tabs.c_str());
		return false;
	}
}