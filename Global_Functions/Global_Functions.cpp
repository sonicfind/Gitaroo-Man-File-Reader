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

GlobalVars g_global;

size_t g_dllCount = 0;
FileType::dllPair::~dllPair()
{
	if (m_dll != nullptr)
		FreeLibrary(m_dll);
}

bool FileType::load()
{
	HINSTANCE temp = m_libraries[0].m_dll;
	SetErrorMode(SEM_FAILCRITICALERRORS);
	if (GlobalFunctions::LoadLib(m_libraries[0]))
	{
		//If the library was previously not loaded
		if (temp == nullptr)
			g_dllCount++;
		for (size_t lib = 1; lib < m_libraries.size(); lib++)
			GlobalFunctions::LoadLib(m_libraries[lib]);
		SetErrorMode(0);
		return true;
	}
	else
	{
		SetErrorMode(0);
		return false;
	}
}

void FileType::load(LinkedList::List<const wchar_t*>& errors)
{
	HINSTANCE temp = m_libraries[0].m_dll;
	if (GlobalFunctions::LoadLib(m_libraries[0]))
	{
		// If the library was previously not loaded
		if (temp == nullptr)
			g_dllCount++;
		for (size_t lib = 1; lib < m_libraries.size(); lib++)
		{
			if (!GlobalFunctions::LoadLib(m_libraries[lib]))
			{
				if (GetLastError() == 127)
					errors.push_back(m_libraries[lib].m_name);
			}
		}
	}
	else
	{
		if (GetLastError() == 127)
			errors.push_back(m_libraries[0].m_name);
	}
}

/*
An array that holds data pertaining to every one of the 20 possible extension types.
Extensions are in this order:
0-CHC; 1-IMC; 2-XGM; 3-SSQ; 4-WEB;5-PAK; 6-VAG; 7-SPR; 8-SCL; 9-ANIM;
10-CLR; 11-ROT; 12-INT; 13-FLT; 14-BOL; 15-V2; 16-GIM; 17-PRX; 18-PMF; 19-SFO;
*/
FileType g_dlls[20] =
{
	{{"CHC"},				{L".\\CHC\\CHC_Base.dll", L".\\CHC\\CHC_TAS.dll", L".\\CHC\\CHC_CloneHero.dll"}},
	{{"IMC"},				{L".\\IMC\\IMC_Base.dll"}},
	{{"XGM", "XG", "IMX"},	{L".\\XGM\\XGM_Base.dll"}},
	{{"SSQ"},				{L".\\SSQ\\SSQ_Base.dll"}},
	{{"WEB"},				{L".\\WEB\\WEB_Base.dll"}},
	{{"PAK"},				{L".\\PAK\\PAK_Base.dll"}},
	{{"VAG"},				{L".\\VAG\\VAG_Base.dll"}},
	{{"SPR"},				{L".\\SPR\\SPR_Base.dll"}},
	{{"SCL"},				{L".\\SCL\\SCL_Base.dll"}},
	{{"ANIM"},				{L".\\ANIM\\ANIM_Base.dll"}},
	{{"CLR"},				{L".\\CLR\\CLR_Base.dll"}},
	{{"ROT"},				{L".\\ROT\\ROT_Base.dll"}},
	{{"INT"},				{L".\\INT\\INT_Base.dll"}},
	{{"FLT"},				{L".\\FLT\\FLT_Base.dll"}},
	{{"BOL"},				{L".\\BOL\\BOL_Base.dll"}},
	{{"V2" },				{L".\\V2\\V2_Base.dll"}},
	{{"GIM"},				{L".\\GIM\\GIM_Base.dll"}},
	{{"PRX"},				{L".\\PRX\\PRX_Base.dll"}},
	{{"PMF"},				{L".\\PMF\\PMF_Base.dll"}},
	{{"SFO"},				{L".\\SFO\\SFO_Base.dll"}}
};

namespace GlobalFunctions
{
	int peek()
	{
		char val;
		scanf_s("%c", &val, 1);
		return ungetc(val, stdin);
	}

