#pragma once
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
#ifdef GLOBALFUNCTIONS_EXPORTS
#define GLOBALFUNCTIONS_API __declspec(dllexport)
#else
#define GLOBALFUNCTIONS_API __declspec(dllimport)
#endif
#include "List.h"
#include <windows.h>

constexpr long double M_PI = 3.1415926535897931L;

//Holds all data pertaining to applicable file extensions.
struct FileType
{
	struct dllPair
	{
		const wchar_t* name;
		HINSTANCE dll = nullptr;
		dllPair() : name(nullptr), dll(nullptr) {}
		dllPair(const wchar_t* nam) : name(nam), dll(nullptr) {}
		//dllPair(dllPair& pair) : name(pair.name), dll(pair.dll) {}
		~dllPair();
	};
	struct extension
	{
		std::string ext = "";
		List<std::string> files;
		extension() {}
		extension(const char* name) : ext(name) {}
	};
	//Libraries[0] will ALWAYS be the base dll
	List<dllPair> libraries;
	List<extension> extensions;
	FileType(const std::initializer_list<const char*>& exts, const std::initializer_list<const wchar_t*>& names);
	GLOBALFUNCTIONS_API bool load();
};
extern "C" GLOBALFUNCTIONS_API size_t dllCount;
extern "C" GLOBALFUNCTIONS_API FileType dlls[20];
/*
App-wide structure that holds universal values used by many functions across the solution.

	"quit"  - bool value for ending do/while loops when taking in user inputs. Setting this to true should terminate a loop when used.
   "input"  - char value that holds a single character from an input stream, usually std::cin. Commonly used to buffer an function to recieve a value of a different type.
   "multi"  - bool value used for handling multiple menu steps in one go. For example: fixing a CHC file and printing it out as a .txt with only one press of ENTER.
	"tabs"  - std::string used to add empty space at the beginning of most new lines in the console window. Different menus will add or remove space from this std::string depening on scope. Utilizing this can make the console window look more organized.
  "invalid" - std::string used to capture an entire segment of invalid inputs for any menu screen.
*/
struct GlobalVars
{
	bool quit = false;
	char input = 0;
	bool multi = false;
	std::string tabs = "", invalid = "";
};
extern "C" GLOBALFUNCTIONS_API GlobalVars global;

extern "C" GLOBALFUNCTIONS_API bool LoadLib(FileType::dllPair& pair);
extern "C" GLOBALFUNCTIONS_API bool FreeLib(HINSTANCE & lib);
extern "C" GLOBALFUNCTIONS_API char loadProc(HINSTANCE & lib, std::string proc);
/*
Searches for a function with the given name (proc) inside the provided dll (lib).
If found, it will use object as the parameter and return the result.
Otherwise, it will return -1.
*/
template<typename T>
extern char loadProc(HINSTANCE& lib, std::string proc, T& object)
{
	typedef bool(__cdecl* MYPROC)(T&);
	MYPROC ProcAdd = (MYPROC)GetProcAddress(lib, proc.c_str());
	if (ProcAdd)
		return ProcAdd(object);
	else
	{
		std::cout << global.tabs << proc << " failed to load" << std::endl;
		return -1;
	}
}
//Generates banner
extern "C" GLOBALFUNCTIONS_API void banner(std::string title, float coef = 1);
//Adjust pre-fix tab "||"s to the given length
extern "C" GLOBALFUNCTIONS_API void adjustTabs(char value);
//Used for printing to a max of two files at once
extern "C" GLOBALFUNCTIONS_API void dualvfprintf_s(FILE* out1, FILE* out2, const char* format, ...);
//Reads a file name from the input window, removing certain unnecessary characters from the string
extern "C" GLOBALFUNCTIONS_API char filenameInsertion(std::string& filename, std::string specials = "");

