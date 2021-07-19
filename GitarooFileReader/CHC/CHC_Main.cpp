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
const std::string CHC::multiChoiceString = "ewpfbdcktgi";

bool CHC::menu(bool nextFile, const std::pair<bool, const char*> nextExtension)
{
	while (true)
	{
		banner(" " + m_filename + ".CHC - Mode Selection ");
		std::string choices = "swpdtei";
		printf_tab("S - Save\n");
		printf_tab("W - Write %s_CHC.txt\n", m_filename.c_str());
		printf_tab("P - Swap players\n");
		if (!isOrganized() || !isOptimized())
		{
			printf_tab("F - Fix & organize %s.CHC where necessary (may not get all present bugs)\n", m_filename.c_str());
			printf_tab("B - Both 'P' & 'F'\n");
			choices += "fb";
		}
		printf_tab("D - Detailed modification\n");
		if (isPS2Compatible())
			printf_tab("T - Convert into a PCSX2 TAS (only works with songs available in the PS2 version)\n");
		printf_tab("G - Export as \".chart\" for use in Guitar/Clone Hero and chart editing in Moonscraper\n");
		printf_tab("I - Import notes from a Clone/Guitar Hero \".chart\" file\n");
		if (isPS2Compatible())
		{
			printf_tab("C - Create a PCSX2 Phrase Bar Color Cheat template\n");
			choices += 'c';
		}
		printf_tab("? - Help info\n");
		if (nextFile)
		{
			printf_tab("N - Next .CHC file\n");
			choices += 'n';
		}

		if (nextExtension.first)
			printf_tab("Q - Proceed to the next filetype (%s)\n", nextExtension.second);
		else
			printf_tab("Q - Quit\n");

		switch (menuChoices(choices))
		{
		case ResultType::Quit:
			if (checkSave(true))
				return true;
			break;
		case ResultType::Help:
		{
			printf_tab("W - Write %s.txt\n", m_filename.c_str());
			printf_tab("Writes out the data present in the CHC file to a readable txt file. This would include all SSQ & cue data, audio values,\n");
			printf_tab("section & subsection data - including values for every separate note -, and damage & energy factors. Creates two files -\n");
			printf_tab("with one being a simplistic form with only the bare minimum necessary information.\n");
			printf_tab("\n");

			printf_tab("P - Swap players\n");
			printf_tab("Goes through and swaps the charts between the two main payers. For PS2 compatible CHCs, this will swap all charts for P1/P3 with\n");
			printf_tab("P2/P4 respectively. For duet CHCs, this will only swap the charts of P1 with P3 as P2 is not important enough to warrant a swap & P4 is empty.\n");
			printf_tab("Note: only use this on either 1. A base CHC that was not swapped using an alternate method, or 2. a CHC pre-swapped with this program.\n");
			printf_tab("\n");

			if (!isOrganized() || !isOptimized())
			{
				printf_tab("F - Fix & organize %s.CHC where necessary (may not get all present bugs)\n", m_filename.c_str());
				printf_tab("Many CHC files provided from the base game are very prone to a multitude of glitches or contain sizable mistakes.\n");
				printf_tab("For example: Stage 8 having two guard marks being only 2 samples away from eachother, or Stage 3 having a broken attack phrase note.\n");
				printf_tab("This function aims to fix many of these issues while also applying an chart organization to evenly balance out chart counts amongst players.\n");
				printf_tab("Note: if you come across an issue with it or have the program crash, send me the CHC file used in the Gitaroo Pals discord: https://discord.gg/ed6P8Jt \n");
				printf_tab("(sonicfind#6404)\n");
				printf_tab("\n");

				printf_tab("B - Both 'P' & 'F'\n");
				printf_tab("Does both option 'S' & option 'F' in a single function.\n");
				printf_tab("\n");
			}

			printf_tab("D - Detailed modification\n");
			printf_tab("Takes you to a menu where you can manually edit some of the smaller details of the CHC, including but not limited to: IMC and audio settings,\n");
			printf_tab("gameplay speed/speed of all notes, .SSQ data, and player damage/energy multiplier factors.\n");
			printf_tab("\n");

			printf_tab("T - Convert into a PCSX2 TAS (only works with songs compatible with the PS2 version)\n");
			printf_tab("Generates a TAS file that can be played in PCSX2 from the CHC's note data. Very useful for testing charts that have been edited.\n");
			printf_tab("Note 1: obviously, this only works on CHCs compatible with the PS2 version of the game.\n");
			printf_tab("Note 2: currently requires using my own custom build (for multitap support and optimized file sizes)\n");
			printf_tab("\n");

			printf_tab("E - Export as \".chart\" for chart editing in Moonscraper\n");
			printf_tab("Converts the CHC into .chart for use in the Guitar Hero/Clone Hero chart editor, Moonscraper. You can download Moonscraper from the #test-builds channel.\n");
			printf_tab("in their discord: https://discord.gg/5BdQnsN \n");
			printf_tab("Note: this can only be used with CHCs that are fixed & organized.\n");
			printf_tab("\n");

			printf_tab("I - Import notes from a Clone/Guitar Hero \".chart\" file\n");
			printf_tab("Inserts & replaces section and note data from correctly formatted .chart files into each given CHC file. This is will not generate any new files or \n");
			printf_tab("overwrite the current file automatically. That decision will be made during the process with each separate CHC.\n");
			printf_tab("Refer to the included Chart_import_README.txt for details on formatting a .chart file for this use\n");
			printf_tab("Note: this can only be used with CHCs that are fixed & organized.\n");
			printf_tab("\n");

			printf_tab("C - Create a PCSX2 Phrase Bar Color Cheat template\n");
			printf_tab("Generates a outline txt file for use in conjunction with BoringPerson#2570's phrase bar coloring Pyhton script. It will leave all chosen\n");
			printf_tab("sections blank for you to fill in and all others filled with a base color. Feel free to change the colors in the file after it's generated.\n");
			printf_tab("\n");
			break;
		}
		case ResultType::Success:
			++g_global;
			switch (g_global.answer.character)
			{
			case 'n':
				if (checkSave(false))
				{
					--g_global;
					return false;
				}
				break;
			case 's':
				fileSavePrompt();
				break;
			case 'w':
				write_to_txt();
			default:
				functionSelection(g_global.answer.character, false);
			}
			--g_global;
		}
	}
}

bool CHC::functionSelection(const char choice, bool isMulti)
{
	switch (choice)
	{
	case 'p':
		return applyChanges(false, true, isMulti);
	case 'f':
		return applyChanges(true, false, isMulti);
	case 'b':
		return applyChanges(true, true, isMulti);
	case 'd':
		return edit(isMulti);
	case 't':
		return buildTAS();
	case 'e':
		return exportForCloneHero();
	case 'i':
		return importFromCloneHero(isMulti);
	case 'c':
		return colorCheatTemplate();
	default:
		return false;
	}
}

void CHC::displayMultiChoices()
{
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
}

void CHC::displayMultiHelp()
{
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
}
