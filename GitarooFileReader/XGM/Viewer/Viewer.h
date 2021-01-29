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

namespace GitarooViewer
{
	struct DagMesh
	{
		xgDagMesh* m_mesh = nullptr;
		std::shared_ptr<float[]> m_vertices;
		unsigned int m_size = 0;
		unsigned int m_VBO = 0;
		unsigned int m_VAO = 0;
		unsigned int m_vertexSize = 0;
		unsigned int m_fanEBO = 0;
		unsigned int m_stripEBO = 0;
		unsigned int m_listEBO = 0;
		unsigned long m_flags = 0;

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
		bool load(XGM* xgm, xgDagMesh* mesh);
		void bindTexture(const IMX& image, unsigned int& ID);
		void draw(const float time, glm::mat4 base, const bool showNormals);
	};

	struct Dag;

	struct DagTransform
	{
		xgDagTransform* m_transform = nullptr;
		std::list<Dag*> m_dags;
		bool m_transparency = false;
		~DagTransform();
		bool load(XGM* xgm, XG_Data::DagBase& dagBase);
		void draw(const float time, glm::mat4 base, const bool showNormals);
	};

	struct Dag
	{
		DagMesh* m_mesh = nullptr;
		DagTransform* m_transform = nullptr;
		bool m_transparency = false;
		~Dag();
		bool load(XGM* xgm, XG_Data::DagBase& dagBase);
		void draw(const float time, glm::mat4 base, const bool showNormals);
	};

	struct Model
	{
		std::list<GitarooViewer::Dag*> m_dags;
		~Model();
		void load(XGM* xgm, XG& xg);
		void draw(const float time, glm::mat4 base, const bool showNormals);
	};
}

class Viewer
{
	// settings
	const unsigned int s_SCR_WIDTH = 800;
	const unsigned int s_SCR_HEIGHT = 600;
	std::vector<GitarooViewer::Model> m_models;
public:
	static bool s_showNormals;
	int viewXG(XGM* xgmObject, const std::vector<size_t>& xgIndices);
};



