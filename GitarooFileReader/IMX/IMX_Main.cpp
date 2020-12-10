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
#include "IMX_Main.h"
using namespace std;

bool IMXType::loadSingle(string filename)
{
	bool val = false;
	try
	{
		IMX_Main imx(filename);
		val = imx.menu();
	}
	catch (string str)
	{
		printf("%s%s\n", g_global.tabs.c_str(), str.c_str());
		printf("%sLoad cancelled for %s.IMX\n", g_global.tabs.c_str(), filename.c_str());
	}
	catch (const char* str)
	{
		printf("%s%s\n", g_global.tabs.c_str(), str);
		printf("%sLoad cancelled for %s.IMX\n", g_global.tabs.c_str(), filename.c_str());
	}
	g_global.adjustTabs(0);
	return val;
}

bool IMXType::loadMulti()
{
	do
	{
		GlobalFunctions::ResultType result = GlobalFunctions::ResultType::Success;
		string choices = "tw";
		if (m_files.size() > 1)
		{
			GlobalFunctions::banner(" IMX Mode Selection ");
			printf("%sT - Evaluate each texture individually\n", g_global.tabs.c_str());
			printf("%sW - Write out all textures included to readable .txts\n", g_global.tabs.c_str());
			for (size_t i = 3; i < g_filetypes.size(); ++i)
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
		else
			g_global.answer.character = 't';
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
						IMX_Main imx(m_files.front());
						switch (choice)
						{
						case 't':
							if (imx.menu(m_files.size()))
							{
								--g_global;
								return false;
							}
							break;
						case 'w':
							imx.writeTxt();
						}
					}
					catch (string str)
					{
						printf("%s%s\n", g_global.tabs.c_str(), str.c_str());
						printf("%sLoad cancelled for %s.IMX\n", g_global.tabs.c_str(), m_files.front().c_str());
					}
					catch (const char* str)
					{
						printf("%s%s\n", g_global.tabs.c_str(), str);
						printf("%sLoad cancelled for %s.IMX\n", g_global.tabs.c_str(), m_files.front().c_str());
					}
					m_files.pop_front();
				}
				--g_global;
				g_global.quit = true;
			}
		}
	} while (!g_global.quit);
	g_global.quit = false;
	printf("%sAll provided IMX files have been evaluated.\n", g_global.tabs.c_str());
	return false;
}

