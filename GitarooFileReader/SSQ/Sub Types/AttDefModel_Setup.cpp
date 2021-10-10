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

void AttDefModelSetup::create(FILE* outFile) const
{
	ModelSetup::create(outFile);
	if (m_headerVersion >= 0x1200)
	{
		fwrite(&m_attackSize_Z, 4, 1, outFile);
		fwrite(&m_attackValues, sizeof(AttackStreamValues), 1, outFile);
	}
}

glm::mat4 AttDefModelSetup::animateFromGameState(XG* xg, const float frame)
{
	if (g_gameState[static_cast<int>(m_type)])
	{
		const glm::mat4 matrix = getModelMatrix(frame);
		const float length = xg->getAnimationLength(0);
		xg->animate(fmod(frame - m_bpmStartFrame, length), 0, matrix);
		return matrix;
	}
	return glm::mat4();
}
