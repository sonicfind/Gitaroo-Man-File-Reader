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

#if defined(_M_X64)
constexpr double M_PI = 3.141592653589793116;
#else
constexpr float M_PI = 3.141592741f;
#endif
const auto processor_count = std::thread::hardware_concurrency();

//Holds all data pertaining to applicable file extensions.
struct FileType
{
	struct dllPair
	{
		const wchar_t* m_name;
		HINSTANCE m_dll = nullptr;
		dllPair() : m_name(nullptr), m_dll(nullptr) {}
		dllPair(const wchar_t* name) : m_name(name), m_dll(nullptr) {}
		//dllPair(dllPair& pair) : name(pair.name), dll(pair.dll) {}
		~dllPair();
	};
	struct extension
	{
		std::string m_ext = "";
		LinkedList::List<std::string> m_files;
		extension() {}
		extension(const char* name) : m_ext(name) {}
	};
	//Libraries[0] will ALWAYS be the base dll
	LinkedList::List<dllPair> m_libraries;
	LinkedList::List<extension> m_extensions;
	/*
	Takes two {} arrays as inputs for establishing the dll/extension structure for any type.
	Names - the list of dlls that the program should attempt to load on startup.
	Exts -- the list of file extensions that the first dll in "names" will be applicable for.
	*/
	FileType(const std::initializer_list<const char*>& exts, const std::initializer_list<const wchar_t*>& names)
		: m_extensions(exts), m_libraries(names) {}
	GLOBALFUNCTIONS_API bool load();
	GLOBALFUNCTIONS_API void load(LinkedList::List<const wchar_t*>& errors);
};
extern "C" GLOBALFUNCTIONS_API size_t g_dllCount;
extern "C" GLOBALFUNCTIONS_API FileType g_dlls[20];

/*
	App-wide structure that holds universal values used by many functions across the solution.

		"quit"  - bool value for ending do/while loops when taking in user inputs. Setting this to true should terminate a loop when used.
	   "input"  - char value that holds a single character from an input stream, usually std::cin. Commonly used to buffer an function to recieve a value of a different type.
	   "multi"  - bool value used for handling multiple menu steps in one go. For example: fixing a CHC file and printing it out as a .txt with only one press of ENTER.
		"tabs"  - std::string used to add empty space at the beginning of most new lines in the console window. Different menus will add or remove space from this std::string depening on scope. Utilizing this can make the console window look more organized.
	  "invalid" - std::string used to capture an entire segment of invalid inputs for any menu screen.
	*/
class GlobalVars
{
	size_t tabCount = 0;
public:
	bool quit = false;
	char input = 0;
	struct Answers
	{
		char character = 0;
		size_t index = 0;
	} answer;
	bool multi = false;
	std::string tabs = "", invalid = "";
	//Adjust pre-fix tab "||"s to the given length
	void adjustTabs(const unsigned value)
	{
		tabCount = value;
		if (tabCount)
			tabs = std::string((tabCount << 3) - 2, '=') + "||";
		else
			tabs.clear();
	}
	void operator++() { tabs = std::string((++tabCount << 3) - 2, '=') + "||"; }
	void operator--()
	{
		if (tabCount)
		{
			if (--tabCount)
				tabs = std::string((tabCount << 3) - 2, '=') + "||";
			else
				tabs.clear();
		}
	}
};
extern "C" GLOBALFUNCTIONS_API GlobalVars g_global;

namespace GlobalFunctions
{
	extern "C" GLOBALFUNCTIONS_API int peek();
	//Pulls remaining characters from the input stream into the invalid string variable
	extern "C" GLOBALFUNCTIONS_API void fillInvalid();
	extern "C" GLOBALFUNCTIONS_API void clearIn();
	extern "C" GLOBALFUNCTIONS_API void testForMulti();

