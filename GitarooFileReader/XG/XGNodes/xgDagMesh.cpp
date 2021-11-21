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
#include "xgDagMesh.h"
#include <glad/glad.h>
unsigned int xgDagMesh::s_matrixUBO = 0;
unsigned long xgDagMesh::read(FILE* inFile, const std::list<std::unique_ptr<XGNode>>& nodeList)
{
	PString::pull(inFile);
	fread(&m_primType, 4, 1, inFile);

	m_prim = std::make_unique<Triangle_Prim>(inFile, 0);
	m_triFan = std::make_unique<Triangle_Fan>(inFile, m_primType);
	m_triStrip = std::make_unique<Triangle_Strip>(inFile, m_primType);
	m_triList = std::make_unique<Triangle_List>(inFile, m_primType);

	PString::pull(inFile);
	fread(&m_cullFunc, 4, 1, inFile);

	int sizechange = 0;
	PString test(inFile);
	while (!strchr(test.m_pstring, '}'))
	{
		if (strstr(test.m_pstring, "inputGeometry"))
		{
			// Removes duplicate geometries
			if (m_inputGeometry)
				sizechange += 30ULL + m_inputGeometry->getName().m_size;
			m_inputGeometry.fill(inFile, nodeList);
		}
		else
		{
			// Removes duplicate materials
			if (m_inputMaterial)
				sizechange += 30 + m_inputMaterial->getName().m_size;
			m_inputMaterial.fill(inFile, nodeList);
		}
		PString::pull(inFile);
		test.fill(inFile);
	}
	return sizechange;
}

void xgDagMesh::create(FILE* outFile) const
{
	XGNode::create(outFile);

	PString::push('{', outFile);
	PString::push("primType", outFile);
	fwrite(&m_primType, 4, 1, outFile);

	m_prim->create(outFile);
	m_triFan->create(outFile);
	m_triStrip->create(outFile);
	m_triList->create(outFile);

	PString::push("cullFunc", outFile);
	fwrite(&m_cullFunc, 4, 1, outFile);

	PString::push("inputGeometry", outFile);
	m_inputGeometry->push(outFile);
	PString::push("outputGeometry", outFile);

	PString::push("inputMaterial", outFile);
	m_inputMaterial->push(outFile);
	PString::push("outputMaterial", outFile);
	PString::push('}', outFile);
}

void xgDagMesh::write_to_txt(FILE* txtFile, const char* tabs) const
{
	XGNode::write_to_txt(txtFile, tabs);

	fprintf_s(txtFile, "\t\t\t%s     PrimType: ", tabs);
	if (m_primType == 4)
		fprintf_s(txtFile, "Kick vertices separately\n");
	else if (m_primType == 5)
		fprintf_s(txtFile, "Kick vertices in groups\n");
	else
		fprintf_s(txtFile, "Who the heck knows\n");

	m_triFan->write_to_txt(txtFile, tabs);
	m_triStrip->write_to_txt(txtFile, tabs);
	m_triList->write_to_txt(txtFile, tabs);

	fprintf_s(txtFile, "\t\t\t%s    CullFunc: ", tabs);
	switch (m_cullFunc)
	{
	case 0:
		fprintf_s(txtFile, "Disabled\n");
		break;
	case 1:
		fprintf_s(txtFile, "Clockwise-winding triangles (CCW is front-facing) [???]\n");
		break;
	case 2:
		fprintf_s(txtFile, "Counterclockwise-winding triangles (CW is front-facing) [???]\n");
	}

	if (m_inputGeometry)
		fprintf_s(txtFile, "\t\t%s      Input Geometry: %s\n", tabs, m_inputGeometry->getName().m_pstring);
	if (m_inputMaterial)
		fprintf_s(txtFile, "\t\t%s      Input Material: %s\n", tabs, m_inputMaterial->getName().m_pstring);
}

