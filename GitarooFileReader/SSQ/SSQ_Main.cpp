#pragma once
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
#include "SSQ_Main.h"
using namespace std;

bool SSQType::loadSingle(string filename)
{
	bool val = false;
	try
	{
		SSQ_Main ssq(filename);
		val = ssq.menu();
	}
	catch (string str)
	{
		printf("%s%s\n", g_global.tabs.c_str(), str.c_str());
		printf("%sLoad cancelled for %s.SSQ\n", g_global.tabs.c_str(), filename.c_str());
	}
	catch (const char* str)
	{
		printf("%s%s\n", g_global.tabs.c_str(), str);
		printf("%sLoad cancelled for %s.SSQ\n", g_global.tabs.c_str(), filename.c_str());
	}
	g_global.adjustTabs(0);
	return val;
}

bool SSQType::loadMulti()
{
	do
	{
		GlobalFunctions::ResultType result = GlobalFunctions::ResultType::Success;
		string choices = "ew";
		if (m_files.size() > 1)
		{
			GlobalFunctions::banner(" XG Mode Selection ");
			printf("%sE - Evaluate each SSQ individually\n", g_global.tabs.c_str());
			printf("%sW - Write all SSQs included to readable .txts\n", g_global.tabs.c_str());
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
						SSQ_Main ssq(m_files.front());
						switch (choice)
						{
						case 'e':
							if (ssq.menu(m_files.size()))
							{
								--g_global;
								return false;
							}
							break;
						case 'w':
							ssq.writeTxt();
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
	printf("%sAll provided SSQ files have been evaluated.\n", g_global.tabs.c_str());
	return false;
}

bool SSQ_Main::menu(size_t fileCount)
{
	do
	{
		GlobalFunctions::banner(" " + m_ssq.m_shortname + ".SSQ - Mode Selection ");
		string choices = "sw";
		printf("%sS - Save\n", g_global.tabs.c_str());
		printf("%sW - Write %s_SSQ.txt\n", g_global.tabs.c_str(), m_ssq.m_shortname.c_str());
		if (fileCount > 1)
		{
			printf("%sN - Next SSQ file\n", g_global.tabs.c_str());
			choices += 'n';
		}
		if (fileCount > 0)
		{
			bool next = false;
			for (size_t i = 5; i < g_filetypes.size(); ++i)
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
			if (!m_ssq.m_saved)
			{
				printf("%s\n", g_global.tabs.c_str());
				printf("%sRecent changes have not been saved externally to a SSQ file. Which action will you take?\n", g_global.tabs.c_str());
				printf("%sS - Save SSQ and Exit\n", g_global.tabs.c_str());
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
						if (!m_ssq.m_saved)
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

void SSQ_Main::saveFile(bool onExit)
{
	GlobalFunctions::banner(" " + m_ssq.m_shortname + ".SSQ Save Prompt ");
	string ext = "_T";
	string filename = m_ssq.m_filename.substr(0, m_ssq.m_filename.length() - 4);
	do
	{
		string choices = "a";
		if (m_ssq.m_saved != 2)
		{
			printf("%sS - Save & Overwrite %s.SSQ\n", g_global.tabs.c_str(), m_ssq.m_shortname.c_str());
			choices += 's';
		}
		printf("%sA - Save as \"%s_T.SSQ\"\n", g_global.tabs.c_str(), m_ssq.m_shortname.c_str());
		printf("%sQ - Back Out\n", g_global.tabs.c_str());
		switch (GlobalFunctions::menuChoices(choices))
		{
		case GlobalFunctions::ResultType::Quit:
			return;
		case GlobalFunctions::ResultType::Success:
			switch (g_global.answer.character)
			{
			case 's':
				printf("%sSaving %s.SSQ\n", g_global.tabs.c_str(), m_ssq.m_shortname.c_str());
				m_ssq.create(m_ssq.m_filename);
				m_ssq.m_saved = 2;
				g_global.quit = true;
				break;
			case 'a':
				do
				{
					switch (GlobalFunctions::fileOverwriteCheck(filename + ext + ".SSQ"))
					{
					case GlobalFunctions::ResultType::Quit:
						return;
					case GlobalFunctions::ResultType::No:
						printf("%s\n", g_global.tabs.c_str());
						ext += "_T";
						break;
					default:
						m_ssq.create(filename + ext + ".SSQ");
						if (!onExit)
						{
							do
							{
								printf("%sSwap loaded file to %s.SSQ? [Y/N]\n", g_global.tabs.c_str(), (filename + ext).c_str());
								switch (GlobalFunctions::menuChoices("yn"))
								{
								case GlobalFunctions::ResultType::Success:
									if (g_global.answer.character == 'y')
									{
										m_ssq.m_filename = filename + ext + ".SSQ";
										m_ssq.m_shortname += ext;
										m_ssq.m_saved = 2;
									}
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
