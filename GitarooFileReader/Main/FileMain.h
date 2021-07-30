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
#include "FileType.h"
class AbstractMain
{
	friend class FileMainList;
protected:
	std::queue<std::string> m_filenames;

public:
	bool doesContainFiles();
	/*
	Main menu prompt used for choosing what action to perform on the loaded CHC file.
	Returns false if this is used from the multimenu and if the user wants to proceed to the next CHC.
	*/
	virtual bool singleFile() = 0;
	virtual bool multipleFiles(const std::pair<bool, const char*>) = 0;
	virtual std::string extension() const = 0;
	virtual bool compareExtension(const std::string& filename) = 0;
	virtual bool testExtension(const std::string& filename) = 0;
};

template<class T>
class FileMain : public AbstractMain
{
	FileType* loadFile()
	{
		const std::string name = m_filenames.front();
		m_filenames.pop();
		try
		{
			return new T(name);
		}
		catch (std::string str)
		{
			printf("%s%s\n", g_global.tabs.c_str(), str.c_str());
			return nullptr;
		}
		catch (const char* str)
		{
			printf("%s%s\n", g_global.tabs.c_str(), str);
			return nullptr;
		}
	}

public:
	std::string extension() const { return std::string(typeid(T).name()).substr(6); }

	bool compareExtension(const std::string& filename)
	{
		const std::string extension = this->extension();

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
		if (!fopen_s(&test, (filename + '.' + extension()).c_str(), "r"))
		{
			fclose(test);
			m_filenames.push(filename);
			return true;
		}
		return false;
	}

	bool singleFile()
	{
		if (FileType* object = loadFile())
		{
			bool retVal = object->menu(false, std::pair<bool, const char*>(false, ""));
			delete object;
			return retVal;
		}
		return false;
	}

	bool multipleFiles(const std::pair<bool, const char*> nextExtension)
	{
		if (m_filenames.size() == 0)
			return false;

		while (true)
		{
			GlobalFunctions::ResultType result = GlobalFunctions::ResultType::Success;
			if (m_filenames.size() > 1)
			{
				GlobalFunctions::banner(" " + extension() + " Mode Selection ");
				T::displayMultiChoices();
				if (nextExtension.first)
					GlobalFunctions::printf_tab("N - Proceed to the next filetype (%s)\n", nextExtension.second);

				GlobalFunctions::printf_tab("Q - Quit Program\n");
				result = GlobalFunctions::menuChoices(T::multiChoiceString);
			}
			else
				g_global.answer.character = 'e';

			switch (result)
			{
			case GlobalFunctions::ResultType::Quit:
				return true;
			case GlobalFunctions::ResultType::Help:
				T::displayMultiHelp();
				__fallthrough;
			case GlobalFunctions::ResultType::Failed:
				break;
			case GlobalFunctions::ResultType::Success:
				if (g_global.answer.character != 'n')
				{
					const char choice = g_global.answer.character;
					++g_global;
					while (m_filenames.size())
					{
						if (FileType* object = loadFile())
						{
							if (choice == 'e')
							{
								if (object->menu(doesContainFiles(), nextExtension))
								{
									delete object;
									--g_global;
									return false;
								}
							}
							else
								object->functionSelection(choice, true);
						}
					}
					--g_global;
					GlobalFunctions::printf_tab("All provided %s files have been evaluated.\n", extension().c_str());
				}
				return false;
			}
		}
	}
};

class FileMainList
{
	AbstractMain* m_mains[4];

public:
	FileMainList();
	~FileMainList();

	bool compareExtensions(const std::string& filename, const bool openSingleMenu = true);
	bool testAllExtensions(const std::string& filename, const bool openSingleMenu = true);
	bool allFiles();
};
