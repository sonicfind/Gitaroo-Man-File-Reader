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
#include "Global_Functions.h"
#include "XG_Main.h"
using namespace std;

bool XGType::loadSingle(string filename)
{
	bool val = false;
	try
	{
		XG_Main xg(filename);
		val = xg.menu();
	}
	catch (string str)
	{
		printf("%s%s\n", g_global.tabs.c_str(), str.c_str());
		printf("%sLoad cancelled for %s.XG\n", g_global.tabs.c_str(), filename.c_str());
	}
	catch (const char* str)
	{
		printf("%s%s\n", g_global.tabs.c_str(), str);
		printf("%sLoad cancelled for %s.XG\n", g_global.tabs.c_str(), filename.c_str());
	}
	g_global.adjustTabs(0);
	return val;
}

bool XGType::loadMulti()
{
	do
	{
		GlobalFunctions::ResultType result = GlobalFunctions::ResultType::Success;
		string choices = "iw";
		if (m_files.size() > 1)
		{
			GlobalFunctions::banner(" XG Mode Selection ");
			printf("%sI - Evaluate each XG individually\n", g_global.tabs.c_str());
			printf("%sW - Write all XGs included to readable .txts\n", g_global.tabs.c_str());
			for (size_t i = 4; i < g_filetypes.size(); ++i)
			{
				if (g_filetypes[i]->m_files.size() > 0)
				{
					printf("%sN - Proceed to the next filetype (%s)\n", g_global.tabs.c_str(), g_filetypes[i]->getType());
					choices += 'n';
					break;
				}
			}
			g_global.quit = false;
			printf("%sQ - Quit Program\n", g_global.tabs.c_str());
			result = GlobalFunctions::menuChoices(choices);
		}
		switch (result)
		{
		case GlobalFunctions::ResultType::Quit:
			return true;
		case GlobalFunctions::ResultType::Help:

			break;
		case GlobalFunctions::ResultType::Success:
			if (g_global.answer.character == 'n')
				return false;
			else
			{
				++g_global;
				const char choice = g_global.answer.character;
				while (m_files.size())
				{
					try
					{
						XG_Main xg(m_files.front());
						switch (choice)
						{
						case 'i':
							if (xg.menu(m_files.size()))
							{
								--g_global;
								return false;
							}
							break;
						case 'w':
							xg.writeTxt();
						}
					}
					catch (string str)
					{
						printf("%s%s\n", g_global.tabs.c_str(), str.c_str());
						printf("%sLoad cancelled for %s.XG\n", g_global.tabs.c_str(), m_files.front().c_str());
					}
					catch (const char* str)
					{
						printf("%s%s\n", g_global.tabs.c_str(), str);
						printf("%sLoad cancelled for %s.XG\n", g_global.tabs.c_str(), m_files.front().c_str());
					}
					m_files.pop_front();
				}
				--g_global;
				g_global.quit = true;
			}
		}
	} while (!g_global.quit);
	g_global.quit = false;
	printf("%sAll provided XG files have been evaluated.\n", g_global.tabs.c_str());
	return false;
}

bool XG_Main::menu(size_t fileCount)
{
	do
	{
		GlobalFunctions::banner(" " + xg.m_shortname + ".XG - Mode Selection ");
		string choices = "sw";
		printf("%sS - Save\n", g_global.tabs.c_str());
		printf("%sW - Write %s.txt\n", g_global.tabs.c_str(), xg.m_shortname.c_str());
		if (fileCount > 1)
		{
			printf("%sN - Next XG file\n", g_global.tabs.c_str());
			choices += 'n';
		}
		if (fileCount > 0)
		{
			bool next = false;
			for (size_t i = 4; i < g_filetypes.size(); ++i)
			{
				if (g_filetypes[i]->m_files.size() > 0)
				{
					printf("%sQ - Proceed to the next filetype (%s)\n", g_global.tabs.c_str(), g_filetypes[i]->getType());
					next = true;
					break;
				}
			}
			if (!next)
				printf("%sQ - Quit Program\n", g_global.tabs.c_str());
		}
		else
			printf("%sQ - Main Menu\n", g_global.tabs.c_str());
		switch (GlobalFunctions::menuChoices(choices))
		{
		case GlobalFunctions::ResultType::Quit:
			if (!xg.m_saved)
			{
				printf("%s\n", g_global.tabs.c_str());
				printf("%sRecent changes have not been saved externally to a XG file. Which action will you take?\n", g_global.tabs.c_str());
				printf("%sS - Save XG and Exit\n", g_global.tabs.c_str());
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
						if (!xg.m_saved)
							break;
					}
					__fallthrough;
				case GlobalFunctions::ResultType::Quit:
					g_global.quit = true;
				}
			}
			else
				g_global.quit = true;
			break;
		case GlobalFunctions::ResultType::Help:
		
			break;
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
				}
				--g_global;
			}
		}
	} while (!g_global.quit);
	g_global.quit = false;
	return true;
}

