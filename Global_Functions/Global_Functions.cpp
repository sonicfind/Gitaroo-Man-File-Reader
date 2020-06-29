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
	FreeLib(dll);
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

//If a dll with the given filename is not loaded, attempt to load it and return whether loading was successful
//Returns true for an already loaded dll
bool LoadLib(FileType::dllPair& pair)
{
	if (pair.dll == nullptr)
		pair.dll = LoadLibrary(pair.name);
	return pair.dll != nullptr;
}

//If a dll with the given filename is loaded, attempt to unload it and return whether unloading was successful
//Returns true for an already unloaded dll
bool FreeLib(HINSTANCE& lib)
{
	if (lib != nullptr)
		FreeLibrary(lib);
	return lib == nullptr;
}

/*
	Searches for a function with the given name (proc) inside the provided dll (lib).
	If found, return the result.
	Otherwise, it will return -1.
	*/
extern "C" char loadProc(HINSTANCE & lib, string proc)
{
	typedef bool(__cdecl* MYPROC)();
	MYPROC ProcAdd = (MYPROC)GetProcAddress(lib, proc.c_str());
	if (ProcAdd)
		return ProcAdd();
	else
	{
		cout << global.tabs << proc << " failed to load" << endl;
		return -1;
	}
}

//Generates banner
void banner(string title, float coef)
{
	size_t sep = title.length();
	int space = 2 * (int)round(32 * coef);
	cout << string(space - (sep >> 1), '=') << title + "#BLM " << string(space + (sep >> 1) - sep, '=') << '\n';
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
		cin.putback(global.input);
		cout << '*';
	}
	do
	{
		cin >> global.input;
	} while (global.input == ' ');
	switch (global.input)
	{
	case ';':
	case '\n':
		cout << global.tabs << "Please remember to type SOMETHING before pressing 'Enter'\n";
		cout << global.tabs << endl;
		cin.sync();
		return -2;
	default:
		switch (cin.peek())
		{
		case '\n': //If the given input is only one character long before reaching either one of the "halt" characters
		case ';':
		{
			char exitVal = -2;
			if (tolower(global.input) == 'q')
				exitVal = -1;
			else
			{
				for (unsigned char index = 0; index < specials.length(); index++)
				{
					if (tolower(global.input) == specials[index])
					{
						exitVal = index + 1;
						break;
					}
				}
			}
			if (exitVal > -2)
			{
				if (global.multi)
					cout << toupper(global.input) << endl;
				do
				{
					cin >> global.input;
				} while (global.input == ' ' || global.input == ';');
				if (global.input != '\n')
					global.multi = true;
				else
					global.multi = false;
				return exitVal;
			}
			//Continues to normal filename insertion code if exitVal is unchanged
		}
		default:
		{
			unsigned quotes = 0;
			while (quotes != 2 && global.input != '\n' && global.input != ';')
			{
				global.input = toupper(global.input);
				if (global.multi)
					cout << global.input;
				if (global.input != '"')
					filename += global.input;
				else
					quotes++;
				cin >> global.input;
			}
			if (global.multi)
				cout << endl;
			while (global.input == ' ' || global.input == ';')
				cin >> global.input;
			if (global.input != '\n')
				global.multi = true;
			else
				global.multi = false;
			return 0;
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

/*
	Universal menu function that accepts a list of characters as a string as option choices
	@param choices - List of characters that serves as the possible menu options
	@return 0+ - Valid entry/Index of the character
	@return -1 - User entered the "quit" or 'Q' character
	@return -2 - User entered an invalid character
	@return -3 - User entered the "help" or '?' character
	*/
size_t menuChoices(string choices)
{
	cin.unsetf(ios::skipws);
	size_t size = choices.length();
	cout << global.tabs << "Input: ";
	if (global.multi)
	{
		cin.putback(global.input);
		cout << '*' << char(toupper(global.input)) << endl;
		global.multi = false;
	}
	do
	{
		cin >> global.input;
	} while (global.input == ' ' || global.input == ';');
	if (global.input == '\n')
	{
		cout << global.tabs << "Please remember to select a valid option before pressing 'Enter'\n";
		cout << global.tabs << endl;
		cin.sync();
		return -2;
	}
	char ans = tolower(global.input);
	do
	{
		cin >> global.input;
	} while (global.input == ' ' || global.input == ';');
	if (global.input != '\n')
		global.multi = true;
	if (ans == '?')
		return -3;
	if (ans == 'q')
		return -1;
	for (size_t index = 0; index < size; index++)
		if (ans == choices[index])
			return index;
	global.invalid = ans;
	do
	{
		switch (global.input)
		{
		case '\n':
			cin.clear();
			global.multi = false;
			global.quit = true;
			break;
		default:
			global.invalid += global.input;
			cin >> global.input;
		}
	} while (!global.quit);
	global.quit = false;
	cout << global.tabs << "\"" << global.invalid << "\" is not a valid response." << endl << global.tabs << endl;
	return -2;
}

/*
	Used to check if a file of the specified name already exists at its location.
	If one exists, it'll provide a prompt where the user can choose whether or not to override the file.
	*/
char fileOverwriteCheck(string fileName)
{
	FILE* test;
	while (!global.quit && !fopen_s(&test, fileName.c_str(), "r"))
	{
		fclose(test);
		cout << global.tabs << "Override/Replace " << fileName << "? [Y/N][C to recheck for file][Q to not generate a file]\n";
		switch (menuChoices("ync"))
		{
		case -1:
			return -1;
		case 1:
			return 0;
		case 0:
			global.quit = true;
		}
	}
	global.quit = false;
	return 1;
}

long radiansToDegrees(double angle)
{
	return (long)round((180 * (angle / M_PI)));
}

string angleToFraction(double angle)
{
	static const int factors[] = { 90, 60, 45, 36, 30, 20, 18, 15, 12, 10, 9, 6, 5, 4, 3, 2 };
	static const char* dems[] = { "2", "3", "4", "5", "6", "9", "10", "12", "15", "18", "20", "30", "45", "60", "90" };
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