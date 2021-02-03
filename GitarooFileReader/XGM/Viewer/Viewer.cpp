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
#include "Viewer.h"
#include "InputHandler.h"
#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

Viewer::Viewer()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_window = glfwCreateWindow(s_SCR_WIDTH, s_SCR_HEIGHT, "XG Viewer", NULL, NULL);
	if (!m_window)
	{
		glfwTerminate();
		throw "Failed to create GLFW window";
	}
	glfwMakeContextCurrent(m_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw "Failed to initialize GLAD";

	glViewport(0, 0, s_SCR_WIDTH, s_SCR_HEIGHT);

	g_baseShader.createProgram("Vertex.glsl", "Fragment.glsl");
	g_boneShader.createProgram("Vertex - Bones.glsl", "Fragment.glsl");
	g_shapeShader.createProgram("Vertex - Shapes.glsl", "Fragment.glsl");
	g_baseGeometryShader.createProgram("Geo - Vertex.glsl", "Geo - Geometry.glsl", "Geo - Fragment.glsl");
	g_boneGeometryShader.createProgram("Geo - Vertex - Bones.glsl", "Geo - Geometry.glsl", "Geo - Fragment.glsl");
	g_shapeGeometryShader.createProgram("Geo - Vertex - Shapes.glsl", "Geo - Geometry.glsl", "Geo - Fragment.glsl");

	glfwSetFramebufferSizeCallback(m_window, InputHandling::framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glGenBuffers(1, &m_UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
	glBufferData(GL_UNIFORM_BUFFER, 128, NULL, GL_STATIC_DRAW);

	unsigned int uniform_index = glGetUniformBlockIndex(g_baseShader.ID, "Matrices");
	glUniformBlockBinding(g_baseShader.ID, uniform_index, 0);

	uniform_index = glGetUniformBlockIndex(g_boneShader.ID, "Matrices");
	glUniformBlockBinding(g_boneShader.ID, uniform_index, 0);

	uniform_index = glGetUniformBlockIndex(g_shapeShader.ID, "Matrices");
	glUniformBlockBinding(g_shapeShader.ID, uniform_index, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_UBO);


	glGenBuffers(1, &m_lightUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, m_lightUBO);
	glBufferData(GL_UNIFORM_BUFFER, 56, NULL, GL_STATIC_DRAW);

	uniform_index = glGetUniformBlockIndex(g_baseShader.ID, "Lights");
	glUniformBlockBinding(g_baseShader.ID, uniform_index, 1);

	uniform_index = glGetUniformBlockIndex(g_boneShader.ID, "Lights");
	glUniformBlockBinding(g_boneShader.ID, uniform_index, 1);

	uniform_index = glGetUniformBlockIndex(g_shapeShader.ID, "Lights");
	glUniformBlockBinding(g_shapeShader.ID, uniform_index, 1);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_lightUBO);

	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, glm::value_ptr(m_lightAmbient));
	glBufferSubData(GL_UNIFORM_BUFFER, 16, 4, glm::value_ptr(m_lightDiffuse));
	glBufferSubData(GL_UNIFORM_BUFFER, 32, 4, glm::value_ptr(m_lightSpecular));
	glBufferSubData(GL_UNIFORM_BUFFER, 44, 4, &m_lightConstant);
	glBufferSubData(GL_UNIFORM_BUFFER, 48, 4, &m_lightLinear);
	glBufferSubData(GL_UNIFORM_BUFFER, 52, 4, &m_lightQuadratic);


	glGenBuffers(1, &m_geoUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, m_geoUBO);
	glBufferData(GL_UNIFORM_BUFFER, 64, NULL, GL_STATIC_DRAW);

	uniform_index = glGetUniformBlockIndex(g_baseGeometryShader.ID, "View");
	glUniformBlockBinding(g_baseGeometryShader.ID, uniform_index, 2);

	uniform_index = glGetUniformBlockIndex(g_boneGeometryShader.ID, "View");
	glUniformBlockBinding(g_boneGeometryShader.ID, uniform_index, 2);

	uniform_index = glGetUniformBlockIndex(g_shapeGeometryShader.ID, "View");
	glUniformBlockBinding(g_shapeGeometryShader.ID, uniform_index, 2);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, m_geoUBO);


	glGenBuffers(1, &m_geoLineUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, m_geoLineUBO);
	glBufferData(GL_UNIFORM_BUFFER, 64, NULL, GL_STATIC_DRAW);

	uniform_index = glGetUniformBlockIndex(g_baseGeometryShader.ID, "Projection");
	glUniformBlockBinding(g_baseGeometryShader.ID, uniform_index, 3);

	uniform_index = glGetUniformBlockIndex(g_boneGeometryShader.ID, "Projection");
	glUniformBlockBinding(g_boneGeometryShader.ID, uniform_index, 3);

	uniform_index = glGetUniformBlockIndex(g_shapeGeometryShader.ID, "Projection");
	glUniformBlockBinding(g_shapeGeometryShader.ID, uniform_index, 3);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, m_geoLineUBO);

	glGenBuffers(1, &m_boneUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, m_boneUBO);
	glBufferData(GL_UNIFORM_BUFFER, 4096, NULL, GL_STATIC_DRAW);
	uniform_index = glGetUniformBlockIndex(g_boneShader.ID, "Bones");
	glUniformBlockBinding(g_boneShader.ID, uniform_index, 4);

	uniform_index = glGetUniformBlockIndex(g_boneGeometryShader.ID, "Bones");
	glUniformBlockBinding(g_boneGeometryShader.ID, uniform_index, 4);
	glBindBufferBase(GL_UNIFORM_BUFFER, 4, m_boneUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glm::vec3 m_lightPos = glm::vec3(0, 100, 100);
	glm::vec3 m_lightAmbient = glm::vec3(.5, .5, .5);
	glm::vec3 m_lightDiffuse = glm::vec3(1, 1, 1);
	glm::vec3 m_lightSpecular = glm::vec3(.5, .5, .5);
	float m_lightConstant = 1.0f;
	float m_lightLinear = 0.007f;
	float m_lightQuadratic = 0.0002f;
}