/*
Universal menu function that accepts a list of characters as a std::string as option choices
@param choices - List of characters that serves as the possible menu options
@return 0+ - Valid entry/Index of the character
@return -1 - User entered the "quit" or 'Q' character
@return -2 - User entered an invalid character
@return -3 - User entered the "help" or '?' character
*/
extern "C" GLOBALFUNCTIONS_API size_t menuChoices(std::string choices);

/*
Used to check if a file of the specified name already exists at its location.
If one exists, it'll provide a prompt where the user can choose whether or not to override the file.
*/
extern "C" GLOBALFUNCTIONS_API char fileOverwriteCheck(std::string fileName);


/*
Function for inserting any value from the input stream
Return values:
1 -- Valid Entry
2+- One of the special input characters
0 -- User entered the "quit" or 'Q' character
-1 - Input stream given a negative value with allowNegatives set to false
-2 - Input value less than minimum
-3 - Input value greater than maximum
-4 - User entered an invalid character
*/
template<typename T>
extern char valueInsert(T& value, bool allowNegatives = false, T min = 0, T max = 0, std::string specials = "")
{
	if (global.multi)
	{
		std::cin.putback(global.input);
		std::cout << '*' << char(toupper(global.input)) << std::endl;
		global.multi = false;
	}
	do
	{
		std::cin >> global.input;
	} while (global.input == ' ');
	switch (global.input)
	{
	case ';':
		std::cin >> global.input;
	case '\n':
		std::cout << global.tabs << "Please remember to type SOMETHING before pressing 'Enter'\n";
		std::cout << global.tabs << std::endl;
		std::cin.sync();
		return -2;
	default:
		char ans = tolower(global.input);
		if (ans == 'q')
		{
			do
			{
				std::cin >> global.input;
			} while (global.input == ' ' || global.input == ';');
			if (global.input != '\n')
				global.multi = true;
			return 0;
		}
		auto rangeTest = [&]()
		{
			double tmp;
			std::cin >> tmp;
			if (!min)
			{
				if (!max)
				{
					value = (T)tmp;
					do
					{
						std::cin >> global.input;
					} while (global.input == ' ');
					if (global.input != '\n')
						global.multi = true;
					return 1;
				}
				else
				{
					if ((T)tmp <= max)
					{
						value = (T)tmp;
						do
						{
							std::cin >> global.input;
						} while (global.input == ' ');
						if (global.input != '\n')
							global.multi = true;
						return 1;
					}
					else
						return -3;
				}
			}
			else
			{
				if ((T)tmp >= min)
				{
					if (!max)
					{
						value = (T)tmp;
						do
						{
							std::cin >> global.input;
						} while (global.input == ' ');
						if (global.input != '\n')
							global.multi = true;
						return 1;
					}
					else
					{
						if ((T)tmp <= max)
						{
							value = (T)tmp;
							do
							{
								std::cin >> global.input;
							} while (global.input == ' ');
							if (global.input != '\n')
								global.multi = true;
							return 1;
						}
						else
							return -3;
					}
				}
				else
					return -2;
			}
		};
		auto inval = [&]()
		{
			global.invalid = ans;
			do
			{
				std::cin >> global.input;
				switch (global.input)
				{
				case '\n':
					global.quit = true;
					break;
				default:
					global.invalid += global.input;
				}
			} while (!global.quit);
			return -4;
		};
		if (ans == '-')
		{
			if (allowNegatives)
			{
				char backup = std::cin.peek();
				if (backup == '.')
				{
					std::cin.get();
					if (std::cin.peek() >= '0' && std::cin.peek() <= '9')
					{
						std::cin.putback(backup);
						std::cin.putback(ans);
						return rangeTest();
					}
					else
					{
						std::cin.putback(backup);
						return inval();
					}
				}
				else if (backup >= '0' && backup <= '9')
				{
					std::cin.putback(ans);
					return rangeTest();
				}
				else
					return inval();
			}
			else
			{
				std::cin.putback(ans);
				double tmp;
				std::cin >> tmp;
				return -1;
			}
		}
		else if (ans == '.')
		{
			if (std::cin.peek() >= '0' && std::cin.peek() <= '9')
			{
				std::cin.putback(ans);
				return rangeTest();
			}
			else
				return inval();
		}
		else if (ans >= '0' && ans <= '9')
		{
			std::cin.putback(ans);
			return rangeTest();
		}
		else
		{
			for (unsigned char index = 0; index < specials.length(); index++)
			{
				if (tolower(ans) == specials[index])
				{
					std::cin.putback(ans);
					return index + 2;
				}
			}
			return inval();
		}
	}
}

