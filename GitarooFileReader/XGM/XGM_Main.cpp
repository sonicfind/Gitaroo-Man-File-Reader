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
#include "XGM_Main.h"
#include "IMX/IMX_Main.h"
#include "XG/XG_Main.h"
using namespace std;

bool XGMType::loadSingle(string filename)
{
	bool val = false;
	try
	{
		XGM_Main xgm(filename);
		val = xgm.menu();
	}
	catch (string str)
	{
		printf("%s%s\n", g_global.tabs.c_str(), str.c_str());
		printf("%sLoad cancelled for %s.XGM\n", g_global.tabs.c_str(), filename.c_str());
	}
	catch (const char* str)
	{
		printf("%s%s\n", g_global.tabs.c_str(), str);
		printf("%sLoad cancelled for %s.XGM\n", g_global.tabs.c_str(), filename.c_str());
	}
	g_global.adjustTabs(0);
	return val;
}

bool XGMType::loadMulti()
{
	do
	{
		GlobalFunctions::ResultType result = GlobalFunctions::ResultType::Success;
		string choices = "iwem";
		if (m_files.size() > 1)
		{
			GlobalFunctions::banner(" XGM Mode Selection ");
			printf("%sI - Evaluate each XGM individually\n", g_global.tabs.c_str());
			printf("%sW - Write all XGMs included to readable .txts\n", g_global.tabs.c_str());
			printf("%sE - Export all textures from every XGM to image files [Gitarootools install required]\n", g_global.tabs.c_str());
			printf("%sM - Import textures from image files into all XGMs [Gitarootools install required]\n", g_global.tabs.c_str());
			for (size_t i = 2; i < g_filetypes.size(); ++i)
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
						XGM_Main xgm(m_files.front());
						switch (choice)
						{
						case 'i':
							if (xgm.menu(m_files.size()))
							{
								--g_global;
								return false;
							}
							break;
						case 'w':
							xgm.writeTxt();
							break;
						case 'e':
							xgm.exportPNGs();
							break;
						case 'm':
							xgm.importPNGs();
						}
					}
					catch (string str)
					{
						printf("%s%s\n", g_global.tabs.c_str(), str.c_str());
						printf("%sLoad cancelled for %s.XGM\n", g_global.tabs.c_str(), m_files.front().c_str());
					}
					catch (const char* str)
					{
						printf("%s%s\n", g_global.tabs.c_str(), str);
						printf("%sLoad cancelled for %s.XGM\n", g_global.tabs.c_str(), m_files.front().c_str());
					}
					m_files.pop_front();
				}
				--g_global;
				g_global.quit = true;
			}
		}
	} while (!g_global.quit);
	g_global.quit = false;
	printf("%sAll provided XGM files have been evaluated.\n", g_global.tabs.c_str());
	return false;
}

