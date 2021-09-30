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
#include "xgBone.h"
constexpr int MAX_INSTANCES = 32;
constexpr int MAX_BONES = 4;
struct Weight
{
	float values[MAX_BONES];
};

class xgBgGeometry;
class xgEnvelope : public XGNode
{
	unsigned long m_startVertex = 0;
	std::vector<Weight> m_weights;
	std::vector<std::vector<unsigned long>> m_vertexTargets;
	std::vector<SharedNode<xgBone>> m_inputMatrices;
	SharedNode<xgBgGeometry> m_inputGeometry;

	glm::mat4 m_matrices[MAX_INSTANCES][MAX_BONES];
	static unsigned int s_BoneSSBU;
public:
	using XGNode::XGNode;
	unsigned long read(FILE* inFile, const std::list<std::unique_ptr<XGNode>>& nodeList);
	void create(FILE* outFile) const;
	void write_to_txt(FILE* txtFile, const char* tabs = "") const;
	const char* getType() { return "xgEnvelope"; }
	static bool compareType(const PString& str) { return strcmp("xgEnvelope", str.m_pstring) == 0; }
	const size_t getSize() const;

	static void generateBoneUniform();
	static void deleteBoneUniform();
	static void bindBoneUniform();
	static void unbindBoneUniform();
	void bindBoneWeights(unsigned long envIndex) const;
	void restPose();
	void animate(unsigned long instance);
	void updateBoneMatrices(unsigned long envIndex, const size_t numInstances) const;
};
