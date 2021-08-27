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
#include "xgVec3Interpolator.h"
#include "xgQuatInterpolator.h"
class xgBgMatrix : public XGNode
{
	glm::vec3 m_position;
	glm::quat m_rotation;
	glm::vec3 m_scale;
	SharedNode<xgVec3Interpolator> m_inputPosition;
	SharedNode<xgQuatInterpolator> m_inputRotation;
	SharedNode<xgVec3Interpolator> m_inputScale;
	SharedNode<xgBgMatrix> m_inputParentMatrix;

public:
	using XGNode::XGNode;
	unsigned long read(FILE* inFile, const std::vector<std::unique_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full) const;
	void write_to_txt(FILE* txtFile, const char* tabs = "");
	const char* getType() { return "xgBgMatrix"; }
	static bool compare(const PString& str) { return strcmp("xgBgMatrix", str.m_pstring) == 0; }
	void applyTransformations(glm::vec3& pos, glm::quat& rot, glm::vec3& scl);
};
