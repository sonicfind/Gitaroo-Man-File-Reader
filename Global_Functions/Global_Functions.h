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
	/*
	Takes two {} arrays as inputs for establishing the dll/extension structure for any type.
	Names - the list of dlls that the program should attempt to load on startup.
	Exts -- the list of file extensions that the first dll in "names" will be applicable for.
	*/
	FileType(const std::initializer_list<const char*>& exts, const std::initializer_list<const wchar_t*>& names)
		: extensions(exts), libraries(names) {}
	GLOBALFUNCTIONS_API bool load();
	GLOBALFUNCTIONS_API void load(List<const wchar_t*>& errors);
};
extern "C" GLOBALFUNCTIONS_API size_t dllCount;
extern "C" GLOBALFUNCTIONS_API FileType dlls[20];

extern "C" GLOBALFUNCTIONS_API int peek();
extern "C" GLOBALFUNCTIONS_API void clearIn();

//Generates banner
extern "C" GLOBALFUNCTIONS_API void banner(std::string title, double coef = 1);

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
	//Adjust pre-fix tab "||"s to the given length
	void adjustTabs(size_t value) { tabs = value > 0 ? std::string(--value, '\t') + "      ||" : ""; }
	//Pulls remaining characters from the input stream into the invalid string variable
	char fillInvalid() 
	{
		invalid = "";
		while (input != '\n')
		{
			invalid += input;
			scanf_s("%c", &input, 1);
		}
		clearIn();
		multi = false;
		return '*';
	}
};
extern "C" GLOBALFUNCTIONS_API GlobalVars global;

extern "C" GLOBALFUNCTIONS_API bool LoadLib(FileType::dllPair & pair);

/*
Searches for a function with the given name (proc) inside the provided dll (lib).
If found, it will use object as the parameter and return the result.
Otherwise, it will return -1.
*/
template<class...Args>
extern char loadProc(HINSTANCE& lib, const char* proc, Args&... args)
{
	typedef bool(__cdecl* MYPROC)(Args&...);
	MYPROC ProcAdd = (MYPROC)GetProcAddress(lib, proc);
	if (ProcAdd)
		return ProcAdd(args...);
	else
	{
		printf("%s%s failed to load\n", global.tabs.c_str(), proc);
		return -1;
	}
}

//Used for printing to a max of two files at once
extern "C" GLOBALFUNCTIONS_API void dualvfprintf_s(FILE* out1, FILE* out2, const char* format, ...);
//Reads a file name from the input window, removing certain unnecessary characters from the string
extern "C" GLOBALFUNCTIONS_API char filenameInsertion(std::string& filename, std::string specials = "");

/*
The basic universal menu system that should be used for almost everything that deals with user choice
@param choices - List of characters that serves as the possible menu options
@param indexMode - Determines whether to return the chosen character itself or the index of said character from "choices"
@return The character that was pulled from the stream, or '*' if the choice was invalid. If indexMode is true, return the index from "choices". 
*/
extern "C" GLOBALFUNCTIONS_API size_t menuChoices(std::string choices, bool indexMode = false);

/*
Used to check if a file of the specified name already exists at its location.
If one exists, it'll provide a prompt where the user can choose whether or not to override the file.
*/
extern "C" GLOBALFUNCTIONS_API char fileOverwriteCheck(std::string fileName);


/*
Function for inserting any value from the standard input stream
Return values:
'!' - Valid Entry
'q' - User entered the "quit" or 'Q' character
'-' - Input stream given a negative value with allowNegatives set to false
'<' - Input value less than minimum
'>' - Input value greater than maximum
'*' - User entered an invalid character
 or one of the special input characters
*/
template<typename T>
extern char valueInsert(T& value, bool allowNegatives = false, T min = 0, T max = 0, std::string specials = "")
{
	if (global.multi)
	{
		ungetc(global.input, stdin);
		printf("*%c\n", toupper(global.input));
		global.multi = false;
	}
	do
	{
		scanf_s("%c", &global.input, 1); 
	} while (global.input == ' ');
	switch (global.input)
	{
	case ';':
		scanf_s("%c", &global.input, 1); 
	case '\n':
		printf("%sPlease remember to type SOMETHING before pressing 'Enter'\n", global.tabs.c_str());
		printf("%s\n", global.tabs.c_str());
		clearIn();
		return '*';
	case 'q':
	case 'Q':
		do
		{
			scanf_s("%c", &global.input, 1);
		} while (global.input == ' ' || global.input == ';');
		if (global.input != '\n')
			global.multi = true;
		return 'q';
	default:
		auto rangeTest = [&]()
		{
			double tmp;
			scanf_s("%lg", &tmp);
			if (!min)
			{
				if (!max)
				{
					value = (T)tmp;
					do
					{
						scanf_s("%c", &global.input, 1); 
					} while (global.input == ' ');
					if (global.input != '\n')
						global.multi = true;
					return '!';
				}
				else
				{
					if ((T)tmp <= max)
					{
						value = (T)tmp;
						do
						{
							scanf_s("%c", &global.input, 1); 
						} while (global.input == ' ');
						if (global.input != '\n')
							global.multi = true;
						return '!';
					}
					else
						return '>';
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
							scanf_s("%c", &global.input, 1); 
						} while (global.input == ' ');
						if (global.input != '\n')
							global.multi = true;
						return '!';
					}
					else
					{
						if ((T)tmp <= max)
						{
							value = (T)tmp;
							do
							{
								scanf_s("%c", &global.input, 1); 
							} while (global.input == ' ');
							if (global.input != '\n')
								global.multi = true;
							return '!';
						}
						else
							return '>';
					}
				}
				else
					return '<';
			}
		};
		if (global.input == '-')
		{
			if (allowNegatives)
			{
				char backup = getchar();
				if (backup == '.')
				{
					char backup_2 = peek();
					ungetc(backup, stdin);
					if (backup_2 >= '0' && backup_2 <= '9')
					{
						ungetc(global.input, stdin);
						return rangeTest();
					}
					else
						return global.fillInvalid();
				}
				else
				{
					ungetc(backup, stdin);
					if (backup >= '0' && backup <= '9')
					{
						ungetc(global.input, stdin);
						return rangeTest();
					}
					else
						return global.fillInvalid();
				}
			}
			else
			{
				ungetc(global.input, stdin);
				double tmp;
				scanf_s("%lg", &tmp);
				return '-';
			}
		}
		else if (global.input == '.')
		{
			char backup = peek();
			if (backup >= '0' && backup <= '9')
			{
				ungetc(global.input, stdin);
				return rangeTest();
			}
			else
				return global.fillInvalid();
		}
		else if (global.input >= '0' && global.input <= '9')
		{
			ungetc(global.input, stdin);
			return rangeTest();
		}
		else
		{
			char answer = tolower(global.input);
			if (specials.find(answer) != std::string::npos)
			{
				do
				{
					scanf_s("%c", &global.input, 1);
				} while (global.input == ' ' || global.input == ';');
				if (global.input != '\n')
					global.multi = true;
				return answer;
			}
			else
				return global.fillInvalid();
		}
	}
}

