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
#include "Vertex.h"
void Vertex::position_to_OBJ(FILE* objFile)
{
	fprintf(objFile, "v %f %f %f\n", m_position.x, m_position.y, m_position.z);
}

void Vertex::texCoord_to_OBJ(FILE* objFile)
{
	fprintf(objFile, "vt %f %f\n", m_texCoord.s, m_texCoord.t);
}

void Vertex::normal_to_OBJ(FILE* objFile)
{
	fprintf(objFile, "vn %f %f %f\n", m_normal.x, m_normal.y, m_normal.z);
}

bool Vertex::operator==(const Vertex& vert)
{
	return m_position == vert.m_position;
}

void ListType<Vertex>::read(FILE* inFile)
{
	fread(&m_vertexFlags, 4, 1, inFile);
	
	unsigned long size;
	fread(&size, 4, 1, inFile);
	m_vertices.reserve(size);
	m_vertices.resize(size);

	for (auto& vertex : m_vertices)
	{
		// Position
		if (m_vertexFlags & 1)
			fread(&vertex.m_position, sizeof(float), 4, inFile);
		// Normal
		if (m_vertexFlags & 2)
			fread(&vertex.m_normal, sizeof(float), 3, inFile);
		// Color
		if (m_vertexFlags & 4)
			fread(&vertex.m_color, sizeof(float), 4, inFile);
		// Texture Coordinate
		if (m_vertexFlags & 8)
			fread(&vertex.m_texCoord, sizeof(float), 2, inFile);
	}
}

void ListType<Vertex>::create(FILE* outFile) const
{
	fwrite(&m_vertexFlags, 4, 1, outFile);

	unsigned long size = (unsigned long)m_vertices.size();
	fwrite(&size, 4, 1, outFile);
	for (auto& vertex : m_vertices)
	{
		// Position
		if (m_vertexFlags & 1)
			fwrite(&vertex.m_position, sizeof(float), 4, outFile);
		// Normal
		if (m_vertexFlags & 2)
			fwrite(&vertex.m_normal, sizeof(float), 3, outFile);
		// Color
		if (m_vertexFlags & 4)
			fwrite(&vertex.m_color, sizeof(float), 4, outFile);
		// Texture Coordinate
		if (m_vertexFlags & 8)
			fwrite(&vertex.m_texCoord, sizeof(float), 2, outFile);
	}
}
