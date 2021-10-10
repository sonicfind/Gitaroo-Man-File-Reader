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
#include "xgBgGeometry.h"
unsigned long xgBgGeometry::read(FILE* inFile, const std::list<std::unique_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	fread(&m_density, 4, 1, inFile);

	PString::pull(inFile);
	m_vertexList.read(inFile);

	PString test(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		SharedNode<> node(inFile, nodeList);
		if (xgEnvelope* env = node.get<xgEnvelope>())
			m_inputEnvelopes.push_back(env);
		else if (xgShapeInterpolator* shape = node.get<xgShapeInterpolator>())
			m_inputShapeInterpolator = shape;
		else
		{
			// Operator= will handle checking whether the node is valid for the type
			m_inputVertexInterpolator = node;
			m_inputNormalInterpolator = node;
			m_inputTexCoordInterpolator = node;
		}
		PString::pull(inFile);
		test.fill(inFile);
	}
	return 0;
}

void xgBgGeometry::create(FILE* outFile) const
{
	XGNode::create(outFile);

	PString::push('{', outFile);
	PString::push("density", outFile);
	fwrite(&m_density, 4, 1, outFile);
		
	PString::push("vertices", outFile);
	m_vertexList.create(outFile);

	for (auto& node : m_inputEnvelopes)
	{
		PString::push("inputGeometry", outFile);
		node->push(outFile);
		PString::push("outputGeometry", outFile);
	}

	if (m_inputVertexInterpolator)
	{
		PString::push("inputGeometry", outFile);
		m_inputVertexInterpolator->push(outFile);
		PString::push("outputGeometry", outFile);
	}

	if (m_inputNormalInterpolator)
	{
		PString::push("inputGeometry", outFile);
		m_inputNormalInterpolator->push(outFile);
		PString::push("outputGeometry", outFile);
	}

	if (m_inputTexCoordInterpolator)
	{
		PString::push("inputGeometry", outFile);
		m_inputTexCoordInterpolator->push(outFile);
		PString::push("outputGeometry", outFile);
	}

	if (m_inputShapeInterpolator)
	{
		PString::push("inputGeometry", outFile);
		m_inputShapeInterpolator->push(outFile);
		PString::push("outputGeometry", outFile);
	}
	PString::push('}', outFile);
}

void xgBgGeometry::write_to_txt(FILE* txtFile, const char* tabs) const
{
	XGNode::write_to_txt(txtFile, tabs);

	fprintf_s(txtFile, "\t\t\t%s     Density: %g\n", tabs, m_density);
	m_vertexList.write_to_txt(txtFile, "", tabs);

	if (m_inputEnvelopes.size())
	{
		fprintf_s(txtFile, "\t\t%s# of Input Envelopes: %zu\n", tabs, m_inputEnvelopes.size());
		for (size_t index = 0; index < m_inputEnvelopes.size(); ++index)
			fprintf_s(txtFile, "\t\t\t%s   %zu. %s\n", tabs, index + 1, m_inputEnvelopes[index]->getName().m_pstring);
	}
	else if (m_inputShapeInterpolator)
		fprintf_s(txtFile, "%s    Input Shape Interpolator: %s\n", tabs, m_inputShapeInterpolator->getName().m_pstring);
	else
	{
		if (m_inputVertexInterpolator)
			fprintf_s(txtFile, "%s   Input Vertex Interpolator: %s\n", tabs, m_inputVertexInterpolator->getName().m_pstring);

		if (m_inputNormalInterpolator)
			fprintf_s(txtFile, "%s   Input Normal Interpolator: %s\n", tabs, m_inputNormalInterpolator->getName().m_pstring);

		if (m_inputTexCoordInterpolator)
			fprintf_s(txtFile, "%s Input TexCoord Interpolator: %s\n", tabs, m_inputTexCoordInterpolator->getName().m_pstring);
	}
}

const size_t xgBgGeometry::getSize() const
{
	size_t size = XGNode::getSize()
		+ PSTRING_LEN_VAR("density", m_density)
		+ PSTRING_LEN("vertices")
		+ m_vertexList.getFileSize();

	const size_t inputSize = PSTRING_LEN("inputGeometry") + PSTRING_LEN("outputGeometry");
	for (auto& node : m_inputEnvelopes)
		size += inputSize + node->getName().getSize();

	if (m_inputShapeInterpolator)
		size += inputSize + m_inputShapeInterpolator->getName().getSize();
	else
	{
		if (m_inputVertexInterpolator)
			size += inputSize + m_inputVertexInterpolator->getName().getSize();

		if (m_inputNormalInterpolator)
			size += inputSize + m_inputNormalInterpolator->getName().getSize();

		if (m_inputTexCoordInterpolator)
			size += inputSize + m_inputTexCoordInterpolator->getName().getSize();
	}
	return size;
}

void xgBgGeometry::positions_to_obj(FILE* objFile) const
{
	m_vertexList.positions_to_obj(objFile);
}

void xgBgGeometry::texCoords_to_obj(FILE* objFile) const
{
	m_vertexList.texCoords_to_obj(objFile);
}

void xgBgGeometry::normals_to_obj(FILE* objFile) const
{
	m_vertexList.normals_to_obj(objFile);
}

#include <glad/glad.h>
bool xgBgGeometry::generateVertexBuffer()
{
	bool dynamic = m_inputShapeInterpolator || m_inputVertexInterpolator || m_inputNormalInterpolator || m_inputTexCoordInterpolator;
	if (m_vertexList.generateVertexBuffer(m_inputEnvelopes.size() > 0, dynamic))
	{
		// Does nothing if bones aren't used
		for (unsigned long index = 0; index < m_inputEnvelopes.size(); ++index)
			m_inputEnvelopes[index]->bindBoneWeights(index);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		return true;
	}
	return false;
}

void xgBgGeometry::bindVertexBuffer(const size_t numInstances) const
{
	m_vertexList.bind();
	if (m_inputEnvelopes.size())
	{
		xgEnvelope::bindBoneUniform();
		for (unsigned long i = 0; i < m_inputEnvelopes.size(); ++i)
			m_inputEnvelopes[i]->updateBoneMatrices(i, numInstances);
		xgEnvelope::unbindBoneUniform();
	}
}

void xgBgGeometry::deleteVertexBuffer()
{
	m_vertexList.deleteVertexBuffer();
}

void xgBgGeometry::activateShader(const bool normals) const
{
	if (m_inputEnvelopes.size())
	{
		if (!normals)
			g_boneShaders.m_base.use();
		else
			g_boneShaders.m_normals.use();
	}
	else if (!normals)
		g_shaders.m_base.use();
	else
		g_shaders.m_normals.use();
}

void xgBgGeometry::restPose() const
{
	if (m_inputEnvelopes.size() == 0)
		m_vertexList.restPose();
	else
		for (auto& env : m_inputEnvelopes)
			env->restPose();
}

void xgBgGeometry::animate(unsigned long instance)
{
	for (const auto& env : m_inputEnvelopes)
		env->animate(instance);

	if (m_inputShapeInterpolator)
		m_vertexList.replace(m_inputShapeInterpolator->interpolate());

	else if (m_inputVertexInterpolator || m_inputNormalInterpolator || m_inputTexCoordInterpolator)
		m_vertexList.replace(
			m_inputVertexInterpolator ? m_inputVertexInterpolator->interpolate() : std::vector<glm::vec3>(),
			m_inputNormalInterpolator ? m_inputNormalInterpolator->interpolate() : std::vector<glm::vec3>(),
			m_inputTexCoordInterpolator ? m_inputTexCoordInterpolator->interpolate() : std::vector<glm::vec2>()
		);
}
