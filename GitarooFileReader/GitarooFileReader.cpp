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
#include "..\Header\pch.h"
#include "Global_Functions.h"
using namespace std;

bool multi(int fileCount, char** files);
bool single();
void loadDlls();

/*---------------------------------------------------------------------\
The main function has two modes:
	Mode 1: Drag & drop/.bat multi activation
		This mode is specialized for when a user immediately inserts
		any number of files onto the exe. It will autocheck for proper
		files and then run the multimenu function for each type with a
		list of applicable files.

	Mode 2: Single click activation
		Loads up a prompt/menu where the user can provide any
		applicable file for the program to process. It alsos autocheck
		for a proper file - with the added bonus of at-used dll loading
		(if the dll for the filetype wasn't loaded beforehand).
All dlls are unloaded at exit time.
\---------------------------------------------------------------------*/
int main(int argc, char** argv)
{
	loadDlls();
	if (dllCount)
	{
		banner(" Gitaroo File Reader v0.7.0.3 Beta ");
		banner(" 'Q' is the universal \"quit\" option | Use ';' for \"multi-step\" actions ");
		if (!multi(argc, argv))
			while (single());
		//Free all loaded extensions
		for (size_t i = 0; i < 20; i++)
		{
			bool unloaded = false;
			for (size_t h = 0; h < dlls[i].libraries.size(); h++)
			{
				if (dlls[i].libraries[h].dll != nullptr)
					if (FreeLibrary(dlls[i].libraries[h].dll))
						unloaded = true;
			}
			if (unloaded)
			{
				printf("Unloaded ");
				for (size_t s = 0; s < dlls[i].extensions.size(); s++)
					printf("%s ", dlls[i].extensions[s].ext.c_str());
				putchar('\n');
			}
		}
	}
	else
	{
		printf("No BASE filetype extensions found.\n To use this program, you must have at least *one* of the available BASE extensions in its respective folder.\n");
		printf("You can get all currently available extentions from the pinned message board in the #hacking channel in the \"Gitaroo Pals\" discord.\n");
		printf("If you're getting this error despite having the dlls, you most likely just misplaced them. A file type's extensions *must* be in its own designated folder.\n");
		printf("Example: CHC_Base & other CHC extensions go in a folder named CHC. The base extension is the most important.\n");
		printf("Any non-file type specific dlls should be placed in the same folder as the application.\n");;
		printf("Discord Link: https://discord.gg/ed6P8Jt \n");
	}
	printf("Press 'enter' to close the window...");
	char end;
	if (global.multi)
		clearIn();
	//Different from clearIn() as this usually getchar() at least once guaranteed.
	do
	{
		end = getchar();
	} while (end != '\n' && end != EOF);
	return 0;
}

bool multi(int fileCount, char** files)
{
	string filename = "";
	int filesInsert = 0;
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
					for (size_t index = 0; index < namDistance; index++)
						sub[index] = toupper(sub[index]);
					for (size_t i = 0; i < 20 && !global.quit; i++)
					{
						//If the file's extension matches one of the extensions applicable with the current dll
						for (size_t s = 0; s < dlls[i].extensions.size() && !global.quit; s++)
						{
							//File extension needs to have '.' AND at least one valid character before it
							if (namDistance >= dlls[i].extensions[s].ext.length() + 2 && sub.find(dlls[i].extensions[s].ext) != string::npos)
							{
								if (dlls[i].libraries[0].dll != nullptr)
								{
									dlls[i].extensions[s].files.push_back(filename.substr(0, filename.find_last_of('.')));
									filesInsert++;
								}
								else
								{
									size_t pos = filename.find_last_of('\\');
									printf("%s\"%s\" skipped due to its Base DLL not being found.\n", global.tabs.c_str(), filename.substr(pos != string::npos ? pos + 1 : 0).c_str());
								}
								global.quit = true;
							}
							else if (i == 19 && s + 1 == dlls[i].extensions.size())
								printf("%s\"%s\" is not a valid extension.\n", global.tabs.c_str(), filename.substr(filename.find_last_of('.')).c_str());
						}
					}
					global.quit = false;
				}
				else
				{
					size_t pos = filename.find_last_of('\\');
					printf("%sCould not locate the file \"%s\".\n", global.tabs.c_str(), filename.substr(pos != string::npos ? pos + 1 : 0).c_str());
				}
			}
			else if (extension == 0)
			{
				bool valid = false;
				//Apply every extension available to the end of the filename and check if the result exists
				for (size_t i = 0; i < 20; i++)
				{
					for (size_t s = 0; s < dlls[i].extensions.size(); s++)
					{
						FILE* test;
						//If the resulting file exists, add the original filename to that extension's list of filenames
						if (!fopen_s(&test, (filename + '.' + dlls[i].extensions[s].ext).c_str(), "r"))
						{
							fclose(test);
							if (dlls[i].libraries[0].dll != nullptr) // Only the file if the extension is loaded
							{
								dlls[i].extensions[s].files.push_back(filename);
								valid = true;
								filesInsert++;
							}
							else
							{
								size_t pos = filename.find_last_of('\\');
								printf("%s\"%s\" skipped due to its Base DLL not being found.\n", global.tabs.c_str(), (filename.substr(pos != string::npos ? pos + 1 : 0) + '.' + dlls[i].extensions[s].ext).c_str());
							}
						}
					}
				}
				if (!valid)
				{
					size_t pos = filename.find_last_of('\\');
					printf("%sCould not locate a file with the file name \"%s\" using any of the accepted extensions\n", global.tabs.c_str(), filename.substr(pos != string::npos ? pos + 1 : 0).c_str());
				}
			}
			else
			{
				printf("%s\"%s\" is not a valid extension.\n", global.tabs.c_str(), filename.substr(filename.find_last_of('.')).c_str());
				clearIn();
			}
		}
		//Go through every extension that has 1+ file associated with it and run its "Multi" function
		for (size_t i = 0; i < 20 && !global.quit; i++)
		{
			if (dlls[i].libraries[0].dll != nullptr)
			{
				for (size_t s = 0; s < dlls[i].extensions.size() && !global.quit; s++)
				{
					//If an extension has files and its Multi function returns true, exit the program
					if (dlls[i].extensions[s].files.size())
						if (loadProc(dlls[i].libraries[0].dll, ("loadMulti" + dlls[i].extensions[s].ext).c_str(), dlls[i].extensions[s].files))
							global.quit = true;
				}
			}
		}
		global.quit = false;
	}
	return filesInsert;
}

