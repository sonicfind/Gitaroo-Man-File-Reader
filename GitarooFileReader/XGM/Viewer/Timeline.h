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
#include "XG/XG_Nodes.h"
#include <glm/glm.hpp>

struct Vertex
{
	float m_position[4] = { 0 };
	float m_normal[3] = { 0 };
	float m_color[4] = { 0 };
	float m_texCoord[2] = { 0 };
};

struct BoneVertex
{
	Vertex vertex;
	float m_weights[4] = { 0 };
	unsigned long m_boneIDs[4] = { 0 };
};

struct ShapeVertex
{
	Vertex vertex;
	Vertex nextVertex;
};

struct Timeline
{
	// Holds the initial/base values to transform bones from
	struct Bone
	{
		const xgBone* m_bone;
		const glm::mat4 m_rest;
		Bone(const xgBone* bone);
	};
	std::vector<Bone> m_bones;
	float(*m_boneMatrices)[16] = nullptr;
	~Timeline()
	{
		if (m_boneMatrices)
			delete[m_bones.size()] m_boneMatrices;
	}

	// Holds the vertex data for every keyframe
	// Also holds the pose data for when animation is disabled
	struct Shape
	{
		unsigned long m_numVerts;
		std::shared_ptr<ShapeVertex[]> m_animated;
		std::vector<Vertex*> m_keys;
		std::vector<unsigned long> m_times;
		~Shape()
		{
			if (m_numVerts)
				for (auto& key : m_keys)
					delete[m_numVerts] key;
		}
	};
	std::list<Shape> m_shapes;

	// First matrix is always going to be an identity matrix
	// All other matrices will be the result of a dagTransform node
	struct ModelTransform
	{
		const xgDagTransform* m_transform;
		glm::mat4 m_transformMatrix = glm::mat4(1.0f);
		ModelTransform(const xgDagTransform* transform)
			: m_transform(transform) {}
	};
	std::vector<ModelTransform> m_modelTransforms;

	// Shape/Vertex/Normal/TexCoord Interpolator fill functions
	void fillShape(xgShapeInterpolator* interpolator, const Vertex* pose);
	void fillPositions(xgVertexInterpolator* interpolator, const Vertex* pose, bool& preFilled);
	void fillNormals(xgNormalInterpolator* interpolator, const Vertex* pose, bool& preFilled);
	void fillTexCoords(xgTexCoordInterpolator* interpolator, const Vertex* pose, bool preFilled);
};