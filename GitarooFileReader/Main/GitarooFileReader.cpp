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
#include "Global_Functions.h"
#include "FileMain.h"
bool multi(int fileCount, char** files);
void single();
FileMainList g_fileMains;

/*---------------------------------------------------------------------\
The main function has two modes:
	Mode 1: Drag & drop/.bat multi activation
		This mode is specialized for when a user immediately inserts
		any number of files onto the exe. It will autocheck for proper
		files and then run the multimenu function for each type with a
		list of applicable files.

	Mode 2: Single click activation
		Loads up a prompt/menu where the user can provide any
		applicable file for the program to process.
\---------------------------------------------------------------------*/
int main(int argc, char** argv)
{
	GlobalFunctions::banner(" Gitaroo File Reader v1.1.0 ");
	GlobalFunctions::banner(" 'Q' is the universal \"quit\" option | Use ';' for \"multi-step\" actions ");

	if (!multi(argc, argv))
		single();

	return 0;
}

bool multi(int fileCount, char** files)
{
	bool hadValidFiles = false;
	if (fileCount > 1)
	{
		std::string filename = "";
		
		for (int index = 1; index < fileCount; index++)
		{
			filename = files[index];
			if (filename[0] == '\"')
			{
				filename.erase(0);
				filename.erase(filename.length() - 1);
			}

			if ((filename.find('.') != std::string::npos && g_fileMains.compareExtensions(filename, false))
				|| g_fileMains.testAllExtensions(filename, false))
				hadValidFiles = true;
		}
	}

	// Go through every extension that has 1+ file associated with it and run its "Multi" function
	//
	// If no "quit app" command is sent, exit based off whether any valid files were found 
	return g_fileMains.allFiles() || hadValidFiles;
}

void single()
{
	while (true)
	{
		std::string filename = "";
		GlobalFunctions::banner(" File Selection ");
		GlobalFunctions::printf_tab("Accepted File Types: .CHC, .IMX, .XG, .XGM\n");
		GlobalFunctions::printf_tab("Provide the name of the file you wish to use (Or 'Q' to exit): ");

		switch (GlobalFunctions::stringInsertion(filename))
		{
		case GlobalFunctions::ResultType::Quit:
			return;
		case GlobalFunctions::ResultType::Success:
			if (filename.find('.') == std::string::npos || !g_fileMains.compareExtensions(filename))
				g_fileMains.testAllExtensions(filename);
		}
	}
}