	void fillInvalid()
	{
		g_global.invalid = "";
		while (g_global.input != '\n')
		{
			g_global.invalid += g_global.input;
			scanf_s("%c", &g_global.input, 1);
		}
		clearIn();
		g_global.multi = false;
	}

	void clearIn()
	{
		char end = g_global.input;
		while (end != '\n' && end != EOF)
			end = getchar();
	}

	void testForMulti()
	{
		do scanf_s("%c", &g_global.input, 1);
		while (g_global.input == ' ' || g_global.input == ';');
		if (g_global.input != '\n')
			g_global.multi = true;
	}

	//If a dll with the given filename is not loaded, attempt to load it and return whether loading was successful
	//Returns true for an already loaded dll
	bool LoadLib(FileType::dllPair& pair)
	{
		if (pair.m_dll == nullptr)
			pair.m_dll = LoadLibrary(pair.m_name);
		return pair.m_dll != nullptr;
	}

	/*
		Searches for a function with the given name (proc) inside the provided dll (lib).
		If found, return the result.
		Otherwise, it will return -1.
		*/
	extern "C" char loadProc(HINSTANCE & lib, const char* proc)
	{
		typedef bool(__cdecl* MYPROC)();
		MYPROC ProcAdd = (MYPROC)GetProcAddress(lib, proc);
		if (ProcAdd)
			return ProcAdd();
		else
		{
			printf("%s%s failed to load\n", g_global.tabs.c_str(), proc);
			return -1;
		}
	}

	//Generates banner
	void banner(string title, float coef)
	{
		size_t sep = title.length() + 5;
		int space = 2 * (int)roundf(32 * coef);
		printf("%s%s#BLM %s\n", string(space - (sep >> 1), '=').c_str(), title.c_str(), string(space + (sep >> 1) - sep, '=').c_str());
	}

	ResultType stringInsertion(string& name, string specials, size_t maxSize)
	{
		if (g_global.multi)
		{
			ungetc(g_global.input, stdin);
			putchar('*');
		}

		do scanf_s("%c", &g_global.input, 1);
		while (g_global.input == ' ');

		switch (g_global.input)
		{
		case ';':
		case '\n':
			printf("%sPlease remember to type SOMETHING before pressing 'Enter'\n", g_global.tabs.c_str());
			printf("%s\n", g_global.tabs.c_str());
			clearIn();
			return ResultType::Failed;
		default:
			switch (peek())
			{
			case '\n': //If the given input is only one character long before reaching either one of the "halt" characters
			case ';':
				switch (g_global.input)
				{
				case 'q':
				case 'Q':
					scanf_s("%c", &g_global.input, 1);
					return ResultType::Quit;
				default:
					if (specials.find(tolower(g_global.input)) != string::npos)
					{
						if (g_global.multi)
						{
							printf("%c\n", toupper(g_global.input));
							g_global.multi = false;
						}
						g_global.answer.character = tolower(g_global.input);
						testForMulti();
						return ResultType::SpecialCase;
					}
				}
				__fallthrough;
			default:
			{
				unsigned quotes = 0;
				while (name.size() < maxSize && quotes != 2 &&
					g_global.input != '\n' && g_global.input != ';')
				{
					if (g_global.multi)
						printf("%c", g_global.input);
					if (g_global.input != '"')
						name += g_global.input;
					else
						quotes++;
					scanf_s("%c", &g_global.input, 1);
				}

				if (g_global.multi)
					printf("\n");

				while (g_global.input == ' ' || g_global.input == ';')
					scanf_s("%c", &g_global.input, 1);

				g_global.multi = g_global.input != '\n';

				return ResultType::Success;
			}
			}
		}
	}