bool XGM_Main::menu(size_t fileCount)
{
	do
	{
		GlobalFunctions::banner(" " + xgm.m_shortname + ".XGM - Mode Selection ");
		string choices = "sweitm";
		printf("%sS - Save\n", g_global.tabs.c_str());
		printf("%sW - Write %s.txt\n", g_global.tabs.c_str(), xgm.m_shortname.c_str());
		printf("%sE - Export textures to image files [Gitarootools install required]\n", g_global.tabs.c_str());
		printf("%sI - Import textures from image files [Gitarootools install required]\n", g_global.tabs.c_str());
		printf("%sT - Select a texture [Count: %zu]\n", g_global.tabs.c_str(), xgm.m_textures.size());
		printf("%sM - Select a model   [Count: %zu]\n", g_global.tabs.c_str(), xgm.m_models.size());
		if (fileCount > 1)
		{
			printf("%sN - Next XGM file\n", g_global.tabs.c_str());
			choices += 'n';
		}
		if (fileCount > 0)
		{
			bool next = false;
			for (size_t i = 2; i < g_filetypes.size(); ++i)
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
			if (!xgm.m_saved)
			{
				printf("%s\n", g_global.tabs.c_str());
				printf("%sRecent changes have not been saved externally to an XGM file. Which action will you take?\n", g_global.tabs.c_str());
				printf("%sS - Save XGM and Exit\n", g_global.tabs.c_str());
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
						if (!xgm.m_saved)
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
					break;
				case 'e':
					exportPNGs();
					break;
				case 'i':
					importPNGs();
					break;
				case 't':
					do
					{
						GlobalFunctions::banner(" " + xgm.m_shortname + ".XGM - Texture Selection ");
						switch (GlobalFunctions::indexSelector(xgm.m_textures, "texture"))
						{
						case GlobalFunctions::ResultType::Success:
						{
							++g_global;
							size_t index = g_global.answer.index;
							IMX_Main texture(xgm.m_textures[index]);
							texture.menu();
							if (texture.imx.m_saved != xgm.m_textures[index].m_saved)
								xgm.m_saved = 0;
							xgm.m_textures[index] = texture.imx;
							--g_global;
							break;
						}
						case GlobalFunctions::ResultType::Quit:
							g_global.quit = true;
							break;
						}
					} while (!g_global.quit);
					g_global.quit = false;
					break;
				case 'm':
					do
					{
						GlobalFunctions::banner(" " + xgm.m_shortname + ".XGM - Model Selection ");
						switch (GlobalFunctions::indexSelector(xgm.m_models, "model"))
						{
						case GlobalFunctions::ResultType::Success:
						{
							++g_global;
							size_t index = g_global.answer.index;
							XG_Main model(xgm.m_models[index]);
							model.menu();
							--g_global;
						}
							__fallthrough;
						case GlobalFunctions::ResultType::Quit:
							g_global.quit = true;
							break;
						}
					} while (!g_global.quit);
					g_global.quit = false;
					break;
				case 'n':
					return false;
				}
				--g_global;
			}
		}
	} while (!g_global.quit);
	g_global.quit = false;
	return true;
}