/*
Function for inserting any value from a file
Return values:
'!' - Valid Entry
'q' - User entered the "quit" or 'Q' character
'-' - Input stream given a negative value with allowNegatives set to false
'<' - Input value less than minimum
'>' - Input value greater than maximum
'*' - User entered an invalid character
*/
template<typename T>
extern char valueInsertFromFile(FILE* in, T& value, bool allowNegatives = false, T min = 0, T max = 0)
{
	auto rangeTest = [&]()
	{
		double tmp;
		fscanf_s(in, " %lg", &tmp);
		if (!min)
		{
			if (!max)
			{
				value = (T)tmp;
				return '!';
			}
			else
			{
				if ((T)tmp <= max)
				{
					value = (T)tmp;
					return '!';
				}
				else
					return '>';
			}
		}
		else
		{
			if ((T)tmp >= min)
			{
				if (!max)
				{
					value = (T)tmp;
					return '!';
				}
				else
				{
					if ((T)tmp <= max)
					{
						value = (T)tmp;
						return '!';
					}
					else
						return '>';
				}
			}
			else
				return '<';
		}
	};
	auto inval = [&]()
	{
		global.invalid = "";
		while (global.input != '\n')
		{
			global.invalid += global.input;
			global.input = fgetc(in);
		}
		return '*';
	};
	fscanf_s(in, " %c", &global.input, 1);
	switch (global.input)
	{
	case 'q':
	case 'Q':
		return 'q';
	case '-':
		if (allowNegatives)
		{
			char backup = fgetc(in);
			if (backup == '.')
			{
				char backup_2 = fgetc(in);
				ungetc(backup_2, in);
				ungetc(backup, in);
				if (backup_2 >= '0' && backup_2 <= '9')
				{
					ungetc(global.input, in);
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
					ungetc(global.input, in);
					return rangeTest();
				}
				else
					return inval();
			}
		}
		else
		{
			ungetc(global.input, in);
			double tmp;
			fscanf_s(in, " %lg", &tmp);
			return '-';
		}
	case '.':
		{
			char peek = fgetc(in);
			ungetc(peek, in);
			if (peek >= '0' && peek <= '9')
			{
				ungetc(global.input, in);
				return rangeTest();
			}
			else
				return inval();
		}
	default:
		if (global.input >= '0' && global.input <= '9')
		{
			ungetc(global.input, in);
			return rangeTest();
		}
		else
			return inval();
	}
}

/*
Function for inserting any value from the input stream (with no set custom min or max) [has special characters]
Return values:
'!' - Valid Entry
'q' - User entered the "quit"/'Q' character
'-' - Input stream given a negative value with allowNegatives set to false
'*' - User entered an invalid character
 or one of the special input characters
*/
template<typename T>
extern char valueInsert(T& value, bool allowNegatives, std::string specials)
{
	return valueInsert(value, allowNegatives, T(0), T(0), specials);
}

/*
Function for inserting a list values all in one go from the std::cin input stream
Return values:
'!' -- End of the given list of input values
'?' - User entered the "help"/'?' character
'q' -- User entered the "quit"/'Q' character
'*' - User entered an invalid character before the list ended
or one of the special input characters
*/
extern "C" GLOBALFUNCTIONS_API char listValueInsert(List<size_t>& values, std::string outCharacters, size_t max, bool allowRepeats = true, size_t min = 0);

extern "C" GLOBALFUNCTIONS_API long radiansToDegrees(double angle);

extern GLOBALFUNCTIONS_API std::string angleToFraction(double angle);