void Viewer::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	g_camera.turnCamera(xpos, ypos);
}

void Viewer::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	g_camera.zoom(xoffset, yoffset);
}

int Viewer::viewXG(XGM* xgmObject, const std::vector<size_t>& xgIndices)
{
	g_camera.reset();
	for (size_t modelIndex = 0; modelIndex < xgIndices.size(); ++modelIndex)
		m_models.emplace_back(xgmObject, xgmObject->m_models[xgIndices[modelIndex]]);

	m_previousTime = (float)glfwGetTime();
	bool showNormals = false;
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(m_window, InputHandling::mouse_callback);
	glfwSetScrollCallback(m_window, InputHandling::scroll_callback);
	while (!glfwWindowShouldClose(m_window))
	{
		m_currentTime = (float)glfwGetTime();
		InputHandling::processInputs(m_window, m_currentTime);

		if (InputHandling::g_input_keyboard.KEY_ESCAPE.isPressed())
			break;
		if (InputHandling::g_input_keyboard.KEY_M.isPressed())
		{
			if (m_activeMouse)
			{
				glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				glfwSetCursorPosCallback(m_window, NULL);
				glfwSetScrollCallback(m_window, NULL);
			}
			else
			{
				glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				glfwSetCursorPosCallback(m_window, InputHandling::mouse_callback);
				glfwSetScrollCallback(m_window, InputHandling::scroll_callback);
				g_camera.setFirstMouse();
			}
			m_activeMouse = !m_activeMouse;
		}

		if (m_activeMouse)
			g_camera.moveCamera(m_currentTime - m_previousTime);

		if (InputHandling::g_input_keyboard.KEY_N.isPressed())
			showNormals = !showNormals;

		glClearColor(0.2f, 0.5f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 view = g_camera.getViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(g_camera.m_fov), float(s_SCR_WIDTH) / s_SCR_HEIGHT, 0.1f, 40000.0f);

		glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(projection));
		glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(view));

		glBindBuffer(GL_UNIFORM_BUFFER, m_geoUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(view));

		glBindBuffer(GL_UNIFORM_BUFFER, m_geoLineUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(projection));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		for (auto& model : m_models)
		{
			model.m_animator.update(m_currentTime);
			if (model.m_animator && model.m_animator.m_timeline.m_boneMatrices)
			{
				glBindBuffer(GL_UNIFORM_BUFFER, m_boneUBO);
				glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * sizeof(float) * model.m_animator.m_timeline.m_bones.size(), model.m_animator.m_timeline.m_boneMatrices);
				glBindBuffer(GL_UNIFORM_BUFFER, 0);
			}
			model.draw(m_currentTime, glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, -1)), showNormals);
		}
		glBindVertexArray(0);

		// Check calls
		glfwSwapBuffers(m_window);
		glfwPollEvents();
		m_previousTime = m_currentTime;
	}
	
	m_models.clear();
	GitarooViewer::DagMesh::s_allMeshes.clear();
	g_baseShader.closeProgram();
	g_boneShader.closeProgram();
	g_shapeShader.closeProgram();
	g_baseGeometryShader.closeProgram();
	g_boneGeometryShader.closeProgram();
	g_shapeGeometryShader.closeProgram();
	InputHandling::resetInputs();

	glfwTerminate();
	return 0;
}

std::list<GitarooViewer::DagMesh*> GitarooViewer::DagMesh::s_allMeshes;

GitarooViewer::Model::~Model()
{
	for (auto& mesh : m_meshes)
		delete mesh;
}

GitarooViewer::DagMesh::~DagMesh()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteVertexArrays(1, &m_transformVAO);
	glDeleteBuffers(1, &m_transformVBO);
	if (m_fanEBO)
		glDeleteBuffers(1, &m_fanEBO);
	if (m_stripEBO)
		glDeleteBuffers(1, &m_stripEBO);
	if (m_listEBO)
		glDeleteBuffers(1, &m_listEBO);
}

GitarooViewer::DagMesh::Material::~Material()
{
	if (texture_24)
		glDeleteTextures(1, &textureID);
	else if (texture_32)
		glDeleteTextures(1, &textureID);
}

