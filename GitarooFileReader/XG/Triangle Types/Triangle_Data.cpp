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
	fread(&m_numPrimitives, 4, 1, inFile);
}

Triangle_Data::Triangle_Data(const unsigned long numPrimitives)
	: m_numPrimitives(numPrimitives) {}

void Triangle_Data::createCount(FILE* outFile) const
{
	fwrite(&m_numPrimitives, 4, 1, outFile);
}

void Triangle_Data::write_to_txt(FILE* txtFile, const char* tabs) const
{	
	fprintf_s(txtFile, "\t\t\t\t%s    Count: %lu\n", tabs, m_numPrimitives);
}

const size_t Triangle_Data::getSize() const
{
	return sizeof(unsigned long) * m_numPrimitives;
}

std::vector<std::vector<unsigned long>> Triangle_Data::extract() const
{
	return std::vector<std::vector<unsigned long>>(m_numPrimitives);
}

//void Triangle_Data::generateIBO(const size_t structSize)
//{
//	glGenBuffers(1, &m_IBO);
//	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_IBO);
//	glBufferData(GL_DRAW_INDIRECT_BUFFER, structSize * m_numPrimitives, NULL, GL_STATIC_DRAW);
//	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
//}