	ResultType charArrayInsertion(char* name, size_t maxSize, std::string specials)
	{
		if (g_global.multi)
		{
			ungetc(g_global.input, stdin);
			putchar('*');
		}

		do scanf_s("%c", &g_global.input, 1);
		while (g_global.input == ' ');

		switch (g_global.input)
		{
		case ';':
		case '\n':
			printf("%sPlease remember to type SOMETHING before pressing 'Enter'\n", g_global.tabs.c_str());
			printf("%s\n", g_global.tabs.c_str());
			clearIn();
			return ResultType::Failed;
		default:
			switch (peek())
			{
			case '\n': //If the given input is only one character long before reaching either one of the "halt" characters
			case ';':
				switch (g_global.input)
				{
				case 'q':
				case 'Q':
					return ResultType::Quit;
				default:
					if (specials.find(tolower(g_global.input)) != string::npos)
					{
						if (g_global.multi)
						{
							printf("%c\n", toupper(g_global.input));
							g_global.multi = false;
						}
						g_global.answer.character = tolower(g_global.input);
						testForMulti();
						return ResultType::SpecialCase;
					}
				}
				__fallthrough;
			default:
			{
				int index = 0;
				while (index < maxSize && g_global.input != '\n' && g_global.input != ';')
				{
					name[index++] = g_global.input;
					scanf_s("%c", &g_global.input, 1);
				}

				if (g_global.multi)
				{
					printf("%s\n", name);
					g_global.multi = false;
				}

				if (index == maxSize)
				{
					switch (g_global.input)
					{
					case '\n':
						break;
					case ';':
					case ' ':
						testForMulti();
						break;
					default:
						printf("\n%sString insertion overflow - Input buffer cleared\n", g_global.tabs.c_str());
						clearIn();
					}
				}
				else
				{
					while (g_global.input == ' ' || g_global.input == ';')
						scanf_s("%c", &g_global.input, 1);

					if (g_global.input != '\n')
						g_global.multi = true;
				}

				return ResultType::Success;
			}
			}
		}
	}

	void dualvfprintf_s(FILE* out1, FILE* out2, const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		vfprintf_s(out1, format, args);
		vfprintf_s(out2, format, args);
		va_end(args);
	}


	ResultType menuChoices(string choices, bool indexMode)
	{
		choices = "q?" + choices;
		printf("%sInput: ", g_global.tabs.c_str());
		if (g_global.multi)
		{
			ungetc(g_global.input, stdin);
			printf("*%c\n", toupper(g_global.input));
			g_global.multi = false;
		}
		do
		{
			scanf_s("%c", &g_global.input, 1);
		} while (g_global.input == ' ' || g_global.input == ';');
		if (g_global.input == '\n')
		{
			printf("%sPlease remember to select a valid option before pressing 'Enter'\n", g_global.tabs.c_str());
			printf("%s\n", g_global.tabs.c_str());
			clearIn();
			return ResultType::Failed;
		}
		g_global.answer.index = choices.find(tolower(g_global.input));
		switch (g_global.answer.index)
		{
		case string::npos:
			GlobalFunctions::fillInvalid();
			printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
			return ResultType::Failed;
		case 0:
			testForMulti();
			return ResultType::Quit;
		case 1:
			testForMulti();
			return ResultType::Help;
		default:
			if (indexMode)
				g_global.answer.index -= 2;
			else
				g_global.answer.character = choices[g_global.answer.index];
			testForMulti();
			return ResultType::Success;
		}	
	}

	/*
		Used to check if a file of the specified name already exists at its location.
		If one exists, it'll provide a prompt where the user can choose whether or not to override the file.
		*/
	ResultType fileOverwriteCheck(string fileName)
	{
#pragma warning(suppress : 4996)
		FILE* test = fopen(fileName.c_str(), "r");
		while (test)
		{
			fclose(test);
			printf("%sOverride/Replace %s? [Y/N][Q to not generate a file]\n", g_global.tabs.c_str(), fileName.c_str());
			switch (menuChoices("yn"))
			{
			case ResultType::Quit:
				return ResultType::Quit;
			case ResultType::Help:
				printf("%sHelp: [TBD]\n%s\n", g_global.tabs.c_str(), g_global.tabs.c_str());
				break;
			case ResultType::Success:
				if (g_global.answer.character == 'n')
					return ResultType::No;
				else
					return ResultType::Yes;
			}
#pragma warning(suppress : 4996)
			test = fopen(fileName.c_str(), "r");
		}
		return ResultType::Yes;
	}

