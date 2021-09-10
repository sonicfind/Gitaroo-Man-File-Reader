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
#include "PString.h"
	PString::PString() : m_size(0), m_pstring(nullptr) {}

PString::PString(std::string str)
{
	m_size = (char)str.length();
	m_pstring = new char[m_size + 1ULL]();
	std::copy(str.begin(), str.end(), m_pstring);
}

PString::PString(FILE* inFile)
{
	m_size = fgetc(inFile);
	m_pstring = new char[m_size + 1ULL]();
	fread(m_pstring, 1, m_size, inFile);
}

void PString::fill(const std::string& str)
{
	if (m_pstring != nullptr)
		delete[m_size + 1ULL] m_pstring;
	m_size = (char)str.length();
	m_pstring = new char[m_size + 1ULL]();
	std::copy(str.begin(), str.end(), m_pstring);
}

void PString::fill(FILE* inFile)
{
	if (m_pstring != nullptr)
		delete[m_size + 1ULL] m_pstring;
	m_size = fgetc(inFile);
	m_pstring = new char[m_size + 1ULL]();
	fread(m_pstring, 1, m_size, inFile);
}

void PString::push(FILE* outFile) const
{
	fwrite(&m_size, 1, 1, outFile);
	fwrite(m_pstring, 1, m_size, outFile);
}

const unsigned long PString::getSize() const
{
	return m_size + 1ULL;
}

void PString::pull(FILE* inFile)
{
	// Grabs the number of characters to skip
	const long numCharacters = fgetc(inFile);
	fseek(inFile, numCharacters, SEEK_CUR);
}

void PString::push(const char character, FILE* outFile)
{
	fputc(1, outFile);
	fwrite(&character, 1, 1, outFile);
}

void PString::push(const std::string& str, FILE* outFile)
{
	char size = (char)str.length();
	fputc(size, outFile);
	fwrite(str.c_str(), 1, size, outFile);
}

PString::PString(const PString& str) : m_size(str.m_size), m_pstring(new char[str.m_size + 1ULL]())
{
	std::copy(str.m_pstring, str.m_pstring + m_size, m_pstring);
}

PString& PString::operator=(const PString& str)
{
	if (m_pstring != nullptr)
		delete[m_size + 1ULL] m_pstring;
	m_size = str.m_size;
	m_pstring = new char[m_size + 1ULL]();
	std::copy(str.m_pstring, str.m_pstring + m_size, m_pstring);
	return *this;
}

PString PString::operator+(const std::string& str) const
{
	return PString(m_pstring + str);
}

bool PString::operator==(const PString& str) const
{
	return m_size == str.m_size && strncmp(m_pstring, str.m_pstring, m_size) == 0;
}

bool PString::operator!=(const PString& str) const
{
	return m_size != str.m_size || strncmp(m_pstring, str.m_pstring, m_size) != 0;
}

PString::~PString()
{
	if (m_pstring != nullptr)
		delete[m_size + 1ULL] m_pstring;
}
