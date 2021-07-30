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
#include "FileMain.h"
#include "XGM/XGM.h"
using namespace GlobalFunctions;
const std::string XGM::multiChoiceString = "ew";

bool XGM::menu(bool nextFile, const std::pair<bool, const char*> nextExtension)
{
	const std::string choices = nextFile ? "swtmn" : "swtm";
	while (true)
	{
		banner(" " + m_filename + ".XGM - Mode Selection ");
		printf_tab("S - Save\n");
		printf_tab("W - Write %s_XGM.txt\n", m_filename.c_str());
		printf_tab("T - Select a texture [Count: %zu]\n", m_textures.size());
		printf_tab("M - Select a model   [Count: %zu]\n", m_models.size());

		if (nextFile)
			printf_tab("N - Next .XGM file\n");

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

bool XGM::functionSelection(const char choice, bool isMulti)
{
	switch (choice)
	{
	case 'w':
		return write_to_txt();
	case 't':
		return selectTexture();
	case 'm':
		return selectModel();
	default:
		return false;
	}
}

void XGM::displayMultiChoices()
{
	printf_tab("E - Evaluate each XGM individually\n");
	printf_tab("W - Write all XGMs included to readable .txts\n");
}

void XGM::displayMultiHelp()
{

}
