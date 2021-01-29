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
#include <glad/glad.h>
#include "Shaders.h"
#include <glm/gtc/type_ptr.hpp>
#include "InputHandler.h"
#include "Camera.h"
#include "Viewer.h"
#include <algorithm>

Shader* lightingShader;
Shader* geoShader;
glm::mat4 view, projection;
glm::vec3 lightPos(0, 30, 20);
float g_previousTime;
glm::vec3 lightAmbient(.5, .5, .5);
glm::vec3 lightDiffuse(.5, .5, .5);
glm::vec3 lightSpecular(.2, .2, .2);

bool Viewer::s_showNormals = false;

int Viewer::viewXG(XGM* xgmObject, const std::vector<size_t>& xgIndices)
{
	g_camera.reset(glm::vec3(0.0f, 40.0f, 200.0f));
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(s_SCR_WIDTH, s_SCR_HEIGHT, "XG Viewer", NULL, NULL);
	if (!window)
	{
		printf("Failed to create GLFW window\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to initialize GLAD\n");
		return -1;
	}

	glViewport(0, 0, s_SCR_WIDTH, s_SCR_HEIGHT);
	lightingShader = new Shader;
	geoShader = new Shader("GeoShader.vs", "GeoShader.gs", "GeoShader.fs");
	glfwSetFramebufferSizeCallback(window, InputHandling::framebuffer_size_callback);
	
	m_models.resize(xgIndices.size());
	for (size_t modelIndex = 0; modelIndex < xgIndices.size(); ++modelIndex)
		m_models[modelIndex].load(xgmObject, xgmObject->m_models[xgIndices[modelIndex]]);

	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, InputHandling::mouse_callback);
	glfwSetScrollCallback(window, InputHandling::scroll_callback);

	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	g_previousTime = (float)glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		float currentTime = (float)glfwGetTime();
		InputHandling::processInputs(window, currentTime);

		if (InputHandling::g_input_keyboard.KEY_ESCAPE.isPressed())
			break;

		g_camera.moveCamera(currentTime - g_previousTime);

		if (InputHandling::g_input_keyboard.KEY_N.isPressed())
			showNormals = !showNormals;

		glClearColor(0.2f, 0.5f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		view = g_camera.getViewMatrix();
		projection = glm::perspective(glm::radians(g_camera.m_fov), float(s_SCR_WIDTH) / s_SCR_HEIGHT, 0.1f, 4000.0f);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		for (auto& model : m_models)
			model.draw(currentTime, glm::scale(glm::vec3(1, 1, -1)), showNormals);
		glBindVertexArray(0);

		// Check calls
		glfwSwapBuffers(window);
		glfwPollEvents();
		g_previousTime = currentTime;
	}
		
	GitarooViewer::DagMesh::s_allMeshes.clear();
	delete lightingShader;
	delete geoShader;

	glfwTerminate();
	return 0;
}

std::list<GitarooViewer::DagMesh*> GitarooViewer::DagMesh::s_allMeshes;

GitarooViewer::Model::~Model()
{
	for (auto& dag : m_dags)
		delete dag;
}

GitarooViewer::Dag::~Dag()
{
	if (m_mesh)
		delete m_mesh;
	else
		delete m_transform;
}

GitarooViewer::DagTransform::~DagTransform()
{
	for (auto& dag : m_dags)
		delete dag;
}

GitarooViewer::DagMesh::~DagMesh()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	if (m_fanEBO)
		glDeleteBuffers(1, &m_fanEBO);
	if (m_stripEBO)
		glDeleteBuffers(1, &m_stripEBO);
	if (m_listEBO)
		glDeleteBuffers(1, &m_listEBO);

	for (auto& mat : m_materials)
		glDeleteTextures(1, &mat.texture);
}

void GitarooViewer::Model::load(XGM* xgm, XG& xg)
{
	for (int dagIndex = 0; dagIndex < xg.m_data->m_dag.size(); ++dagIndex)
	{
		Dag* dag = new Dag;
		if (dag->load(xgm, xg.m_data->m_dag[dagIndex]))
			m_dags.push_back(dag);
		else
			m_dags.push_front(dag);
	}
}

