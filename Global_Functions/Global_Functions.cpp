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
size_t dllCount = 0;

FileType::dllPair::~dllPair()
{
	if (dll != nullptr)
		FreeLibrary(dll);
}

/*
Takes two {} arrays as inputs for establishing the dll/extension structure for any type.
Names - the list of dlls that the program should attempt to load on startup.
Exts -- the list of file extensions that the first dll in "names" will be applicable for.
*/
FileType::FileType(const initializer_list<const char*>& exts, const initializer_list<const wchar_t*>& names) : extensions(exts), libraries(names)
{
	if (LoadLib(libraries[0]))
	{
		dllCount++;
		for (size_t lib = 1; lib < libraries.size(); lib++)
			LoadLib(libraries[lib]);
	}
}

bool FileType::load()
{
	HINSTANCE temp = libraries[0].dll;
	if (LoadLib(libraries[0]))
	{
		//If the library was previously not loaded
		if (temp == nullptr)
			dllCount++;
		for (size_t lib = 1; lib < libraries.size(); lib++)
			LoadLib(libraries[lib]);
		return true;
	}
	else
		return false;
}

/*
An array that holds data pertaining to every one of the 20 possible extension types.
Extensions are in this order:
0-CHC; 1-IMC; 2-XGM; 3-SSQ; 4-WEB;5-PAK; 6-VAG; 7-SPR; 8-SCL; 9-ANIM;
10-CLR; 11-ROT; 12-INT; 13-FLT; 14-BOL; 15-V2; 16-GIM; 17-PRX; 18-PMF; 19-SFO;
*/
FileType dlls[20] =
{
	{{"CHC"}, {L".\\CHC\\CHC_Base.dll", L".\\CHC\\CHC_TAS.dll", L".\\CHC\\CHC_CloneHero.dll"}},
	{{"IMC"}, {L".\\IMC\\IMC_Base.dll"}},
	{{"XGM", "XG", "IMX"}, {L".\\XGM\\XGM_Base.dll"}},
	{{"SSQ"}, {L".\\SSQ\\SSQ_Base.dll"}},
	{{"WEB"}, {L".\\WEB\\WEB_Base.dll"}},
	{{"PAK"}, {L".\\PAK\\PAK_Base.dll"}},
	{{"VAG"}, {L".\\VAG\\VAG_Base.dll"}},
	{{"SPR"}, {L".\\SPR\\SPR_Base.dll"}},
	{{"SCL"}, {L".\\SCL\\SCL_Base.dll"}},
	{{"ANIM"}, {L".\\ANIM\\ANIM_Base.dll"}},
	{{"CLR"}, {L".\\CLR\\CLR_Base.dll"}},
	{{"ROT"}, {L".\\ROT\\ROT_Base.dll"}},
	{{"INT"}, {L".\\INT\\INT_Base.dll"}},
	{{"FLT"}, {L".\\FLT\\FLT_Base.dll"}},
	{{"BOL"}, {L".\\BOL\\BOL_Base.dll"}},
	{{"V2"}, {L".\\V2\\V2_Base.dll"}},
	{{"GIM"}, {L".\\GIM\\GIM_Base.dll"}}, 
	{{"PRX"}, {L".\\PRX\\PRX_Base.dll"}},
	{{"PMF"}, {L".\\PMF\\PMF_Base.dll"}},
	{{"SFO"}, {L".\\SFO\\SFO_Base.dll"}}
};

GlobalVars global;

int peek()
{
	char val;
	scanf_s("%c", &val, 1);
	return ungetc(val, stdin);
}

void clearIn()
{
	char end = global.input;
	while (end != '\n' && end != EOF)
		end = getchar();
}

//If a dll with the given filename is not loaded, attempt to load it and return whether loading was successful
//Returns true for an already loaded dll
bool LoadLib(FileType::dllPair& pair)
{
	if (pair.dll == nullptr)
		pair.dll = LoadLibrary(pair.name);
	return pair.dll != nullptr;
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
		printf("%s%s failed to load\n", global.tabs.c_str(), proc);
		return -1;
	}
}

//Generates banner
void banner(string title, double coef)
{
	size_t sep = title.length() + 5;
	int space = 2 * (int)round(32 * coef);
	printf("%s%s#BLM %s\n", string(space - (sep >> 1), '=').c_str(), title.c_str(), string(space + (sep >> 1) - sep, '=').c_str());
}

