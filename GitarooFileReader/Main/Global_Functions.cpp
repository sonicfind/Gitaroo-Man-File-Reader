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
#include "pch.h"
#include "Global_Functions.h"
using namespace std;

GlobalVars g_global;
std::vector<FileType*> g_filetypes;

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
		do
			scanf_s("%c", &g_global.input, 1);
		while (g_global.input == ' ' || g_global.input == ';');
		g_global.multi = g_global.input != '\n';
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
			putchar('*');
		else
		{

			do scanf_s("%c", &g_global.input, 1);
			while (g_global.input == ' ');
		}

		switch (g_global.input)
		{
		case ';':
		case '\n':
			printf_tab("Please remember to type SOMETHING before pressing 'Enter'\n");
			printf_tab("\n");
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
					if (g_global.multi)
						printf("Q\n");

					testForMulti();
					return ResultType::Quit;
				default:
					if (specials.find(tolower(g_global.input)) != string::npos)
					{
						if (g_global.multi)
							printf("%c\n", toupper(g_global.input));

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
			putchar('*');
		else
			scanf_s(" %c", &g_global.input, 1);

		switch (g_global.input)
		{
		case ';':
		case '\n':
			printf_tab("Please remember to type SOMETHING before pressing 'Enter'\n");
			printf_tab("\n");
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
					if (g_global.multi)
					{
						printf("Q\n");
						g_global.multi = false;
					}
					scanf_s("%c", &g_global.input, 1);
					return ResultType::Quit;
				default:
					if (specials.find(tolower(g_global.input)) != string::npos)
					{
						if (g_global.multi)
							printf("%c\n", toupper(g_global.input));

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
					printf("%s\n", name);
				else
					putchar('\n');

				if (index == maxSize)
				{
					switch (g_global.input)
					{
					case '\n':
						g_global.multi = false;
						break;
					case ';':
					case ' ':
						testForMulti();
						break;
					default:
						g_global.multi = false;
						printf_tab("String insertion overflow - Input buffer cleared\n");
						clearIn();
					}
				}
				else
				{
					ungetc(g_global.input, stdin);
					testForMulti();
				}

				return ResultType::Success;
			}
			}
		}
	}

	void printf_tab(const char* format, ...)
	{
		printf("%s", g_global.tabs.c_str());
		va_list args;
		va_start(args, format);
		vprintf_s(format, args);
		va_end(args);
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
		printf_tab("Input: ");

		if (g_global.multi)
			printf("*%c\n", toupper(g_global.input));
		else
		{
			do
			{
				scanf_s(" %c", &g_global.input, 1);
			} while (g_global.input == ';');
		}

		g_global.answer.index = choices.find(tolower(g_global.input));
		if (g_global.answer.index != string::npos)
		{
			testForMulti();
			if (g_global.answer.index == 0)
				return ResultType::Quit;
			else if (g_global.answer.index == 1)
				return ResultType::Help;
			else if (indexMode)
				g_global.answer.index -= 2;
			else
				g_global.answer.character = choices[g_global.answer.index];
			return ResultType::Success;
		}
		else
		{
			fillInvalid();
			printf_tab("\"%s\" is not a valid response.\n%s\n", g_global.invalid.c_str(), g_global.tabs.c_str());
			return ResultType::Failed;
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
			printf_tab("Override/Replace %s? [Y/N][Q to not generate a file]\n", fileName.c_str());
			switch (menuChoices("yn"))
			{
			case ResultType::Quit:
				return ResultType::Quit;
			case ResultType::Help:
				printf_tab("Help: [TBD]\n%s\n", g_global.tabs.c_str());
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
		printf_tab("Input: ");
		auto printValues = [&](const bool writeInput)
		{
			if (g_global.multi)
			{
				for (size_t val : values)
					printf("%zu ", val);

				if (writeInput)
					putchar(g_global.input);
				putchar('\n');
			}
		};

		if (g_global.multi)
			putchar('*');
		else
			scanf_s("%c", &g_global.input, 1);
		do
		{
			switch (g_global.input)
			{
			case ' ':
				scanf_s("%c", &g_global.input, 1);
				break;
			case '?':
			case 'q':
				printValues(true);
				testForMulti();
				if (g_global.input == '?')
					return ResultType::Help;
				else
					return ResultType::Quit;
			case '\n':
				printValues(false);
				g_global.multi = false;
				return ResultType::Success;
			case ';':
				printValues(false);
				testForMulti();
				return ResultType::Success;
			default:
				if (g_global.input >= '0' && g_global.input <= '9')
				{
					ungetc(g_global.input, stdin);

					char val[40] = { g_global.input, 0 };
					scanf_s("%38[0-9.]", val + 1, 39);
					double tmp = atof(val);
					size_t value = size_t(tmp);

					if (value < min || value >= max)
						printf_tab("%zu is not within range. Skipping value.\n%s\n", value, g_global.tabs.c_str());
					else if (allowRepeats || !checkForIndex(values, value))
						values.push_back(value);
					else
						printf_tab("%zu is already in this list.\n%s\n", value, g_global.tabs.c_str());
					scanf_s("%c", &g_global.input, 1);
				}
				else if (outCharacters.find(tolower(g_global.input)) != std::string::npos)
				{
					g_global.answer.character = tolower(g_global.input);
					printValues(false);
					g_global.multi = true;
					return ResultType::SpecialCase;
				}
				else
				{
					fillInvalid();
					printf_tab("\"%s\" is not a valid response.\n%s\n", g_global.invalid.c_str(), g_global.tabs.c_str());
					return ResultType::Failed;
				}
			}
		} while (true);
	}

	bool checkForIndex(std::vector<size_t>& values, const size_t value)
	{
		auto iter = std::lower_bound(values.begin(), values.end(), value);
		return iter != values.end() && *iter == value;
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
		if (degree == 0)
			return "0";
		else if (degree == 180)
			return "1";
		else if (degree == -180)
			return "-1";
		else
		{
			if (degree >= 360 || degree <= -360)
				degree %= 360;

			if (degree > 180)
				degree -= 360;
			else if (degree < -180)
				degree += 360;

			for (size_t factor = 0; factor < 16; factor++)
				if (degree % factors[factor] == 0)
					return to_string(degree / factors[factor]) + '/' + dems[factor];
			return to_string(angle / M_PI);
		}
	}
}
