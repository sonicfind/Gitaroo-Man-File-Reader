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
#include "xgEnvelope.h"
#include "xgVertexInterpolator.h"
#include "xgNormalInterpolator.h"
#include "xgTexCoordInterpolator.h"
#include "xgShapeInterpolator.h"
#include "XGM/Viewer/Shaders.h"
class xgBgGeometry : public XGNode
{
	float m_density = 0;
	ListType<Vertex> m_vertexList;
	std::vector<SharedNode<xgEnvelope>> m_inputEnvelopes;
	SharedNode<xgVertexInterpolator> m_inputVertexInterpolator;
	SharedNode<xgNormalInterpolator> m_inputNormalInterpolator;
	SharedNode<xgTexCoordInterpolator> m_inputTexCoordInterpolator;
	SharedNode<xgShapeInterpolator> m_inputShapeInterpolator;

	bool m_hasBeenAnimated = false;

public:
	using XGNode::XGNode;
	unsigned long read(FILE* inFile, const std::vector<std::unique_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full) const;
	void write_to_txt(FILE* txtFile, const char* tabs = "");
	const char* getType() { return "xgBgGeometry"; }
	static bool compare(const PString& str) { return strcmp("xgBgGeometry", str.m_pstring) == 0; }
	unsigned long getVertexFlags() const { return m_vertexList.m_vertexFlags; }
	size_t getNumVertices() const { return m_vertexList.m_vertices.size(); }

	void positions_to_obj(FILE* objFile) const;
	void texCoords_to_obj(FILE* objFile) const;
	void normals_to_obj(FILE* objFile) const;
	

private:
	unsigned int m_VAO;
	unsigned int m_VBO;

public:
	bool generateVertexBuffer();
	void bindVertexBuffer() const;
	void deleteVertexBuffer();
	ShaderCombo* activateShader() const;
	void restPose() const;
	void animate();
};