bool GitarooViewer::Dag::load(XGM* xgm, XG_Data::DagBase& dagBase)
{
	if (dagBase.m_connected.size())
	{
		m_transform = new DagTransform;
		m_transparency = m_transform->load(xgm, dagBase);
		return m_transparency;
	}
	else
	{
		m_mesh = new DagMesh;
		m_transparency = m_mesh->load(xgm, (xgDagMesh*)dagBase.m_base.m_node);
		return m_transparency;
	}
}

bool GitarooViewer::DagTransform::load(XGM* xgm, XG_Data::DagBase& dagBase)
{
	m_transform = (xgDagTransform*)dagBase.m_base.m_node;
	for (int dagIndex = 0; dagIndex < dagBase.m_connected.size(); ++dagIndex)
	{
		Dag* dag = new Dag;
		if (dag->load(xgm, dagBase.m_connected[dagIndex]))
		{
			m_transparency = true;
			m_dags.push_back(dag);
		}
		else
			m_dags.push_front(dag);
	}
	return m_transparency;
}

bool GitarooViewer::DagMesh::load(XGM* xgm, xgDagMesh* mesh)
{
	bool newGeometry = true, newMaterial = true;
	m_mesh = mesh;
	for (auto& prevMesh : s_allMeshes)
	{
		if (newGeometry && prevMesh->m_mesh->m_inputGeometries.front().m_node == mesh->m_inputGeometries.front().m_node)
		{
			m_vertices = prevMesh->m_vertices;
			m_size = prevMesh->m_size;
			m_vertexSize = prevMesh->m_vertexSize;
			m_VAO = prevMesh->m_VAO;
			m_VBO = prevMesh->m_VBO;
			m_flags = prevMesh->m_flags;
			newGeometry = false;
		}

		if (newMaterial && prevMesh->m_mesh->m_inputMaterials.front().m_node == m_mesh->m_inputMaterials.front().m_node)
		{
			m_materials = prevMesh->m_materials;
			m_transparency = prevMesh->m_transparency;
			newMaterial = false;
		}

		if (!newGeometry && !newMaterial)
			goto Bind_Buffers;
	}

	if (newMaterial)
	{
		XGNode* node = m_mesh->m_inputMaterials.front().m_node;
		if (dynamic_cast<xgMaterial*>(node))
		{
			xgMaterial* mat = (xgMaterial*)node;
			m_materials.push_back({ mat });
			if (mat->m_blendType || mat->m_flags & 1)
				m_transparency = true;
			if (mat->m_inputTextures.size())
			{
				PString textNode = ((xgTexture*)mat->m_inputTextures.front().m_node)->m_imxName;
				for (int index = 0; index < textNode.m_size; ++index)
					textNode.m_pstring[index] = toupper(textNode.m_pstring[index]);
				for (IMX image : xgm->m_textures)
				{
					if (strstr(image.getName(), textNode.m_pstring))
						bindTexture(image, m_materials.back().texture);
				}
			}
		}
		else
		{
			for (auto& shared : ((xgMultiPassMaterial*)node)->m_inputMaterials)
			{
				xgMaterial* mat = (xgMaterial*)shared.m_node;
				m_materials.push_back({ mat });
				if (mat->m_blendType || mat->m_flags & 1)
					m_transparency = true;
				if (mat->m_inputTextures.size())
				{
					PString textNode = mat->m_inputTextures.front()->m_imxName;
					for (int index = 0; index < textNode.m_size; ++index)
						textNode.m_pstring[index] = toupper(textNode.m_pstring[index]);
					for (IMX image : xgm->m_textures)
					{
						if (strstr(image.getName(), textNode.m_pstring))
							bindTexture(image, m_materials.back().texture);
					}
				}
			}
		}
	}

	if (newGeometry)
	{
		xgBgGeometry* geo = m_mesh->m_inputGeometries.front().m_node;
		m_flags = geo->m_vertexFlags;
		glGenBuffers(1, &m_VBO);
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		if (geo->m_vertexFlags & 1) // Position
			m_vertexSize += 4;
		if (geo->m_vertexFlags & 2) // Normal
			m_vertexSize += 3;
		if (geo->m_vertexFlags & 4) // Color
			m_vertexSize += 4;
		if (geo->m_vertexFlags & 8) // Texture Coordinate
			m_vertexSize += 2;

		m_size = m_vertexSize * geo->m_numVerts;
		m_vertices = std::make_shared<float[]>(m_size);
		std::copy(geo->m_vertices, geo->m_vertices + m_size, m_vertices.get());
		glBufferData(GL_ARRAY_BUFFER, 4 * m_size, m_vertices.get(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, m_vertexSize * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, m_vertexSize * sizeof(float), (void*)(4 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		if (geo->m_vertexFlags & 4)
		{
			glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, m_vertexSize * sizeof(float), (void*)(7 * sizeof(float)));
			if (geo->m_vertexFlags & 8)
			{
				glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, m_vertexSize * sizeof(float), (void*)(11 * sizeof(float)));
				glEnableVertexAttribArray(3);
			}
		}
		else
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, m_vertexSize * sizeof(float), (void*)(7 * sizeof(float)));

		glEnableVertexAttribArray(2);
	}

Bind_Buffers:
	if (m_mesh->m_primType == 4)
	{
		if (m_mesh->m_triFanCount)
		{
			glGenBuffers(1, &m_fanEBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_fanEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned long) * m_mesh->m_triFanData.m_arraySize, m_mesh->m_triFanData.m_arrayData, GL_STATIC_DRAW);
		}

		if (m_mesh->m_triStripCount)
		{
			glGenBuffers(1, &m_stripEBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_stripEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned long) * m_mesh->m_triStripData.m_arraySize, m_mesh->m_triStripData.m_arrayData, GL_STATIC_DRAW);
		}

		if (m_mesh->m_triListCount)
		{
			glGenBuffers(1, &m_listEBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_listEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned long) * m_mesh->m_triListData.m_arraySize, m_mesh->m_triListData.m_arrayData, GL_STATIC_DRAW);
		}
	}
	else if (m_mesh->m_primType == 5)
	{
		if (m_mesh->m_triFanCount)
		{
			for (unsigned long valueIndex = 0, index = m_mesh->m_triFanData.m_arrayData[valueIndex++];
				valueIndex < m_mesh->m_triFanData.m_arraySize;)
			{
				const unsigned int numVerts = m_mesh->m_triFanData.m_arrayData[valueIndex++];
				m_groups.push_back({ index, numVerts, GL_TRIANGLE_FAN });
				index += numVerts;
			}
		}

		if (m_mesh->m_triStripCount)
		{
			for (unsigned long valueIndex = 0, index = m_mesh->m_triStripData.m_arrayData[valueIndex++];
				valueIndex < m_mesh->m_triStripData.m_arraySize;)
			{
				const unsigned int numVerts = m_mesh->m_triStripData.m_arrayData[valueIndex++];
				m_groups.push_back({ index, numVerts, GL_TRIANGLE_STRIP });
				index += numVerts;
			}
		}

		if (m_mesh->m_triListCount)
		{
			for (unsigned long valueIndex = 0, index = m_mesh->m_triListData.m_arrayData[valueIndex++];
				valueIndex < m_mesh->m_triListData.m_arraySize;)
			{
				const unsigned int numVerts = m_mesh->m_triListData.m_arrayData[valueIndex++];
				m_groups.push_back({ index, numVerts, GL_TRIANGLES });
				index += numVerts;
			}
		}
	}

	s_allMeshes.push_back(this);
	return m_transparency;
}

