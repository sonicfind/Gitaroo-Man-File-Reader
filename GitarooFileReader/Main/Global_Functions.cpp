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
#include "pch.h"
#include "Global_Functions.h"
using namespace std;

GlobalVars g_global;
LinkedList::List<FileType*> g_filetypes;

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

	ResultType insertIndexValues(std::vector<size_t>& values, std::string outCharacters, const size_t max, bool allowRepeats, const size_t min)
	{
		printf("%sInput: ", g_global.tabs.c_str());
		auto printValues = [&](const int type = 0)
		{
			if (g_global.multi)
			{
				for (size_t val : values)
					printf("%zu ", val);
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
			putchar('*');
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
					else if (allowRepeats || !checkForIndex(values, value))
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
						fillInvalid();
						printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
						return ResultType::Failed;
					}
				}
			}
		} while (true);
	}

	bool checkForIndex(std::vector<size_t>& values, const size_t value)
	{
		size_t max = values.size(), min = 0;
		while (max > min)
		{
			if (values[min] > value || values[max - 1] < value)
				return false;
			else if (values[min] == value || values[max - 1] == value)
				return true;
			else
			{
				size_t index = (max + min) >> 1;
				if (values[index] == value)
					return true;
				else if (values[index] > value)
					max = index;
				else
					min = index + 1;
			}
		}
		return false;
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
