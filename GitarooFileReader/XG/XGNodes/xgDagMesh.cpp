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

void xgDagMesh::intializeBuffers()
{
	// If a buffer was made, then this is a unique geometry node
	m_doGeometryAnimation = m_inputGeometry->generateVertexBuffer();
	m_inputMaterial->intializeBuffers();
}

void xgDagMesh::deleteBuffers()
{
	m_inputGeometry->deleteVertexBuffer();
	m_inputMaterial->deleteBuffers();
}

void xgDagMesh::restPose() const
{
	if (m_doGeometryAnimation)
		m_inputGeometry->restPose();
}

void xgDagMesh::animate(unsigned long instance)
{
	if (m_doGeometryAnimation)
		m_inputGeometry->animate(instance);
}

#include "XGM/Viewer/Camera.h"
#include <glm/gtc/type_ptr.hpp>
unsigned long xgDagMesh::s_currentCulling = 0;
void xgDagMesh::draw(const glm::mat4 view, const glm::mat4* models, const unsigned long numInstances, const bool showNormals, const bool doTransparents) const
{
	static const std::string indices[] =
	{
		"[0]","[1]","[2]","[3]","[4]","[5]","[6]","[7]","[8]","[9]","[10]","[11]","[12]","[13]","[14]","[15]",
		"[16]","[17]","[18]","[19]","[20]","[21]","[22]","[23]","[24]","[25]","[26]","[27]","[28]","[29]","[30]","[31]"
	};

	if (doTransparents == m_inputMaterial->hasTransparency())
	{
		ShaderCombo* active = m_inputGeometry->activateShader();
		active->m_base.setInt("doMulti", 0);
		glActiveTexture(GL_TEXTURE0);
		m_inputMaterial->setShaderValues(&active->m_base, indices[0]);

		active->m_base.setVec3("lightPosition", glm::value_ptr(g_camera.m_position));
		active->m_base.setVec3("viewPos", glm::value_ptr(g_camera.m_position));
		std::vector<glm::mat3> normals;
		for (size_t i = 0; i < numInstances; ++i)
		{
			active->m_base.setMat4("models" + indices[i], (float*)glm::value_ptr(models[i]));
			normals.push_back(glm::mat3(glm::transpose(glm::inverse(view * models[i]))));
			active->m_base.setMat3("normalMatrices" + indices[i], glm::value_ptr(normals[i]));
		}
		m_inputGeometry->bindVertexBuffer(numInstances);

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
		
		m_triFan->draw(GL_TRIANGLE_FAN);
		m_triStrip->draw(GL_TRIANGLE_STRIP);
		m_triList->draw(GL_TRIANGLES);

		glBindTexture(GL_TEXTURE_2D, 0);
		if (showNormals)
		{
			active->m_geometry.use();
			for (size_t i = 0; i < numInstances; ++i)
			{
				active->m_geometry.setMat4("models" + indices[i], (float*)glm::value_ptr(models[i]));
				active->m_geometry.setMat3("normalMatrices" + indices[i], glm::value_ptr(normals[i]));
			}

			m_triFan->draw(GL_TRIANGLE_FAN);
			m_triStrip->draw(GL_TRIANGLE_STRIP);
			m_triList->draw(GL_TRIANGLES);
		}
	}
}
