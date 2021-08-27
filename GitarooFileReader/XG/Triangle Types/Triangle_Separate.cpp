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
#include "Triangle_Separate.h"
#include "PString/PString.h"
Triangle_Separate::Triangle_Separate(FILE* inFile)
	: Triangle_Data(inFile)
{
	PString::pull(inFile);
	unsigned long size;
	fread(&size, 4, 1, inFile);
	if (m_counts.size() > 0)
	{
		m_indices.reserve(m_counts.size());
		m_indices.resize(m_counts.size());
		for (size_t i = 0; i < m_counts.size(); ++i)
		{
			fread(&m_counts[i], 4, 1, inFile);
			m_indices[i] = new unsigned long[m_counts[i]];
			fread(m_indices[i], 4, m_counts[i], inFile);
		}
	}
}

Triangle_Separate::~Triangle_Separate()
{
	for (size_t i = 0; i < m_counts.size(); ++i)
		delete[m_counts[i]] m_indices[i];
}

void Triangle_Separate::create(FILE* outFile) const
{
	Triangle_Data::create(outFile);

	// Counting all the "numVert" values
	unsigned long size = (unsigned long)m_counts.size();
	for (const auto& count : m_counts)
		size += count;

	fwrite(&size, 4, 1, outFile);
	for (size_t i = 0; i < m_counts.size(); ++i)
	{
		fwrite(&m_counts[i], 4, 1, outFile);
		fwrite(m_indices[i], 4, m_counts[i], outFile);
	}
}

void Triangle_Separate::write_to_txt(FILE* txtFile, const char* tabs) const
{
	Triangle_Data::write_to_txt(txtFile, tabs);
	for (size_t countIndex = 0; countIndex < m_counts.size(); ++countIndex)
	{
		fprintf_s(txtFile, "\t\t\t\t%s   Group %03zu\n", tabs, countIndex);
		fprintf_s(txtFile, "\t\t\t\t%s       # of Vertex Indexes: %03lu\n", tabs, m_counts[countIndex]);
		for (size_t vertexIndex = 0; vertexIndex < m_counts[countIndex]; ++vertexIndex)
			fprintf_s(txtFile, "\t\t\t\t\t\t%s   Index %03zu: %lu\n", tabs, vertexIndex + 1, m_indices[countIndex][vertexIndex]);
	}
}

std::vector<std::vector<unsigned long>> Triangle_Separate::extract() const
{
	std::vector<std::vector<unsigned long>> indexSets;
	indexSets.reserve(m_counts.size());
	indexSets.resize(m_counts.size());

	for (size_t vectIndex = 0; vectIndex < m_counts.size(); ++vectIndex)
	{
		indexSets[vectIndex].reserve(m_counts[vectIndex]);
		indexSets[vectIndex].resize(m_counts[vectIndex]);
		memcpy(indexSets[vectIndex].data(), m_indices[vectIndex], sizeof(unsigned long) * m_counts[vectIndex]);
	}
	return indexSets;
}
