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
#include "SSQ.h"
using namespace GlobalFunctions;
const std::string SSQ::multiChoiceString = "ew";

bool SSQ::menu(bool nextFile, const std::pair<bool, const char*> nextExtension)
{
	const std::string choices = nextFile ? "swn" : "sw";
	while (true)
	{
		banner(" " + m_filename + m_extension + " - Mode Selection ");
		printf_tab("S - Save\n");
		printf_tab("W - Write %s_SSQ.txt\n", m_filename.c_str());
		//printf_tab("I - Import from a .obj file\n");
		if (nextFile)
			printf_tab("N - Next .SSQ file\n");

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

			break;
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
			default:
				functionSelection(g_global.answer.character, false);
			}
			--g_global;
		}
	}
}

bool SSQ::functionSelection(const char choice, bool isMulti)
{
	switch (choice)
	{
	case 'w':
		return write_to_txt();
	default:
		return false;
	}
}

void SSQ::displayMultiChoices()
{
	printf_tab("E - Evaluate each SSQ individually\n");
	printf_tab("W - Write all SSQs included to readable .txts\n");
}

void SSQ::displayMultiHelp()
{

}

