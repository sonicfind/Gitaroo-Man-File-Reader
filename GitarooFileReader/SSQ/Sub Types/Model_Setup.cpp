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
#include "Model_Setup.h"
ModelSetup::ModelSetup(FILE* inFile, ModelType type, char(&name)[16])
	: m_name(name)
	, m_type(type)
{
	// Block Tag
	char tmp[5] = { 0 };
	fread(tmp, 1, 4, inFile);
	if (!strstr(tmp, "GMPX"))
	{
		int val = ftell(inFile) - 4;
		fclose(inFile);
		throw "Error: No 'GMPX' Tag at byte " + std::to_string(val);
	}

	fread(&m_headerVersion, 4, 1, inFile);
	fread(&m_controllableIndex, 4, 1, inFile);
	fread(&m_bpmStartFrame, 4, 1, inFile);
	fread(&m_controllableStartFrame, 4, 1, inFile);

	fread(m_junk, 1, 16, inFile);
	unsigned long numPositions, numRotations;
	fread(&numPositions, 4, 1, inFile);
	fread(&numRotations, 4, 1, inFile);

	m_positions.resize(numPositions);
	fread(&m_positions.front(), sizeof(Position), numPositions, inFile);

	m_rotations.resize(numRotations);
	fread(&m_rotations.front(), sizeof(Rotation), numRotations, inFile);
	flipHand(m_rotations);

	unsigned long numAnimations;
	fread(&numAnimations, 4, 1, inFile);
	if (numAnimations > 1)
	{
		m_animations.resize(numAnimations);
		fread(&m_animations.front(), sizeof(ModelAnim), numAnimations, inFile);
	}

	if (m_headerVersion >= 0x1100)
	{
		unsigned long numScalars;
		fread(&numScalars, 4, 1, inFile);
		if (numScalars > 1)
		{
			m_scalars.resize(numScalars);
			fread(&m_scalars.front(), sizeof(ModelScalar), numScalars, inFile);
		}
		fread(&m_baseValues, sizeof(BaseValues), 1, inFile);
	}
}

void ModelSetup::create(FILE* outFile) const
{
	// Block Tag
	fprintf(outFile, "GMPX");

	fwrite(&m_headerVersion, 4, 1, outFile);
	fwrite(&m_controllableIndex, 4, 1, outFile);
	fwrite(&m_bpmStartFrame, 4, 1, outFile);
	fwrite(&m_controllableStartFrame, 4, 1, outFile);

	fwrite(m_junk, 1, 16, outFile);

	unsigned long numPositions = (unsigned long)m_positions.size(), numRotations = (unsigned long)m_rotations.size();

	fwrite(&numPositions, 4, 1, outFile);
	fwrite(&numRotations, 4, 1, outFile);
	fwrite(&m_positions.front(), sizeof(Position), numPositions, outFile);
	std::vector<Rotation> tmp = m_rotations;
	flipHand(tmp);
	fwrite(&tmp.front(), sizeof(Rotation), numRotations, outFile);

	unsigned long size = (unsigned long)m_animations.size();
	if (!size)
		size = 1;
	fwrite(&size, 4, 1, outFile);

	if (size > 1)
		fwrite(&m_animations.front(), sizeof(ModelAnim), size, outFile);

	if (m_headerVersion >= 0x1100)
	{
		size = (unsigned long)m_scalars.size();
		if (!size)
			size = 1;
		fwrite(&size, 4, 1, outFile);

		if (size > 1)
			fwrite(&m_scalars.front(), sizeof(ModelScalar), size, outFile);
		fwrite(&m_baseValues, sizeof(BaseValues), 1, outFile);
	}
}

void ModelSetup::setBPMFrame(const float frame)
{
	m_bpmStartFrame = frame;
}

void ModelSetup::reset()
{
	m_controllableIndex = 0;
	m_controllableStartFrame = m_bpmStartFrame;
}

#include <glm/gtx/transform.hpp>
glm::mat4 ModelSetup::getModelMatrix(const float frame) const
{
	glm::vec3 position;
	if (m_positions.empty())
		position = m_baseValues.m_basePosition;
	else
	{
		auto iter = getIter(m_positions, frame);
		if (!iter->m_doInterpolation || iter + 1 == m_positions.end())
			position = iter->m_position;
		else
			position = glm::mix(iter->m_position, (iter + 1)->m_position, (frame - iter->m_frame) * iter->m_coefficient);
	}

	glm::quat rotation;
	if (m_rotations.empty())
		rotation = m_baseValues.m_baseRotation;
	else
	{
		auto iter = getIter(m_rotations, frame);
		if (!iter->m_doInterpolation || iter + 1 == m_rotations.end())
			rotation = iter->m_rotation;
		else
			rotation = glm::slerp(iter->m_rotation, (iter + 1)->m_rotation, (frame - iter->m_frame) * iter->m_coefficient);
	}

	glm::mat4 result = glm::toMat4(rotation);
	result[3] = glm::vec4(position, 1);
	
	if (!m_scalars.empty())
	{
		glm::vec3 scale;
		auto iter = getIter(m_scalars, frame);
		if (!iter->m_doInterpolation || iter + 1 == m_scalars.end())
			scale = iter->m_scalar;
		else
			scale = glm::mix(iter->m_scalar, (iter + 1)->m_scalar, (frame - iter->m_frame) * iter->m_coefficient);
		result[0] *= scale.x;
		result[1] *= scale.y;
		result[2] *= scale.z;
	}
	return result;
}

std::pair<bool, glm::mat4> ModelSetup::animate(XG* xg, const float frame)
{
	std::pair<bool, glm::mat4> shadowMatrix(false, getModelMatrix(frame));
	if (!m_animations.empty())
	{
		auto iter = getIter(m_animations, frame);
		if (!iter->m_noDrawing)
		{
			const bool looping = iter->m_loop;
			const unsigned long animIndex = xg->getValidatedAnimationIndex(iter->m_animIndex);
			shadowMatrix.first = iter->m_dropShadow;

			while (iter != m_animations.begin()
				&& !iter->m_startOverride
				&& !iter->m_pollGameState
				&& xg->getValidatedAnimationIndex((iter - 1)->m_animIndex) == animIndex)
				--iter;
			
			if (!iter->m_pollGameState)
			{
				const float length = xg->getAnimationLength(animIndex);
				if (frame < length + iter->m_frame)
					xg->animate(frame - iter->m_frame, animIndex, shadowMatrix.second);
				else if (looping)
					xg->animate(fmod(frame - iter->m_frame, length), animIndex, shadowMatrix.second);
				else
					xg->animate(length - 1, animIndex, shadowMatrix.second);
			}
			else
				animateFromGameState(xg, shadowMatrix.second, frame);
		}
	}
	else
		xg->animate(fmod(frame, xg->getAnimationLength(m_baseValues.m_baseAnimIndex_maybe)), m_baseValues.m_baseAnimIndex_maybe, shadowMatrix.second);
	return shadowMatrix;
}
