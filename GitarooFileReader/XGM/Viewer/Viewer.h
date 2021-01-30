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
#include "XGM/XGM.h"
#include "Animator.h"
#include "Shaders.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace GitarooViewer
{
	struct DagMesh
	{
		xgDagMesh* m_mesh = nullptr;
		size_t m_dagTransformIndex = 0;

		unsigned int m_numVerts = 0;
		std::shared_ptr<Vertex[]> m_vertices;
		std::shared_ptr<BoneVertex[]> m_boneVertices;
		std::shared_ptr<ShapeVertex[]> m_shapeVertices;

		unsigned int m_VBO = 0;
		unsigned int m_VAO = 0;
		unsigned int m_transformVBO = 0;
		unsigned int m_transformVAO = 0;

		Shader m_transformShader;
		Shader m_transformGeoShader;

		unsigned int m_fanEBO = 0;
		unsigned int m_stripEBO = 0;
		unsigned int m_listEBO = 0;

		struct Material
		{
			xgMaterial* mat;
			unsigned int texture = 0;
		};
		std::vector<Material> m_materials;
		bool m_transparency = false;

		struct TriGroup
		{
			unsigned long index;
			unsigned long numVerts;
			unsigned int mode;
		};
		std::vector<TriGroup> m_groups;
		static std::list<DagMesh*> s_allMeshes;

		~DagMesh();
		bool load(XGM* xgm, xgDagMesh* mesh, Timeline& timeline, size_t transformIndex);
		void bindTexture(const IMX& image, unsigned int& ID);
	};

	struct Model
	{
		std::list<DagMesh*> m_meshes;
		Animator m_animator;
		Model(XGM* xgm, XG& xg);
		~Model();
		void loadTransform(XGM* xgm, XG_Data::DagBase& dagBase);
		void draw(const float time, glm::mat4 base, const bool showNormals);
	};
}

class Viewer
{
	// settings
	const unsigned int s_SCR_WIDTH = 960;
	const unsigned int s_SCR_HEIGHT = 720;
	std::list<GitarooViewer::Model> m_models;
	GLFWwindow* m_window;
	unsigned int m_UBO;
	unsigned int m_lightUBO;
	unsigned int m_geoUBO;
	unsigned int m_geoLineUBO;
	unsigned int m_boneUBO;
	float m_currentTime = 0;
	float m_previousTime = 0;

	glm::vec3 m_lightPos;
	glm::vec3 m_lightAmbient;
	glm::vec3 m_lightDiffuse;
	glm::vec3 m_lightSpecular;
	float m_lightConstant;
	float m_lightLinear;
	float m_lightQuadratic;
public:
	Viewer();
	int viewXG(XGM* xgmObject, const std::vector<size_t>& xgIndices);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};