//Adjust pre-fix tab "||"s to the given length
void adjustTabs(char value)
{
	global.tabs = value > 0 ? string(--value, '\t') + "      ||" : "";
}


char filenameInsertion(string& filename, string specials)
{
	if (global.multi)
	{
		ungetc(global.input, stdin);
		putchar('*');
	}
	do
	{
		scanf_s("%c", &global.input, 1); 
	} while (global.input == ' ');
	switch (global.input)
	{
	case ';':
	case '\n':
		printf("%sPlease remember to type SOMETHING before pressing 'Enter'\n", global.tabs.c_str());
		printf("%s\n", global.tabs.c_str());
		clearIn();
		return '*';
	default:
		switch (peek())
		{
		case '\n': //If the given input is only one character long before reaching either one of the "halt" characters
		case ';':
		{
			specials = 'q' + specials;
			if (specials.find(tolower(global.input)) != string::npos)
			{
				char ret = global.input;
				if (global.multi)
					printf("%c\n", toupper(global.input));
				do
				{
					scanf_s("%c", &global.input, 1);
				} while (global.input == ' ' || global.input == ';');
				if (global.input != '\n')
					global.multi = true;
				else
					global.multi = false;
				return ret;
			}
			//Continues to normal filename insertion code if exitVal is unchanged
		}
		default:
		{
			unsigned quotes = 0;
			while (quotes != 2 && global.input != '\n' && global.input != ';')
			{
				if (global.multi)
					printf("%c", global.input);
				if (global.input != '"')
					filename += global.input;
				else
					quotes++;
				scanf_s("%c", &global.input, 1); 
			}
			if (global.multi)
				putchar('\n');
			while (global.input == ' ' || global.input == ';')
				scanf_s("%c", &global.input, 1); 
			if (global.input != '\n')
				global.multi = true;
			else
				global.multi = false;
			return '!';
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


size_t menuChoices(string choices, bool indexMode)
{
	choices = "q?" + choices;
	printf("%sInput: ", global.tabs.c_str());
	if (global.multi)
	{
		ungetc(global.input, stdin);
		printf("*%c\n", toupper(global.input));
		global.multi = false;
	}
	do
	{
		scanf_s("%c", &global.input, 1); 
	} while (global.input == ' ' || global.input == ';');
	if (global.input == '\n')
	{
		printf("%sPlease remember to select a valid option before pressing 'Enter'\n", global.tabs.c_str());
		printf("%s\n", global.tabs.c_str());
		clearIn();
		return '*';
	}
	size_t ret = choices.find(global.input);
	do
	{
		scanf_s("%c", &global.input, 1); 
	} while (global.input == ' ' || global.input == ';');
	if (global.input != '\n')
		global.multi = true;
	if (ret == string::npos)
	{
		global.invalid = global.input;
		do
		{
			switch (global.input)
			{
			case '\n':
				clearIn();
				global.multi = false;
				global.quit = true;
				break;
			default:
				global.invalid += global.input;
				scanf_s("%c", &global.input, 1);
			}
		} while (!global.quit);
		global.quit = false;
		printf("%s\"%s\" is not a valid response.\n%s\n", global.tabs.c_str(), global.invalid.c_str(), global.tabs.c_str());
		ret = '*';
	}
	else if (indexMode && ret >= 2) //Disregards the added "q?" in the index
		ret -= 2;
	else
		ret = choices[ret];
	return ret;
}

/*
	Used to check if a file of the specified name already exists at its location.
	If one exists, it'll provide a prompt where the user can choose whether or not to override the file.
	*/
char fileOverwriteCheck(string fileName)
{
	FILE* test;
	while (!fopen_s(&test, fileName.c_str(), "r"))
	{
		fclose(test);
		printf("%sOverride/Replace %s? [Y/N][C to recheck for file][Q to not generate a file]\n", global.tabs.c_str(), fileName.c_str());
		switch (menuChoices("ync"))
		{
		case 'q':
			return 'q';
		case 'n':
			return 'n';
		case 'y':
			return 'y';
		case '?':
			printf("%sHelp: [TBD]\n%s\n", global.tabs.c_str(), global.tabs.c_str());
		}
	}
	return 'y';
}

/*
Function for inserting a list values all in one go from the std::cin input stream
Return values:
'!' -- End of the given list of input values
'?' - User entered the "help"/'?' character
'q' -- User entered the "quit"/'Q' character
'*' - User entered an invalid character before the list ended
or One of the special input characters
*/
char listValueInsert(List<size_t>& values, std::string outCharacters, size_t max, bool allowRepeats, size_t min)
{
	printf("%sInput: ", global.tabs.c_str());
	if (global.multi)
	{
		ungetc(global.input, stdin);
		std::putchar('*');
	}
	scanf_s("%c", &global.input, 1);
	do
	{
		switch (global.input)
		{
		case ' ':
			scanf_s("%c", &global.input, 1);
			break;
		case '?':
			if (global.multi)
			{
				for (size_t valIndex = 0; valIndex < values.size(); valIndex++)
					printf("%zu ", values[valIndex]);
				printf("?\n");
			}
			do
			{
				scanf_s("%c", &global.input, 1);
			} while (global.input == ' ' || global.input == ';');
			if (global.input != '\n')
				global.multi = true;
			else
				global.multi = false;
			return '?';
		case 'q':
			if (global.multi)
			{
				for (size_t valIndex = 0; valIndex < values.size(); valIndex++)
					printf("%zu ", values[valIndex]);
				printf("Q\n");
			}
			do
			{
				scanf_s("%c", &global.input, 1);
			} while (global.input == ' ' || global.input == ';');
			if (global.input != '\n')
				global.multi = true;
			else
				global.multi = false;
			return 'q';
		case '\n':
			if (global.multi)
			{
				for (size_t valIndex = 0; valIndex < values.size(); valIndex++)
					printf("%zu ", values[valIndex]);
				printf("\n");
				global.multi = false;
			}
			return '!';
		case ';':
			if (global.multi)
			{
				for (size_t valIndex = 0; valIndex < values.size(); valIndex++)
					printf("%zu ", values[valIndex]);
				printf("\n");
			}
			do
			{
				scanf_s("%c", &global.input, 1);
			} while (global.input == ' ');
			if (global.input != '\n')
				global.multi = true;
			return '!';
		default:
			if (global.input >= '0' && global.input <= '9')
			{
				ungetc(global.input, stdin);
				double tmp;
				scanf_s("%lf", &tmp);
				size_t value = size_t(tmp);
				if (value < min || value >= max)
					printf("%s%zu is not within range. Skipping value.\n%s\n", global.tabs.c_str(), value, global.tabs.c_str());
				else if (allowRepeats || values.search(value) == -1)
					values.push_back(value);
				else
					printf("%s%zu is already in this list.\n%s\n", global.tabs.c_str(), value, global.tabs.c_str());
				scanf_s("%c", &global.input, 1);
			}
			else
			{
				char ret = tolower(global.input);
				if (outCharacters.find(ret) != std::string::npos)
				{
					if (global.multi)
					{
						for (size_t valIndex = 0; valIndex < values.size(); valIndex++)
							printf("%zu ", values[valIndex]);
						putchar('\n');
					}
					else
						global.multi = true;
				}
				else
				{
					ret = '*';
					global.multi = false;
					global.invalid = global.input;
					do
					{
						scanf_s("%c", &global.input, 1);
						if (global.input == '\n')
						{
							clearIn();
							global.quit = true;
						}
						else
							global.invalid += global.input;
					} while (!global.quit);
					global.quit = false;
					printf("%s\"%s\" is not a valid response.\n%s\n", global.tabs.c_str(), global.invalid.c_str(), global.tabs.c_str());
				}
				return ret;
			}
		}
	} while (true);
}

long radiansToDegrees(double angle)
{
	return (long)round((180 * (angle / M_PI)));
}

string angleToFraction(double angle)
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
		if(degree >= 360 || degree <= -360)
			degree %= 360;
		if (degree > 180)
			degree -= 360;
		else if (degree < -180)
			degree += 360;
		size_t size = sizeof(factors) / sizeof(int);
		for (size_t factor = 0; factor < size; factor++)
		{
			if (!(degree % factors[factor]))
			{
				fraction = to_string(degree / factors[factor]) + '/' + dems[factor];
				break;
			}
			else if (factor + 1 == size)
				fraction = to_string(angle / M_PI);
		}
	}
	return fraction;
}