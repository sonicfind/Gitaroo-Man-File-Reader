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
#include "CHC/CHC.h"
#include "XGM/XGM.h"

bool AbstractMain::doesContainFiles() { return m_filenames.size() > 0; }

FileMainList::FileMainList()
{
	m_mains[0] = new FileMain<CHC>;
	m_mains[1] = new FileMain<IMX>;
	m_mains[2] = new FileMain<XG>;
	m_mains[3] = new FileMain<XGM>;
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
					m_mains[index]->singleFile();
				return true;
			}

		GlobalFunctions::printf_tab("\"%s\" is not a valid extension.\n", filename.substr(filename.find_last_of('.')).c_str());
		GlobalFunctions::clearIn();
		return false;
	}
	else
	{
		size_t pos = filename.find_last_of('\\');
		GlobalFunctions::printf_tab("Could not locate the file \"%s\".\n", filename.substr(pos != std::string::npos ? pos + 1 : 0).c_str());
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
				main->singleFile();
			found = true;
		}

	if (!found)
	{
		size_t pos = filename.find_last_of('\\');
		GlobalFunctions::printf_tab("Could not locate a file with the file name \"%s\" using any of the accepted extensions\n"
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
