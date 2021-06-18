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
#include <queue>
#include <string>
#include "Global_Functions.h"
#include "FileType.h"

template<class T>
class FileMain
{
	friend struct FileMainList;
protected:
	std::queue<std::string> m_filenames;
	FileType* m_file = nullptr;
public:
	bool compareExtension(const std::string& filename)
	{
		const std::string extension = std::string(typeid(T).name()).substr(6);

		if (filename.length() >= extension.length() + 1
			&& std::equal(filename.begin() + filename.length() - extension.length(),
				filename.end(),
				extension.begin(),
				extension.end(),
				[](const char a, const char b) { return toupper(a) == b; }))
		{
			m_filenames.push(filename.substr(0, filename.find_last_of('.')));
			return true;
		}
		return false;
	}

	bool testExtension(const std::string& filename)
	{
		FILE* test;
		//If the resulting file exists, add the original filename
		if (!fopen_s(&test, (filename + '.' + std::string(typeid(T).name()).substr(6)).c_str(), "r"))
		{
			fclose(test);
			m_filenames.push(filename);
			return true;
		}
		return false;
	}

	bool doesContainFiles() { return m_filenames.size() > 0; }

	/*
	Main menu prompt used for choosing what action to perform on the loaded CHC file.
	Returns false if this is used from the multimenu and if the user wants to proceed to the next CHC.
	*/
	bool singleFile() { return false; }
	bool multipleFiles() { return false; }

protected:
	T* loadFile()
	{
		try
		{
			m_file = new T(m_filenames.front());
		}
		catch (std::string str)
		{
			printf("%s%s\n", g_global.tabs.c_str(), str.c_str());
			m_file = nullptr;
		}
		catch (const char* str)
		{
			printf("%s%s\n", g_global.tabs.c_str(), str);
			m_file = nullptr;
		}
		return (T*)m_file;
	}

	bool saveFile(bool onExit = false)
	{
		GlobalFunctions::banner(" " + m_file->m_filename + m_file->m_extension + " Save Prompt ");
		std::string ext = "_T";
		const std::string filename = m_file->m_directory + m_file->m_filename;
		bool saved = false;
		do
		{
			std::string choices = "a";
			if (m_file->getSaveStatus() != 2)
			{
				printf("%sS - Save & Overwrite %s\n", g_global.tabs.c_str(), (m_file->m_filename + m_file->m_extension).c_str());
				choices += 's';
			}
			printf("%sA - Save as \"%s\"\n", g_global.tabs.c_str(), (m_file->m_filename + ext + m_file->m_extension).c_str());
			printf("%sQ - Back Out\n", g_global.tabs.c_str());
			switch (GlobalFunctions::menuChoices(choices))
			{
			case GlobalFunctions::ResultType::Quit:
				return false;
			case GlobalFunctions::ResultType::Success:
				switch (g_global.answer.character)
				{
				case 's':
					if (m_file->create(filename))
					{
						m_file->setSaveStatus(2);
						saved = true;
						g_global.quit = true;
					}
					break;
				case 'a':
					do
					{
						switch (GlobalFunctions::fileOverwriteCheck(filename + ext + m_file->m_extension))
						{
						case GlobalFunctions::ResultType::Quit:
							return false;
						case GlobalFunctions::ResultType::No:
							printf("%s\n", g_global.tabs.c_str());
							ext += "_T";
							break;
						default:
							if (m_file->create(filename + ext))
							{
								saved = true;
								if (!onExit)
								{
									do
									{
										printf("%sChange current file location to point to %s? [Y/N]\n", g_global.tabs.c_str(), (filename + ext + m_file->m_extension).c_str());
										switch (GlobalFunctions::menuChoices("yn"))
										{
										case GlobalFunctions::ResultType::Success:
											if (g_global.answer.character == 'y')
											{
												m_file->m_filename += ext;
												m_file->setSaveStatus(2);
											}
										case GlobalFunctions::ResultType::Quit:
											g_global.quit = true;
										}
									} while (!g_global.quit);
								}
								else
									g_global.quit = true;
							}
						}
					} while (!g_global.quit);
				}
			}
		} while (!g_global.quit);
		g_global.quit = false;
		return saved;
	}
};
