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
#include "Dag.h"
class XG_Data
{
	friend class XG;
	void addNode(const PString& type, const PString& name);
public:
	std::list<std::unique_ptr<XGNode>> m_nodes;
	std::vector<Dag> m_dagMap;
	SharedNode<xgTime> m_timeNode;
	XG_Data() = default;
	XG_Data(FILE* inFile, unsigned long& filesize);
	void create(FILE* outFile);

	// Finds all xgMaterial nodes that contain references to a texture
	// and connects each to the respective IMX pointer
	void connectTextures(std::vector<IMX>& textures);
	// Constructs all the vertex and uniform buffers for use in the OpenGL viewer
	void initializeViewerState();
	// Deletes all the vertex and uniform buffers created for the OpenGL viewer
	void uninitializeViewerState();
	// Sets all vertex and bone matrix values to their defaults
	void restPose() const;
	// Updates all data to the current frame
	void animate(float frame, unsigned long instance, const glm::mat4 matrix);
	// Draws all vertex data to the current framebuffer
	void draw(const glm::mat4 view, const unsigned long numInstances, const bool showNormals, const bool doTransparents, const bool isAnimated = true) const;
};
