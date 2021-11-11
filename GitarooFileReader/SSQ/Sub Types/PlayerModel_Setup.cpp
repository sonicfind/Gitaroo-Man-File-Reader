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
PlayerModelSetup::PlayerModelSetup(FILE* inFile, ModelType type)
	: ModelSetup(inFile, type)
{
	if (m_headerVersion >= 0x1300)
	{
		fread(&m_numControllables, 4, 1, inFile);

		m_controllables.reserve(m_numControllables);
		m_controllables.resize(m_numControllables);
		fread(m_controllables.data(), sizeof(Controllable), m_numControllables, inFile);
		//m_controllables[1].ulong_h = 1;

		m_connections.reserve(m_numControllables);
		m_connections.resize(m_numControllables);
		for (auto& set : m_connections)
		{
			fread(&set.size, 4, 1, inFile);
			if (set.size)
			{
				set.controllableList.reserve(set.size);
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
void PlayerModelSetup::animateFromGameState(const glm::mat4& matrix, const float frame)
{
	const Controllable* current = &m_controllables[m_controllableIndex];
	float length = m_xg->getAnimationLength(current->m_animIndex) + current->m_holdTime;
	if (current->m_eventFlag == 64 ||
		(current->m_eventFlag == 4 && 
			current->m_angleMin == -3.14159298f &&
			current->m_angleMax == 3.14159298f))
		length += m_xg->getAnimationLength(12);

	while (!current->m_interruptible && frame >= length + m_controllableStartFrame)
	{
		m_controllableIndex = m_endings[m_controllableIndex];
		current = &m_controllables[m_controllableIndex];

		if (!current->m_interruptible)
			m_controllableStartFrame = length + m_controllableStartFrame;
		else
			m_controllableStartFrame = m_bpmStartFrame;

		length = m_xg->getAnimationLength(current->m_animIndex) + current->m_holdTime;
		if (current->m_eventFlag == 64 ||
			(current->m_eventFlag == 4 &&
				current->m_angleMin == -3.14159298f &&
				current->m_angleMax == 3.14159298f))
			length += m_xg->getAnimationLength(12);
	}

	while (current->m_interruptible && !checkInterruptible(frame))
		current = &m_controllables[m_controllableIndex];
	
	const float delta = frame - m_controllableStartFrame;
	if (length > m_xg->getAnimationLength(current->m_animIndex) + current->m_holdTime)
	{
		float anim12 = .5f * m_xg->getAnimationLength(12);
		if (delta < anim12)
			m_xg->animate(2 * delta, 12, matrix, 1);
		else if (delta < length - anim12)
			m_xg->animate(delta - anim12, current->m_animIndex, matrix, 1);
		else
			m_xg->animate(2 * (delta - (length - anim12)), 12, matrix, 0);
	}
	else
		m_xg->animate(fmod(delta, length), current->m_animIndex, matrix, current->m_playbackDirection);
}

bool PlayerModelSetup::checkInterruptible(const float frame)
{
	if (m_controllables[m_controllableIndex].m_randomize != 1)
	{
		const auto player = g_gameState.getPlayerState(static_cast<int>(m_type) - 1);
		for (const auto& connection : m_connections[m_controllableIndex].controllableList)
		{
			if (m_controllables[connection].m_angleMin <= player.getAngle() &&
				player.getAngle() < m_controllables[connection].m_angleMax &&
				player.getEvent() == m_controllables[connection].m_eventFlag &&
				player.getDescriptor() == m_controllables[connection].m_descriptor)
			{
				m_controllableIndex = connection;

				// Has to be in this order
				if (!m_controllables[m_controllableIndex].m_interruptible)
					m_controllableStartFrame = frame;
				else
					checkInterruptible(frame);
				return true;
			}
		}

		if (m_endings[m_controllableIndex] != -1)
		{
			m_controllableIndex = m_endings[m_controllableIndex];
			if (!m_controllables[m_controllableIndex].m_interruptible)
				m_controllableStartFrame = frame;
			return false;
		}
	}
	else
		m_controllableIndex = m_connections[m_controllableIndex].controllableList[rand() % m_connections[m_controllableIndex].size];
	return true;
}