void XG_Main::saveFile(bool onExit)
{
	GlobalFunctions::banner(" " + xg.m_shortname + ".XG Save Prompt ");
	string ext = "_T";
	string filename = xg.m_directory + xg.m_shortname;
	do
	{
		string choices = "a";
		if (xg.m_saved != 2)
		{
			printf("%sS - Save & Overwrite Original file\n", g_global.tabs.c_str());
			choices += 's';
		}
		printf("%sA - Save as \"%s_T.XG\"\n", g_global.tabs.c_str(), xg.m_shortname.c_str());
		printf("%sQ - Back Out\n", g_global.tabs.c_str());
		switch (GlobalFunctions::menuChoices(choices))
		{
		case GlobalFunctions::ResultType::Quit:
			return;
		case GlobalFunctions::ResultType::Success:
			if (g_global.answer.character == 's')
			{
				printf("%sSaving %s.XG\n", g_global.tabs.c_str(), xg.m_shortname.c_str());
				xg.create(filename + ".XG");
				xg.m_saved = 2;
				g_global.quit = true;
			}
			else
			{
				do
				{
					switch (GlobalFunctions::fileOverwriteCheck(filename + ext + ".XG"))
					{
					case GlobalFunctions::ResultType::Quit:
						return;
					case GlobalFunctions::ResultType::No:
						printf("%s\n", g_global.tabs.c_str());
						ext += "_T";
						break;
					default:
						xg.create(filename + ext + ".XG");
						if (!onExit)
						{
							do
							{
								printf("%sSwap loaded file to %s? [Y/N]\n", g_global.tabs.c_str(), (filename + ext + ".XG").c_str());
								switch (GlobalFunctions::menuChoices("yn"))
								{
								case GlobalFunctions::ResultType::Success:
									if (g_global.answer.character == 'y')
									{
										xg.m_shortname += ext;
										xg.m_saved = 2;
									}
									__fallthrough;
								case GlobalFunctions::ResultType::Quit:
									g_global.quit = true;
								}
							} while (!g_global.quit);
						}
					}
				} while (!g_global.quit);
			}
		}
	} while (!g_global.quit);
	g_global.quit = false;
}

void XG_Main::writeTxt()
{
	GlobalFunctions::banner(" Writing " + xg.m_shortname + "_XG.txt ");
	FILE* outSimpleTXT;
	if (fopen_s(&outSimpleTXT, (xg.m_directory + xg.m_shortname + "_XG_SIMPLIFIED.txt").c_str(), "w"))
		printf("%s%s Could not be created.\n", g_global.tabs.c_str(), (xg.m_directory + xg.m_shortname + "_XG_SIMPLIFIED.txt").c_str());
	else
	{
		fprintf_s(outSimpleTXT, "# of Nodes: %zu\n", xg.m_data->m_nodes.size());
		for (size_t i = 0; i < xg.m_data->m_nodes.size(); ++i)
			fprintf_s(outSimpleTXT, "\t Node %03zu - %s\n", i + 1, xg.m_data->m_nodes[i]->m_name.m_pstring);
		fclose(outSimpleTXT);
	}

	FILE* outTXT;
	if (fopen_s(&outTXT, (xg.m_directory + xg.m_shortname + "_XG.txt").c_str(), "w"))
		printf("%s%s Could not be created.\n", g_global.tabs.c_str(), (xg.m_directory + xg.m_shortname + "_XG.txt").c_str());
	else
	{
		fprintf_s(outTXT, "# of Nodes: %zu\n", xg.m_data->m_nodes.size());
		for (size_t i = 0; i < xg.m_data->m_nodes.size(); ++i)
		{
			shared_ptr<XGNode>& node = xg.m_data->m_nodes[i];
			fprintf_s(outTXT, "\t Node %03zu - %s: %s\n", i + 1, node->getType(), node->m_name.m_pstring);
			node->writeTXT(outTXT);
		}
		fclose(outTXT);
	}
}

void XG_Main::writeTxt(FILE* outTXT, FILE* outSimpleTXT)
{
	fprintf_s(outTXT, "\t\t            # of Nodes: %zu\n", xg.m_data->m_nodes.size());
	fprintf_s(outSimpleTXT, "\t\t       # of Nodes: %zu\n", xg.m_data->m_nodes.size());
	for (size_t index = 0; index < xg.m_data->m_nodes.size(); index++)
	{
		std::shared_ptr<XGNode>& node = xg.m_data->m_nodes[index];
		GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t\t\t\t     Node %03zu - %s: %s\n", index + 1, node->getType(), node->m_name.m_pstring, 17);
		node->writeTXT(outTXT, "\t\t\t    ");
		fflush(outTXT);
	}
}
