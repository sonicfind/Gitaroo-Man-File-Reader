#pragma once
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
#include "FileMain.h"
#include "CHC/CHC.h"

struct FileMainList
{
	FileMain<CHC> m_CHC;

	template<typename T>
	bool checkFilenameLists(const FileMain<T>* fileMain, int index)
	{
		bool addNextFileChoice = false;
		if (fileMain->m_filenames.size() > 1)
		{
			printf("%sN - Next .%s file\n", g_global.tabs.c_str(), typeid(T).name());
			addNextFileChoice = true;
		}

		switch (index)
		{
		case 0:
			//case NEXT_MAIN_HERE
			//if (NEXT_MAIN_HERE.m_filenames.size())
			//{
			//	printf("%sQ - Proceed to the next filetype (%s)\n", g_global.tabs.c_str(), nextExtension);
			//	break;
			//}
		default:
			printf("%sQ - Quit\n", g_global.tabs.c_str());
		}
		return addNextFileChoice;
	}

	bool compareExtensions(const std::string& filename, const bool openSingleMenu = true);
	bool testAllExtensions(const std::string& filename, const bool openSingleMenu = true);
	bool allFiles();
};
extern FileMainList g_fileMains;
