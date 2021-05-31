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
#include "CHC/CHC_Main.h"
using namespace std;

bool multi(int fileCount, char** files);
bool single();

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
	g_filetypes = { new CHCType };
	GlobalFunctions::banner(" Gitaroo File Reader v1.0 ");
	GlobalFunctions::banner(" 'Q' is the universal \"quit\" option | Use ';' for \"multi-step\" actions ");

	if (!multi(argc, argv))
		while (single());

	for (FileType* type : g_filetypes)
		delete type;

	printf("Press 'enter' to close the window...");
	char end = 0;

	if (g_global.multi)
		GlobalFunctions::clearIn();

	//Different from clearIn() as this usually is at least once guaranteed
	do
	{
		end = getchar();
	} while (end != '\n' && end != EOF);
	return 0;
}

bool multi(int fileCount, char** files)
{
	string filename = "";
	bool hadValidFiles = false;
	if (fileCount > 1)
	{
		for (int index = 1; index < fileCount; index++)
		{
			filename = files[index];
			int extension = 0;
			for (unsigned index = 0; index < filename.length();)
			{
				if (filename[index] == '\"')
					filename.erase(index);
				else
				{
					if (filename[index] == '.')
					{
						if (index >= filename.length() - 6)
							extension = 2;
						else if (index >= filename.length() - 10)
							extension = 1;
						else
							filename[index] = toupper(filename[index]);
					}
					else
						filename[index] = toupper(filename[index]);
					index++;
				}
			}
			//If the filename provided at the current index already has a defined extension tied to it
			if (extension == 2)
			{
				FILE* test;
				//If the file is found
				if (!fopen_s(&test, filename.c_str(), "r"))
				{
					fclose(test);
					size_t namDistance = filename.length();
					if (namDistance > 6)
						namDistance = 6;
					//Check through the list of available extensions to find the dll suited for the file type
					string sub = filename.substr(filename.length() - namDistance);
					for (char& c : sub)
						c = toupper(c);

					//If the file's extension matches one of the extensions applicable with the current dll
					for (FileType* type : g_filetypes)
					{
						//File extension needs to have '.' AND at least one valid character before it
						if (namDistance >= type->size() && sub.find(type->getType()) != string::npos)
						{
							type->m_files.push_back(filename.substr(0, filename.find_last_of('.')));
							hadValidFiles = true;
							break;
						}
						else if (type == g_filetypes.back())
						{
							printf("%s\"%s\" is not a valid extension.\n", g_global.tabs.c_str(), filename.substr(filename.find_last_of('.')).c_str());
							GlobalFunctions::clearIn();
						}
					}
				}
				else
				{
					size_t pos = filename.find_last_of('\\');
					printf("%sCould not locate the file \"%s\".\n", g_global.tabs.c_str(),
											filename.substr(pos != string::npos ? pos + 1 : 0).c_str());
				}
			}
			else if (extension == 0)
			{
				bool valid = false;
				//Apply every extension available to the end of the filename and check if the result exists
				for (FileType* type : g_filetypes)
				{
					FILE* test;
					//If the resulting file exists, add the original filename to that extension's list of filenames
					if (!fopen_s(&test, (filename + type->getType()).c_str(), "r"))
					{
						fclose(test);
						type->m_files.push_back(filename);
						valid = true;
						hadValidFiles = true;
					}
				}
				if (!valid)
				{
					size_t pos = filename.find_last_of('\\');
					printf("%sCould not locate a file with the file name \"%s\" using any of the accepted extensions\n", g_global.tabs.c_str(), filename.substr(pos != string::npos ? pos + 1 : 0).c_str());
				}
			}
			else
			{
				printf("%s\"%s\" is not a valid extension.\n", g_global.tabs.c_str(), filename.substr(filename.find_last_of('.')).c_str());
				GlobalFunctions::clearIn();
			}
		}
		//Go through every extension that has 1+ file associated with it and run its "Multi" function
		for (FileType* type : g_filetypes)
		{
			//If an extension has files and its Multi function returns true, exit the program
			if (type->m_files.size())
				if (type->loadMulti())
					break;
		}
	}

	return hadValidFiles;
}

bool single()
{
	string filename = "";
	GlobalFunctions::banner(" File Selection ");
	printf("%sAccepted File Types: ", g_global.tabs.c_str());
	for (FileType* type : g_filetypes)
		printf("%s ", type->getType());

	putchar('\n');
	printf("%sProvide the name of the file you wish to use (Or 'Q' to exit): ", g_global.tabs.c_str());
	//Breaks the loop if quit character is choosen
	switch (GlobalFunctions::stringInsertion(filename))
	{
	case GlobalFunctions::ResultType::Quit:
		return false;
	case GlobalFunctions::ResultType::Success:
	{
		size_t namDistance = filename.length();
		if (namDistance > 6)
			namDistance = 6;

		//If the filename given has an extension that could be one of the 20 valid choices
		if (filename.substr(filename.length() - namDistance).find('.') != string::npos)
		{
			bool valid = false;
			string sub = filename.substr(filename.length() - namDistance);
			for (char& c : sub)
				c = toupper(c);

			//If the file's extension matches one of the extensions applicable with the current dll
			for (FileType* type : g_filetypes)
			{
				//File extension needs to have '.' AND at least one valid character before it
				if (namDistance >= type->size() && sub.find(type->getType()) != string::npos)
				{
					type->loadSingle(filename.substr(0, filename.find_last_of('.')));
					break;
				}
				else if (type == g_filetypes.back())
				{
					printf("%s\"%s\" is not a valid extension.\n", g_global.tabs.c_str(), filename.substr(filename.find_last_of('.')).c_str());
					GlobalFunctions::clearIn();
				}
			}
		}
		else
		{
			namDistance = filename.length();
			if (namDistance > 10)
				namDistance = 10;
			//Check to make sure the file has no extension
			if (filename.substr(filename.length() - namDistance).find('.') == string::npos)
			{
				bool valid = false;
				//Cycle through all available extensions and run their "single" functions if a file is found with the filename
				for (FileType* type : g_filetypes)
				{
					FILE* test;
					//Check for a file existing with the current extension
					if (!fopen_s(&test, (filename + type->getType()).c_str(), "r"))
					{
						fclose(test);
						valid = true;
						GlobalFunctions::banner(' ' + filename + type->getType() + ' ');
						type->loadSingle(filename);
					}
				}
				if (!valid)
				{
					size_t pos = filename.find_last_of('\\');
					printf("%sCould not locate a file with the file name \"%s\" using any of the accepted extensions\n", g_global.tabs.c_str(), filename.substr(pos != string::npos ? pos + 1 : 0).c_str());
					GlobalFunctions::clearIn();
				}
			}
			else
			{
				printf("%s\"%s\" is not a valid extension.\n", g_global.tabs.c_str(), filename.substr(filename.find_last_of('.')).c_str());
				GlobalFunctions::clearIn();
			}
		}
	}
	default:
		return true;
	}
}
