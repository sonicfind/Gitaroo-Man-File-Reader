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
VertexList::VertexList(const VertexList& a, const VertexList& b, float coefficient)
	: m_vertexFlags(a.m_vertexFlags)
	, m_vertices(a.m_vertices.size())
{
	if (m_vertexFlags & 1)
		for (size_t i = 0; i < a.size(); ++i)
			m_vertices[i].m_position = glm::mix(a[i].m_position, b[i].m_position, coefficient);
	if (m_vertexFlags & 2)
		for (size_t i = 0; i < a.size(); ++i)
			m_vertices[i].m_normal = glm::mix(a[i].m_normal, b[i].m_normal, coefficient);
	if (m_vertexFlags & 4)
		for (size_t i = 0; i < a.size(); ++i)
			m_vertices[i].m_color = glm::mix(a[i].m_color, b[i].m_color, coefficient);
	if (m_vertexFlags & 8)
		for (size_t i = 0; i < a.size(); ++i)
			m_vertices[i].m_texCoord = glm::mix(a[i].m_texCoord, b[i].m_texCoord, coefficient);
}

Vertex& VertexList::operator[](size_t i)
{
	assert(i < m_vertices.size());
	return m_vertices[i];
}

const Vertex& VertexList::operator[](size_t i) const
{
	assert(i < m_vertices.size());
	return m_vertices[i];
}

size_t VertexList::size() const { return m_vertices.size(); }

void VertexList::read(FILE* inFile)
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

void VertexList::create(FILE* outFile) const
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

void VertexList::write_to_txt(FILE* txtFile, const char* tabs_1, const char* tabs_2) const
{
	fprintf_s(txtFile, "\t%s%sVertex Flags: %lu\n", tabs_1, tabs_2, m_vertexFlags);
	fprintf_s(txtFile, "%s%s       # of Vertices: %zu\n", tabs_1, tabs_2, m_vertices.size());
	for (unsigned long index = 0; index < m_vertices.size(); ++index)
	{
		fprintf_s(txtFile, "\t%s%s    Vertex %03lu\n", tabs_1, tabs_2, index + 1);
		const Vertex& vertex = m_vertices[index];
		// Position
		if (m_vertexFlags & 1)
			fprintf_s(txtFile, "\t\t\t%s%sPosition (XYZW): %g, %g, %g, %g\n", tabs_1, tabs_2, vertex.m_position.x, vertex.m_position.y, vertex.m_position.z, vertex.m_position.w);
		// Normal
		if (m_vertexFlags & 2)
			fprintf_s(txtFile, "\t\t\t%s%s   Normal (XYZ): %g, %g, %g\n", tabs_1, tabs_2, vertex.m_normal.x, vertex.m_normal.y, vertex.m_normal.z);
		// Color
		if (m_vertexFlags & 4)
			fprintf_s(txtFile, "\t\t\t%s%s   Color (RGBA): %g, %g, %g, %g\n", tabs_1, tabs_2, vertex.m_color.x, vertex.m_color.y, vertex.m_color.z, vertex.m_color.w);
		// Texture Coordinate
		if (m_vertexFlags & 8)
			fprintf_s(txtFile, "\t\t%s%sTexture Coordinate (ST): %g, %g\n", tabs_1, tabs_2, vertex.m_texCoord.x, vertex.m_texCoord.y);
	}
}

void VertexList::positions_to_obj(FILE* objFile) const
{
	for (const auto& vertex : m_vertices)
		fprintf(objFile, "v %f %f %f\n", vertex.m_position.x, vertex.m_position.y, vertex.m_position.z);
}

void VertexList::texCoords_to_obj(FILE* objFile) const
{
	if (m_vertexFlags & 8)
		for (const auto& vertex : m_vertices)
			fprintf(objFile, "vt %f %f\n", vertex.m_texCoord.x, vertex.m_texCoord.y);
}

void VertexList::normals_to_obj(FILE* objFile) const
{
	for (const auto& vertex : m_vertices)
		fprintf(objFile, "vn %f %f %f\n", vertex.m_normal.x, vertex.m_normal.y, vertex.m_normal.z);
}
