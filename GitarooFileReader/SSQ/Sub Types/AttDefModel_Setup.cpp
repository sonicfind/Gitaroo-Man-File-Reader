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
AttDefModelSetup::AttDefModelSetup(FILE* inFile, ModelType type, char(&name)[16])
	: ModelSetup(inFile, type, name)
{
	if (m_headerVersion >= 0x1200)
	{
		fread(&m_attackSize_Z, 4, 1, inFile);
		fread(&m_attackValues, sizeof(AttackStreamValues), 1, inFile);
	}
}

std::vector<std::string> AttDefModelSetup::getConnectedNames() const
{
	if (m_attackValues.m_targetModel[0] != 0)
	{
		std::vector<std::string> names{ m_attackValues.m_targetModel , m_attackValues.m_startingModel };
		if (names.front().length() == 0)
			names.erase(names.begin());
		return names;
	}
	return {};
}

void AttDefModelSetup::setConnectedMatrices(glm::mat4* mat_1, glm::mat4* mat_2)
{
	m_matrix1 = mat_1;
	m_matrix2 = mat_2;
}

void AttDefModelSetup::create(FILE* outFile) const
{
	ModelSetup::create(outFile);
	if (m_headerVersion >= 0x1200)
	{
		fwrite(&m_attackSize_Z, 4, 1, outFile);
		fwrite(&m_attackValues, sizeof(AttackStreamValues), 1, outFile);
	}
}

void AttDefModelSetup::animateFromGameState(XG* xg, const glm::mat4& matrix, const float frame)
{
	if (g_gameState[static_cast<int>(m_type)])
	{
		const float animLength = xg->getAnimationLength(0);
		if (m_attackValues.m_targetModel[0] != 0)
		{
			const glm::vec3 start = glm::vec3((*m_matrix1) * glm::vec4(m_attackValues.m_startOffset, 1));
			const glm::vec3 end = glm::vec3((*m_matrix2) * glm::vec4(m_attackValues.m_targetOffset, 1));
			const glm::vec3 diff = end - start;
			const glm::vec3 forward = glm::mat3(matrix) * glm::vec3(0, 0, -1);
			const float length = glm::length(diff);
			const glm::vec3 cross = glm::cross(forward, diff);
			const float w = length + glm::dot(forward, diff);

			glm::mat4 result = glm::toMat4(glm::normalize(glm::quat(w, cross))) * matrix;
			result[2] *= length / m_attackSize_Z;
			result[3] = glm::vec4(start, 1);

			xg->animate(fmod(frame - m_bpmStartFrame, animLength), 0, result);
		}
		else
			xg->animate(fmod(frame - m_bpmStartFrame, animLength), 0, matrix);
	}
}
