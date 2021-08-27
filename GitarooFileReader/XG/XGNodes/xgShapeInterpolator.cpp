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
#include "xgShapeInterpolator.h"
unsigned long xgShapeInterpolator::read(FILE* inFile, const std::vector<std::unique_ptr<XGNode>>& nodeList)
{
	xgTimedInterpolator::read(inFile, nodeList);

	unsigned long sizechange = 0;
	PString test(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		// Removes duplicate xgTime node pointers
		if (m_inputTime)
			sizechange += 22 + m_inputTime->getName().m_size;
		m_inputTime.fill(inFile, nodeList);
		PString::pull(inFile);
		test.fill(inFile);
	}
	return sizechange;
}

void xgShapeInterpolator::create(FILE* outFile, bool full) const
{
	PString::push("xgShapeInterpolator", outFile);
	m_name.push(outFile);
	if (full)
	{
		PString::push('{', outFile);
		xgTimedInterpolator::create(outFile, full);

		PString::push("inputTime", outFile);
		m_inputTime->push(outFile);
		PString::push("outputTime", outFile);
		PString::push('}', outFile);
	}
	else
		PString::push(';', outFile);
}

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
void xgShapeInterpolator::replaceVertexData() const
{
	const ListType<Vertex> list = interpolate();
	for (size_t i = 0; i < list.m_vertices.size(); ++i)
	{
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * i, sizeof(Vertex), glm::value_ptr(list.m_vertices[i].m_position));
		/*size_t offset = 0;
		if (list.m_vertexFlags & 1)
		{
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * i, sizeof(glm::vec4), glm::value_ptr(list.m_vertices[i].m_position));
			offset += 4;
		}

		if (list.m_vertexFlags & 2)
		{
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * i + offset, sizeof(glm::vec3), glm::value_ptr(list.m_vertices[i].m_normal));
			offset += 3;
		}

		if (list.m_vertexFlags & 4)
		{
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * i + offset, sizeof(glm::vec4), glm::value_ptr(list.m_vertices[i].m_color));
			offset += 4;
		}

		if (list.m_vertexFlags & 8)
		{
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * i + offset, sizeof(glm::vec2), glm::value_ptr(list.m_vertices[i].m_texCoord));
			offset += 2;
		}*/
	}
}
