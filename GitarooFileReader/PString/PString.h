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

struct PString
{
	char m_size;
	char* m_pstring;
	PString();
	//For List emplacement only
	PString(std::string str);
	//For List emplacement only
	PString(FILE* inFile);
	PString(const PString& str);
	PString& operator=(const PString& str);
	void fill(const std::string& str);
	//Fill in values from a File stream
	void fill(FILE* inFile);
	//Prints values into a file stream
	void push(FILE* outFile) const;
	//Pulls and discards data of the PString format from the given file stream
	static void pull(FILE* inFile);
	//Prints the given character to the given file stream using the PString format
	static void push(const char character, FILE* outFile);
	//Prints the given string to the given file stream using the PString format
	static void push(const std::string& str, FILE* outFile);
	bool operator==(const PString& str) const;
	bool operator!=(const PString& str) const;
	~PString();
};
