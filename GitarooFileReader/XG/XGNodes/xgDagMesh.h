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
#include "XG/Triangle Types/Triangle_Fan.h"
#include "XG/Triangle Types/Triangle_Strip.h"
#include "XG/Triangle Types/Triangle_List.h"
#include "xgBgGeometry.h"
#include "xgMultiPassMaterial.h"
#include <glm/glm.hpp>
class xgDagMesh : public DagNode
{
	unsigned long m_primType = 0;
	std::unique_ptr<Triangle_Prim> m_prim;
	std::unique_ptr<Triangle_Fan> m_triFan;
	std::unique_ptr<Triangle_Strip> m_triStrip;
	std::unique_ptr<Triangle_List> m_triList;
	unsigned long m_cullFunc = 0;
	SharedNode<xgBgGeometry> m_inputGeometry;
	SharedNode<MaterialNode> m_inputMaterial;

	bool m_doGeometryAnimation = true;
public:
	using DagNode::DagNode;
	unsigned long read(FILE* inFile, const std::list<std::unique_ptr<XGNode>>& nodeList);
	void create(FILE* outFile) const;
	void write_to_txt(FILE* txtFile, const char* tabs = "") const;
	static bool compareType(const PString& str) { return strcmp("xgDagMesh", str.m_pstring) == 0; }
	const size_t getSize() const;

	void queue_for_obj(std::vector<std::pair<size_t, xgBgGeometry*>>& history) const;
	void faces_to_obj(FILE* objFile, std::vector<std::pair<size_t, xgBgGeometry*>>& history) const;

private:
	static unsigned long s_currentCulling;

public:
	void connectTextures(std::vector<IMX>& textures);
	void intializeBuffers();
	void deleteBuffers();
	void restPose() const;
	void animate();
	void draw(const glm::mat4 view, glm::mat4 model, const bool showNormals, const bool doTransparents) const;
};
