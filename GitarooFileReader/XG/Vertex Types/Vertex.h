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
#include <glm/gtx/quaternion.hpp>
struct Vertex
{
	glm::vec4 m_position;
	glm::vec3 m_normal;
	glm::vec4 m_color;
	glm::vec2 m_texCoord;
};

struct BoneVertex : public Vertex
{
	unsigned long m_envelopeIndex;
	float m_weights[4];
};

class VertexList
{
	unsigned long m_vertexFlags;
	std::vector<Vertex> m_vertices;

	unsigned int m_VAO;
	unsigned int m_VBO;
public:
	VertexList() = default;
	VertexList(const VertexList& a, const VertexList& b, float coefficient);
	Vertex& operator[](size_t i);
	const Vertex& operator[](size_t i) const;
	bool containsPositions() const { return m_vertexFlags & 1; }
	bool containsNormals() const { return m_vertexFlags & 2; }
	bool containsColors() const { return m_vertexFlags & 4; }
	bool containsTexCoords() const { return m_vertexFlags & 8; }

	size_t size() const;
	void read(FILE* inFile);
	void create(FILE* outFile) const;
	void write_to_txt(FILE* txtFile, const char* tabs_1, const char* tabs_2) const;

	void positions_to_obj(FILE* objFile) const;
	void texCoords_to_obj(FILE* objFile) const;
	void normals_to_obj(FILE* objFile) const;

	bool generateVertexBuffer(const bool bones, const bool dynamic);
	void deleteVertexBuffer();
	void bind() const;
	void restPose() const;
	void replace(const VertexList& list) const;
	void replace(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texCoords) const;
};

