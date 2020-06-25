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

/*---------------------------------------------------------------------\
Before startup, Global_Functions will load all possible dlls in to a
dll array, incrementing FileType::dllCount on every success.

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
	cin.unsetf(ios::skipws);
	string filename = "";
	bool fileCount = false;
	if (dllCount)
	{
		banner(" Gitaroo File Reader v0.5 Beta ");
		banner(" 'Q' is the universal \"quit\" option | Use ';' for \"multi-step\" actions ");
		//If click & drag or a batch file is used
		if (argc > 1)
		{
			for (int index = 1; index < argc; index++)
			{
				filename = argv[index];
				bool extension = false;
				for (unsigned index = 0; index < filename.length();)
				{
					if (filename[index] == '\"')
						filename.erase(index);
					else
					{
						if (index >= filename.length() - 5 && filename[index] == '.')
							extension = true;
						else
							filename[index] = toupper(filename[index]);
						index++;
					}
				}
				//If the filename provided at the current index already has a defined extension tied to it
				if (extension)
				{
					FILE* test;
					//If the file is found
					if (!fopen_s(&test, filename.c_str(), "r"))
					{
						fclose(test);
						//Check through the list of available extensions to find the dll suited for the file type 
						for (size_t i = 0; i < 20 && !global.quit; i++)
						{
							//If the file's extension matches one of the extensions applicable with the current dll
							for (size_t s = 0; s < dlls[i].extensions.size() && !global.quit; s++)
							{
								if (filename.substr(filename.length() - dlls[i].extensions[s].ext.length()).find(dlls[i].extensions[s].ext) != string::npos)
								{
									if (dlls[i].libraries[0].dll != nullptr)
									{
										dlls[i].extensions[s].files.push_back(filename.substr(0, filename.find_last_of('.')));
										fileCount = true;
									}
									else
									{
										size_t pos = filename.find_last_of('\\');
										cout << global.tabs << '\"' << filename.substr(pos != string::npos ? pos + 1 : 0) << "\" skipped due to its Base DLL not being found." << endl;
									}
									global.quit = true;
								}
								else if (i == 19 && s + 1 == dlls[i].extensions.size())
									cout << global.tabs << "\"" << filename.substr(filename.find_last_of('.')) << "\" is not a valid extension." << endl;
							}
						}
						global.quit = false;
					}
					else
					{
						size_t pos = filename.find_last_of('\\');
						cout << global.tabs << "Could not locate the file \"" << filename.substr(pos != string::npos ? pos + 1 : 0) << "\"." << endl;
					}
				}
				else if (filename.substr(filename.length() - 10).find('.') == string::npos)
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
									fileCount = true;
								}
								else
								{
									size_t pos = filename.find_last_of('\\');
									cout << global.tabs << '\"' << filename.substr(pos != string::npos ? pos + 1 : 0) + '.' + dlls[i].extensions[s].ext << "\" skipped due to its Base DLL not being found." << endl;
								}
							}
						}
					}
					if (!valid)
					{
						size_t pos = filename.find_last_of('\\');
						cout << global.tabs << "Could not locate a file with the file name \"" << filename.substr(pos != string::npos ? pos + 1 : 0) << "\" using any of the accepted extensions" << endl;
					}
				}
				else
				{
					cout << global.tabs << "\"" << filename.substr(filename.find_last_of('.')) << "\" is not a valid extension." << endl;
					cin.clear();
				}
			}
			//Go through every extension that has 1+ file associated with it and run its "Multi" function
			for (size_t i = 0; i < 20; i++)
			{
				if (dlls[i].libraries[0].dll != nullptr)
				{
					for (size_t s = 0; s < dlls[i].extensions.size(); s++)
					{
						//If an extension has files and its Multi function returns true, exit the program
						if (dlls[i].extensions[s].files.size())
							if (loadProc(dlls[i].libraries[0].dll, "loadMulti" + dlls[i].extensions[s].ext, dlls[i].extensions[s].files))
								break;
					}
				}
			}
		}
		//If either click & drag wasn't used or if none of the click & drag files returned valid
		if (!fileCount)
		{
			while (true)
			{
				banner(" File Selection ");
				cout << global.tabs << "Accepted File Types: ";
				for (size_t i = 0; i < 20; i++)
				{
					if (dlls[i].libraries[0].dll != nullptr)
					{
						for (size_t s = 0; s < dlls[i].extensions.size(); s++)
							cout << dlls[i].extensions[s].ext << ' ';
					}
				}
				cout << endl;
				cout << global.tabs << "Provide the name of the file you wish to use (Or 'Q' to exit): ";
				filename.clear();
				//Breaks the loop if quit character is choosen
				char val = filenameInsertion(filename);
				if (val == -1)
					break;
				else if (val == -2)
					continue;
				long namDistance;
				if (filename.length() < 5)
					namDistance = (char)filename.length();
				else
					namDistance = 5;
				//If the filename given has an extension that could be one of the 20 valid choices
				if (filename.substr(filename.length() - namDistance).find('.') != string::npos)
				{
					FILE* test;
					//If the file is found
					if (!fopen_s(&test, filename.c_str(), "r"))
					{
						fclose(test);
						bool valid = false;
						for (size_t i = 0; i < 20 && !global.quit; i++)
						{
							//If the file's extension matches one of the extensions applicable with the current dll
							for (size_t s = 0; s < dlls[i].extensions.size() && !global.quit; s++)
							{
								int startIndex = int(filename.length() - dlls[i].extensions[s].ext.length());
								if (startIndex >= 0 && filename.substr(startIndex).find(dlls[i].extensions[s].ext) != string::npos)
								{
									//Check if extension is loaded
									if (dlls[i].load())
									{
										//Truncates the file extension
										filename = filename.substr(0, filename.find_last_of('.'));
										loadProc(dlls[i].libraries[0].dll, "loadSingle" + dlls[i].extensions[s].ext, filename);
									}
									else
									{
										size_t pos = filename.find_last_of('\\');
										cout << global.tabs << '\"' << filename.substr(pos != string::npos ? pos + 1 : 0) << "\" skipped due to its Base DLL not being found." << endl;
									}
									global.quit = true;
								}
								else if (i == 19 && s + 1 == dlls[i].extensions.size())
								{
									cout << global.tabs << "\"" << filename.substr(filename.find_last_of('.')) << "\" is not a valid extension." << endl;
									cin.clear();
								}
							}
						}
						global.quit = false;
					}
				}
				//Check to make sure the file has no extension
				else if (filename.substr(filename.length() - 10).find('.') == string::npos)
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
									loadProc(dlls[i].libraries[0].dll, "loadSingle" + dlls[i].extensions[s].ext, filename);
								else
								{
									size_t pos = filename.find_last_of('\\');
									cout << global.tabs << '\"' << filename.substr(pos != string::npos ? pos + 1 : 0) << "\" skipped due to its Base DLL not being found." << endl;
								}
							}
						}
					}
					if (!located)
					{
						size_t pos = filename.find_last_of('\\');
						cout << global.tabs << "Could not locate a file with the file name \"" << filename.substr(pos != string::npos ? pos + 1 : 0) << "\" using any of the accepted extensions" << endl;
						cin.clear();
					}
				}
				else
				{
					cout << global.tabs << "\"" << filename.substr(filename.find_last_of('.')) << "\" is not a valid extension." << endl;
					cin.clear();
				}
				for (unsigned i = 0; i < 20; i++)
				{
					//Auto loads the main and sub dlls
					dlls[i].load();
				}
			};
		}
		//Free all loaded extensions
		for (size_t i = 0; i < 20; i++)
		{
			if (dlls[i].libraries[0].dll != nullptr)
			{
				loadProc(dlls[i].libraries[0].dll, "freeSubLibraries");
				FreeLibrary(dlls[i].libraries[0].dll);
				cout << "Unloaded ";
				for (size_t s = 0; s < dlls[i].extensions.size(); s++)
					cout << dlls[i].extensions[s].ext << ' ';
				cout << endl;
			}
		}
	}
	else
	{
		cout << "No BASE filetype extensions found.\n To use this program, you must have at least *one* of the available BASE extensions in its respective folder.\n"
			 << "You can get all currently available extentions from the pinned message board in the #hacking channel in the \"Gitaroo Pals\" discord.\n"
			 << "If you're getting this error despite having the dlls, you most likely don't have them in the right folder. A file type's extensions *must* be in their own designated folder.\n"
			 << "Example: CHC_Base & other CHC extensions go in a folder named CHC. The base extension is the most important. Any non-file type specific dll stays next to the exe.\n"
			 << "Discord Link: https://discord.gg/ed6P8Jt" << endl;
	}
	cout << "Press 'enter' to close the window...";
	cin.sync();
	char end;
	cin >> end;
	return 0;
}