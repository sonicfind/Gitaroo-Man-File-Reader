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
PlayerModelSetup::PlayerModelSetup(FILE* inFile, ModelType type, char(&name)[16])
	: ModelSetup(inFile, type, name)
{
	if (m_headerVersion >= 0x1300)
	{
		fread(&m_numControllables, 4, 1, inFile);

		m_controllables.reserve(m_numControllables);
		m_controllables.resize(m_numControllables);
		fread(m_controllables.data(), sizeof(Controllable), m_numControllables, inFile);
		//m_controllables[1].ulong_h = 1;

		m_connections.resize(m_numControllables);
		m_connections.reserve(m_numControllables);
		for (auto& set : m_connections)
		{
			fread(&set.size, 4, 1, inFile);
			if (set.size)
			{
				set.controllableList.resize(set.size);
				fread(set.controllableList.data(), 4, set.size, inFile);
			}
		}

		m_endings.reserve(m_numControllables);
		m_endings.resize(m_numControllables);
		fread(m_endings.data(), 4, m_numControllables, inFile);
	}
}

void PlayerModelSetup::create(FILE* outFile) const
{
	ModelSetup::create(outFile);
	if (m_headerVersion >= 0x1300)
	{
		fwrite(&m_numControllables, 4, 1, outFile);
		fwrite(m_controllables.data(), sizeof(Controllable), m_numControllables, outFile);

		for (auto& set : m_connections)
		{
			fwrite(&set.size, 4, 1, outFile);
			if (set.size)
				fwrite(set.controllableList.data(), 4, set.size, outFile);
		}

		fwrite(m_endings.data(), 4, m_numControllables, outFile);
	}
}

#include <time.h>
void PlayerModelSetup::animateFromGameState(XG* xg, const float frame)
{
	// Insert check to see if an uninterruptible animation isn't finished
	int flag = 0;
	float ang = 0;
	int descriptor = 0;

	const Controllable* current = &m_controllables[m_controllableIndex];
	float length = xg->getAnimationLength(current->m_animIndex) + current->m_holdTime;

	if (current->m_interruptible || frame >= length + m_controllableStartFrame)
	{
		do
		{
			if (!current->m_interruptible)
			{
				m_controllableIndex = m_endings[m_controllableIndex];
				current = &m_controllables[m_controllableIndex];
			}

			bool connectionMade = current->m_randomize;
			if (!connectionMade)
			{
				for (const auto& connection : m_connections[m_controllableIndex].controllableList)
				{
					if (m_controllables[connection].m_angleMin <= ang
						&& ang < m_controllables[connection].m_angleMax
						&& flag == m_controllables[connection].m_eventFlag
						&& descriptor == m_controllables[connection].m_descriptor)
					{
						m_controllableIndex = connection;
						connectionMade = true;
						break;
					}
				}
			}
			else
			{
				srand(unsigned int(time(0)));
				m_controllableIndex = m_connections[m_controllableIndex].controllableList[rand() % m_connections[m_controllableIndex].size];
			}

			if (connectionMade)
			{
				current = &m_controllables[m_controllableIndex];
				if (current->m_useCurrentFrame_maybe)
					m_controllableStartFrame = length + m_controllableStartFrame;
				else
					m_controllableStartFrame = m_bpmStartFrame;

				length = xg->getAnimationLength(current->m_animIndex) + current->m_holdTime;
			}
		} while (!current->m_interruptible && frame >= length + m_controllableStartFrame);
	}

	const glm::mat4 matrix = getModelMatrix(frame);
	xg->animate(fmod(frame - m_controllableStartFrame, length), current->m_animIndex, matrix);
}
