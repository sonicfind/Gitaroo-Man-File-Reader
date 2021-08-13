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
#include "Primitives.h"
Primitives::operator bool()
{
	return m_numPrimitives > 0;
}

void TriElements::set(const xgDagMesh::Data& data)
{
	if (data.m_elementCount)
	{
		glGenBuffers(1, &m_EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned long) * data.m_arraySize, data.m_arrayData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_numPrimitives = data.m_elementCount;
		m_vertCounts = new GLsizei[data.m_elementCount];
		m_indices = new unsigned long* [data.m_elementCount];

		for (int element = 0, arrayIndex = 0; element < m_numPrimitives; ++element, ++arrayIndex)
		{
			m_vertCounts[element] = data.m_arrayData[arrayIndex++];
			m_indices[element] = new unsigned long[m_vertCounts[element]];
			for (int vertex = 0; vertex < m_vertCounts[element]; ++vertex)
				m_indices[element][vertex] = data.m_arrayData[arrayIndex++];
		}
	}
}

void TriArrays::set(const xgDagMesh::Data& data)
{
	if (data.m_elementCount)
	{
		m_numPrimitives = data.m_elementCount;
		m_vertCounts = new GLsizei[data.m_elementCount];
		m_indices = new GLint[data.m_elementCount];
		for (int element = 0, arrayIndex = 1, vertexIndex = data.m_arrayData[0];
			element < m_numPrimitives; ++element, ++arrayIndex)
		{
			m_indices[element] = vertexIndex;
			m_vertCounts[element] = data.m_arrayData[arrayIndex];
			vertexIndex += data.m_arrayData[arrayIndex];
		}
	}
}

void TriElements::draw()
{
	if (m_numPrimitives)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glMultiDrawElements(m_mode, m_vertCounts, GL_UNSIGNED_INT, (void* const*)m_indices, m_numPrimitives);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void TriArrays::draw()
{
	if (m_numPrimitives)
		glMultiDrawArrays(m_mode, m_indices, m_vertCounts, m_numPrimitives);
}

Primitives::~Primitives()
{
	if (m_numPrimitives)
		delete[m_numPrimitives] m_vertCounts;
}

TriElements::~TriElements()
{
	if (m_numPrimitives)
	{
		glDeleteBuffers(1, &m_EBO);
		for (int index = 0; index < m_numPrimitives; ++index)
			delete[m_vertCounts[index]] m_indices[index];
		delete[m_numPrimitives] m_indices;
	}
}

TriArrays::~TriArrays()
{
	if (m_numPrimitives)
		delete[m_numPrimitives] m_indices;
}
