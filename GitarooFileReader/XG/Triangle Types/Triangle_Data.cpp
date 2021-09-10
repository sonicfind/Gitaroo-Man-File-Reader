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
#include "Triangle_Data.h"
#include "PString/PString.h"
Triangle_Data::Triangle_Data(FILE* inFile)
{
	PString::pull(inFile);
	unsigned long size;
	fread(&size, 4, 1, inFile);
	if (size)
	{
		m_counts.reserve(size);
		m_counts.resize(size);
	}
}

Triangle_Data::Triangle_Data(const std::vector<unsigned long>& counts)
	: m_counts(counts) {}

void Triangle_Data::create(FILE* outFile, bool writeData) const
{
	if (!writeData)
	{
		unsigned long size = (unsigned long)m_counts.size();
		fwrite(&size, 4, 1, outFile);
	}
	else
		fwrite("\0\0\0\0", 1, 4, outFile);
}

void Triangle_Data::write_to_txt(FILE* txtFile, const char* tabs) const
{	
	fprintf_s(txtFile, "\t\t\t\t%s    Count: %zu\n", tabs, m_counts.size());
}

std::vector<std::vector<unsigned long>> Triangle_Data::extract() const
{
	return std::vector<std::vector<unsigned long>>(m_counts.size());
}

const size_t Triangle_Data::getSize() const
{
	return sizeof(unsigned long) * m_counts.size();
}