GitarooViewer::Model::Model(XGM* xgm, XG& xg)
{
	m_animator.load(&xg);
	// Map an identity matrix to the first node
	m_animator.m_timeline.m_modelTransforms.emplace_back(nullptr);

	for (int dagIndex = 0; dagIndex < xg.m_data->m_dag.size(); ++dagIndex)
	{
		if (dynamic_cast<xgDagTransform*>(xg.m_data->m_dag[dagIndex].m_base.m_node))
			loadTransform(xgm, xg.m_data->m_dag[dagIndex]);
		else
		{
			DagMesh* mesh = new DagMesh;
			if (mesh->load(xgm, (xgDagMesh*)xg.m_data->m_dag[dagIndex].m_base.m_node, m_animator.m_timeline, 0))
				m_meshes.push_back(mesh);
			else
			{
				auto iter = m_meshes.begin();
				while (iter != m_meshes.end() && !(*iter)->m_transparency)
					++iter;
				m_meshes.insert(iter, mesh);
			}
		}
	}

	if (m_animator.m_timeline.m_bones.size())
		m_animator.m_timeline.m_boneMatrices = new float[m_animator.m_timeline.m_bones.size()][16];
}

void GitarooViewer::Model::loadTransform(XGM* xgm, XG_Data::DagBase& dagBase)
{
	xgDagTransform* transformNode = (xgDagTransform*)dagBase.m_base.m_node;
	size_t transformIndex = 0;
	if (transformNode->m_inputMatrices.size())
	{
		for (; transformIndex < m_animator.m_timeline.m_modelTransforms.size(); ++transformIndex)
			if (m_animator.m_timeline.m_modelTransforms[transformIndex].m_transform == transformNode)
				goto Load_Meshes;

		m_animator.m_timeline.m_modelTransforms.emplace_back(transformNode);
	}

Load_Meshes:
	for (int dagIndex = 0; dagIndex < dagBase.m_connected.size(); ++dagIndex)
	{
		if (dynamic_cast<xgDagTransform*>(dagBase.m_connected[dagIndex].m_base.m_node))
			loadTransform(xgm, dagBase.m_connected[dagIndex]);
		else
		{
			DagMesh* mesh = new DagMesh;
			if (mesh->load(xgm, (xgDagMesh*)dagBase.m_connected[dagIndex].m_base.m_node, m_animator.m_timeline, transformIndex))
				m_meshes.push_back(mesh);
			else
			{
				auto iter = m_meshes.begin();
				while (iter != m_meshes.end() && !(*iter)->m_transparency)
					++iter;
				m_meshes.insert(iter, mesh);
			}
		}
	}
}