	/*
	Function for inserting a list values all in one go from the std::cin input stream
	Return values:
	'!' -- End of the given list of input values
	'?' - User entered the "help"/'?' character
	'q' -- User entered the "quit"/'Q' character
	GlobalFunctions::ResultType::Failed - User entered an invalid character before the list ended
	or One of the special input characters
	*/
	ResultType listValueInsert(LinkedList::List<size_t>& values, std::string outCharacters, size_t max, bool allowRepeats, size_t min)
	{
		printf("%sInput: ", g_global.tabs.c_str());
		auto printValues = [&](const int type = 0)
		{
			if (g_global.multi)
			{
				for (size_t valIndex = 0; valIndex < values.size(); valIndex++)
					printf("%zu ", values[valIndex]);
				switch (type)
				{
				case 0:
					printf("\n");
					break;
				case 2:
					printf("Q\n");
					break;
				default:
					printf("?\n");
				}
			}
		};
		if (g_global.multi)
		{
			ungetc(g_global.input, stdin);
			std::putchar('*');
		}
		scanf_s("%c", &g_global.input, 1);
		do
		{
			switch (g_global.input)
			{
			case ' ':
				scanf_s("%c", &g_global.input, 1);
				break;
			case '?':
				printValues(1);
				testForMulti();
				return ResultType::Help;
			case 'q':
				printValues(2);
				testForMulti();
				return ResultType::Quit;
			case '\n':
				printValues(0);
				return ResultType::Success;
			case ';':
				printValues(0);
				testForMulti();
				return ResultType::Success;
			default:
				if (g_global.input >= '0' && g_global.input <= '9')
				{
					ungetc(g_global.input, stdin);
					char val[40] = { 0 };
					scanf_s("%39[0-9. ]", val, 40);
					double tmp = atof(val);
					size_t value = size_t(tmp);
					if (value < min || value >= max)
						printf("%s%zu is not within range. Skipping value.\n%s\n", g_global.tabs.c_str(), value, g_global.tabs.c_str());
					else if (allowRepeats || values.search(value) == -1)
						values.push_back(value);
					else
						printf("%s%zu is already in this list.\n%s\n", g_global.tabs.c_str(), value, g_global.tabs.c_str());
					scanf_s("%c", &g_global.input, 1);
				}
				else
				{
					if (outCharacters.find(tolower(g_global.input)) != std::string::npos)
					{
						g_global.answer.character = tolower(g_global.input);
						printValues(0);
						g_global.multi = true;
						return ResultType::SpecialCase;
					}
					else
					{
						GlobalFunctions::fillInvalid();
						printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
						return ResultType::Failed;
					}
				}
			}
		} while (true);
	}

	long radiansToDegrees(float angle)
	{
		return (long)roundf((180 * (angle / M_PI)));
	}

	string angleToFraction(float angle)
	{
		static const int factors[] = { 90, 60, 45, 36, 30, 20, 18, 15, 12, 10, 9, 6, 5, 4, 3, 2 };
		static const char* dems[] = { "2", "3", "4", "5", "6", "9", "10", "12", "15", "18", "20", "30", "36", "45", "60", "90" };
		long degree = radiansToDegrees(angle);
		string fraction;
		if (degree == 0)
			fraction = '0';
		else if (degree == 180)
			fraction = '1';
		else if (degree == -180)
			fraction = "-1";
		else
		{
			if (degree >= 360 || degree <= -360)
				degree %= 360;
			if (degree > 180)
				degree -= 360;
			else if (degree < -180)
				degree += 360;
			for (size_t factor = 0; factor < 16; factor++)
			{
				if (!(degree % factors[factor]))
				{
					fraction = to_string(degree / factors[factor]) + '/' + dems[factor];
					break;
				}
				else if (factor + 1 == 16)
					fraction = to_string(angle / M_PI);
			}
		}
		return fraction;
	}
}
