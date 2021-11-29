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

	m_positions.fill(numPositions, inFile);
	m_rotations.fill(numRotations, inFile);
	flipHand(m_rotations.m_vect);

	unsigned long numAnimations;
	fread(&numAnimations, 4, 1, inFile);
	if (numAnimations > 1)
		m_animations.fill(numAnimations, inFile);

	if (m_headerVersion >= 0x1100)
	{
		unsigned long numScalars;
		fread(&numScalars, 4, 1, inFile);
		if (numScalars > 1)
			m_scalars.fill(numScalars, inFile);
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
	m_positions.write(outFile);
	std::vector<Rotation> tmp = m_rotations.m_vect;
	flipHand(tmp);
	fwrite(&tmp.front(), sizeof(Rotation), numRotations, outFile);
	m_animations.write_conditioned(outFile);

	if (m_headerVersion >= 0x1100)
	{
		m_scalars.write_conditioned(outFile);
		fwrite(&m_baseValues, sizeof(BaseValues), 1, outFile);
	}
}

bool ModelSetup::bindXG(XG* xg)
{
	m_xg = xg;
	if (!m_baseValues.depthTest)
		xg->disableDepthMask();
	return m_baseValues.depthTest;
}

void ModelSetup::jumpToFrame(const float frame)
{
	m_bpmStartFrame = frame;
}

void ModelSetup::reset()
{
	m_controllableIndex = 0;
	m_controllableStartFrame = m_bpmStartFrame;
}

void ModelSetup::updateMatrix(const float frame)
{
	glm::vec3 position;
	if (m_positions.empty())
		position = m_baseValues.basePosition;
	else
	{
		auto iter = m_positions.update(frame);
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
		auto iter = m_rotations.update(frame);
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
		auto iter = m_scalars.update(frame);
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
		auto iter = m_animations.update(frame);
		doShadow = iter->dropShadow && !iter->noDrawing;
		if (!iter->noDrawing && !iter->pollGameState)
		{
			if (m_controllableIndex != 0)
			{
				m_controllableIndex = 0;
				m_controllableStartFrame = m_bpmStartFrame;
			}
			
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

				if (test->startOverride && test->loop)
				{
					if (m_xg->isAnimationTempoBased(iter->animIndex))
						start = m_bpmStartFrame;
				}
			}

			unsigned long index = iter->animIndex;
			if (!iter->loop)
			{
				float length = m_xg->getAnimationLength(iter->animIndex);
				if (frame >= length + start &&
					!iter->holdLastFrame &&
					iter + 1 != m_animations.end() &&
					iter->animIndex + 1 == (iter + 1)->animIndex &&
					!(iter + 1)->loop && !(iter + 1)->startOverride)
				{
					++index;
					start += length;
				}
			}

			m_xg->animate(frame - start, index, m_matrix, iter->loop);
		}
		else if (iter->pollGameState)
			animateFromGameState(frame, !iter->noDrawing);
	}
	else
		m_xg->animate(frame, m_baseValues.baseAnimIndex_maybe, m_matrix, true);
	return doShadow;
}