bool single()
{
	string filename = "";
	banner(" File Selection ");
	printf("%sAccepted File Types: ", global.tabs.c_str());
	for (size_t i = 0; i < 20; i++)
	{
		if (dlls[i].libraries[0].dll != nullptr)
		{
			for (size_t s = 0; s < dlls[i].extensions.size(); s++)
				printf("%s ", dlls[i].extensions[s].ext.c_str());
		}
	}
	putchar('\n');
	printf("%sProvide the name of the file you wish to use (Or 'Q' to exit): ", global.tabs.c_str());
	//Breaks the loop if quit character is choosen
	switch (filenameInsertion(filename))
	{
	case 'q':
		return false;
	case '!':
	{
		size_t namDistance = filename.length();
		if (namDistance > 6)
			namDistance = 6;
		//If the filename given has an extension that could be one of the 20 valid choices
		if (filename.substr(filename.length() - namDistance).find('.') != string::npos)
		{
			FILE* test;
			//If the file is found
			if (!fopen_s(&test, filename.c_str(), "r"))
			{
				fclose(test);
				bool valid = false;
				string sub = filename.substr(filename.length() - namDistance);
				for (size_t index = 0; index < namDistance; index++)
					sub[index] = toupper(sub[index]);
				for (size_t i = 0; i < 20 && !global.quit; i++)
				{
					//If the file's extension matches one of the extensions applicable with the current dll
					for (size_t s = 0; s < dlls[i].extensions.size() && !global.quit; s++)
					{
						//File extension needs to have '.' AND at least one valid character before it
						if (namDistance >= dlls[i].extensions[s].ext.size() + 2 && sub.find(dlls[i].extensions[s].ext) != string::npos)
						{
							//Check if extension is loaded
							if (dlls[i].load())
							{
								//Truncates the file extension
								filename = filename.substr(0, filename.find_last_of('.'));
								loadProc(dlls[i].libraries[0].dll, ("loadSingle" + dlls[i].extensions[s].ext).c_str(), filename);
							}
							else
							{
								size_t pos = filename.find_last_of('\\');
								printf("%s\"%s\" skipped due to its Base DLL not being found.\n", global.tabs.c_str(), filename.substr(pos != string::npos ? pos + 1 : 0).c_str());
							}
							global.quit = true;
						}
						else if (i == 19 && s + 1 == dlls[i].extensions.size())
						{
							printf("%s\"%s\" is not a valid extension.\n", global.tabs.c_str(), filename.substr(filename.find_last_of('.')).c_str());
							clearIn();
						}
					}
				}
				global.quit = false;
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
				bool located = false;
				//Cycle through all available extensions and run their "single" functions if a file is found with the filename
				for (size_t i = 0; i < 20; i++)
				{
					for (size_t s = 0; s < dlls[i].extensions.size(); s++)
					{
						FILE* test;
						//Check for a file existing with the current extension
						if (!fopen_s(&test, (filename + '.' + dlls[i].extensions[s].ext).c_str(), "r"))
						{
							fclose(test);
							located = true;
							banner(' ' + filename + '.' + dlls[i].extensions[s].ext + ' ');
							if (dlls[i].load())
								loadProc(dlls[i].libraries[0].dll, ("loadSingle" + dlls[i].extensions[s].ext).c_str(), filename);
							else
							{
								size_t pos = filename.find_last_of('\\');
								printf("%s\"%s\" skipped due to its Base DLL not being found.\n", global.tabs.c_str(), filename.substr(pos != string::npos ? pos + 1 : 0).c_str());
							}
						}
					}
				}
				if (!located)
				{
					size_t pos = filename.find_last_of('\\');
					printf("%sCould not locate a file with the file name \"%s\" using any of the accepted extensions\n", global.tabs.c_str(), filename.substr(pos != string::npos ? pos + 1 : 0).c_str());
					clearIn();
				}
			}
			else
			{
				printf("%s\"%s\" is not a valid extension.\n", global.tabs.c_str(), filename.substr(filename.find_last_of('.')).c_str());
				clearIn();
			}
		}
	}
	default:
		loadDlls();
		return true;
	}
}

void loadDlls()
{
	LinkedList::List<const wchar_t*> errorList;
	SetErrorMode(SEM_FAILCRITICALERRORS);
	for (unsigned i = 0; i < 20; i++)
	{
		//Auto loads the main and sub dlls
		//If a loadlib fails, and the file existed, add the dllname to the errorlist
		dlls[i].load(errorList);
	}
	SetErrorMode(0);
	const size_t size = errorList.size();
	if (size > 0)
	{
		FILE* loadErrorLog;
		fopen_s(&loadErrorLog, "DllLoadingErrors.txt", "w");
		fprintf(loadErrorLog, "These dlls are out of date and must be updated to be compatible with this version of GMFR:\n");
		for (size_t i = 0; i < size; i++)
			fprintf(loadErrorLog, "%ls\n", errorList[i]);
		fclose(loadErrorLog);
	}
}