void XGM_Main::saveFile(bool onExit)
{
	GlobalFunctions::banner(" " + xgm.m_shortname + ".XGM Save Prompt ");
	string m_ext = "_T";
	string filename = xgm.m_name.substr(0, xgm.m_name.length() - 4);
	do
	{
		string choices = "a";
		if (xgm.m_saved != 2)
		{
			printf("%sS - Save & Overwrite Original file\n", g_global.tabs.c_str());
			choices += 's';
		}
		printf("%sA - Save as \"%s_T.XGM\"\n", g_global.tabs.c_str(), xgm.m_shortname.c_str());
		printf("%sQ - Back Out\n", g_global.tabs.c_str());
		switch (GlobalFunctions::menuChoices(choices))
		{
		case GlobalFunctions::ResultType::Quit:
			return;
		case GlobalFunctions::ResultType::Success:
			if (g_global.answer.character == 's')
			{
				printf("%sSaving %s.XGM\n", g_global.tabs.c_str(), xgm.m_shortname.c_str());
				xgm.create(xgm.m_name);
				g_global.quit = true;
				xgm.m_saved = 2;
			}
			else
			{
				do
				{
					switch (GlobalFunctions::fileOverwriteCheck(filename + m_ext + ".XGM"))
					{
					case GlobalFunctions::ResultType::Quit:
						return;
					case GlobalFunctions::ResultType::No:
						printf("%s\n", g_global.tabs.c_str());
						m_ext += "_T";
						break;
					default:
						xgm.create(filename + m_ext + ".XGM");
						if (!onExit)
						{
							do
							{
								printf("%sSwap loaded file to %s.XGM? [Y/N]\n", g_global.tabs.c_str(), (filename + m_ext).c_str());
								switch (GlobalFunctions::menuChoices("yn"))
								{
								case GlobalFunctions::ResultType::Success:
									if (g_global.answer.character == 'y')
									{
										xgm.m_name = filename + m_ext + ".XGM";
										xgm.m_shortname += m_ext;
										xgm.m_saved = 2;
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

void XGM_Main::writeTxt()
{
	GlobalFunctions::banner(" Writing " + xgm.m_shortname + "_XGM.txt ");
	FILE* outTXT, * outSimpleTXT;
	fopen_s(&outTXT, (xgm.m_name.substr(0, xgm.m_name.length() - 4) + "_XGM.txt").c_str(), "w");
	fopen_s(&outSimpleTXT, (xgm.m_name.substr(0, xgm.m_name.length() - 4) + "_XGM_SIMPLIFIED.txt").c_str(), "w");
	GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "# of Textures: %zu\n", xgm.m_textures.size());
	GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "  # of Models: %zu\n", xgm.m_models.size());
	GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "    Textures (.IMX):\n");
	for (size_t index = 0, sizes = 0; index < xgm.m_textures.size(); index++)
	{
		IMX& imx = xgm.m_textures[index];
		//IMX Header
		GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t   Texture %03zu - %s:\n", index + 1, imx.m_name, 17);
		fprintf_s(outTXT, "\t\t    Original File Path: %s\n", imx.m_filepath);
		fprintf_s(outTXT, "\t\t\t     File Size: %lu bytes\n", imx.m_fileSize);
		fprintf_s(outSimpleTXT, "\t\t\tFile Size: %lu bytes\n", imx.m_fileSize);
		fprintf_s(outTXT, "\t\tPrevious File Size Sum: %zu bytes\n", sizes);
		sizes += imx.m_fileSize;
		fprintf_s(outTXT, "\t\t   Used for Non-Models: %s\n", (imx.m_non_model ? "TRUE" : "FALSE"));
		fprintf_s(outTXT, "\t\t    Unknown Bool Value: %s\n", (imx.m_unk ? "TRUE" : "FALSE"));
		//IMX Data
		IMX_Main i_Main(imx);
		i_Main.writeTxt(outTXT, outSimpleTXT);
		fflush(outTXT);
		fflush(outSimpleTXT);
	}
	GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "    Models (.XG):\n");
	for (size_t index = 0; index < xgm.m_models.size(); index++)
	{
		XG& xg = xgm.m_models[index];
		//XG Header
		GlobalFunctions::dualvfprintf_s(outTXT, outSimpleTXT, "\t   Model %03lu - %s:\n", index + 1, xg.m_name, 17);
		fprintf_s(outTXT, "\t\t    Original File Path: %s\n", xg.m_filepath);
		fprintf_s(outTXT, "\t\t\t     File Size: %lu bytes\n", xg.m_fileSize);
		fprintf_s(outSimpleTXT, "\t\t\tFile Size: %lu bytes\n", xg.m_fileSize);
		fprintf_s(outTXT, "\t\t       # of Animations: %zu\n", xg.m_animations.size());
		fprintf_s(outSimpleTXT, "\t\t  # of Animations: %zu\n", xg.m_animations.size());
		for (size_t index2 = 0; index2 < xg.m_animations.size(); index2++)
		{
			fprintf_s(outTXT, "\t\t\t    Animation Entry %02zu\n", index2 + 1);
			XG::Animation& anim = xg.m_animations[index2];
			fprintf_s(outTXT, "\t\t\t\t      Playback Length: %g\n", anim.m_length);
			fprintf_s(outTXT, "\t\t\t\t    Keyframe Interval: %g\n", anim.m_keyframe_interval);
			fprintf_s(outTXT, "\t\t\t\t\t    Framerate: %g\n", anim.m_framerate);
			fprintf_s(outTXT, "\t\t\t\t    Starting Keyframe: %g\n", anim.m_starting_keyframe);
			fprintf_s(outTXT, "\t\t\t\t     Not Tempo Linked: %s\n", (anim.m_non_tempo ? "TRUE" : "FALSE"));
		}
		//XG Data
		XG_Main x_Main(xg);
		x_Main.writeTxt(outTXT, outSimpleTXT);
		fflush(outSimpleTXT);
		// outTXT already flushed
	}
	fclose(outTXT);
	fclose(outSimpleTXT);
}

bool XGM_Main::exportPNGs()
{
	GlobalFunctions::banner(" " + xgm.m_shortname + " - Multi-Texture Export ");
	return xgm.exportPNGs();
}

bool XGM_Main::importPNGs()
{
	GlobalFunctions::banner(" " + xgm.m_shortname + " - Multi-Texture Import ");
	return xgm.importPNGs();
}
