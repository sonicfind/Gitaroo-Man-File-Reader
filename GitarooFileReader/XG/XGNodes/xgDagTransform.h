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
#include "xgBgMatrix.h"
class xgDagTransform : public DagNode
{
	SharedNode<xgBgMatrix> m_inputMatrix;

public:
	using DagNode::DagNode;
	unsigned long read(FILE* inFile, const std::vector<std::unique_ptr<XGNode>>& nodeList);
	void create(FILE* outFile, bool full) const;
	void write_to_txt(FILE* txtFile, const char* tabs = "");
	const char* getType() { return "xgDagTransform"; }
	static bool compare(const PString& str) { return strcmp("xgDagTransform", str.m_pstring) == 0; }
	glm::mat4x4 getModelMatrix() const;
};
