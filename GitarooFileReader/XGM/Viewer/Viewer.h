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
#include "Primitives.h"
#include "Material.h"
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

		std::shared_ptr<std::list<Material>> m_materials;
		bool m_transparency = false;

		TriElements m_triFanElements;
		TriElements m_triStripElements;
		TriElements m_triListElements;
		TriArrays m_triFanArrays;
		TriArrays m_triStripArrays;
		TriArrays m_triListArrays;

		static std::list<DagMesh*> s_allMeshes;

		DagMesh();
		~DagMesh();
		bool load(XGM* xgm, xgDagMesh* mesh, Timeline& timeline, size_t transformIndex);
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

enum class AspectRatioMode
{
	SDTV,
	Widescreen,
	UltraWide
};

class Viewer
{
	// settings
	static AspectRatioMode s_aspectRatio;
	static unsigned int s_screenWidth;
	static unsigned int s_screenHeight;

	GLFWwindow* m_window;
	bool m_activeMouse = true;
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

	std::list<GitarooViewer::Model> m_models;

public:
	Viewer(XGM* xgmObject, const std::vector<size_t>& xgIndices);
	int viewXG();
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	static std::string getAspectRatioString();
	static unsigned int getScreenHeight() { return s_screenHeight; }
	static void switchAspectRatio();
	static bool changeHeight();

private:
	static void setWidth();
};