void GitarooViewer::DagMesh::bindTexture(const IMX& image, unsigned int& ID)
{
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	if (image.m_data->m_pixelVal2 == 2)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB + image.m_data->m_pixelVal1 - 3, image.m_data->m_width, image.m_data->m_height,
			0, GL_RGB + image.m_data->m_pixelVal1 - 3, GL_UNSIGNED_BYTE, image.m_data->m_colorData->m_image);
	}
	else
	{
		unsigned char(*textureData)[4] = new unsigned char[image.m_data->m_width * image.m_data->m_height][4]();
		if (image.m_data->m_pixelVal1 != 0 || image.m_data->m_pixelVal2 != 0)
		{
			for (unsigned long row = 0, index = 0; row < image.m_data->m_height; ++row)
				for (unsigned long col = 0; col < image.m_data->m_width; ++col, ++index)
					memcpy(textureData[index], image.m_data->m_colorData->m_palette[image.m_data->m_colorData->m_image[index]], 4);
		}
		else
		{
			for (unsigned long row = 0, index = 0; row < image.m_data->m_height; ++row)
			{
				for (unsigned long col = 0; col < image.m_data->m_width; ++col, ++index)
				{
					IMX_Data::Pixel4* pix = (IMX_Data::Pixel4*)&image.m_data->m_colorData->m_image[index >> 1];
					memcpy(textureData[index], image.m_data->m_colorData->m_palette[index & 1 ? pix->pixel2 : pix->pixel1], 4);
				}
			}
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.m_data->m_width, image.m_data->m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
		delete[] textureData;
	}
	glGenerateMipmap(GL_TEXTURE_2D);
}

