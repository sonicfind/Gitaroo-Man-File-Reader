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
ModelSetup::ModelSetup(FILE* inFile, ModelType type, glm::mat4& mat)
	: m_type(type)
	, m_matrix(mat)
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

void ModelSetup::bindXG(XG* xg)
{
	m_xg = xg;
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

void ModelSetup::updateMatrix(const float frame) const
{
	glm::vec3 position;
	if (m_positions.empty())
		position = m_baseValues.basePosition;
	else
	{
		auto iter = getIter(m_positions, frame);
		if (!iter->doInterpolation || iter + 1 == m_positions.end())
			position = iter->position;
		else
			position = glm::mix(iter->position, (iter + 1)->position, (frame - iter->frame) * iter->coefficient);
	}

	glm::quat rotation;
	if (m_rotations.empty())
		rotation = m_baseValues.baseRotation;
	else
	{
		auto iter = getIter(m_rotations, frame);
		if (!iter->doInterpolation || iter + 1 == m_rotations.end())
			rotation = iter->rotation;
		else
			rotation = glm::slerp(iter->rotation, (iter + 1)->rotation, (frame - iter->frame) * iter->coefficient);
	}

	m_matrix = glm::toMat4(rotation);
	m_matrix[3] = glm::vec4(position, 1);
	
	if (!m_scalars.empty())
	{
		glm::vec3 scale;
		auto iter = getIter(m_scalars, frame);
		if (!iter->doInterpolation || iter + 1 == m_scalars.end())
			scale = iter->scalar;
		else
			scale = glm::mix(iter->scalar, (iter + 1)->scalar, (frame - iter->frame) * iter->coefficient);
		m_matrix[0] *= scale.x;
		m_matrix[1] *= scale.y;
		m_matrix[2] *= scale.z;
	}
}

bool ModelSetup::animate(const float frame)
{
	bool doShadow = false;
	if (!m_animations.empty())
	{
		auto iter = getIter(m_animations, frame);
		doShadow = iter->dropShadow && !iter->noDrawing;
		if (!iter->noDrawing && !iter->pollGameState)
		{
			if (m_controllableIndex != 0)
			{
				m_controllableIndex = 0;
				m_controllableStartFrame = m_bpmStartFrame;
			}
			float length = m_xg->getAnimationLength(iter->animIndex);
			float start = iter->frame;
			
			{
				std::vector<ModelAnim>::const_iterator test = iter;
				while (test != m_animations.begin() &&
					!test->startOverride)
				{
					--test;
					if (test->animIndex == iter->animIndex)
						start = test->frame;
					else
					{
						if (!test->loop)
							start = test->frame + m_xg->getAnimationLength(test->animIndex);
						break;
					}
				}
			}

			if (iter->loop)
				m_xg->animate(fmod(frame - start, length), iter->animIndex, m_matrix);
			else if (frame >= length + start &&
				!iter->holdLastFrame &&
				iter + 1 != m_animations.end() &&
				iter->animIndex + 1 == (iter + 1)->animIndex &&
				!(iter + 1)->loop && !(iter + 1)->startOverride)
				m_xg->animate(frame - (start + length), (iter + 1)->animIndex, m_matrix);
			else
				m_xg->animate(frame - start, iter->animIndex, m_matrix);
		}
		else if (iter->pollGameState)
			animateFromGameState(frame, !iter->noDrawing);
	}
	else
		m_xg->animate(fmod(frame, m_xg->getAnimationLength(m_baseValues.baseAnimIndex_maybe)), m_baseValues.baseAnimIndex_maybe, m_matrix);
	return doShadow;
}