/*
Function for inserting any value from a file
Return values:
1 -- Valid Entry
2+- One of the special input characters
0 -- User entered the "quit" or 'Q' character
-1 - Input stream given a negative value with allowNegatives set to false
-2 - Input value less than minimum
-3 - Input value greater than maximum
-4 - User entered an invalid character
*/
template<typename T>
extern char valueInsertFromFile(FILE* in, T& value, bool allowNegatives = false, T min = 0, T max = 0)
{
	char ans = 0;
	fscanf_s(in, " %c", &ans, 1);
	ans = tolower(ans);
	if (ans == 'q')
		return 0;
	auto rangeTest = [&]()
	{
		double tmp;
		fscanf_s(in, " %lf", &tmp);
		if (!min)
		{
			if (!max)
			{
				value = (T)tmp;
				return 1;
			}
			else
			{
				if ((T)tmp <= max)
				{
					value = (T)tmp;
					return 1;
				}
				else
					return -3;
			}
		}
		else
		{
			if ((T)tmp >= min)
			{
				if (!max)
				{
					value = (T)tmp;
					return 1;
				}
				else
				{
					if ((T)tmp <= max)
					{
						value = (T)tmp;
						return 1;
					}
					else
						return -3;
				}
			}
			else
				return -2;
		}
	};
	auto inval = [&]()
	{
		global.invalid = ans;
		do
		{
			ans = fgetc(in);
			switch (ans)
			{
			case '\n':
				global.quit = true;
				break;
			default:
				global.invalid += ans;
			}
		} while (!global.quit);
		return -4;
	};
	if (ans == '-')
	{
		if (allowNegatives)
		{
			char backup = fgetc(in);
			if (backup == '.')
			{
				char peek = fgetc(in);
				ungetc(peek, in);
				if (peek >= '0' && peek <= '9')
				{
					ungetc(backup, in);
					ungetc(ans, in);
					return rangeTest();
				}
				else
					return inval();
			}
			else
			{
				ungetc(backup, in);
				if (backup >= '0' && backup <= '9')
				{
					ungetc(ans, in);
					return rangeTest();
				}
				else
					return inval();
			}
		}
		else
		{
			ungetc(ans, in);
			double tmp;
			fscanf_s(in, " %lf", &tmp);
			return -1;
		}
	}
	else if (ans == '.')
	{
		char peek = fgetc(in);
		ungetc(peek, in);
		if (peek >= '0' && peek <= '9')
		{
			ungetc(ans, in);
			return rangeTest();
		}
		else
			return inval();
	}
	else if (ans >= '0' && ans <= '9')
	{
		ungetc(ans, in);
		return rangeTest();
	}
	else
		return inval();
}

/*
Function for inserting any value from the input stream (with no set custom min or max) [has special characters]
Return values:
1 -- Valid Entry
2+- One of the special input characters
0 -- User entered the "quit" or 'Q' character
-1 - Input stream given a negative value with allowNegatives set to false
-4 - User entered an invalid character
*/
template<typename T>
extern char valueInsert(T& value, bool allowNegatives, std::string specials)
{
	return valueInsert(value, allowNegatives, T(0), T(0), specials);
}

/*
Function for inserting any value from a file (with no set custom min or max) [has special characters]
Return values:
1 -- Valid Entry
2+- One of the special input characters
0 -- User entered the "quit" or 'Q' character
-1 - Input stream given a negative value with allowNegatives set to false
-4 - User entered an invalid character
*/
template<typename T>
extern char valueInsertFromFile(FILE* in, T& value, bool allowNegatives, std::string specials)
{
	return valueInsertFromFile(in, value, allowNegatives, T(0), T(0), specials);
}

