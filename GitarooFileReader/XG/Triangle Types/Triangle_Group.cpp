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
#include "Triangle_Group.h"
#include "PString/PString.h"
Triangle_Group::Triangle_Group(FILE* inFile)
	: Triangle_Data(inFile)
{
	PString::pull(inFile);
	fseek(inFile, 4, SEEK_CUR);
	if (m_counts.size() > 0)
	{
		m_indices.reserve(m_counts.size());
		m_indices.resize(m_counts.size());

		unsigned long index;
		fread(&index, 4, 1, inFile);
		fread(m_counts.data(), 4, m_counts.size(), inFile);
		for (size_t i = 0; i < m_counts.size(); ++i)
		{
			m_indices[i] = index;
			index += m_counts[i];
		}
	}
}

void Triangle_Group::create(FILE* outFile) const
{
	Triangle_Data::create(outFile);

	unsigned long size = 0;
	if (m_counts.size())
	{
		size = (unsigned long)m_counts.size() + 1;
		fwrite(&size, 4, 1, outFile);
		fwrite(&m_indices.front(), 4, 1, outFile);
		fwrite(m_counts.data(), 4, m_counts.size(), outFile);
	}
	else
		fwrite(&size, 4, 1, outFile);
}

void Triangle_Group::write_to_txt(FILE* txtFile, const char* tabs) const
{
	Triangle_Data::write_to_txt(txtFile, tabs);
	if (m_counts.size())
	{
		fprintf_s(txtFile, "\t\t\t%s       Base Index: %03lu\n", tabs, m_indices.front());
		for (unsigned long index = 0; index < m_counts.size(); ++index)
		{
			fprintf_s(txtFile, "\t\t\t\t%s   Group %03lu\n", tabs, index + 1);
			fprintf_s(txtFile, "\t\t\t\t%s       # of Vertices: %lu\n", tabs, m_counts[index]);
		}
	}
}

std::vector<std::vector<unsigned long>> Triangle_Group::extract() const
{
	std::vector<std::vector<unsigned long>> indexSets;
	indexSets.reserve(m_counts.size());
	indexSets.resize(m_counts.size());

	for (size_t vectIndex = 0; vectIndex < m_counts.size(); ++vectIndex)
	{
		indexSets[vectIndex].reserve(m_counts[vectIndex]);
		indexSets[vectIndex].resize(m_counts[vectIndex]);
		for (unsigned long i = 0; i < m_counts[vectIndex]; ++i)
			indexSets[vectIndex][i] = m_indices[vectIndex] + i;
	}
	return indexSets;
}
