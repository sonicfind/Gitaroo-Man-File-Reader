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
	if (m_numPrimitives > 0)
	{
		fread(&m_initialIndex, 4, 1, inFile);
		m_counts.reserve(m_numPrimitives);
		m_counts.resize(m_numPrimitives);
		fread(m_counts.data(), 4, m_numPrimitives, inFile);
	}
}

void Triangle_Group::create(FILE* outFile) const
{
	unsigned long size = 0;
	if (m_numPrimitives)
	{
		size = m_numPrimitives + 1;
		fwrite(&size, 4, 1, outFile);
		fwrite(&m_initialIndex, 4, 1, outFile);
		fwrite(m_counts.data(), 4, m_numPrimitives, outFile);
	}
	else
		fwrite(&size, 4, 1, outFile);
}

void Triangle_Group::write_to_txt(FILE* txtFile, const char* tabs) const
{
	Triangle_Data::write_to_txt(txtFile, tabs);
	if (m_numPrimitives)
	{
		fprintf_s(txtFile, "\t\t\t%s       Base Index: %03lu\n", tabs, m_initialIndex);
		for (unsigned long index = 0; index < m_counts.size(); ++index)
		{
			fprintf_s(txtFile, "\t\t\t\t%s   Group %03lu\n", tabs, index + 1);
			fprintf_s(txtFile, "\t\t\t\t%s       # of Vertices: %lu\n", tabs, m_counts[index]);
		}
	}
}

const size_t Triangle_Group::getSize() const
{
	size_t size = Triangle_Data::getSize();
	if (m_counts.size() > 0)
		size += sizeof(unsigned long);
	return size;
}

std::vector<std::vector<unsigned long>> Triangle_Group::extract() const
{
	std::vector<std::vector<unsigned long>> indexSets;
	indexSets.reserve(m_counts.size());
	indexSets.resize(m_counts.size());
	unsigned long index = m_initialIndex;
	for (size_t vectIndex = 0; vectIndex < m_counts.size(); ++vectIndex)
	{
		indexSets[vectIndex].reserve(m_counts[vectIndex]);
		indexSets[vectIndex].resize(m_counts[vectIndex]);
		for (unsigned long i = 0; i < m_counts[vectIndex]; ++i, ++index)
			indexSets[vectIndex][i] = index;
	}
	return indexSets;
}

//void Triangle_Group::intializeBuffers()
//{
//	if (m_numPrimitives)
//	{
//		generateIBO(sizeof(DrawArraysIndirectCommand));
//		unsigned long index = m_initialIndex;
//		for (const auto& count : m_counts)
//		{
//			m_commands.push_back({ count, 0, index, 0 });
//			index += count;
//		}
//	}
//}
//
//void Triangle_Group::deleteBuffers()
//{
//	if (m_numPrimitives)
//	{
//		glDeleteBuffers(1, &m_IBO);
//		m_commands.clear();
//	}
//}

void Triangle_Group::draw(GLenum mode, unsigned int numInstances)
{
	if (m_numPrimitives)
	{
		unsigned long index = m_initialIndex;
		for (const auto& count : m_counts)
		{
			glDrawArraysInstanced(mode, index, count, numInstances);
			index += count;
		}

		// Commented out due to what I'm gonna assume are driver errors causing flickering.
		// I use intel uhd graphics, so that may be why.
		// Would be a good idea to see if having this run on more powerful discrete GPUs
		// fixes it.

		/*glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_IBO);

		for (auto& command : m_commands)
			command.instanceCount = numInstances;
		glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, sizeof(DrawArraysIndirectCommand) * m_numPrimitives, m_commands.data());

		glMultiDrawArraysIndirect(mode, 0, m_numPrimitives, 0);
		int val = glGetError();
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);*/
	}
}