bool IMX_Main::menu(size_t fileCount)
{
	do
	{
		GlobalFunctions::banner(" " + imx.m_shortname + ".IMX - Mode Selection "); string choices = "sw";
		printf("%sS - Save\n", g_global.tabs.c_str());
		printf("%sW - Write %s.txt\n", g_global.tabs.c_str(), imx.m_shortname.c_str());
		if (fileCount > 1)
		{
			printf("%sN - Next IMX file\n", g_global.tabs.c_str());
			choices += 'n';
		}
		if (fileCount > 0)
		{
			bool next = false;
			for (size_t i = 3; i < g_filetypes.size(); ++i)
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
			if (!imx.m_saved)
			{
				printf("%s\n", g_global.tabs.c_str());
				printf("%sRecent changes have not been saved externally to an IMX file. Which action will you take?\n", g_global.tabs.c_str());
				printf("%sS - Save IMX and Exit\n", g_global.tabs.c_str());
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
						if (!imx.m_saved)
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

void IMX_Main::saveFile(bool onExit)
{
	GlobalFunctions::banner(" " + imx.m_shortname + ".IMX Save Prompt ");
	string ext = "_T";
	string filename = imx.m_directory + imx.m_shortname;
	do
	{
		string choices = "a";
		if (imx.m_saved != 2)
		{
			printf("%sS - Save & Overwrite Original file\n", g_global.tabs.c_str());
			choices += 's';
		}
		printf("%sA - Save as \"%s_T.IMX\"\n", g_global.tabs.c_str(), imx.m_shortname.c_str());
		printf("%sQ - Back Out\n", g_global.tabs.c_str());
		switch (GlobalFunctions::menuChoices(choices))
		{
		case GlobalFunctions::ResultType::Quit:
			return;
		case GlobalFunctions::ResultType::Success:
			if (g_global.answer.character == 's')
			{
				printf("%sSaving %s.IMX\n", g_global.tabs.c_str(), imx.m_shortname.c_str());
				imx.create(filename + ".IMX");
				if (!imx.m_fromXGM)
					imx.m_saved = 2;
				g_global.quit = true;
			}
			else
			{
				do
				{
					switch (GlobalFunctions::fileOverwriteCheck(filename + ext + ".IMX"))
					{
					case GlobalFunctions::ResultType::Quit:
						return;
					case GlobalFunctions::ResultType::No:
						printf("%s\n", g_global.tabs.c_str());
						ext += "_T";
						break;
					default:
						imx.create(filename + ext + ".IMX");
						if (!onExit)
						{
							do
							{
								printf("%sSwap loaded file to %s? [Y/N]\n", g_global.tabs.c_str(), (filename + ext + ".IMX").c_str());
								switch (GlobalFunctions::menuChoices("yn"))
								{
								case GlobalFunctions::ResultType::Success:
									if (g_global.answer.character == 'y')
									{
										imx.m_shortname += ext;
										imx.m_saved = 2;
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

void IMX_Main::writeTxt()
{
	GlobalFunctions::banner(" Writing " + imx.m_shortname + "_IMX.txt ");
	FILE* outTXT;
	fopen_s(&outTXT, (imx.m_directory + imx.m_shortname + "_IMX.txt").c_str(), "w");
	fprintf_s(outTXT, "       Image Width: %lu pixels\n", imx.m_data->m_width);
	fprintf_s(outTXT, "      Image Height: %lu pixels\n", imx.m_data->m_height);
	fputs("Pixel Storage Mode: ", outTXT);
	if ((imx.m_data->m_pixelVal1 == 0 && imx.m_data->m_pixelVal2 == 0) || (imx.m_data->m_pixelVal1 == 1 && imx.m_data->m_pixelVal2 == 1))
	{
		if (imx.m_data->m_pixelVal1 == 0 && imx.m_data->m_pixelVal2 == 0)
			fputs("4-bit indexed (each byte is 2 pixels, lower 4 bits come first), RGBA 8888 palette\n", outTXT);
		else
			fputs("8-bit indexed, RGBA 8888 palette\n", outTXT);
		fprintf_s(outTXT, "\t    Palette Size: %lu bytes (%lu colors)\n", imx.m_data->m_colorData->m_paletteSize, imx.m_data->m_colorData->m_paletteSize >> 2);
		fputs("\t\t Palette: R  | G  | B  | A\n", outTXT);
		fputs("\t\t         -------------------\n", outTXT);
		for (unsigned char* palette = imx.m_data->m_colorData->m_palette[0], *end = palette + imx.m_data->m_colorData->m_paletteSize;
			palette < end;)
			fprintf_s(outTXT, "\t\t\t  %02x | %02x | %02x | %02x\n", *(palette++), *(palette++), *(palette++), *(palette++));
	}
	else if (imx.m_data->m_pixelVal1 == 3 && imx.m_data->m_pixelVal2 == 2)
		fputs("24-bit RGB 888\n", outTXT);
	else if (imx.m_data->m_pixelVal1 == 4 && imx.m_data->m_pixelVal2 == 2)
		fputs("32-bit RGBA 8888\n", outTXT);
	else
		fputs("Unknown\n", outTXT);
	fprintf_s(outTXT, "       Image Data Size: %lu\n", imx.m_data->m_colorData->m_imageSize);
	fclose(outTXT);
}

void IMX_Main::writeTxt(FILE* outTXT, FILE* outSimpleTXT)
{
	fprintf_s(outTXT, "\t\t\t   Image Width: %lu pixels\n", imx.m_data->m_width);
	fprintf_s(outSimpleTXT, "\t\t      Image Width: %lu pixels\n", imx.m_data->m_width);
	fprintf_s(outTXT, "\t\t\t  Image Height: %lu pixels\n", imx.m_data->m_height);
	fprintf_s(outSimpleTXT, "\t\t     Image Height: %lu pixels\n", imx.m_data->m_height);
	fputs("\t\t    Pixel Storage Mode: ", outTXT);
	if ((imx.m_data->m_pixelVal1 == 0 && imx.m_data->m_pixelVal2 == 0) || (imx.m_data->m_pixelVal1 == 1 && imx.m_data->m_pixelVal2 == 1))
	{
		if (imx.m_data->m_pixelVal1 == 0 && imx.m_data->m_pixelVal2 == 0)
			fputs("4-bit indexed (each byte is 2 pixels, lower 4 bits come first), RGBA 8888 palette\n", outTXT);
		else
			fputs("8-bit indexed, RGBA 8888 palette\n", outTXT);
		fprintf_s(outTXT, "\t\t\t\tPalette Size: %lu bytes (%lu colors)\n", imx.m_data->m_colorData->m_paletteSize, imx.m_data->m_colorData->m_paletteSize >> 2);
		fputs("\t\t\t\t     Palette: R  | G  | B  | A\n", outTXT);
		fputs("\t\t\t\t             -------------------\n", outTXT);
		for (unsigned char* palette = imx.m_data->m_colorData->m_palette[0], *end = palette + imx.m_data->m_colorData->m_paletteSize;
			palette < end;)
			fprintf_s(outTXT, "\t\t\t\t\t      %02x | %02x | %02x | %02x\n", *(palette++), *(palette++), *(palette++), *(palette++));
	}
	else if (imx.m_data->m_pixelVal1 == 3 && imx.m_data->m_pixelVal2 == 2)
		fputs("24-bit RGB 888\n", outTXT);
	else if (imx.m_data->m_pixelVal1 == 4 && imx.m_data->m_pixelVal2 == 2)
		fputs("32-bit RGBA 8888\n", outTXT);
	else
		fputs("Unknown\n", outTXT);
	fprintf_s(outTXT, "\t\t       Image Data Size: %lu\n", imx.m_data->m_colorData->m_imageSize);
	fprintf_s(outSimpleTXT, "\t\t  Image Data Size: %lu\n", imx.m_data->m_colorData->m_imageSize);
}
