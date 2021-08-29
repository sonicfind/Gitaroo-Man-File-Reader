#pragma once
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
#include <glm/glm.hpp>
struct Vertex
{
	glm::vec4 m_position;
	glm::vec3 m_normal;
	glm::vec4 m_color;
	glm::vec2 m_texCoord;
	Vertex mix(const Vertex& nextVertex, const float coefficient, const unsigned long flags) const;
	bool operator==(const Vertex&);
	void position_to_OBJ(FILE* objFile);
	void texCoord_to_OBJ(FILE* objFile);
	void normal_to_OBJ(FILE* objFile);
};

struct BoneVertex
{
	Vertex m_vertex;
	unsigned m_envelope;
	float m_weights[4];
};

template<typename T>
struct ListType
{
	std::vector<T> m_values;
	void read(FILE* inFile)
	{
		unsigned long size;
		fread(&size, 4, 1, inFile);
		m_values.reserve(size);
		m_values.resize(size);
		fread(m_values.data(), sizeof(T), m_values.size(), inFile);
	}

	void create(FILE* outFile) const
	{
		unsigned long size = (unsigned long)m_values.size();
		fwrite(&size, 4, 1, outFile);
		fwrite(m_values.data(), sizeof(T), m_values.size(), outFile);
	}
};

template<>
struct ListType<Vertex>
{
	unsigned long m_vertexFlags = 0;
	std::vector<Vertex> m_vertices;
	void read(FILE* inFile);
	void create(FILE* outFile) const;
};