const size_t xgDagMesh::getSize() const
{
	size_t size = XGNode::getSize()
		+ PSTRING_LEN_VAR("primType", m_primType)
		+ m_prim->getSize()
		+ m_triFan->getSize()
		+ m_triStrip->getSize()
		+ m_triList->getSize()
		+ PSTRING_LEN_VAR("cullFunc", m_cullFunc);

	if (m_inputGeometry)
		size += m_inputGeometry->getName().getSize() + PSTRING_LEN("inputGeometry") + PSTRING_LEN("outputGeometry");
	if (m_inputMaterial)
		size += m_inputMaterial->getName().getSize() + PSTRING_LEN("inputMaterial") + PSTRING_LEN("outputMaterial");
	return size;
}

void xgDagMesh::queue_for_obj(std::vector<std::pair<size_t, xgBgGeometry*>>& history) const
{
	for (const auto& geo : history)
		if (geo.second == m_inputGeometry.get())
			return;

	history.back().second = m_inputGeometry.get();
	history.push_back({ history.back().first + m_inputGeometry->getVertices().size(), nullptr });
}

void xgDagMesh::faces_to_obj(FILE* objFile, std::vector<std::pair<size_t, xgBgGeometry*>>& history) const
{
	for (const auto& element : history)
	{
		if (element.second == m_inputGeometry.get())
		{
			const bool texture = m_inputGeometry->getVertices().containsTexCoords();
			m_triFan->write_to_obj(objFile, element.first, texture);
			m_triStrip->write_to_obj(objFile, element.first, texture);
			m_triList->write_to_obj(objFile, element.first, texture);
			return;
		}
	}
}

void xgDagMesh::connectTextures(std::vector<IMX>& textures)
{
	if (xgMaterial* mat = m_inputMaterial.get<xgMaterial>())
		mat->connectTexture(textures);
	else if (xgMultiPassMaterial* mat = m_inputMaterial.get<xgMultiPassMaterial>())
		mat->connectTextures(textures);
}