bool GitarooViewer::DagMesh::load(XGM* xgm, xgDagMesh* mesh, Timeline& timeline, size_t transformIndex)
{
	bool newGeometry = true, newMaterial = true;
	m_mesh = mesh;

	// Checks if we can just copy vertex or texture data of another mesh
	for (auto& prevMesh : s_allMeshes)
	{
		if (newGeometry && prevMesh->m_mesh->m_inputGeometries.front().m_node == mesh->m_inputGeometries.front().m_node)
		{
			m_dagTransformIndex = prevMesh->m_dagTransformIndex;
			m_numVerts = prevMesh->m_numVerts;

			m_vertices = prevMesh->m_vertices;
			m_boneVertices = prevMesh->m_boneVertices;
			m_shapeVertices = prevMesh->m_shapeVertices;

			m_VAO = prevMesh->m_VAO;
			m_VBO = prevMesh->m_VBO;
			m_transformVAO = prevMesh->m_transformVAO;
			m_transformVBO = prevMesh->m_transformVBO;

			m_transformShader = prevMesh->m_transformShader;
			m_transformGeoShader = prevMesh->m_transformGeoShader;
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
		m_materials = std::make_shared<std::list<Material>>();
		if (dynamic_cast<xgMaterial*>(node))
		{
			xgMaterial* xgMat = (xgMaterial*)node;
			m_materials->push_back({ xgMat, GL_LESS });
			if ((xgMat->m_blendType != 0 && xgMat->m_blendType != 4) || xgMat->m_flags & 1)
				m_transparency = true;
			for (auto& tex : xgMat->m_inputTextures)
			{
				PString textNode = tex->m_imxName;
				for (int index = 0; index < textNode.m_size; ++index)
					textNode.m_pstring[index] = toupper(textNode.m_pstring[index]);
				for (IMX image : xgm->m_textures)
				{
					if (strstr(image.getName(), textNode.m_pstring))
						m_materials->back().loadTexture(image);
				}
			}
			m_materials->back().bindTexture();
		}
		else if (dynamic_cast<xgMultiPassMaterial*>(node))
		{
			for (auto& shared : ((xgMultiPassMaterial*)node)->m_inputMaterials)
			{
				xgMaterial* xgMat = (xgMaterial*)shared.m_node;
				if (!m_materials->size())
					m_materials->push_back({ xgMat, GL_LESS });
				else
					m_materials->push_back({ xgMat, GL_LEQUAL });
				if (xgMat->m_blendType != 0 && xgMat->m_blendType != 4 || xgMat->m_flags & 1)
					m_transparency = true;
				for (auto& tex : xgMat->m_inputTextures)
				{
					PString textNode = tex->m_imxName;
					for (int index = 0; index < textNode.m_size; ++index)
						textNode.m_pstring[index] = toupper(textNode.m_pstring[index]);
					for (IMX image : xgm->m_textures)
					{
						if (strstr(image.getName(), textNode.m_pstring))
							m_materials->back().loadTexture(image);
					}
				}
				m_materials->back().bindTexture();
			}
		}
	}

	if (newGeometry)
	{
		xgBgGeometry* geo = m_mesh->m_inputGeometries.front().m_node;
		m_numVerts = geo->m_numVerts;
		m_dagTransformIndex = transformIndex;
		size_t size = 0;
		if (geo->m_vertexFlags & 1)
			size += 4;
		if (geo->m_vertexFlags & 2)
			size += 3;
		if (geo->m_vertexFlags & 4)
			size += 4;
		if (geo->m_vertexFlags & 8)
			size += 2;

		glGenBuffers(1, &m_VBO);
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

		m_vertices = std::make_shared<Vertex[]>(m_numVerts);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(4 * sizeof(float)));
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(7 * sizeof(float)));
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(11 * sizeof(float)));

		Vertex* verts = m_vertices.get();
		switch (geo->m_vertexFlags)
		{
		case 15:
			glEnableVertexAttribArray(3);
			__fallthrough;
		case 7:
			glEnableVertexAttribArray(2);
			__fallthrough;
		case 3:
			glEnableVertexAttribArray(1);
			__fallthrough;
		case 1:
			glEnableVertexAttribArray(0);
			for (unsigned long index = 0; index < m_numVerts; ++index)
				memcpy(verts + index, geo->m_vertices + size * index, size * sizeof(float));
			break;
		default:
		{
			float* data = geo->m_vertices;
			if (geo->m_vertexFlags & 1)
			{
				glEnableVertexAttribArray(0);
				for (unsigned long index = 0; index < m_numVerts; ++index)
					memcpy(verts[index].m_position, data + size * index, 4 * sizeof(float));
				data += 4;
			}

			if (geo->m_vertexFlags & 2)
			{
				glEnableVertexAttribArray(1);
				for (unsigned long index = 0; index < m_numVerts; ++index)
					memcpy(verts[index].m_normal, data + size * index, 3 * sizeof(float));
				data += 3;
			}

			if (geo->m_vertexFlags & 4)
			{
				glEnableVertexAttribArray(2);
				for (unsigned long index = 0; index < m_numVerts; ++index)
					memcpy(verts[index].m_color, data + size * index, 4 * sizeof(float));
				data += 4;
			}

			if (geo->m_vertexFlags & 8)
			{
				glEnableVertexAttribArray(3);
				for (unsigned long index = 0; index < m_numVerts; ++index)
					memcpy(verts[index].m_texCoord, data + size * index, 2 * sizeof(float));
			}
		}
		}

		glBufferData(GL_ARRAY_BUFFER, m_numVerts * sizeof(Vertex), m_vertices.get(), GL_STATIC_DRAW);

		if (geo->m_inputEnvelopes.size())
		{
			m_transformShader = g_boneShader;
			m_transformGeoShader = g_boneGeometryShader;

			glGenBuffers(1, &m_transformVBO);
			glGenVertexArrays(1, &m_transformVAO);
			glBindVertexArray(m_transformVAO);
			glBindBuffer(GL_ARRAY_BUFFER, m_transformVBO);

			m_boneVertices = std::make_shared<BoneVertex[]>(m_numVerts);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)(4 * sizeof(float)));
			glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)(7 * sizeof(float)));
			glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)(11 * sizeof(float)));
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)(13 * sizeof(float)));
			glVertexAttribIPointer(5, 4, GL_UNSIGNED_INT, sizeof(BoneVertex), (void*)(17 * sizeof(float)));

			if (geo->m_vertexFlags & 1)
				glEnableVertexAttribArray(0);
			if (geo->m_vertexFlags & 2)
				glEnableVertexAttribArray(1);
			if (geo->m_vertexFlags & 4)
				glEnableVertexAttribArray(2);
			if (geo->m_vertexFlags & 8)
				glEnableVertexAttribArray(3);

			glEnableVertexAttribArray(4);
			glEnableVertexAttribArray(5);

			for (unsigned long index = 0; index < m_numVerts; ++index)
				memcpy(m_boneVertices.get() + index, verts + index, sizeof(Vertex));

			for (auto& env : geo->m_inputEnvelopes)
			{
				unsigned long indices[4] = { 0 };
				for (size_t bone = 0; bone < env->m_inputMatrices.size(); ++bone)
				{
					xgBone* boneNode = env->m_inputMatrices[bone].m_node;
					unsigned long index = 0;
					for (; index < timeline.m_bones.size(); ++index)
						if (timeline.m_bones[index].m_bone == boneNode)
							goto Set_Index;

					timeline.m_bones.emplace_back(boneNode);

				Set_Index:
					indices[bone] = index;
				}

				for (size_t i = 0, weightIndex = 0; i < env->m_numTargets; ++i, ++weightIndex)
				{
					unsigned long target = env->m_vertexTargets[i];
					do
					{
						memcpy(m_boneVertices[target].m_weights, env->m_weights[weightIndex], 4 * sizeof(float));
						memcpy(m_boneVertices[target].m_boneIDs, indices, 4 * sizeof(unsigned long));
						target = env->m_vertexTargets[++i];
					} while (target != -1);
				}
			}

			glBufferData(GL_ARRAY_BUFFER, m_numVerts * sizeof(BoneVertex), m_boneVertices.get(), GL_STATIC_DRAW);
		}
		else if (geo->m_inputShapeInterpolator.isValid() ||
				geo->m_inputVertexInterpolator.isValid() ||
				geo->m_inputNormalInterpolator.isValid() ||
				geo->m_inputTexCoordInterpolator.isValid())
		{
			m_transformShader = g_shapeShader;
			m_transformGeoShader = g_shapeGeometryShader;

			timeline.m_shapes.emplace_back();
			Timeline::Shape& shape = timeline.m_shapes.back();
			shape.m_numVerts = m_numVerts;

			// Now set the buffers for the manipulated vertices

			glGenBuffers(1, &m_transformVBO);
			glGenVertexArrays(1, &m_transformVAO);
			glBindVertexArray(m_transformVAO);
			glBindBuffer(GL_ARRAY_BUFFER, m_transformVBO);

			shape.m_animated = std::make_shared<ShapeVertex[]>(m_numVerts);
			m_shapeVertices = shape.m_animated;
			glBufferData(GL_ARRAY_BUFFER, m_numVerts * sizeof(ShapeVertex), m_shapeVertices.get(), GL_DYNAMIC_DRAW);

			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (void*)0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (void*)(4 * sizeof(float)));
			glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (void*)(7 * sizeof(float)));
			glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (void*)(11 * sizeof(float)));
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (void*)(13 * sizeof(float)));
			glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (void*)(17 * sizeof(float)));
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (void*)(20 * sizeof(float)));
			glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (void*)(24 * sizeof(float)));

			if (geo->m_vertexFlags & 1)
			{
				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(4);
			}

			if (geo->m_vertexFlags & 2)
			{
				glEnableVertexAttribArray(1);
				glEnableVertexAttribArray(5);
			}

			if (geo->m_vertexFlags & 4)
			{
				glEnableVertexAttribArray(2);
				glEnableVertexAttribArray(6);
			}

			if (geo->m_vertexFlags & 8)
			{
				glEnableVertexAttribArray(3);
				glEnableVertexAttribArray(7);
			}

			if (geo->m_inputShapeInterpolator.isValid())
				timeline.fillShape(geo->m_inputShapeInterpolator.m_node, verts);
			else
			{
				bool filled = false;
				if (geo->m_inputVertexInterpolator.isValid())
					timeline.fillPositions(geo->m_inputVertexInterpolator.m_node, verts, filled);

				if (geo->m_inputNormalInterpolator.isValid())
					timeline.fillNormals(geo->m_inputNormalInterpolator.m_node, verts, filled);

				if (geo->m_inputTexCoordInterpolator.isValid())
					timeline.fillTexCoords(geo->m_inputTexCoordInterpolator.m_node, verts, filled);
			}
		}
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

