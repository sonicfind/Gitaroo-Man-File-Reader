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

constexpr float M_PI = 3.141592741f;
const auto processor_count = std::thread::hardware_concurrency();

//Holds all data pertaining to applicable file extensions.
struct FileType
{
	std::list<std::string> m_files;
	virtual const char* getType() = 0;
	virtual const size_t size() = 0;
	virtual bool loadSingle(std::string filename) = 0;
	virtual bool loadMulti() = 0;
};
extern std::vector<FileType*> g_filetypes;

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
extern GlobalVars g_global;

namespace GlobalFunctions
{
	int peek();
	//Pulls remaining characters from the input stream into the invalid string variable
	void fillInvalid();
	void clearIn();
	void testForMulti();

	//Generates banner
	void banner(std::string title, float coef = 1);

	// Used for printing to two files at once
	void dualvfprintf_s(FILE* out1, FILE* out2, const char* format, ...);

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
	ResultType stringInsertion(std::string& name, std::string specials = "", size_t maxSize = SIZE_MAX);

	// Reads a string from the input window, removing certain unnecessary characters from said string
	ResultType charArrayInsertion(char* name, size_t maxSize, std::string specials = "");

	/*
	The basic universal menu system that should be used for almost everything that deals with user choice
	@param choices - List of characters that serves as the possible menu options
	@param indexMode - Determines whether to return the chosen character itself or the index of said character from "choices"
	@return The character that was pulled from the stream, or ResultType::Failed if the choice was invalid. If indexMode is true, return the index from "choices".
	*/
	ResultType menuChoices(std::string choices, bool indexMode = false);

	/*
	Used to check if a file of the specified name already exists at its location.
	If one exists, it'll provide a prompt where the user can choose whether or not to override the file.
	*/
	ResultType fileOverwriteCheck(std::string fileName);