void xgDagMesh::generateMatrixUniform()
{
	glGenBuffers(1, &s_matrixUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, s_matrixUBO);
	glBufferData(GL_UNIFORM_BUFFER, 2048, NULL, GL_DYNAMIC_DRAW);

	// Block 6 is for sprite sizes
	g_shaderList.m_baseShaders.bindUniformBlock(7, "Models");
	g_shaderList.m_boneShaders.bindUniformBlock(7, "Models");

	glBindBufferBase(GL_UNIFORM_BUFFER, 7, s_matrixUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void xgDagMesh::deleteMatrixUniform()
{
	glDeleteBuffers(1, &s_matrixUBO);
	s_matrixUBO = 0;
}

void xgDagMesh::intializeBuffers()
{
	for (auto& mat : m_matrices)
		mat = glm::identity<glm::mat4>();

	// If a buffer was made, then this is a unique geometry node
	m_doGeometryAnimation = m_inputGeometry->generateVertexBuffer();
	m_inputMaterial->intializeBuffers();
	/*m_prim->intializeBuffers();
	m_triFan->intializeBuffers();
	m_triStrip->intializeBuffers();
	m_triList->intializeBuffers();*/
}

void xgDagMesh::deleteBuffers()
{
	m_inputGeometry->deleteVertexBuffer();
	m_inputMaterial->deleteBuffers();
	/*m_prim->deleteBuffers();
	m_triFan->deleteBuffers();
	m_triStrip->deleteBuffers();
	m_triList->deleteBuffers();*/
}

void xgDagMesh::restPose(const glm::mat4 matrix)
{
	if (m_doGeometryAnimation)
		m_inputGeometry->restPose();
	m_matrices[0] = matrix;
}

void xgDagMesh::animate(unsigned long instance, const glm::mat4 matrix)
{
	if (m_doGeometryAnimation)
		m_inputGeometry->animate(instance);
	m_matrices[instance] = matrix;
}

unsigned long xgDagMesh::s_currentCulling = 0;
void xgDagMesh::draw(const unsigned long numInstances, const bool showNormals, const bool doTransparents) const
{
	auto drawTriangles = [&] {
		m_triFan->draw(numInstances);
		m_triStrip->draw(numInstances);
		m_triList->draw(numInstances);
	};

	if (doTransparents == m_inputMaterial->hasTransparency() || showNormals)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, s_matrixUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * numInstances, m_matrices);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		m_inputGeometry->bindVertexBuffer(numInstances);
		if (showNormals)
		{
			m_inputGeometry->activateShader(true);
			drawTriangles();
		}

		if (doTransparents == m_inputMaterial->hasTransparency())
		{
			if (m_cullFunc != s_currentCulling)
			{
				switch (m_cullFunc)
				{
				case 0:
					glDisable(GL_CULL_FACE);
					break;
				case 1:
					if (s_currentCulling == 0)
						glEnable(GL_CULL_FACE);
					glCullFace(GL_BACK);
					break;
				case 2:
					if (s_currentCulling == 0)
						glEnable(GL_CULL_FACE);
					glCullFace(GL_FRONT);
				}
				s_currentCulling = m_cullFunc;
			}

			glActiveTexture(GL_TEXTURE0);
			m_inputGeometry->activateShader(false);
			m_inputMaterial->setShaderValues(0);
			drawTriangles();

			if (m_inputMaterial->getNumMaterials() > 1)
			{
				if (!doTransparents)
					glEnable(GL_BLEND);
				// Allows for multipass materials to be applied to the same vertex
				glDepthFunc(GL_LEQUAL);

				for (int index = 1; index < m_inputMaterial->getNumMaterials(); ++index)
				{
					m_inputMaterial->setShaderValues(index);
					drawTriangles();
				}

				if (!doTransparents)
					glDisable(GL_BLEND);
				glDepthFunc(GL_LESS);
			}

			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

void xgDagMesh::drawAIO(const unsigned long numInstances, const bool showNormals) const
{
	auto drawTriangles = [&] {
		m_triFan->draw(numInstances);
		m_triStrip->draw(numInstances);
		m_triList->draw(numInstances);
	};
	
	bool transparent = m_inputMaterial->hasTransparency();
	glBindBuffer(GL_UNIFORM_BUFFER, s_matrixUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * numInstances, m_matrices);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	m_inputGeometry->bindVertexBuffer(numInstances);
	if (showNormals)
	{
		m_inputGeometry->activateShader(true);
		drawTriangles();
	}

	if (m_cullFunc != s_currentCulling)
	{
		switch (m_cullFunc)
		{
		case 0:
			glDisable(GL_CULL_FACE);
			break;
		case 1:
			if (s_currentCulling == 0)
				glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			break;
		case 2:
			if (s_currentCulling == 0)
				glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
		}
		s_currentCulling = m_cullFunc;
	}

	glActiveTexture(GL_TEXTURE0);
	m_inputGeometry->activateShader(false);
	m_inputMaterial->setShaderValues(0);

	if (transparent)
	{
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		if (!m_updateDepthMask)
			glDepthFunc(GL_ALWAYS);
	}
	drawTriangles();

	if (m_inputMaterial->getNumMaterials() > 1)
	{
		if (m_updateDepthMask)
		{
			if (!transparent)
				glEnable(GL_BLEND);
			// Allows for multipass materials to be applied to the same vertex
			glDepthFunc(GL_LEQUAL);
		}

		for (int index = 1; index < m_inputMaterial->getNumMaterials(); ++index)
		{
			m_inputMaterial->setShaderValues(index);
			drawTriangles();
		}

		if (m_updateDepthMask)
		{
			if (!transparent)
				glDisable(GL_BLEND);
			glDepthFunc(GL_LESS);
		}
	}

	if (transparent)
	{
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		if (!m_updateDepthMask)
			glDepthFunc(GL_LESS);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}
