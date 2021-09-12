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
#include "FileType.h"
using namespace GlobalFunctions;
const std::string FileType::multiChoiceString;

FileType::FileType(const char* extension, const bool saved)
	: m_extension(extension)
	, m_saved(saved) {}


FileType::FileType(std::string filename, const char* extension)
	: FileType(extension, true)
{
	size_t pos = filename.find_last_of('\\');
	if (pos != std::string::npos)
	{
		m_directory = filename.substr(0, pos + 1);
		m_filename = filename.substr(pos + 1);
	}
	else
		m_filename = filename;

	banner(" Loading " + m_filename + m_extension + ' ');

	if (fopen_s(&m_filePtr, (filename + m_extension).c_str(), "rb"))
		throw "Error: " + filename + m_extension + " does not exist.";
}

bool FileType::create(std::string filename)
{
	size_t pos = filename.find_last_of('\\');
	GlobalFunctions::banner(" Saving " + filename.substr(pos != std::string::npos ? pos + 1 : 0) + m_extension + ' ');

	if (fopen_s(&m_filePtr, (filename + m_extension).c_str(), "wb"))
		// Add printf later
		return false;
	else
		return true;
}

bool FileType::create_bannerless(std::string filename)
{
	if (fopen_s(&m_filePtr, (filename + m_extension).c_str(), "wb"))
		// Add printf later
		return false;
	else
		return true;
}

bool FileType::checkSave(bool toMainMenu)
{
	if (!m_saved)
	{
		printf_tab("\n");
		printf_tab("Recent changes have not been saved externally to a %s file. Which action will you take?\n", m_extension.c_str());

		if (toMainMenu)
		{
			printf_tab("S - Save file and Exit\n");
			printf_tab("Q - Exit without saving\n");
		}
		else
		{
			printf_tab("S - Save file and Exit\n");
			printf_tab("Q - Skip to the next file without saving\n");
		}
		printf_tab("C - Cancel\n");

		switch (menuChoices("sc"))
		{
		case ResultType::Success:
			if (g_global.answer.character == 'c'
				|| !fileSavePrompt())
				return false;
			__fallthrough;
		case ResultType::Quit:
			return true;
		default:
			return false;
		}
	}
	return true;
}

bool FileType::fileSavePrompt()
{
	banner(" " + m_filename + m_extension + " - Save Prompt ");
	std::string ext = "_T";
	const std::string filename = m_directory + m_filename;
	do
	{
		std::string choices = "a";
		if (!m_saved)
		{
			printf_tab("S - Save & Overwrite %s\n", (m_filename + m_extension).c_str());
			choices += 's';
		}
		printf_tab("A - Save as \"%s\"\n", (m_filename + ext + m_extension).c_str());
		printf_tab("Q - Back Out\n");
		switch (menuChoices(choices))
		{
		case ResultType::Quit:
			return false;
		case ResultType::Success:
			switch (g_global.answer.character)
			{
			case 's':
				if (create(filename))
					g_global.quit = true;
				break;
			case 'a':
				do
				{
					switch (fileOverwriteCheck(filename + ext + m_extension))
					{
					case ResultType::Quit:
						return false;
					case ResultType::No:
						ext += "_T";
						break;
					default:
						create(filename + ext);
						m_filename += ext;
						g_global.quit = true;
					}
				} while (!g_global.quit);
			}
		}
	} while (!g_global.quit);
	g_global.quit = false;
	return m_saved;
}

bool FileType::write_to_txt(FILE*& txtFile)
{
	GlobalFunctions::banner(" Writing " + m_filename + '_' + m_extension.substr(1) + ".txt ");
	return !fopen_s(&txtFile, (m_directory + m_filename + '_' + m_extension.substr(1) + ".txt").c_str(), "w");
}

bool FileType::write_to_txt(FILE*& txtFile, FILE*& simpleTxtFile)
{
	write_to_txt(txtFile);
	return !fopen_s(&simpleTxtFile, (m_directory + m_filename + '_' + m_extension.substr(1) + "_SIMPLIFIED.txt").c_str(), "w")
		|| txtFile != nullptr;
}