/*
Function for inserting a list values all in one go from the std::cin input stream
Return values:
1 -- End of the given list of input values
2+- One of the special input characters
0 -- User entered the "quit" or 'Q' character
-1 - User entered the "help" or '?' character
-2 - User entered an invalid character before the list ended
*/
template<typename T>
extern char vectorValueInsert(List<T>& values, std::string outCharacters, T max, bool allowRepeats = true, T min = 0)
{
	std::cout << global.tabs << "Input: ";
	if (global.multi)
	{
		std::cin.putback(global.input);
		std::cout << '*';
	}
	std::cin >> global.input;
	do
	{
		switch (tolower(global.input))
		{
		case ' ':
			std::cin >> global.input;
			break;
		case '?':
			if (global.multi)
			{
				for (size_t valIndex = 0; valIndex < values.size(); valIndex++)
					std::cout << values[valIndex] << ' ';
				std::cout << '?' << std::endl;
			}
			do
			{
				std::cin >> global.input;
			} while (global.input == ' ' || global.input == ';');
			if (global.input != '\n')
				global.multi = true;
			else
				global.multi = false;
			return -1;
		case 'q':
			if (global.multi)
			{
				for (size_t valIndex = 0; valIndex < values.size(); valIndex++)
					std::cout << values[valIndex] << ' ';
				std::cout << 'Q' << std::endl;
			}
			do
			{
				std::cin >> global.input;
			} while (global.input == ' ' || global.input == ';');
			if (global.input != '\n')
				global.multi = true;
			else
				global.multi = false;
			return 0;
		case '\n':
			if (global.multi)
			{
				for (size_t valIndex = 0; valIndex < values.size(); valIndex++)
					std::cout << values[valIndex] << ' ';
				std::cout << std::endl;
				global.multi = false;
			}
			return 1;
		case ';':
			if (global.multi)
			{
				for (size_t valIndex = 0; valIndex < values.size(); valIndex++)
					std::cout << values[valIndex] << ' ';
				std::cout << std::endl;
			}
			do
			{
				std::cin >> global.input;
			} while (global.input == ' ');
			if (global.input != '\n')
				global.multi = true;
			return 1;
		default:
			if (global.input >= '0' && global.input <= '9')
			{
				std::cin.putback(global.input);
				T value;
				std::cin >> value;
				if (value < min || value >= max)
					std::cout << global.tabs << value << " is not within range. Skipping value.\n" << global.tabs << '\n';
				else if (allowRepeats || values.search(value) == -1)
					values.push_back(value);
				else
					std::cout << global.tabs << value << " is already in this list.\n" << global.tabs << '\n';
				std::cin >> global.input;
			}
			else
			{
				size_t size = outCharacters.length();
				for (size_t index = 0; index < size; index++)
				{
					if (tolower(global.input) == outCharacters[index])
					{
						if (global.multi)
						{
							for (size_t valIndex = 0; valIndex < values.size(); valIndex++)
								std::cout << values[valIndex] << ' ';
							std::cout << std::endl;
						}
						else
							global.multi = true;
						return (char)index + 2;
					}
				}
				global.multi = false;
				global.invalid = global.input;
				do
				{
					std::cin >> global.input;
					if (global.input == '\n')
					{
						std::cin.clear();
						global.quit = true;
					}
					else
						global.invalid += global.input;
				} while (!global.quit);
				global.quit = false;
				std::cout << global.tabs << "\"" << global.invalid << "\" is not a valid response.\n" << global.tabs << '\n';
				return -2;
			}
		}
	} while (true);
}

extern "C" GLOBALFUNCTIONS_API long radiansToDegrees(double angle);

extern GLOBALFUNCTIONS_API std::string angleToFraction(double angle);