void GitarooViewer::DagMesh::Material::loadTexture(const IMX& image)
{
	if (!textureID)
	{
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
	}

	if (image.m_data->m_pixelVal1 == 3 && image.m_data->m_pixelVal2 == 2)
	{
		if (!texture_24)
		{
			width = image.m_data->m_width;
			height = image.m_data->m_height;
			texture_24 = new unsigned char[(size_t)width * height][3];
			memcpy(texture_24, image.m_data->m_colorData->m_image, 3 * width * height);
		}
		else
		{
			for (unsigned long p = 0; p < width * height; ++p)
			{
				texture_24[p][0] = unsigned char(texture_24[p][0] * (image.m_data->m_colorData->m_image[3 * p] / 255.0f));
				texture_24[p][1] = unsigned char(texture_24[p][1] * (image.m_data->m_colorData->m_image[3 * p + 1] / 255.0f));
				texture_24[p][2] = unsigned char(texture_24[p][2] * (image.m_data->m_colorData->m_image[3 * p + 2] / 255.0f));
			}
		}
	}
	else if (image.m_data->m_pixelVal1 == 4 && image.m_data->m_pixelVal2 == 2)
	{
		if (!texture_32)
		{
			width = image.m_data->m_width;
			height = image.m_data->m_height;
			texture_32 = new unsigned char[(size_t)width * height][4];
			memcpy(texture_32, image.m_data->m_colorData->m_image, 4 * width * height);
		}
		else
		{
			for (unsigned long p = 0; p < width * height; ++p)
			{
				texture_32[p][0] = unsigned char(texture_32[p][0] * (image.m_data->m_colorData->m_image[4 * p] / 255.0f));
				texture_32[p][1] = unsigned char(texture_32[p][1] * (image.m_data->m_colorData->m_image[4 * p + 1] / 255.0f));
				texture_32[p][2] = unsigned char(texture_32[p][2] * (image.m_data->m_colorData->m_image[4 * p + 2] / 255.0f));
				texture_32[p][3] = unsigned char(texture_32[p][3] * (image.m_data->m_colorData->m_image[4 * p + 3] / 255.0f));
			}
		}
	}
	else
	{
		if (image.m_data->m_pixelVal1 != 0 || image.m_data->m_pixelVal2 != 0)
		{
			if (!texture_32)
			{
				width = image.m_data->m_width;
				height = image.m_data->m_height;
				texture_32 = new unsigned char[(size_t)width * height][4];
				for (unsigned long p = 0; p < width * height; ++p)
					memcpy(texture_32[p], image.m_data->m_colorData->m_palette[image.m_data->m_colorData->m_image[p]], 4);
			}
			else
			{
				for (unsigned long p = 0; p < width * height; ++p)
				{
					unsigned char* pixel = image.m_data->m_colorData->m_palette[image.m_data->m_colorData->m_image[p]];
					texture_32[p][0] = unsigned char(texture_32[p][0] * (pixel[0] / 255.0f));
					texture_32[p][1] = unsigned char(texture_32[p][1] * (pixel[1] / 255.0f));
					texture_32[p][2] = unsigned char(texture_32[p][2] * (pixel[2] / 255.0f));
					texture_32[p][3] = unsigned char(texture_32[p][3] * (pixel[3] / 255.0f));
				}
			}
		}
		else
		{
			if (!texture_32)
			{
				width = image.m_data->m_width;
				height = image.m_data->m_height;
				texture_32 = new unsigned char[(size_t)width * height][4];
				for (unsigned long p = 0; p < width * height; ++p)
				{
					IMX_Data::Pixel4* pix = (IMX_Data::Pixel4*)&image.m_data->m_colorData->m_image[p >> 1];
					memcpy(texture_32[p], image.m_data->m_colorData->m_palette[p & 1 ? (size_t)pix->pixel2 : (size_t)pix->pixel1], 4);
				}
			}
			else
			{
				for (unsigned long p = 0; p < width * height; ++p)
				{
					IMX_Data::Pixel4* pix = (IMX_Data::Pixel4*)&image.m_data->m_colorData->m_image[p >> 1];
					unsigned char* pixel = image.m_data->m_colorData->m_palette[p & 1 ? (size_t)pix->pixel2 : (size_t)pix->pixel1];
					texture_32[p][0] = unsigned char(texture_32[p][0] * (pixel[0] / 255.0f));
					texture_32[p][1] = unsigned char(texture_32[p][1] * (pixel[1] / 255.0f));
					texture_32[p][2] = unsigned char(texture_32[p][2] * (pixel[2] / 255.0f));
					texture_32[p][3] = unsigned char(texture_32[p][3] * (pixel[3] / 255.0f));
				}
			}
		}
		
	}
}

