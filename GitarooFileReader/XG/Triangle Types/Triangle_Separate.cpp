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
	if (m_numPrimitives > 0)
	{
		m_indices.reserve(m_numPrimitives);
		m_indices.resize(m_numPrimitives);
		unsigned long count;
		for (auto& set : m_indices)
		{
			fread(&count, 4, 1, inFile);
			set.reserve(count);
			set.resize(count);
			fread(set.data(), 4, count, inFile);
		}
	}
}

void Triangle_Separate::create(FILE* outFile) const
{
	// Counting all the "numVert" values
	unsigned long size = m_numPrimitives;
	for (const auto& set : m_indices)
		size += (unsigned long)set.size();

	fwrite(&size, 4, 1, outFile);
	for (const auto& set : m_indices)
	{
		size = (unsigned long)set.size();
		fwrite(&size, 4, 1, outFile);
		fwrite(set.data(), 4, size, outFile);
	}
}

void Triangle_Separate::write_to_txt(FILE* txtFile, const char* tabs) const
{
	Triangle_Data::write_to_txt(txtFile, tabs);
	size_t countIndex = 0;
	for (const auto& set : m_indices)
	{
		fprintf_s(txtFile, "\t\t\t\t%s   Group %03zu\n", tabs, countIndex);
		fprintf_s(txtFile, "\t\t\t\t%s       # of Vertex Indexes: %03zu\n", tabs, set.size());
		for (size_t vertexIndex = 0; vertexIndex < set.size(); ++vertexIndex)
			fprintf_s(txtFile, "\t\t\t\t\t\t%s   Index %03zu: %lu\n", tabs, vertexIndex + 1, set[vertexIndex]);
		++countIndex;
	}
}

const size_t Triangle_Separate::getSize() const
{
	size_t size = Triangle_Data::getSize();
	for (const auto& set : m_indices)
		size += sizeof(unsigned long) * set.size();
	return size;
}

std::vector<std::vector<unsigned long>> Triangle_Separate::extract() const
{
	return m_indices;
}

//void Triangle_Separate::intializeBuffers()
//{
//	if (m_numPrimitives)
//	{
//		generateIBO(sizeof(DrawElementsIndirectCommand));
//
//		unsigned long numIndices = 0;
//		for (const auto& set : m_indices)
//			numIndices += (unsigned long)set.size();
//
//		glGenBuffers(1, &m_EBO);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned long) * numIndices, NULL, GL_STATIC_DRAW);
//		numIndices = 0;
//		for (const auto& set : m_indices)
//		{
//			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
//				sizeof(unsigned long) * numIndices,
//				sizeof(unsigned long) * set.size(),
//				set.data());
//			m_commands.push_back({ (unsigned long)set.size(), 0, numIndices, 0, 0 });
//			numIndices += (unsigned long)set.size();
//		}
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	}
//}
//
//void Triangle_Separate::deleteBuffers()
//{
//	if (m_numPrimitives)
//	{
//		glDeleteBuffers(1, &m_IBO);
//		glDeleteBuffers(1, &m_EBO);
//		m_commands.clear();
//	}
//}

void Triangle_Separate::draw(GLenum mode, unsigned int numInstances)
{
	if (m_numPrimitives)
	{
		for (const auto& set : m_indices)
			glDrawElementsInstanced(mode, (unsigned long)set.size(), GL_UNSIGNED_INT, set.data(), numInstances);

		// Commented out due to what I'm gonna assume are driver errors causing flickering.
		// I use intel uhd graphics, so that may be why.
		// Would be a good idea to see if having this run on more powerful discrete GPUs
		// fixes it.

		/*glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

		for (auto& command : m_commands)
			command.instanceCount = numInstances;
		glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, sizeof(DrawElementsIndirectCommand) * m_numPrimitives, m_commands.data());

		glMultiDrawElementsIndirect(mode, GL_UNSIGNED_INT, 0, m_numPrimitives, 0);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/
	}
}