	/*
	Function for inserting any value from the standard input stream
	Success - Valid Entry
	SpecialCase - User entered a special character
	Quit - User entered the "quit" or 'Q' character
	InvalidNegative - Input stream given a negative value with allowNegatives set to false
	MinExceeded - Input value less than minimum
	MaxExceeded - Input value greater than maximum
	Failed - User entered an invalid character
	*/
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
							fillInvalid();
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
							fillInvalid();
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
					fillInvalid();
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
					fillInvalid();
					return ResultType::Failed;
				}
			}
		}
	}

	/*
	Function for inserting any value from a file
	Return values:
	Success - Valid Entry
	Quit - User entered the "quit" or 'Q' character
	InvalidNegative - Input stream given a negative value with allowNegatives set to false
	MinExceeded - Input value less than minimum
	MaxExceeded - Input value greater than maximum
	Failed - User entered an invalid character
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
	Success - Valid Entry
	SpecialCase - User entered a special character
	Quit - User entered the "quit" or 'Q' character
	InvalidNegative - Input stream given a negative value with allowNegatives set to false
	Failed - User entered an invalid character
	*/
	template<typename T>
	extern ResultType valueInsert(T& value, bool allowNegatives, std::string specials)
	{
		return valueInsert(value, allowNegatives, T(0), T(0), specials);
	}

	
	// Function for inserting a vector of values all in one go from the standard input stream
	ResultType insertIndexValues(std::vector<size_t>& values, std::string outCharacters, const size_t max, bool allowRepeats = true, const size_t min = 0);

	template<typename T>
	ResultType indexSelector(std::vector<T>& vect, const char* type)
	{
		printf("%sType the index for the %s that you wish to operate with\n", g_global.tabs.c_str(), type);
		for (size_t i = 0; i < vect.size(); ++i)
			printf("%s%zu - %s\n", g_global.tabs.c_str(), i, vect[i].getName());
		printf("%sInput: ", g_global.tabs.c_str());
		switch (valueInsert(g_global.answer.index, false, size_t(0), vect.size() - 1))
		{
		case ResultType::Quit:
			return ResultType::Quit;
		case ResultType::Success:
			return ResultType::Success;
		case ResultType::InvalidNegative:
			printf("%sGiven index value cannot be negative.\n", g_global.tabs.c_str());
			break;
		case ResultType::MaxExceeded:
			printf("%sGiven index value cannot exceed %zu\n", g_global.tabs.c_str(), vect.size() - 1);
			break;
		case ResultType::Failed:
			printf("%s\"%s\" is not a valid response.\n%s\n", g_global.tabs.c_str(), g_global.invalid.c_str(), g_global.tabs.c_str());
		}
		clearIn();
		return ResultType::Failed;
	}

	bool checkForIndex(std::vector<size_t>& values, const size_t value);

	template<typename T, class... Args>
	size_t emplace_ordered(std::vector<T> vect, Args&&... args)
	{
		T obj(args...);
		size_t max = vect.size(), min = 0, index = 0;
		while (max > min)
		{
			if (vect[min] > obj)
			{
				vect.insert(vect.begin() + min, obj);
				return min;
			}
			else if (vect[max - 1] < obj)
			{
				vect.insert(vect.begin() + max, obj);
				return max;
			}
			else if (vect[min] == obj)
			{
				vect[min] = obj;
				return min;
			}
			else if (vect[max - 1] == obj)
			{
				vect[max - 1] = obj;
				return max - 1;
			}
			else
			{
				index = (max + min) >> 1;
				if (vect[index] == obj)
				{
					vect[index] = obj;
					return index;
				}
				else if (vect[index] > obj)
					max = index;
				else
					min = index + 1;
			}
		}
		vect.insert(vect.begin() + index, obj);
		return index;
	}

	template<typename T>
	size_t insert_ordered(std::vector<T*> vect, T* ptr)
	{
		size_t max = vect.size(), min = 0, index = 0;
		while (max > min)
		{
			if (*vect[min] > *ptr)
			{
				vect.insert(vect.begin() + min, ptr);
				return min;
			}
			else if (*vect[max - 1] < *ptr)
			{
				vect.insert(vect.begin() + max, ptr);
				return max;
			}
			else if (*vect[min] == *ptr)
			{
				*vect[min] = *ptr;
				return min;
			}
			else if (*vect[max - 1] == *ptr)
			{
				*vect[max - 1] = *ptr;
				return max - 1;
			}
			else
			{
				index = (max + min) >> 1;
				if (*vect[index] == *ptr)
				{
					*vect[index] = *ptr;
					return index;
				}
				else if (*vect[index] > *ptr)
					max = index;
				else
					min = index + 1;
			}
		}
		vect.insert(vect.begin() + index, ptr);
		return index;
	}

	template<typename T, class... Args>
	size_t emplace_ordered(std::vector<T*> vect, Args&&... args)
	{
		T* obj = new T(args...);
		size_t max = vect.size(), min = 0, index = 0;
		while (max > min)
		{
			if (*vect[min] > *obj)
			{
				vect.insert(vect.begin() + min, obj);
				return min;
			}
			else if (*vect[max - 1] < *obj)
			{
				vect.insert(vect.begin() + max, obj);
				return max;
			}
			else if (*vect[min] == *obj)
			{
				*vect[min] = *obj;
				delete obj;
				return min;
			}
			else if (*vect[max - 1] == *obj)
			{
				*vect[max - 1] = *obj;
				delete obj;
				return max - 1;
			}
			else
			{
				index = (max + min) >> 1;
				if (vect[index] == *obj)
				{
					*vect[index] = *obj;
					delete obj;
					return index;
				}
				else if (*vect[index] > *obj)
					max = index;
				else
					min = index + 1;
			}
		}
		vect.insert(vect.begin() + index, obj);
		return index;
	}

	template<typename T, class... Args>
	size_t emplace_ordered(std::vector<std::shared_ptr<T>> vect, Args&&... args)
	{
		std::shared_ptr<T> obj = std::make_shared<T>(args...);
		size_t max = vect.size(), min = 0, index = 0;
		while (max > min)
		{
			if (*vect[min] > *obj)
			{
				vect.insert(vect.begin() + min, obj);
				return min;
			}
			else if (*vect[max - 1] < *obj)
			{
				vect.insert(vect.begin() + max, obj);
				return max;
			}
			else if (*vect[min] == *obj)
			{
				*vect[min] = *obj;
				return min;
			}
			else if (*vect[max - 1] == *obj)
			{
				*vect[max - 1] = *obj;
				return max - 1;
			}
			else
			{
				index = (max + min) >> 1;
				if (vect[index] == *obj)
				{
					*vect[index] = *obj;
					return index;
				}
				else if (*vect[index] > *obj)
					max = index;
				else
					min = index + 1;
			}
		}
		vect.insert(vect.begin() + index, obj);
		return index;
	}

	template<typename T>
	void moveElements(std::vector<T> vect, size_t index, size_t newPosition, size_t numElements = 1)
	{
		if (index + numElements > vect.size())
			numElements = vect.size() - index;
		else if (numElements == 0)
			numElements = 1;
		if (newPosition < index)
		{
			for (size_t i = 0; i < numElements; ++i)
			{
				vect.insert(vect.begin() + newPosition + i, vect[index]);
				vect.erase(vect.begin() + index + 1);
			}
		}
		else if (index + numElements < newPosition)
		{
			for (size_t i = 0; i < numElements; ++i)
			{
				vect.insert(vect.begin() + newPosition, vect[index]);
				vect.erase(vect.begin() + index);
			}
		}
	}

	long radiansToDegrees(float angle);

	std::string angleToFraction(float angle);
}
