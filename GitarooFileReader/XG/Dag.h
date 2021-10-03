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
#include "XGNodes/xgDagMesh.h"
#include "XGNodes/xgDagTransform.h"
class Dag
{
	SharedNode<DagNode> m_base;
	std::vector<Dag> m_connected;

public:
	Dag(FILE* inFile, const std::list<std::unique_ptr<XGNode>>& nodeList, bool isRootBranch = false);
	void create(FILE* outFile, bool isRootBranch = false) const;
	void queue_for_obj(std::vector<std::pair<size_t, xgBgGeometry*>>&  history) const;
	void faces_to_obj(FILE* objFile, std::vector<std::pair<size_t, xgBgGeometry*>>& history) const;
	void connectTextures(std::vector<IMX>& textures);

	void initializeViewerState();
	void uninitializeViewerState();
	void restPose() const;
	void animate(unsigned long instance, glm::mat4 matrix);
	void draw(const glm::mat4 view, const unsigned long numInstances, const bool showNormals, const bool doTransparents, const bool isAnimated = true) const;
};
