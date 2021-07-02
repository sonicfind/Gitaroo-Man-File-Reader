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
#include "FileMain.h"
using namespace GlobalFunctions;

bool AbstractMain::doesContainFiles() { return m_filenames.size() > 0; }

bool AbstractMain::saveFile(bool onExit)
{
	banner(" " + m_file->m_filename + m_file->m_extension + " Save Prompt ");
	std::string ext = "_T";
	const std::string filename = m_file->m_directory + m_file->m_filename;
	do
	{
		std::string choices = "a";
		if (!m_file->getSaveStatus())
		{
			printf_tab("S - Save & Overwrite %s\n", (m_file->m_filename + m_file->m_extension).c_str());
			choices += 's';
		}
		printf_tab("A - Save as \"%s\"\n", (m_file->m_filename + ext + m_file->m_extension).c_str());
		printf_tab("Q - Back Out\n");
		switch (menuChoices(choices))
		{
		case ResultType::Quit:
			return false;
		case ResultType::Success:
			switch (g_global.answer.character)
			{
			case 's':
				if (m_file->create(filename))
					g_global.quit = true;
				break;
			case 'a':
				do
				{
					switch (fileOverwriteCheck(filename + ext + m_file->m_extension))
					{
					case ResultType::Quit:
						return false;
					case ResultType::No:
						ext += "_T";
						break;
					default:
						m_file->create(filename + ext);
						m_file->m_filename += ext;
						g_global.quit = true;
					}
				} while (!g_global.quit);
			}
		}
	} while (!g_global.quit);
	g_global.quit = false;
	return m_file->getSaveStatus();
}

#include "CHC/CHC_Main.cpp"

FileMainList::FileMainList()
{
	m_mains[0] = new FileMain<CHC>;
}

FileMainList::~FileMainList()
{
	for (AbstractMain* main : m_mains)
		delete main;
}

bool FileMainList::compareExtensions(const std::string& filename, const bool openSingleMenu)
{
	FILE* test;
	//If the file is found
	if (!fopen_s(&test, filename.c_str(), "r") && test)
	{
		fclose(test);
		for (size_t index = 0; index < sizeof(m_mains) / sizeof(AbstractMain*); ++index)
			if (m_mains[index]->compareExtension(filename))
			{
				if (openSingleMenu)
					m_mains[index]->singleFile(std::pair<bool, const char*>(false, ""));
				return true;
			}

		printf_tab("\"%s\" is not a valid extension.\n", filename.substr(filename.find_last_of('.')).c_str());
		clearIn();
		return false;
	}
	else
	{
		size_t pos = filename.find_last_of('\\');
		printf_tab("Could not locate the file \"%s\".\n", filename.substr(pos != std::string::npos ? pos + 1 : 0).c_str());
		return false;
	}
}

bool FileMainList::testAllExtensions(const std::string& filename, const bool openSingleMenu)
{
	bool found = false;
	for (AbstractMain* main : m_mains)
		if (main->testExtension(filename))
		{
			if (openSingleMenu)
				main->singleFile(std::pair<bool, const char*>(false, ""));
			found = true;
		}

	if (!found)
	{
		size_t pos = filename.find_last_of('\\');
		printf_tab("Could not locate a file with the file name \"%s\" using any of the accepted extensions\n"
			, filename.substr(pos != std::string::npos ? pos + 1 : 0).c_str());
	}
	return found;
}

bool FileMainList::allFiles()
{
	const size_t max = sizeof(m_mains) / sizeof(AbstractMain*);
	for (size_t index = 0; index < max; ++index)
	{
		const bool result = index + 1 < max && m_mains[index + 1]->m_filenames.size();
		if (m_mains[index]->multipleFiles(std::pair<bool, const char*>(result, result ? m_mains[index + 1]->extension().c_str() : "")))
			return true;
	}
	return false;
}
