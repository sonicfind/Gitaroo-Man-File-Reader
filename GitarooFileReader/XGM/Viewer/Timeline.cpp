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
#include "pch.h"
#include "Timeline.h"
#include <glm/gtc/type_ptr.hpp>

Timeline::Bone::Bone(const xgBone* bone)
	: m_bone(bone), m_rest((glm::make_mat4(bone->m_restMatrix))) {}

void Timeline::fillShape(xgShapeInterpolator* interpolator, const Vertex* pose)
{
	const unsigned long numVerts = m_shapes.back().m_numVerts;
	for (size_t i = 0; i < interpolator->m_numkeys; ++i)
	{
		Vertex* verts = new Vertex[numVerts];
		memcpy(verts, pose, numVerts * sizeof(Vertex));

		size_t size = 0;
		if (interpolator->m_keys[i].m_vertexType & 1)
			size += 4;
		if (interpolator->m_keys[i].m_vertexType & 2)
			size += 3;
		if (interpolator->m_keys[i].m_vertexType & 4)
			size += 4;
		if (interpolator->m_keys[i].m_vertexType & 8)
			size += 2;

		// Run through every vertex multiple times, shifting the starting address to line up for certain values
		float* data = interpolator->m_keys[i].m_vertices;
		const size_t end = numVerts * size;

		if (interpolator->m_keys[i].m_vertexType & 1)
		{
			for (size_t index = 0, vert = 0; index < end; index += size, ++vert)
				memcpy(verts[vert].m_position, data + index, 4 * sizeof(float));
			data += 4;
		}
		if (interpolator->m_keys[i].m_vertexType & 2)
		{
			for (size_t index = 0, vert = 0; index < end; index += size, ++vert)
				memcpy(verts[vert].m_normal, data + index, 3 * sizeof(float));
			data += 3;
		}
		if (interpolator->m_keys[i].m_vertexType & 4)
		{
			for (size_t index = 0, vert = 0; index < end; index += size, ++vert)
				memcpy(verts[vert].m_color, data + index, 4 * sizeof(float));
			data += 4;
		}
		if (interpolator->m_keys[i].m_vertexType & 8)
		{
			for (size_t index = 0, vert = 0; index < end; index += size, ++vert)
				memcpy(verts[vert].m_texCoord, data + index, 2 * sizeof(float));
		}

		m_shapes.back().m_keys.push_back(verts);
	}

	for (size_t t = 0; t < interpolator->m_inputTimes.front()->m_numFrames; ++t)
	{
		if (t < interpolator->m_numtimes)
			m_shapes.back().m_times.push_back((unsigned long)interpolator->m_times[t]);
		else
			m_shapes.back().m_times.push_back((unsigned long)interpolator->m_times[interpolator->m_numtimes - 1]);
	}
}

void Timeline::fillPositions(xgVertexInterpolator* interpolator, const Vertex* pose, bool& preFilled)
{
	const unsigned long numVerts = m_shapes.back().m_numVerts;
	for (size_t i = 0; i < interpolator->m_numkeys; ++i)
	{
		Vertex* verts;
		if (!preFilled)
		{
			verts = new Vertex[numVerts];
			memcpy(verts, pose, numVerts * sizeof(Vertex));
			m_shapes.back().m_keys.push_back(verts);
		}
		else
			verts = m_shapes.back().m_keys[i];

		const unsigned long keyIndex = (unsigned long)interpolator->m_times[i];
		for (unsigned long index = 0; index < interpolator->m_keys[keyIndex].m_numPositions; ++index)
			memcpy(verts[interpolator->m_targets[index]].m_position, interpolator->m_keys[keyIndex].m_positions[index], 3 * sizeof(float));
	}

	if (!preFilled)
	{
		for (size_t t = 0; t < interpolator->m_inputTimes.front()->m_numFrames; ++t)
		{
			if (t < interpolator->m_numtimes)
				m_shapes.back().m_times.push_back((unsigned long)interpolator->m_times[t]);
			else
				m_shapes.back().m_times.push_back((unsigned long)interpolator->m_times[interpolator->m_numtimes - 1]);
		}
		preFilled = true;
	}
}

void Timeline::fillNormals(xgNormalInterpolator* interpolator, const Vertex* pose, bool& preFilled)
{
	const unsigned long numVerts = m_shapes.back().m_numVerts;
	for (size_t i = 0; i < interpolator->m_numkeys; ++i)
	{
		Vertex* verts;
		if (!preFilled)
		{
			verts = new Vertex[numVerts];
			memcpy(verts, pose, numVerts * sizeof(Vertex));
			m_shapes.back().m_keys.push_back(verts);
		}
		else
			verts = m_shapes.back().m_keys[i];

		const unsigned long keyIndex = (unsigned long)interpolator->m_times[i];
		for (unsigned long index = 0; index < interpolator->m_keys[keyIndex].m_numNormals; ++index)
			memcpy(verts[interpolator->m_targets[index]].m_normal, interpolator->m_keys[keyIndex].m_normals[index], 3 * sizeof(float));
	}

	if (!preFilled)
	{
		for (size_t t = 0; t < interpolator->m_inputTimes.front()->m_numFrames; ++t)
		{
			if (t < interpolator->m_numtimes)
				m_shapes.back().m_times.push_back((unsigned long)interpolator->m_times[t]);
			else
				m_shapes.back().m_times.push_back((unsigned long)interpolator->m_times[interpolator->m_numtimes - 1]);
		}
		preFilled = true;
	}
}

void Timeline::fillTexCoords(xgTexCoordInterpolator* interpolator, const Vertex* pose, bool preFilled)
{
	const unsigned long numVerts = m_shapes.back().m_numVerts;
	for (size_t i = 0; i < interpolator->m_numkeys; ++i)
	{
		Vertex* verts;
		if (!preFilled)
		{
			verts = new Vertex[numVerts];
			memcpy(verts, pose, numVerts * sizeof(Vertex));
			m_shapes.back().m_keys.push_back(verts);
		}
		else
			verts = m_shapes.back().m_keys[i];

		const unsigned long keyIndex = (unsigned long)interpolator->m_times[i];
		for (unsigned long index = 0; index < interpolator->m_keys[keyIndex].m_numCoords; ++index)
			memcpy(verts[interpolator->m_targets[index]].m_texCoord, interpolator->m_keys[keyIndex].m_texcoords[index], 2 * sizeof(float));
	}

	if (!preFilled)
	{
		for (size_t t = 0; t < interpolator->m_inputTimes.front()->m_numFrames; ++t)
		{
			if (t < interpolator->m_numtimes)
				m_shapes.back().m_times.push_back((unsigned long)interpolator->m_times[t]);
			else
				m_shapes.back().m_times.push_back((unsigned long)interpolator->m_times[interpolator->m_numtimes - 1]);
		}
	}
}