void GitarooViewer::Model::draw(const float time, glm::mat4 base, const bool showNormals)
{
	for (auto& dag : m_dags)
		dag->draw(time, base, showNormals);
}

void GitarooViewer::Dag::draw(const float time, glm::mat4 base, const bool showNormals)
{
	if (m_transform)
		m_transform->draw(time, base, showNormals);
	else
		m_mesh->draw(time, base, showNormals);
}

void GitarooViewer::DagTransform::draw(const float time, glm::mat4 base, const bool showNormals)
{
	for (auto& dag : m_dags)
		dag->draw(time, base, showNormals);
}

void GitarooViewer::DagMesh::draw(const float time, glm::mat4 model, const bool showNormals)
{
	auto triGroupSort = [&](const DagMesh::TriGroup& group_1, const DagMesh::TriGroup& group_2)
	{
		float dist1 = FLT_MAX;
		for (unsigned long index = 0, max = m_vertexSize * group_1.numVerts;
			index < max; index += m_vertexSize)
		{
			float distance = glm::length(g_camera.m_position -
				glm::vec3(m_vertices[group_1.index + index],
					m_vertices[group_1.index + index + 1],
					-m_vertices[group_1.index + index + 2]));
			if (distance < dist1)
				dist1 = distance;
		}
		float dist2 = FLT_MAX;
		for (unsigned long index = 0, max = m_vertexSize * group_2.numVerts;
			index < max; index += m_vertexSize)
		{
			float distance = glm::length(g_camera.m_position -
				glm::vec3(m_vertices[group_2.index + index],
					m_vertices[group_2.index + index + 1],
					-m_vertices[group_2.index + index + 2]));
			if (distance < dist2)
				dist2 = distance;
		}
		return dist1 < dist2;
	};

	for (auto& mat : m_materials)
	{
		unsigned int shader = mat.mat->m_shadingType;
		lightingShader->use(shader);
		if (mat.texture)
		{
			if (shader >= 3)
				lightingShader->setInt("shadingType", shader + 2);
			else
				lightingShader->setInt("shadingType", shader);
			glBindTexture(GL_TEXTURE_2D, mat.texture);
			lightingShader->setInt("useTexAlpha", mat.mat->m_flags & 1);
		}
		else
			lightingShader->setInt("shadingType", shader);

		lightingShader->setVec4("material.color", (float*)&mat.mat->m_diffuse);
		lightingShader->setVec3("material.specular", (float*)&mat.mat->m_specular);
		lightingShader->setFloat("material.shininess", mat.mat->m_specular.exponent);
		lightingShader->setVec3("light.position", glm::value_ptr(lightPos));
		lightingShader->setVec3("light.ambient", glm::value_ptr(lightAmbient));
		lightingShader->setVec3("light.diffuse", glm::value_ptr(lightDiffuse));
		lightingShader->setVec3("light.specular", glm::value_ptr(lightSpecular));
		lightingShader->setFloat("light.constant", 1.0f);
		lightingShader->setFloat("light.linear", 0.007f);
		lightingShader->setFloat("light.quadratic", 0.0002f);
		lightingShader->setVec3("viewPos", glm::value_ptr(g_camera.m_position));
		lightingShader->setInt("blendingType", mat.mat->m_blendType);
		lightingShader->setMat4("view", glm::value_ptr(view));
		lightingShader->setMat4("projection", glm::value_ptr(projection));
		lightingShader->setMat4("model", glm::value_ptr(model));
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * m_size, m_vertices.get());
		glBindVertexArray(m_VAO);
		switch (mat.mat->m_blendType)
		{
		case 0:
		case 4:
			if (mat.mat->m_flags & 1)
			{
				glEnable(GL_BLEND);
				glBlendEquation(GL_FUNC_ADD);
			}
			else
				glDisable(GL_BLEND);
			break;
		case 1:
		case 5:
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			break;
		case 3:
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_SUBTRACT);
		}

		if (m_mesh->m_primType == 4)
		{
			if (m_fanEBO)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_fanEBO);
				for (unsigned long valueIndex = 0; valueIndex < m_mesh->m_triFanData.m_arraySize;)
				{
					const unsigned long numIndexes = m_mesh->m_triFanData.m_arrayData[valueIndex++];
					glDrawElements(GL_TRIANGLE_FAN, numIndexes, GL_UNSIGNED_INT, (void*)(valueIndex * sizeof(unsigned long)));
					valueIndex += numIndexes;
				}
			}

			if (m_stripEBO)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_stripEBO);
				for (unsigned long valueIndex = 0; valueIndex < m_mesh->m_triStripData.m_arraySize;)
				{
					const unsigned long numIndexes = m_mesh->m_triStripData.m_arrayData[valueIndex++];
					glDrawElements(GL_TRIANGLE_STRIP, numIndexes, GL_UNSIGNED_INT, (void*)(valueIndex * sizeof(unsigned long)));
					valueIndex += numIndexes;
				}
			}

			if (m_listEBO)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_listEBO);
				for (unsigned long valueIndex = 0; valueIndex < m_mesh->m_triListData.m_arraySize;)
				{
					const unsigned long numIndexes = m_mesh->m_triListData.m_arrayData[valueIndex++];
					glDrawElements(GL_TRIANGLES, numIndexes, GL_UNSIGNED_INT, (void*)(valueIndex * sizeof(unsigned long)));
					valueIndex += numIndexes;
				}
			}

			if (showNormals)
			{
				geoShader->use();
				geoShader->setMat4("view", glm::value_ptr(view));
				geoShader->setMat4("projection", glm::value_ptr(projection));
				geoShader->setMat4("model", glm::value_ptr(model));
				if (m_fanEBO)
				{
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_fanEBO);
					for (unsigned long valueIndex = 0; valueIndex < m_mesh->m_triFanData.m_arraySize;)
					{
						const unsigned long numIndexes = m_mesh->m_triFanData.m_arrayData[valueIndex++];
						glDrawElements(GL_TRIANGLE_FAN, numIndexes, GL_UNSIGNED_INT, (void*)(valueIndex * sizeof(unsigned long)));
						valueIndex += numIndexes;
					}
				}

				if (m_stripEBO)
				{
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_stripEBO);
					for (unsigned long valueIndex = 0; valueIndex < m_mesh->m_triStripData.m_arraySize;)
					{
						const unsigned long numIndexes = m_mesh->m_triStripData.m_arrayData[valueIndex++];
						glDrawElements(GL_TRIANGLE_STRIP, numIndexes, GL_UNSIGNED_INT, (void*)(valueIndex * sizeof(unsigned long)));
						valueIndex += numIndexes;
					}
				}

				if (m_listEBO)
				{
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_listEBO);
					for (unsigned long valueIndex = 0; valueIndex < m_mesh->m_triListData.m_arraySize;)
					{
						const unsigned long numIndexes = m_mesh->m_triListData.m_arrayData[valueIndex++];
						glDrawElements(GL_TRIANGLES, numIndexes, GL_UNSIGNED_INT, (void*)(valueIndex * sizeof(unsigned long)));
						valueIndex += numIndexes;
					}
				}
			}
		}
		else if (m_mesh->m_primType == 5)
		{
			if (m_transparency)
				std::sort(m_groups.rbegin(), m_groups.rend(), triGroupSort);

			for (auto& group : m_groups)
				glDrawArrays(group.mode, group.index, group.numVerts);

			if (showNormals)
			{
				geoShader->use();
				geoShader->setMat4("view", glm::value_ptr(view));
				geoShader->setMat4("projection", glm::value_ptr(projection));
				geoShader->setMat4("model", glm::value_ptr(model));
				for (auto& group : m_groups)
					glDrawArrays(group.mode, group.index, group.numVerts);
			}
		}
	}
}