	//Generates banner
	extern "C" GLOBALFUNCTIONS_API void banner(std::string title, double coef = 1);

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
			printf("%s%s failed to load\n", g_global.tabs.c_str(), proc);
			return -1;
		}
	}

	// Used for printing to two files at once
	extern "C" GLOBALFUNCTIONS_API void dualvfprintf_s(FILE * out1, FILE * out2, const char* format, ...);

	enum class ResultType : char
	{
		Success = '!',
		SpecialCase = '+',
		Yes = 'y',
		No = 'n',
		Failed = '*',
		InvalidNegative = '-',
		MinExceeded = '<',
		MaxExceeded = '>',
		Help = '?',
		Quit = 'q',
	};

	// Reads a string from the input window, removing certain unnecessary characters from said string
	extern "C" GLOBALFUNCTIONS_API ResultType stringInsertion(std::string& name, std::string specials = "", size_t maxSize = SIZE_MAX);

	// Reads a string from the input window, removing certain unnecessary characters from said string
	extern "C" GLOBALFUNCTIONS_API ResultType charArrayInsertion(char* name, size_t maxSize, std::string specials = "");

	/*
	The basic universal menu system that should be used for almost everything that deals with user choice
	@param choices - List of characters that serves as the possible menu options
	@param indexMode - Determines whether to return the chosen character itself or the index of said character from "choices"
	@return The character that was pulled from the stream, or GlobalFunctions::ResultType::Failed if the choice was invalid. If indexMode is true, return the index from "choices".
	*/
	extern "C" GLOBALFUNCTIONS_API ResultType menuChoices(std::string choices, bool indexMode = false);

	/*
	Used to check if a file of the specified name already exists at its location.
	If one exists, it'll provide a prompt where the user can choose whether or not to override the file.
	*/
	extern "C" GLOBALFUNCTIONS_API ResultType fileOverwriteCheck(std::string fileName);

	// Function for inserting any value from the standard input stream
	template<typename T>
	extern ResultType valueInsert(T& value, bool allowNegatives = false, T min = 0, T max = 0, std::string specials = "")
	{
		if (g_global.multi)
		{
			ungetc(g_global.input, stdin);
			printf("*%c\n", toupper(g_global.input));
			g_global.multi = false;
		}
		do
		{
			scanf_s("%c", &g_global.input, 1);
		} while (g_global.input == ' ');
		switch (g_global.input)
		{
		case ';':
			scanf_s("%c", &g_global.input, 1);
		case '\n':
			printf("%sPlease remember to type SOMETHING before pressing 'Enter'\n", g_global.tabs.c_str());
			printf("%s\n", g_global.tabs.c_str());
			clearIn();
			return ResultType::Failed;
		case 'q':
		case 'Q':
			testForMulti();
			return ResultType::Quit;
		default:
			auto rangeTest = [&]()
			{
				char val[40] = { 0 };
				scanf_s("%39[0-9. ]", val, 40);
				//scanf_s("%c", &wtf, 1);
				double tmp = atof(val);
				//scanf_s("%lg", &tmp);
				if (!min)
				{
					if (!max)
					{
						value = (T)tmp;
						testForMulti();
						return ResultType::Success;
					}
					else
					{
						if ((T)tmp <= max)
						{
							value = (T)tmp;
							testForMulti();
							return ResultType::Success;
						}
						else
							return ResultType::MaxExceeded;
					}
				}
				else
				{
					if ((T)tmp >= min)
					{
						if (!max)
						{
							value = (T)tmp;
							testForMulti();
							return ResultType::Success;
						}
						else
						{
							if ((T)tmp <= max)
							{
								value = (T)tmp;
								testForMulti();
								return ResultType::Success;
							}
							else
								return ResultType::MaxExceeded;
						}
					}
					else
						return ResultType::MinExceeded;
				}
			};
			if (g_global.input == '-')
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
							ungetc(g_global.input, stdin);
							return rangeTest();
						}
						else
						{
							GlobalFunctions::fillInvalid();
							return ResultType::Failed;
						}
					}
					else
					{
						ungetc(backup, stdin);
						if (backup >= '0' && backup <= '9')
						{
							ungetc(g_global.input, stdin);
							return rangeTest();
						}
						else
						{
							GlobalFunctions::fillInvalid();
							return ResultType::Failed;
						}
					}
				}
				else
				{
					ungetc(g_global.input, stdin);
					double tmp;
					scanf_s("%lg", &tmp);
					return ResultType::InvalidNegative;
				}
			}
			else if (g_global.input == '.')
			{
				char backup = peek();
				if (backup >= '0' && backup <= '9')
				{
					ungetc(g_global.input, stdin);
					return rangeTest();
				}
				else
				{
					GlobalFunctions::fillInvalid();
					return ResultType::Failed;
				}
			}
			else if (g_global.input >= '0' && g_global.input <= '9')
			{
				ungetc(g_global.input, stdin);
				return rangeTest();
			}
			else
			{
				g_global.input = tolower(g_global.input);
				if (specials.find(g_global.input) != std::string::npos)
				{
					testForMulti();
					return ResultType::SpecialCase;
				}
				else
				{
					GlobalFunctions::fillInvalid();
					return ResultType::Failed;
				}
			}
		}
	}

	/*
	Function for inserting any value from a file
	Return values:
	'!' - Valid Entry
	'q' - User entered the "quit" or 'Q' character
	'-' - Input stream given a negative value with allowNegatives set to false
	GlobalFunctions::ResultType::MinExceeded - Input value less than minimum
	GlobalFunctions::ResultType::MaxExceeded - Input value greater than maximum
	GlobalFunctions::ResultType::Failed - User entered an invalid character
	*/
	template<typename T>
	extern ResultType valueInsertFromFile(FILE* in, T& value, bool allowNegatives = false, T min = 0, T max = 0)
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
					return ResultType::Success;
				}
				else
				{
					if ((T)tmp <= max)
					{
						value = (T)tmp;
						return ResultType::Success;
					}
					else
						return ResultType::MaxExceeded;
				}
			}
			else
			{
				if ((T)tmp >= min)
				{
					if (!max)
					{
						value = (T)tmp;
						return ResultType::Success;
					}
					else
					{
						if ((T)tmp <= max)
						{
							value = (T)tmp;
							return ResultType::Success;
						}
						else
							return ResultType::MaxExceeded;
					}
				}
				else
					return ResultType::MinExceeded;
			}
		};
		fscanf_s(in, " %c", &g_global.input, 1);
		switch (g_global.input)
		{
		case 'q':
		case 'Q':
			return ResultType::Quit;
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
						ungetc(g_global.input, in);
						return rangeTest();
					}
					else
					{
						g_global.invalid = "";
						while (g_global.input != '\n')
						{
							g_global.invalid += g_global.input;
							g_global.input = fgetc(in);
						}
						return ResultType::Failed;
					}
				}
				else
				{
					ungetc(backup, in);
					if (backup >= '0' && backup <= '9')
					{
						ungetc(g_global.input, in);
						return rangeTest();
					}
					else
					{
						g_global.invalid = "";
						while (g_global.input != '\n')
						{
							g_global.invalid += g_global.input;
							g_global.input = fgetc(in);
						}
						return ResultType::Failed;
					}
				}
			}
			else
			{
				ungetc(g_global.input, in);
				double tmp;
				fscanf_s(in, " %lg", &tmp);
				return ResultType::InvalidNegative;
			}
		case '.':
		{
			char peek = fgetc(in);
			ungetc(peek, in);
			if (peek >= '0' && peek <= '9')
			{
				ungetc(g_global.input, in);
				return rangeTest();
			}
			else
			{
				g_global.invalid = "";
				while (g_global.input != '\n')
				{
					g_global.invalid += g_global.input;
					g_global.input = fgetc(in);
				}
				return ResultType::Failed;
			}
		}
		default:
			if (g_global.input >= '0' && g_global.input <= '9')
			{
				ungetc(g_global.input, in);
				return rangeTest();
			}
			else
			{
				g_global.invalid = "";
				while (g_global.input != '\n')
				{
					g_global.invalid += g_global.input;
					g_global.input = fgetc(in);
				}
				return ResultType::Failed;
			}
		}
	}

	/*
	Function for inserting any value from the input stream (with no set custom min or max) [has special characters]
	Return values:
	'!' - Valid Entry
	'q' - User entered the "quit"/'Q' character
	'-' - Input stream given a negative value with allowNegatives set to false
	GlobalFunctions::ResultType::Failed - User entered an invalid character
	 or one of the special input characters
	*/
	template<typename T>
	extern ResultType valueInsert(T& value, bool allowNegatives, std::string specials)
	{
		return valueInsert(value, allowNegatives, T(0), T(0), specials);
	}

	template<typename T>
	extern ResultType ListIndexSelector(size_t& index, LinkedList::List<T>& list, const char* type)
	{
		printf("%sType the index for the %s that you wish to operate with\n", g_global.tabs.c_str(), type);
		size_t objIndex = 0;
		for (T& object : list)
			printf("%s%zu - %s\n", g_global.tabs.c_str(), objIndex++, object.getName());
		printf("%sInput: ", g_global.tabs.c_str());
		switch (valueInsert(index, false, size_t(0), list.size() - 1))
		{
		case ResultType::Quit:
			return ResultType::Quit;
		case ResultType::Success:
			return ResultType::Success;
		case ResultType::InvalidNegative:
			printf("%sGiven index value cannot be negative.\n", g_global.tabs.c_str());
			break;
		case ResultType::MaxExceeded:
			printf("%sGiven index value cannot exceed %zu\n", g_global.tabs.c_str(), list.size() - 1);
			break;
		case ResultType::Failed:
			printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
		}
		clearIn();
		return ResultType::Failed;
	}

	/*
	Function for inserting a list values all in one go from the standard input stream
	Return values:
	'!' -- End of the given list of input values
	'?' - User entered the "help"/'?' character
	'q' -- User entered the "quit"/'Q' character
	GlobalFunctions::ResultType::Failed - User entered an invalid character before the list ended
	or one of the special input characters
	*/
	extern "C" GLOBALFUNCTIONS_API ResultType listValueInsert(LinkedList::List<size_t> & values, std::string outCharacters, size_t max, bool allowRepeats = true, size_t min = 0);

	extern "C" GLOBALFUNCTIONS_API long radiansToDegrees(double angle);

	extern GLOBALFUNCTIONS_API std::string angleToFraction(double angle);
}