void GitarooViewer::DagMesh::Material::bindTexture()
{
	if (texture_24)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_24);
		delete[width * height] texture_24;
	}
	else if (texture_32)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_32);
		delete[width * height] texture_32;
	}
	glGenerateMipmap(GL_TEXTURE_2D);
}

void GitarooViewer::Model::draw(const float time, glm::mat4 base, const bool showNormals)
{
	for (auto& dag : m_meshes)
	{
		Shader* baseShader;
		glm::mat4 model;
		// If the animator is not active or if there are no transfromations on this mesh
		if (!m_animator || !dag->m_transformShader)
		{
			if (!m_animator)
				model = base;
			else
				model = base * m_animator.m_timeline.m_modelTransforms[dag->m_dagTransformIndex].m_transformMatrix;
			glBindBuffer(GL_ARRAY_BUFFER, dag->m_VBO);
			glBindVertexArray(dag->m_VAO);
			baseShader = &g_baseShader;
			baseShader->use();
		}
		else
		{
			model = base * m_animator.m_timeline.m_modelTransforms[dag->m_dagTransformIndex].m_transformMatrix;
			glBindBuffer(GL_ARRAY_BUFFER, dag->m_transformVBO);
			glBindVertexArray(dag->m_transformVAO);
			baseShader = &dag->m_transformShader;
			baseShader->use();
			if (dag->m_shapeVertices)
			{
				glBufferSubData(GL_ARRAY_BUFFER, 0, dag->m_numVerts * sizeof(ShapeVertex), dag->m_shapeVertices.get());
				xgBgGeometry* geo = dag->m_mesh->m_inputGeometries.front().m_node;
				if (geo->m_inputShapeInterpolator.isValid())
				{
					xgShapeInterpolator* shape = geo->m_inputShapeInterpolator.m_node;
					if (!shape->m_type)
					{
						baseShader->setInt("interpolation.position", 0);
						baseShader->setInt("interpolation.normal", 0);
						baseShader->setInt("interpolation.color", 0);
						baseShader->setInt("interpolation.texCoord", 0);
					}
					else
					{
						baseShader->setInt("interpolation.position", shape->m_keys[0].m_vertexType & 1);
						baseShader->setInt("interpolation.normal", shape->m_keys[0].m_vertexType & 2);
						baseShader->setInt("interpolation.color", shape->m_keys[0].m_vertexType & 4);
						baseShader->setInt("interpolation.texCoord", shape->m_keys[0].m_vertexType & 8);
					}
				}
				else
				{
					baseShader->setInt("interpolation.color", 0);

					baseShader->setInt("interpolation.position",
						geo->m_inputVertexInterpolator.isValid() && geo->m_inputVertexInterpolator->m_type);

					baseShader->setInt("interpolation.normal",
						geo->m_inputNormalInterpolator.isValid() && geo->m_inputNormalInterpolator->m_type);

					baseShader->setInt("interpolation.texCoord",
						geo->m_inputTexCoordInterpolator.isValid() && geo->m_inputTexCoordInterpolator->m_type);
				}
				baseShader->setFloat("coefficient", m_animator.getCoefficient());
			}
		}

		// Used to sort triangle groups for proper transparency
		// Not perfect as some groups can be both foreground AND background
		// Sorting does not take animations into account
		// That will require custom depth buffer manipulation - an ideal currently out of scope
		if (dag->m_mesh->m_primType == 5 && dag->m_transparency)
		{
			std::sort(dag->m_groups.rbegin(), dag->m_groups.rend(),
			[&](const DagMesh::TriGroup& group_1, const DagMesh::TriGroup& group_2)
			{
				float dist1 = FLT_MAX;
				for (unsigned long index = 0; index < group_1.numVerts; ++index)
				{
					float distance = glm::length(g_camera.m_position -
						glm::vec3(dag->m_vertices[group_1.index + index].m_position[0],
							dag->m_vertices[group_1.index + index].m_position[1],
							-dag->m_vertices[group_1.index + index].m_position[0]));
					if (distance < dist1)
						dist1 = distance;
				}
				float dist2 = FLT_MAX;
				for (unsigned long index = 0; index < group_2.numVerts; ++index)
				{
					float distance = glm::length(g_camera.m_position -
						glm::vec3(dag->m_vertices[group_2.index + index].m_position[0],
							dag->m_vertices[group_2.index + index].m_position[1],
							-dag->m_vertices[group_2.index + index].m_position[0]));
					if (distance < dist2)
						dist2 = distance;
				}
				return dist1 < dist2;
			});
		}

		baseShader->setVec3("lightPosition", glm::value_ptr(g_camera.m_position));
		baseShader->setVec3("viewPos", glm::value_ptr(g_camera.m_position));
		baseShader->setMat4("model", glm::value_ptr(model));

		for (auto& mat : *dag->m_materials)
		{
			unsigned int shader = mat.mat->m_shadingType;
			if (mat.textureID)
			{
				// If both a texture and vertex color are applicable
				if (shader >= 3)
					baseShader->setInt("shadingType", shader + 2);
				else
					baseShader->setInt("shadingType", shader);
				glBindTexture(GL_TEXTURE_2D, mat.textureID);
				baseShader->setInt("alphaType", mat.mat->m_flags & 1);
				baseShader->setInt("alphaMultiplier", mat.mat->m_flags & ~1);
				baseShader->setInt("textEnv", mat.mat->m_textureEnv);
			}
			else
				baseShader->setInt("shadingType", shader);
			glBlendColor(mat.mat->m_diffuse.red, mat.mat->m_diffuse.green, mat.mat->m_diffuse.blue, mat.mat->m_diffuse.alpha);
			baseShader->setVec4("material.color", (float*)&mat.mat->m_diffuse);
			baseShader->setVec3("material.specular", (float*)&mat.mat->m_specular);
			baseShader->setFloat("material.shininess", mat.mat->m_specular.exponent);

			baseShader->setInt("blendingType", mat.mat->m_blendType);
			switch (mat.mat->m_blendType)
			{
			case 0:
				if (mat.mat->m_flags & 1)
				{
					glEnable(GL_BLEND);
					glBlendEquation(GL_FUNC_ADD);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				}
				else
					glDisable(GL_BLEND);
				break;
			case 1:
				glEnable(GL_BLEND);
				glBlendEquation(GL_FUNC_ADD);
				if (mat.textureID)
				{
					if (mat.mat->m_flags & 1)
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					else
						glBlendFuncSeparate(GL_CONSTANT_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				}
				else
					glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				break;
			case 2:
				glEnable(GL_BLEND);
				glBlendEquation(GL_FUNC_ADD);
				glBlendFunc(GL_DST_COLOR, GL_ZERO);
				break;
			case 3:
				glEnable(GL_BLEND);
				glBlendEquation(GL_FUNC_ADD);
				glBlendFuncSeparate(GL_CONSTANT_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
				break;
			case 4:
				if (mat.mat->m_flags & 1)
				{
					glEnable(GL_BLEND);
					glBlendEquation(GL_FUNC_ADD);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				}
				else
					glDisable(GL_BLEND);
				break;
			case 5:
				glEnable(GL_BLEND);
				glBlendEquation(GL_FUNC_ADD);
				glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
			}

			glDepthFunc(mat.depth);
			
			if (dag->m_mesh->m_primType == 4)
			{
				if (dag->m_fanEBO)
				{
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dag->m_fanEBO);
					for (unsigned long valueIndex = 0; valueIndex < dag->m_mesh->m_triFanData.m_arraySize;)
					{
						const unsigned long numIndexes = dag->m_mesh->m_triFanData.m_arrayData[valueIndex++];
						glDrawElements(GL_TRIANGLE_FAN, numIndexes, GL_UNSIGNED_INT, (void*)(valueIndex * sizeof(unsigned long)));
						valueIndex += numIndexes;
					}
				}

				if (dag->m_stripEBO)
				{
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dag->m_stripEBO);
					for (unsigned long valueIndex = 0; valueIndex < dag->m_mesh->m_triStripData.m_arraySize;)
					{
						const unsigned long numIndexes = dag->m_mesh->m_triStripData.m_arrayData[valueIndex++];
						glDrawElements(GL_TRIANGLE_STRIP, numIndexes, GL_UNSIGNED_INT, (void*)(valueIndex * sizeof(unsigned long)));
						valueIndex += numIndexes;
					}
				}

				if (dag->m_listEBO)
				{
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dag->m_listEBO);
					for (unsigned long valueIndex = 0; valueIndex < dag->m_mesh->m_triListData.m_arraySize;)
					{
						const unsigned long numIndexes = dag->m_mesh->m_triListData.m_arrayData[valueIndex++];
						glDrawElements(GL_TRIANGLES, numIndexes, GL_UNSIGNED_INT, (void*)(valueIndex * sizeof(unsigned long)));
						valueIndex += numIndexes;
					}
				}
			}
			else if (dag->m_mesh->m_primType == 5)
				for (auto& group : dag->m_groups)
					glDrawArrays(group.mode, group.index, group.numVerts);
		}

		if (showNormals)
		{
			Shader* geoShader;
			if (!m_animator || !dag->m_transformShader)
			{
				geoShader = &g_baseGeometryShader;
				geoShader->use();
			}
			else
			{
				geoShader = &dag->m_transformGeoShader;
				geoShader->use();
				if (dag->m_shapeVertices)
				{
					xgBgGeometry* geo = dag->m_mesh->m_inputGeometries.front().m_node;
					if (geo->m_inputShapeInterpolator.isValid())
					{
						xgShapeInterpolator* shape = geo->m_inputShapeInterpolator.m_node;
						if (!shape->m_type)
						{
							baseShader->setInt("interpolation.position", 0);
							baseShader->setInt("interpolation.normal", 0);
						}
						else
						{
							baseShader->setInt("interpolation.position", shape->m_keys[0].m_vertexType & 1);
							baseShader->setInt("interpolation.normal", shape->m_keys[0].m_vertexType & 2);
						}
					}
					else
					{
						if (geo->m_inputVertexInterpolator.isValid())
							baseShader->setInt("interpolation.position", 1);
						else
							baseShader->setInt("interpolation.position", 0);

						if (geo->m_inputNormalInterpolator.isValid())
							baseShader->setInt("interpolation.normal", 1);
						else
							baseShader->setInt("interpolation.normal", 0);
					}
					baseShader->setFloat("coefficient", m_animator.getCoefficient());
				}
			}

			geoShader->setMat4("model", glm::value_ptr(model));
			if (dag->m_mesh->m_primType == 4)
			{
				if (dag->m_fanEBO)
				{
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dag->m_fanEBO);
					for (unsigned long valueIndex = 0; valueIndex < dag->m_mesh->m_triFanData.m_arraySize;)
					{
						const unsigned long numIndexes = dag->m_mesh->m_triFanData.m_arrayData[valueIndex++];
						glDrawElements(GL_TRIANGLE_FAN, numIndexes, GL_UNSIGNED_INT, (void*)(valueIndex * sizeof(unsigned long)));
						valueIndex += numIndexes;
					}
				}

				if (dag->m_stripEBO)
				{
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dag->m_stripEBO);
					for (unsigned long valueIndex = 0; valueIndex < dag->m_mesh->m_triStripData.m_arraySize;)
					{
						const unsigned long numIndexes = dag->m_mesh->m_triStripData.m_arrayData[valueIndex++];
						glDrawElements(GL_TRIANGLE_STRIP, numIndexes, GL_UNSIGNED_INT, (void*)(valueIndex * sizeof(unsigned long)));
						valueIndex += numIndexes;
					}
				}

				if (dag->m_listEBO)
				{
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dag->m_listEBO);
					for (unsigned long valueIndex = 0; valueIndex < dag->m_mesh->m_triListData.m_arraySize;)
					{
						const unsigned long numIndexes = dag->m_mesh->m_triListData.m_arrayData[valueIndex++];
						glDrawElements(GL_TRIANGLES, numIndexes, GL_UNSIGNED_INT, (void*)(valueIndex * sizeof(unsigned long)));
						valueIndex += numIndexes;
					}
				}
			}
			else if (dag->m_mesh->m_primType == 5)
				for (auto& group : dag->m_groups)
					glDrawArrays(group.mode, group.index, group.numVerts);
		}
	}
}
